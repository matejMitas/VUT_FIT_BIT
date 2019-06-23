/**
 * Projekt ICP 2018
 * Implementace blokoveho editoru
 * @brief Modul pro spravu pinu
 * @author HanakJiri, xhanak33
 * @file Pin.cpp
 */

#include "Pin.h"

Pin::Pin(Component *parent, std::string &port_name, std::string &pin_name, int type) {

    Pin::parent = parent;
    Pin::port_name = port_name;
    Pin::pin_name = pin_name;
    Pin::type = type;
    Pin::value = 0;
    Pin::state = pin_undefined;
}


int Pin::get_status() {
    return Pin::state;
}


double Pin::get_value() {
    return Pin::value;
}


void Pin::set_value(double value){
    Pin::value = value;
    Pin::state = pin_actual;
}


std::string& Pin::get_pin_name() {
    return Pin::pin_name;
}


std::string& Pin::get_port_name() {
    return Pin::port_name;
}


int Pin::get_parent_id() {
    return Pin::parent->get_id();
}


void Pin::reset() {
    Pin::state = pin_undefined;
}


void Pin::connect() {
    Pin::state = pin_connected;
}


void Pin::unconnect() {
    Pin::state = pin_undefined;
}


int Pin::get_type() {
    return Pin::type;
}


void Pin::set_type(int type) {
    Pin::type = type;
}


Pin_in::Pin_in(Component *parent, std::string &port_name, std::string &pin_name, int type) :
        Pin(parent, port_name, pin_name, type) {}


void Pin_in::set_wire(Wire *wire) {
    Pin_in::wire = wire;
    Pin::connect();
}


void Pin_in::rm_wire() {
    Pin_in::wire = nullptr;
    Pin::unconnect();
}


Wire* Pin_in::get_wire() {
    return Pin_in::wire;
}


Pin_out::Pin_out(Component *parent, std::string &port_name, std::string &pin_name, int type) :
        Pin(parent, port_name, pin_name, type) {}


Pin_out::~Pin_out() {
    for (auto const &wire : Pin_out::out_wires) {
        delete wire;
    }
}


void Pin_out::eval() {
    std::vector<double> vec;

    for (auto ops : Pin_out::operands) {
        vec.emplace_back(ops->get_value());

        if (this->get_type() != ops->get_type()) {
            throw "Pins are not compatible";
        }
    }

    Pin_out::set_value(Pin_out::op->eval(vec));

    for (auto const& wire : Pin_out::out_wires) {
        wire->bubble_value();
    }
}


void Pin_out::set_function(std::vector<Pin_in *> operands, std::string &op) {
    for (auto const &opers : operands) {
        Pin_out::operands.emplace_back(opers);
    }

    for (auto const &ops : operators.get_operations()) {
        if (ops->repr() == op) {
            Pin_out::op = ops;
        }
    }

    if (Pin_out::op == nullptr) {
        throw "Unknown operations";
    }
}


std::string Pin_out::get_function() {
    return Pin_out::op->repr();
}


std::vector<Pin_in *> Pin_out::get_operands() {
    return Pin_out::operands;
}


Wire * Pin_out::add_wire(Pin_in *to, std::vector<Position *> &edges) {

    if (this->get_type() != to->get_type()) {
        throw "Pins are not compatible";
    }

    auto *wire = new Wire(this, to, edges);
    Pin_out::out_wires.emplace_back(wire);

    this->connect();
    to->set_wire(wire);
    return wire;
}


void Pin_out::rm_wire(Pin_in * pin_in) {

    Wire *wire = nullptr;
    for (auto const &w : Pin_out::out_wires) {
        if (w->get_pin_in() == pin_in) {
            wire = w;
            break;
        }
    }

    if (wire == nullptr) {
        throw "Unknown wire!";
    }

    Pin_out::out_wires.remove(wire);

    wire->get_pin_in()->rm_wire();
    this->unconnect();
    delete wire;
}


Wire* Pin_out::get_wire(Pin_in *pin_in) {

    Wire *wire = nullptr;
    for (auto const &w : Pin_out::out_wires) {
        if (w->get_pin_in() == pin_in) {
            wire = w;
            break;
        }
    }

    if (wire == nullptr) {
        throw "Unknown wire!";
    }

    return wire;
}

std::list<Wire *> Pin_out::get_wires() {
    return Pin_out::out_wires;
}


