/**
 * Projekt IMS 2018
 * Modul simulace vlaku
 * @Author HanakJiri, xhanak33
 * @Author MitasMatej, xmitas02
 * @file sim_train.cpp
 */

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "model.h"


Model::Model(Database *database) {

    Model::database = database;

    sections.emplace_back();
    sections.emplace_back();

    /// Vytvori simulacni objekty stanic
    for (auto const &station : database->getStations()) {
        stations.emplace_back(new Store(station->name.c_str(), station->rails));
        usage.emplace(station->station_id, std::set<Train_id_t > {});
    }

    /// Simulacni objekty tratovych useku
    for (auto const &section : database->getSections().at(direction_to_brno)) {
        buffer.emplace_back((char *) malloc(sizeof(char) * buffer_size));
        sprintf(buffer.back(), "Usek %s - %s, smer %s",
                database->getStations().at(section->station_id_from)->name.c_str(),
                database->getStations().at(section->station_id_to)->name.c_str(), "Brno");
        sections.at(direction_to_brno).emplace_back(new Section(buffer.back(), section->speed));
    }

    if (double_track) {
        for (auto const &section : database->getSections().at(direction_to_prerov)) {
            buffer.emplace_back((char *) malloc(sizeof(char) * buffer_size));
            sprintf(buffer.back(), "Usek %s - %s, smer %s",
                    database->getStations().at(section->station_id_from)->name.c_str(),
                    database->getStations().at(section->station_id_to)->name.c_str(), "Prerov");
            sections.at(direction_to_prerov).emplace_back(new Section(buffer.back(), section->speed));
        }
    }

    /// Simulacni modely vlaku
    for (auto const &train : database->getTrains()) {
        (new Train(train.second->train_id, train.second->priority))->Activate();
    }

    histograms = {new Histogram("Zpozdeni na prijezdu osobniho vlaku", histogram_min, histogram_step, histogram_count),
                  new Histogram("Zpozdeni na odjezdu osobniho vlaku", histogram_min, histogram_step, histogram_count),
                  new Histogram("Zpozdeni na prijezdu spesneho vlaku", histogram_min, histogram_step, histogram_count),
                  new Histogram("Zpozdeni na odjezdu spesneho", histogram_min, histogram_step, histogram_count),
                  new Histogram("Zpozdeni na prijezdu rychliku", histogram_min, histogram_step, histogram_count),
                  new Histogram("Zpozdeni na odjezdu rychliku", histogram_min, histogram_step, histogram_count),
                  new Histogram("Zpozdeni na prijezdu", histogram_min, histogram_step, histogram_count),
                  new Histogram("Zpozdeni na odjezdu", histogram_min, histogram_step, histogram_count)};

}

Model::~Model() {
    delete database;

    for (auto const &name: buffer) {
        free(name);
    }

    for (auto const &station: stations) {
        delete station;
    }

    for (auto const &histogram: histograms) {
        delete histogram;
    }

    for (auto const &direction : sections) {
        for (auto const &section : direction) {
            delete section;
        }
    }
}

void Model::Print() {

    /// Statistiky stanic
    for (auto const &station: stations) {
        station->Output();
    }

    /// Obecne statistiky zpozdeni na prijezdu a odjezdu
    for (auto const &histogram : histograms) {
        histogram->Output();
    }

    /// Statistiky tratovych useku
    for (auto const &direction: sections) {
        for (auto const &section: direction) {
            section->Output();
        }
    }

    /// Interni statistiky
    SIMLIB_statistics.Output();
}


void Train::Behavior() {

    Train_db * train = model->database->getTrains().at(this->id);
    Time_t time_diff = 0;

    Distance_t distance = 0;
    Time_t travel_time = 0;
    Speed_t speed = 0;

    model->buffer.emplace_back((char *) malloc(sizeof(char) * buffer_size));
    sprintf(model->buffer.back(), "Vlak cislo %d, prijezdove zpozdeni", train->train_id);
    Histogram histogram_arrival_delay(model->buffer.back(), histogram_min, histogram_step, histogram_count);

    model->buffer.emplace_back((char *) malloc(sizeof(char) * buffer_size));
    sprintf(model->buffer.back(), "Vlak cislo %d, odjezdove zpozdeni", train->train_id);
    Histogram histogram_departure_delay(model->buffer.back(), histogram_min, histogram_step, histogram_count);

    /// Vstup do systemu
    dbg_msg("Vlak %d vstupuje do systemu", train->train_id);
    Wait(SECONDS(train->stops.front()->arrival_time));
    dbg_msg("Vlak %d vstoupil do systemu", train->train_id);

    for (auto const &stop: train->stops) {

        if (stop != train->stops.front()) {
            /// Prijezd do stanice
            travel_time = model->compute_time(stop->stops_in, speed, distance, model->compute_acceleration(
                    model->database->getLocomotives().at(train->locomotive_type), train->weight, speed));
            dbg_msg("Vlak %d prijizdi do stanice %s, cas %.0f", train->train_id,
                    model->database->getStations().at(stop->station_id)->name.c_str(), travel_time);
            Wait(travel_time);
        }

        /// Zabrani stanicni koleje
        model->stations.at(stop->station_id)->Enter(this, train_size);

        /// Uvolneni useku
        if (stop != train->stops.front()) {
            if (train->direction == direction_to_prerov) {
                model->sections.at(double_track).at(stop->station_id - 1)->Release(this);
            } else {
                model->sections.at(direction_to_brno).at(stop->station_id)->Release(this);
            }
        }
        time_diff = Time - SECONDS(stop->arrival_time);

        if (train->train_id != 1000) {
            histogram_arrival_delay(time_diff > 0 ? time_diff : 0);
            model->histograms.at((train->priority) * 2)->operator()(time_diff > 0 ? time_diff : 0);
            model->histograms.at(6)->operator()(time_diff > 0 ? time_diff : 0);
        }

        dbg_msg("Vlak %d vjel do stanice %s", train->train_id,
                model->database->getStations().at(stop->station_id)->name.c_str());

        model->usage.at(stop->station_id).emplace(this->id);
        dbg_msg("Vlaky ve stanici %s: ", model->database->getStations().at(stop->station_id)->name.c_str());
        for (auto const &t : model->usage.at(stop->station_id)) {
            dbg_msg("\t%d", t);
        }

        if (stop->stops_in && stop != train->stops.front()) {
            dbg_msg("Vlak %d ceka na cestujici", train->train_id);
            Wait(stop_time[train->priority]);
        }

        if (stop != train->stops.back()) {

            if (stop->stops_in) {
                /// Cekani na pravidelny odjezd
                time_diff = SECONDS(stop->departure_time) - Time;
                if (time_diff > 0) {
                    dbg_msg("Vlak %d ceka na pravidelny odjezd", train->train_id);
                    Wait(time_diff);
                }
            }

            /// Zabrani useku
            /// Tratova rychlost
            /// Draha (do poloviny vzdalenosti) [m]
            if (train->direction == direction_to_prerov) {
                model->sections.at(double_track).at(stop->station_id)->Seize(this);

                speed = model->sections.at(double_track).at(stop->station_id)->max_speed;

                distance = abs(model->database->getStations().at(stop->station_id)->position -
                               model->database->getStations().at(stop->station_id + 1)->position) * 1000 / 2;

            } else {
                model->sections.at(direction_to_brno).at(stop->station_id - 1)->Seize(this);

                speed = model->sections.at(direction_to_brno).at(stop->station_id - 1)->max_speed;

                distance = abs(model->database->getStations().at(stop->station_id)->position -
                               model->database->getStations().at(stop->station_id - 1)->position) * 1000 / 2;
            }

            /// Uvolneni stanicni koleje
            model->stations.at(stop->station_id)->Leave(train_size);
            model->usage.at(stop->station_id).erase(this->id);

            time_diff = Time - SECONDS(stop->departure_time);

            if (train->train_id != 1000) {
                histogram_departure_delay(time_diff > 0 ? time_diff : 0);
                model->histograms.at((train->priority) * 2 + 1)->operator()(time_diff > 0 ? time_diff : 0);
                model->histograms.at(7)->operator()(time_diff > 0 ? time_diff : 0);
            }
            dbg_msg("Vlak %d vyjizdi ze stanice %s", this->id,
                    model->database->getStations().at(stop->station_id)->name.c_str())

            /// Rychlost
            speed = (model->database->getLocomotives().at(
                    train->locomotive_type)->speed > speed) ? speed
                                                            : model->database->getLocomotives().at(
                            train->locomotive_type)->speed;

            travel_time = model->compute_time(stop->stops_in, speed, distance, model->compute_acceleration(
                    model->database->getLocomotives().at(train->locomotive_type), train->weight, speed));

            dbg_msg("Vlak %d je na ceste do stanice %s, cas %.0f", train->train_id,
                    train->direction ? model->database->getStations().at(stop->station_id + 1)->name.c_str() :
                    model->database->getStations().at(stop->station_id - 1)->name.c_str(), travel_time);
            Wait(travel_time);
        } else {
            /// Uvolneni stanicni koleje
            model->stations.at(stop->station_id)->Leave(train_size);
            model->usage.at(stop->station_id).erase(this->id);
        }
    }

    dbg_msg("Vlak %d opousti system", train->train_id);

    histogram_arrival_delay.Output();
    histogram_departure_delay.Output();

    this->Passivate();
}


double Model::compute_time(int stops_in, Speed_t speed, Distance_t distance, double acceleration) {

    Time_t travel_time = 0;
    Distance_t acc_distance;

    if (stops_in) {
        /// Zrychleni (zpomaleni)
        /// t = (v * 1000) / (a * 60 * 60) [s]
        travel_time += ((speed * 1000) / (acceleration * 60 * 60));

        /// s = 0.5 * a * t^2
        acc_distance = 0.5 * acceleration * pow(travel_time, 2);
    } else {
        acc_distance = 0;
    }

    /// Na ceste
    /// t = s / (v * 1000) [h] = s / (v * 1000) * 60 * 60 [s]
    travel_time += ((distance - acc_distance) / (speed * 1000)) * 60 * 60;

    return travel_time;
}

double Model::compute_acceleration(Locomotive_db *locomotive, Weight_t weight, Speed_t speed) {

    double res_m = 1.35 + 0.0008 * speed + 0.00033 * pow(speed, 2);
    double power = locomotive->power - (res_m * weight * gravitation) / 1000;
    return power / (weight + locomotive->weight);

}