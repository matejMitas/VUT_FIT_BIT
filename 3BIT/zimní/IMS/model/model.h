/**
 * Projekt IMS 2018
 * Modul simulace vlaku
 * @Author HanakJiri, xhanak33
 * @Author MitasMatej, xmitas02
 * @file sim_train.h
 */

#ifndef MODEL_TRAIN_H
#define MODEL_TRAIN_H

#include <simlib.h>
#include <string>
#include <set>
#include <cmath>
#include <simlib.h>
#include "database.h"
#include "modul.h"


class Train : public Process {
private:
    Train_id_t id;

public:
    explicit Train(unsigned id, Priority_t p) : Process(p) {this->id = id;};
    void Behavior() final;
};


class Section : public Facility {
public:
    Speed_t max_speed;
    explicit Section(const char *_name, Speed_t max_speed) : Facility(_name){this->max_speed = max_speed;};
};

class Model {
public:
    std::vector<char *> buffer;
    Database * database;
    std::vector<Store *> stations;
    std::vector<std::vector<Section *>> sections;
    std::vector<Histogram *> histograms;
    std::map<Station_id_t, std::set<Train_id_t >> usage;

    explicit Model(Database * database);
    ~Model();

    double compute_time(int stops_in, Speed_t speed, Distance_t distance, double acceleration);
    double compute_acceleration(Locomotive_db *locomotive, Weight_t weight, Speed_t speed);

    void Print();

};

extern Model *model;
#endif //MODEL_TRAIN_H
