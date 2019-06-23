/**
 * Projekt ICP 2018
 * Implementace blokoveho editoru
 * @brief Modul spravu komponent
 * @author HanakJiri, xhanak33
 * @file Component.cpp
 */

#include "Component.h"


Component::Component(int ID, Position *position) {
    Component::component_id = ID;
    Component::position = position;
}


Component::~Component() {
    for (auto const &port : Component::input_ports) {
        for (auto const &pin : port.second) {
            delete pin.second;
        }
    }

    for (auto const &port : Component::output_ports) {
        for (auto const &pin : port.second) {
            delete pin.second;
        }
    }
}


int Component::get_id() {
    return Component::component_id;
}


void Component::add_in_port(std::string &name) {
    auto port = input_ports.find(name);

    if (port != input_ports.end()) {
        throw "Port is already exists!";
    }

    input_ports.emplace(name, std::map<std::string, Pin_in *> {});
}


void Component::add_out_port(std::string &name) {
    auto port = Component::output_ports.find(name);

    if (port != Component::output_ports.end()) {
        throw "Port is already exists!";
    }

    Component::output_ports.emplace(name, std::map<std::string, Pin_out *> {});
}


void Component::rm_in_port(std::string name) {
    auto port = input_ports.find(name);

    if (port == input_ports.end()) {
        throw "Port does not exist!";
    }

    input_ports.erase(name);
}


void Component::rm_out_port(std::string name) {
    auto port = Component::output_ports.find(name);

    if (port == Component::output_ports.end()) {
        throw "Port does not exist!";
    }

    Component::output_ports.erase(name);
}


Pin_in * Component::add_in_pin(std::string port_name, std::string pin_name, int type) {
    auto port = input_ports.find(port_name);

    if (port == input_ports.end()) {
        throw "Port does not exist!";
    }

    auto *pin = new Pin_in(this, port_name, pin_name, type);
    input_ports[port_name].emplace(pin_name, pin);
    return pin;
}


Pin_out * Component::add_out_pin(std::string port_name, std::string pin_name, int type) {
    auto port = Component::output_ports.find(port_name);

    if (port == Component::output_ports.end()) {
        throw "Port does not exist!";
    }

    auto *pin = new Pin_out(this, port_name, pin_name, type);
    Component::output_ports[port_name].emplace(pin_name, pin);
    return pin;
}


void Component::rm_in_pin(std::string &port_name, std::string &pin_name) {
    auto port = input_ports.find(port_name);

    if (port == input_ports.end()) {
        throw "Port does not exist!";
    }

    delete Component::input_ports[port_name][pin_name];
    Component::input_ports[port_name].erase(pin_name);
}


void Component::rm_out_pin(std::string &port_name, std::string &pin_name) {
    auto port = Component::output_ports.find(port_name);

    if (port == Component::output_ports.end()) {
        throw "Port does not exist!";
    }

    delete Component::input_ports[port_name][pin_name];
    Component::output_ports[port_name].erase(pin_name);
}


Pin_in* Component::get_pin_in(std::string &port_name, std::string &pin_name) {
    auto port = Component::input_ports.find(port_name);

    if (port == Component::input_ports.end()) {
        throw "Port does not exist!";
    }

    return Component::input_ports[port_name][pin_name];
}

Pin_out* Component::get_pin_out(std::string &port_name, std::string &pin_name) {
    auto port = Component::output_ports.find(port_name);

    if (port == Component::output_ports.end()) {
        throw "Port does not exist!";
    }

    return Component::output_ports[port_name][pin_name];
}


int Component::eval() {
    for(auto const& port : Component::input_ports) {
        for(auto const& pin : port.second) {
            if (pin.second->get_status() == pin_undefined) {
                // dodej hodnoty
                return eval_need_information;
            }

            if (pin.second->get_status() != pin_actual) {
                // cekame
                Component::state = component_waiting;
                return eval_wait;
            }
        }
    }

    for(auto const& port : Component::output_ports) {
        for(auto const& pin : port.second) {
            pin.second->eval();
        }
    }

    Component::state = component_done;

    return eval_ok;
}


void Component::set_position(Position *position) {
    delete Component::position;
    Component::position = position;
}


Position *Component::get_position() {
    return Component::position;
}

Ports_in & Component::get_ports_in() {
    return Component::input_ports;
}

Ports_out & Component::get_ports_out() {
    return Component::output_ports;
}

int Component::get_state() {
    return Component::state;
}