/**
 * Projekt IMS 2018
 * Modul main
 * @Author HanakJiri, xhanak33
 * @Author MitasMatej, xmitas02
 * @file main.cpp
 */

#include <iostream>
#include <simlib.h>
#include <getopt.h>
#include "model.h"
#include "database.h"

Model * model;
unsigned double_track = 0;
int sim_info = 0;

void print_help();


int main(int argc, char *argv[]) {

    std::string file = "../data/data.xml";
    char *output = nullptr;
    int c, info = 0;
    Database * database;

    while ((c = getopt (argc, argv, "dihvf:o:")) != -1) {
        switch (c) {
            case 'd':
                double_track = 1;
                break;
            case 'v':
                sim_info = 1;
                break;
            case 'i':
                info = 1;
                break;
            case 'h':
                print_help();
                return EXIT_SUCCESS;
            case 'f':
                file = optarg;
                break;
            case 'o':
                output = optarg;
                break;
            default:
                std::cerr << "Chybne parametry!" << std::endl;
                return EXIT_FAILURE;
        }
    }

    try {
        database = new Database(file);
    } catch (const char * msg) {
        std::cerr << "Error: " << msg << "!" << std::endl;
        return EXIT_FAILURE;
    }

    Init(0, 60 * 60 * 24);
    model = new Model(database);

    if (info) {
        database->Print();
    }

    if (output == nullptr) {
        SetOutput("../sim_info.out");
    } else {
        SetOutput(output);
    }

    Print("Model zeleznicni  tratě Brno - Prerov (- Bohumin)\n");

    Run();
    model->Print();
    delete model;

    return EXIT_SUCCESS;
}

void print_help() {
    std::cout << "Napoveda pro simulacni model zeleznicni  tratě Brno - Prerov (- Bohumin)\n"
              << "-f file\tVstupni XML soubor\n"
              << "[-h]\tVytiskne napovedu\n"
              << "[-v]\tPodrobny postup simulace\n"
              << "[-i]\tVytiskne data ziskana z XML souboru\n"
              << "[-d]\tModelovani trate jako dvojkolejne" << std::endl;

}
