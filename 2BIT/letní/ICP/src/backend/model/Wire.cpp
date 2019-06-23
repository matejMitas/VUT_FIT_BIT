/**
 * Projekt ICP 2018
 * Implementace blokoveho editoru
 * @brief Modul pro spravu propojek
 * @author HanakJiri, xhanak33
 * @file Wire.cpp
 */

#include "Wire.h"

Wire::Wire(Pin_out *from, Pin_in *to, std::vector<Position *> &edges) {
    Wire::from = from;
    Wire::to = to;
    for(auto const &edge : edges) {
        Wire::edges.emplace_back(edge);
    }
}

Wire::~Wire() {
    Wire::clean_edges();
}

void Wire::bubble_value() {
    Wire::to->set_value(Wire::from->get_value());
}

void Wire::set_edges(std::vector<Position *> &edges) {
    for(auto const &edge : edges) {
        Wire::edges.emplace_back(edge);
    }
}

std::vector<Position *> Wire::get_edges() {
    return Wire::edges;
}

Pin_in * Wire::get_pin_in() const {
    return Wire::to;
}

Pin_out* Wire::get_pin_out() {
    return Wire::from;
}

void Wire::clean_edges() {
    for(auto const &edge : Wire::edges) {
        delete edge;
    }
    Wire::edges.clear();
}