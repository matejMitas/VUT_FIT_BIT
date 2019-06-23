/**
 * Projekt ICP 2018
 * Implementace blokoveho editoru
 * @brief Modul pro spravu pinu
 * @author HanakJiri, xhanak33
 * @file Pin.h
 */

#ifndef ICP18_FIT_2018_PORT_H
#define ICP18_FIT_2018_PORT_H

#include <string>
#include <vector>
#include <string>
#include <list>
#include "Wire.h"
#include "Operations.h"
#include "Component.h"

/**
 * Stavy pinu
 */
enum Value_state {pin_undefined, pin_actual, pin_connected};

/**
 * Typ pinu
 */
enum Pin_type {type_digital, type_analog, type_mathematic};

static Operations operators;

class Component;
class Wire;

/**
 * Obecny pin
 */
class Pin {
private:
    std::vector<Ops *> operations;
    Component *parent;
    std::string port_name;
    std::string pin_name;
    double value;
    int type;
protected:
    /**
     * Stav pinu
     */
    int state = pin_undefined;
public:
    /**
     * Konstruktor
     * @param parent odkaz na rodicovskou komponentu
     * @param port_name jmeno portu, ke kteremu ma pin patrit
     * @param pin_name jmeno pinu
     * @param type typ pinu
     */
    Pin(Component *parent, std::string &port_name, std::string &pin_name, int type);

    /**
     * Vrati aktualni status
     * @return status
     */
    int get_status();

    /**
     * Vrati hodnotu z pinu
     * @return hodnota na pinu
     */
    double get_value();

    /**
     * Nastavi hodnotu na pin
     * @param value hodnota, kterou se ma nastavit
     */
    void set_value(double value);

    /**
     * Vrati jmeno pinu
     * @return jmeno pinu
     */
    std::string & get_pin_name();

    /**
     * Vrati jmeno portu
     * @return jmeno portu
     */
    std::string & get_port_name();

    /**
     * Vrati id componenty, ke ktere patri
     * @return id component
     */
    int get_parent_id();

    /**
     * Resetuje pin
     */
    void reset();

    /**
     * Pripoji pin
     */
    void connect();

    /**
     * Odpoji pin
     */
    void unconnect();

    /**
     * Vrati typ pinu
     * @return typ pinu
     */
    int get_type();

    /**
     * Nastavityp pinu
     * @param type
     */
    void set_type(int type);
};

/**
 * Vstupni pin
 */
class Pin_in : public Pin {
private:
    Wire *wire;
public:
    /**
     * Konstruktor
     * @param parent odkaz na rodicovskou komponentu
     * @param port_name jmeno portu, ke kteremu ma pin patrit
     * @param pin_name jmeno pinu
     * @param type typ pinu
     */
    Pin_in(Component *parent, std::string &port_name, std::string &pin_name, int type);

    /**
     * Pripoji propojku
     * @param wire
     */
    void set_wire(Wire *wire);

    /**
     * Odpoji propojku
     */
    void rm_wire();

    /**
     * Vrati odkaz na propojku
     * @return propojka
     */
    Wire * get_wire();
};

/**
 * Vystupni pin
 */
class Pin_out : public Pin {
private:
    std::list<Wire *> out_wires;
    std::vector<Pin_in *> operands;
    Ops * op = nullptr;
public:
    /**
     * Konstruktor
     * @param parent odkaz na rodicovskou komponentu
     * @param port_name jmeno portu, ke kteremu ma pin patrit
     * @param pin_name jmeno pinu
     * @param type typ pinu
     */
    Pin_out(Component *parent, std::string &port_name, std::string &pin_name, int type);
    ~Pin_out();

    /**
     * Provede vypocet
     */
    void eval();

    /**
     * Nastavi si poterbne udaje pro vypocet
     * @param operands operandy
     * @param op operator
     */
    void set_function(std::vector<Pin_in *> operands, std::string &op);

    /**
     * Vrati textovou reprezentaci operace
     * @return jmeno operace
     */
    std::string get_function();

    /**
     * Vrati operandy
     * @return vektor vstupnich pinu
     */
    std::vector<Pin_in *> get_operands();

    /**
     * Nastavi propojku k pinu
     * @param to druhy pin propojky
     * @param edges vektor pozic
     * @return ukazatel na vytvorenou propojku
     */
    Wire * add_wire(Pin_in *to, std::vector<Position *> &edges);

    /**
     * Odstrani propojku
     * @param pin_in koncovy pin
     */
    void rm_wire(Pin_in * pin_in);

    /**
     * Vrati odkaz na propojku
     * @param pin_in koncovy pin
     * @return odkaz na propojku
     */
    Wire * get_wire(Pin_in * pin_in);

    /**
     * Vrati vsechny propojky
     * @return list propojek
     */
    std::list<Wire *> get_wires();
};

#endif //ICP18_FIT_2018_PORT_H
