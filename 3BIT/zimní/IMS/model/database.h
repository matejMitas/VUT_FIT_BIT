/**
 * Projekt IMS 2018
 * Modul databaze dat
 * @Author HanakJiri, xhanak33
 * @Author MitasMatej, xmitas02
 * @file database.h
 */

#ifndef MODEL_DATABASE_H
#define MODEL_DATABASE_H

#include <map>
#include <list>
#include <simlib.h>
#include <vector>
#include "modul.h"
#include "libs/pugixml-1.9/pugixml.hpp"

#define XML_IDENTIFICATION "ims_data"
#define XML_VERSION "version"
#define XML_STOPS "stops"
#define XML_TRAINS "trains"
#define XML_PRIORITY "priority"
#define XML_DIRECTION "direction"
#define XML_ARRIVAL "arrival"
#define XML_DEPARTURE "departure"
#define XML_TIME "time"
#define XML_DELAY "delay"
#define XML_LOCOMOTIVES "locomotives"
#define XML_LOCOMOTIVE "locomotive"
#define XML_MAX_SPEED "maxSpeed"
#define XML_RAIL_NO "railNo"
#define XML_STOPS_IN "stopsIn"
#define XML_SPEED "speed"
#define XML_CARRIAGES "carriges"
#define XML_WEIGHT_FULL "weightFull"
#define XML_WEIGHT_EMPTY "weightEmpty"
#define XML_STATION "station"
#define XML_ID "id"
#define XML_DISTANCE "distance"
#define XML_TRACTIVE_FORCE "tractiveForce"
#define XML_WEIGHT "weight"
#define XML_SEATS "seats"


class Station_db {
public:
    Station_id_t station_id;
    Distance_t position;
    unsigned rails;
    std::string name;

    Station_db(Station_id_t station_id, Distance_t position, unsigned int rails, const std::string &name);
};

class Section_db {
public:
    Station_id_t station_id_from;
    Station_id_t station_id_to;
    Speed_t speed;

    Section_db(Station_id_t station_id_from, Station_id_t station_id_to, Speed_t speed);
};

class Stop_db {
public:
    Station_id_t station_id;
    Time_t arrival_time;
    Time_t departure_time;
    Time_t arrival_delay;
    Time_t departure_delay;
    int stops_in;

    Stop_db(Station_id_t station_id, Time_t arrival_time, Time_t departure_time, Time_t arrival_delay,
            Time_t departure_delay, int stops_in);
};

class Train_db {
public:
    Train_id_t train_id;
    Priority_t priority;
    unsigned direction;
    Locomotive_type_t locomotive_type;
    std::list<Stop_db *> stops;
    Weight_t weight;
    Places_t places;

    Train_db(Train_id_t train_id, Priority_t priority, unsigned int direction, Locomotive_type_t locomotive_type,
             const std::list<Stop_db *> &stops, Weight_t weight, Places_t places);

    ~Train_db();
};

class Locomotive_db {
public:
    Speed_t speed;
    Power_t power;
    Weight_t weight;
    Places_t places;

    Locomotive_db(Speed_t speed, Power_t power, Weight_t weight, Places_t places);
};

class Carriage_db {
public:
    Carriage_t carriage_id;
    Weight_t weight_empty;
    Weight_t weight_full;
    Speed_t speed;
    Places_t places;

    Carriage_db(const Carriage_t &carriage_id, Weight_t weight_empty, Weight_t weight_full, Speed_t speed,
                Places_t places);
};


class Database {
private:
    std::map<Train_id_t, Train_db *> trains;
    std::map<Locomotive_type_t, Locomotive_db *> locomotives;
    std::vector<Station_db *> stations;
    std::vector<std::vector<Section_db *>> sections;
    std::map<Carriage_t, Carriage_db *> carriages;

public:

    explicit Database(std::string &file_name);
    ~Database();
    void Print();

    const std::map<Train_id_t, Train_db *> &getTrains() const;
    const std::map<Locomotive_type_t, Locomotive_db *> &getLocomotives() const;
    const std::vector<Station_db *> &getStations() const;
    const std::vector<std::vector<Section_db *>> &getSections() const;
    const std::map<Carriage_t, Carriage_db *> &getCarriages() const;
};


#endif //MODEL_DATABASE_H
