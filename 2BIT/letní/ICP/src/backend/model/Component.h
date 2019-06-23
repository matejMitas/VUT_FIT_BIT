/**
 * Projekt ICP 2018
 * Implementace blokoveho editoru
 * @brief Modul spravu komponent
 * @author HanakJiri, xhanak33
 * @file Component.h
 */

#ifndef ICP18_FIT_2018_COMPONENT_H
#define ICP18_FIT_2018_COMPONENT_H

#include <vector>
#include <string>
#include <map>
#include <list>

#include "Pin.h"
#include "Position.h"

class Pin_in;
class Pin_out;

/**
 * Stavy vypoctu
 */
enum Equal_state {eval_ok, eval_wait, eval_need_information};

/**
 * Stavy komponenty
 */
enum Component_state {component_waiting, component_undefined, component_done};

/**
 * Vstupni porty
 */
typedef std::map<std::string, std::map<std::string, Pin_in *>> Ports_in;

/**
 * Vystupni porty
 */
typedef std::map<std::string, std::map<std::string, Pin_out *>> Ports_out;

/**
 * Komponenta je bez definovane pozice, dokud neni "polozena"
 */
class Component {
private:
    int component_id;
    Component_state state = component_undefined;
    Ports_in input_ports;
    Ports_out output_ports;
    Position *position;
public:

    /**
     * Konstruktor komponenty
     * @param ID identifikace komponenty
     * @param position pozice komponenty
     */
    Component(int ID, Position *position);
    ~Component();

    /**
     * Vrati identifikaci komponenty
     * @return component_id
     */
    int get_id();

    /**
     * Prida vstupni port
     * @param port_name jmeno portu
     */
    void add_in_port(std::string &port_name);

    /**
     * Prida vystupni port
     * @param port_name jmeno portu
     */
    void add_out_port(std::string &port_name);

    /**
     * Odstrani prislusny vstupni port
     * @param port_name jmeno portu
     */
    void rm_in_port(std::string port_name);

    /**
     * Odstrani prislusny vytupni port
     * @param port_name jmeno portu
     */
    void rm_out_port(std::string port_name);

    /**
     * Prida vstupni pin do portu
     * @param port_name jemno vstupniho portu
     * @param pin_name jmeno pinu
     * @param type typ pinu
     */
    Pin_in * add_in_pin(std::string port_name, std::string pin_name, int type);

    /**
     * Prida vystupni pin do portu
     * @param port_name jemno vystupniho portu
     * @param pin_name jmeno pinu
     * @param type typ pinu
     */
    Pin_out * add_out_pin(std::string port_name, std::string pin_name, int type);

    /**
     * Odebere vstupni pin z portu
     * @param port_name jemno vstupniho portu
     * @param pin_name jmeno pinu
     */
    void rm_in_pin(std::string &port_name, std::string &pin_name);

    /**
     * Odebere vstupni pin z portu
     * @param port_name jemno vstupniho portu
     * @param pin_name jmeno pinu
     */
    void rm_out_pin(std::string &port_name, std::string &pin_name);

    /**
     * Vrati instanci pinu
     * @param port_name jmeno portu, ke kteremu pin patri
     * @param pin_name jmeno pinu
     * @return pin
     */
    Pin_in * get_pin_in(std::string &port_name, std::string &pin_name);

    /**
     * Vrati instanci pinu
     * @param port_name jmeno portu, ke kteremu pin patri
     * @param pin_name jmeno pinu
     * @return pin
     */
    Pin_out * get_pin_out(std::string &port_name, std::string &pin_name);

    /**
     * Provede vypocet nad blokem
     * @return status vypoctu
     */
    int eval();

    /**
     * Nastavi bloku umisteni
     * @param position umisteni
     */
    void set_position(Position *position);

    /**
     * Vrati umisteni bloku
     * @return umisteni
     */
    Position * get_position();

    /**
     * Vrati vstupni port
     * @return vstupni porty
     */
    Ports_in & get_ports_in();

    /**
     * Vrati vystupni porty
     * @return vystupni porty
     */
    Ports_out & get_ports_out();

    /**
     * Vrati aktualni stav komponenty
     * @return stav komponenty
     */
    int get_state();

    /**
     * Definice operatoru porovnani
     * @param c komplementarni komponenta
     * @return true pro shodu
     */
    inline bool operator==(const Component& c){
        return this->position->get_x() == c.position->get_x() && this->position->get_y() == c.position->get_y();
    }
};

#endif //ICP18_FIT_2018_COMPONENT_H
