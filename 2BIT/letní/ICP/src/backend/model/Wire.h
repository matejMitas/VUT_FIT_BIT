/**
 * Projekt ICP 2018
 * Implementace blokoveho editoru
 * @brief Modul pro spravu propojek
 * @author HanakJiri, xhanak33
 * @file Wire.h
 */

#ifndef ICP18_FIT_2018_WIRE_H
#define ICP18_FIT_2018_WIRE_H

#include <vector>
#include "Position.h"
#include "Pin.h"

class Pin_in;
class Pin_out;

/**
 * Reprezentace propojky
 */
class Wire {
private:
    std::vector<Position *> edges;
    Pin_out *from;
    Pin_in *to;
public:
    /**
     * Konstruktor na propojky
     * @param from Z ktereho pinu vede
     * @param to Do ktereho pinu vede
     * @param edges vektor Pozic kudy vede
     */
    Wire(Pin_out *from, Pin_in *to, std::vector<Position *> &edges);
    ~Wire();

    /**
     * Provede probublani hodnoty pres propojku
     */
    void bubble_value();

    /**
     * Nastavi nove umiste propojky
     * @param edges rohy propojky
     */
    void set_edges(std::vector<Position *> &edges);

    /**
     * Vrati pozice rohu propojky
     * @return rohy propojky
     */
    std::vector<Position *> get_edges();

    /**
     * Vrati vstupni pin
     * @return vstupni pin
     */
    Pin_in * get_pin_in() const;

    /**
     * Vrati vystupni pin
     * @return vystupni pin
     */
    Pin_out * get_pin_out();

    /**
     * Vymaze vsechny rohy
     */
    void clean_edges();

    /**
     * Definice operatoru porovnani
     * @param w komplementarni propojka
     * @return true pro shodu
     */
    inline bool operator==(const Wire& w){
        return this->from == w.from && this->to == w.to;
    }

};

#endif //ICP18_FIT_2018_WIRE_H
