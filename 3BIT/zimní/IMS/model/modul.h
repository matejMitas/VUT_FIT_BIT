/**
 * Projekt IMS 2018
 * Modul pro obecne nastaveni
 * @Author HanakJiri, xhanak33
 * @Author MitasMatej, xmitas02
 * @file modul.h
 */

#ifndef MODEL_MODUL_H
#define MODEL_MODUL_H

#include "simlib.h"
#include <string>

extern unsigned double_track;
extern int sim_info;

#define dbg_msg(...) if(sim_info){fprintf(stdout, "Time: %.0f\t", Time);fprintf(stdout, __VA_ARGS__);fprintf(stdout, "\n");};

#define SECONDS(min) (min)*60

const int direction_to_prerov = 1;
const int direction_to_brno = 0;

typedef std::string Carriage_t;
typedef unsigned Locomotive_type_t;
typedef unsigned Station_id_t;
typedef unsigned Train_id_t;
typedef unsigned Weight_t;
typedef unsigned Power_t;
typedef unsigned Places_t;
typedef unsigned Priority_t;
typedef double Speed_t;
typedef double Distance_t;
typedef double Time_t;

const size_t buffer_size = 128;
const int histogram_min = 0;
const int histogram_step = 1 * 60;
const int histogram_count = 10;

const size_t train_size = 1;
const double gravitation = 9.81;
const Time_t stop_time[] = {30, 60, 90};
#endif //MODEL_MODUL_H
