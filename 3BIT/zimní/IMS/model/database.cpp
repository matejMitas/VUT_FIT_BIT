/**
 * Projekt IMS 2018
 * Modul databaze dat
 * @Author HanakJiri, xhanak33
 * @Author MitasMatej, xmitas02
 * @file database.cpp
 */

#include <iostream>
#include <iomanip>
#include "database.h"
#include "model.h"


Station_db::Station_db(Station_id_t station_id, Distance_t position, unsigned int rails, const std::string &name)
        : station_id(station_id), position(position), rails(rails), name(name) {}


Section_db::Section_db(Station_id_t station_id_from, Station_id_t station_id_to, Speed_t speed) : station_id_from(
        station_id_from), station_id_to(station_id_to), speed(speed) {}


Stop_db::Stop_db(Station_id_t station_id, Time_t arrival_time, Time_t departure_time, Time_t arrival_delay,
                 Time_t departure_delay, int stops_in) : station_id(station_id), arrival_time(arrival_time),
                                                         departure_time(departure_time), arrival_delay(arrival_delay),
                                                         departure_delay(departure_delay), stops_in(stops_in) {}


Train_db::Train_db(Train_id_t train_id, Priority_t priority, unsigned int direction, Locomotive_type_t locomotive_type,
                   const std::list<Stop_db *> &stops, Weight_t weight, Places_t places) : train_id(train_id),
                                                                                          priority(priority),
                                                                                          direction(direction),
                                                                                          locomotive_type(
                                                                                                  locomotive_type),
                                                                                          stops(stops), weight(weight),
                                                                                          places(places) {}

Train_db::~Train_db() {

    for (auto const &stop : this->stops) {
        delete (stop);
    }
}


Locomotive_db::Locomotive_db(Speed_t speed, Power_t power, Weight_t weight, Places_t places) : speed(speed),
                                                                                               power(power),
                                                                                               weight(weight),
                                                                                               places(places) {}

Carriage_db::Carriage_db(const Carriage_t &carriage_id, Weight_t weight_empty, Weight_t weight_full, Speed_t speed,
                         Places_t places) : carriage_id(carriage_id), weight_empty(weight_empty),
                                            weight_full(weight_full), speed(speed), places(places) {}

Database::Database(std::string &file_name) {

    pugi::xml_document data;
    pugi::xml_parse_result result = data.load_file(file_name.c_str());

    /// Nepodarilo se otevrit soubor
    if (!result) {
        throw "Nelze otevrit soubor";
    }

    /// Zkontroluje verzi zapisu XML
    if (data.child(XML_IDENTIFICATION).attribute(XML_VERSION).as_int() != 1) {
        throw "Nevhodny XML soubor";
    }

    sections.emplace_back();
    sections.emplace_back();

    /// Vlozi lokomotivy do databaze
    for (pugi::xml_node locomotive_node: data.child(XML_IDENTIFICATION).child(XML_LOCOMOTIVES)) {
        locomotives.emplace(locomotive_node.attribute(XML_ID).as_uint(),
                            new Locomotive_db(locomotive_node.attribute(XML_MAX_SPEED).as_uint(),
                                              locomotive_node.attribute(XML_TRACTIVE_FORCE).as_uint(),
                                              locomotive_node.attribute(XML_WEIGHT).as_uint(),
                                              locomotive_node.attribute(XML_SEATS).as_uint()));
    }

    for (pugi::xml_node stop_node: data.child(XML_IDENTIFICATION).child(XML_STOPS)) {

        std::string stop_name = stop_node.attribute(XML_STATION).as_string();

        /// Vlozi stanici do databaze
        stations.push_back(
                new Station_db(stop_node.attribute(XML_ID).as_uint(), stop_node.attribute(XML_DISTANCE).as_double(),
                               stop_node.attribute(XML_RAIL_NO).as_uint(), stop_name));

        /// Vlozi useky do databaze
        if (stations.size() >= 2) {
            sections.at(direction_to_brno).emplace_back(
                    new Section_db(stations.at(stations.size() - 2)->station_id, stations.back()->station_id,
                                   stop_node.attribute(XML_SPEED).as_uint()));

            if (double_track) {
                sections.at(direction_to_prerov).emplace_back(
                        new Section_db(stations.at(stations.size() - 2)->station_id, stations.back()->station_id,
                                       stop_node.attribute(XML_SPEED).as_uint()));
            }
        }
    }

    /// Vlozi vagony do databaze
    for (pugi::xml_node carriage_node: data.child(XML_IDENTIFICATION).child(XML_CARRIAGES)) {

        std::string type = carriage_node.attribute(XML_ID).as_string();
        carriages.emplace(type, new Carriage_db(type,
                                              carriage_node.attribute(XML_WEIGHT_EMPTY).as_uint(),
                                              carriage_node.attribute(XML_WEIGHT_FULL).as_uint(),
                                              carriage_node.attribute(XML_MAX_SPEED).as_uint(),
                                              carriage_node.attribute(XML_SEATS).as_uint()));
    }

    std::list<Stop_db *> stops;
    Weight_t weight;
    Places_t places;

    /// Vlozi vlaky do databaze
    for (pugi::xml_node train_node: data.child(XML_IDENTIFICATION).child(XML_TRAINS)) {

        stops.clear();
        weight = getLocomotives().at(train_node.attribute(XML_LOCOMOTIVE).as_uint())->weight;
        places = getLocomotives().at(train_node.attribute(XML_LOCOMOTIVE).as_uint())->places;

        /// Nacte trasu vlaku
        for (pugi::xml_node stop_node: train_node.child(XML_STOPS)) {

            stops.emplace_back(
                    new Stop_db(stop_node.attribute(XML_ID).as_uint(),
                                stop_node.child(XML_ARRIVAL).attribute(XML_TIME).as_uint(),
                                stop_node.child(XML_DEPARTURE).attribute(XML_TIME).as_uint(),
                                stop_node.child(XML_ARRIVAL).attribute(XML_DELAY).as_uint(),
                                stop_node.child(XML_DEPARTURE).attribute(XML_DELAY).as_uint(),
                                stop_node.attribute(XML_STOPS_IN).as_uint()));
        }

        for (pugi::xml_node carriage: train_node.child(XML_CARRIAGES)) {
            weight += carriages.at(carriage.attribute(XML_ID).as_string())->weight_full;
            places += carriages.at(carriage.attribute(XML_ID).as_string())->places;
        }

        trains.emplace(train_node.attribute(XML_ID).as_uint(), new Train_db(train_node.attribute(XML_ID).as_uint(),
                                                                        train_node.attribute(XML_PRIORITY).as_uint(),
                                                                        train_node.attribute(XML_DIRECTION).as_uint(),
                                                                        train_node.attribute(XML_LOCOMOTIVE).as_uint(),
                                                                        stops, weight, places));
    }
}

Database::~Database() {
    for (auto const &station : stations) {
        delete (station);
    }

    for (auto const &direction : sections) {
        for (auto const &section : direction) {
            delete section;
        }
    }

    for (auto const &train : trains) {
        delete (train.second);
    }

    for (auto const &carriage : carriages) {
        delete (carriage.second);
    }

    for (auto const &locomotive : locomotives) {
        delete (locomotive.second);
    }
}

void Database::Print() {
    std::cout << "Lokomotivy" << std::endl;
    for (auto const &locomotive : getLocomotives()) {
        std::cout << "\tNo: " << locomotive.first << "\trychlost " << locomotive.second->speed << "\tvaha: "
                  << locomotive.second->weight << "\tsila: " << locomotive.second->power << "\tkapacita: "
                  << locomotive.second->places << std::endl;
    }

    std::cout << "Vagony" << std::endl;
    for (auto const &carriage : getCarriages()) {
        std::cout << "\tTyp: " << std::setw(15) << std::left << carriage.second->carriage_id << "\trychlost "
                  << carriage.second->speed << "\tplny: "
                  << carriage.second->weight_full << "\tprazdny: " << carriage.second->weight_empty << "\tmisto: "
                  << carriage.second->places << std::endl;
    }

    std::cout << "\nStanice" << std::endl;
    for (auto const &station : getStations()) {
        std::cout << "\tjmeno: " << std::setw(30) << std::left << station->name << "id: " << station->station_id
                  << "\tkm: " << station->position << "\tkoleje: " << station->rails << std::endl;
    }

    std::cout << "\nUseky" << std::endl;
    for (auto const &section : getSections().at(direction_to_brno)) {
        std::cout << "\tfrom: " << std::setw(18) << std::left << stations[section->station_id_from]->name
                  << "\tto: " << std::setw(20) << std::left << stations[section->station_id_to]->name << "\tSpeed: "
                  << section->speed << std::endl;
    }
    for (auto const &section : getSections().at(direction_to_prerov)) {
        std::cout << "\tfrom: " << std::setw(18) << std::left << stations[section->station_id_to]->name
                  << "\tto: " << std::setw(20) << std::left << stations[section->station_id_from]->name << "\tSpeed: "
                  << section->speed << std::endl;
    }

    std::cout << "\nVlaky" << std::endl;
    for (auto const &train : getTrains()) {
        std::cout << "id: " << std::setw(6) << std::left << train.second->train_id << "smer: "
                  << ((train.second->direction) ? "Z Brna" : "Do Brna") << "\t\tlokomotiva: "
                  << train.second->locomotive_type << "\tvaha: " << train.second->weight << "\tkapacita: "
                  << train.second->places << std::endl;

        for (auto const &stop : train.second->stops) {
            std::cout << "\tname: " << std::setw(18) << std::left << stations[stop->station_id]->name
                      << "\tprijezd: " << stop->arrival_time << "\tdelay: " << stop->arrival_delay << "\todjezd: "
                      << stop->departure_time << "\tdelay: " << stop->departure_delay << "\tstop: " << stop->stops_in
                      << std::endl;
        }
    }
}

const std::map<Train_id_t, Train_db *> &Database::getTrains() const {
    return trains;
}

const std::map<Locomotive_type_t, Locomotive_db *> &Database::getLocomotives() const {
    return locomotives;
}

const std::vector<Station_db *> &Database::getStations() const {
    return stations;
}

const std::vector<std::vector<Section_db *>> &Database::getSections() const {
    return sections;
}

const std::map<Carriage_t, Carriage_db *> &Database::getCarriages() const {
    return carriages;
}
