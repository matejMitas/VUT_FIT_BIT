/**
 * Projekt ICP 2018
 * Implementace blokoveho editoru
 * @brief Modul zastitujici spravu dat
 * @author HanakJiri, xhanak33
 * @file Model.cpp
 */

#include "Model.h"


Model::Model() {
    operators = Operations();
}


Model::~Model() {
    Model::clean_workspace();
    operators.clear();
}


bool Model::get_component_to_eval() {
    for (auto const &component : Model::components) {
        if (component->get_state() == component_undefined) {
            Model::eval_stack.push_front(component);
            return true;
        }
    }
    return false;
}


int Model::new_block(Position *position) {
    auto *component = new Component(Model::Component_id_counter++, position);
    Model::components.emplace_back(component);
    return component->get_id();
}


Component* Model::new_block(int component_id, Position *position) {
    auto *component = new Component(component_id, position);
    Model::components.emplace_back(component);
    return component;
}


void Model::del_block(int component_id) {
    Component * component = Model::get_component(component_id);

    Model::components.remove(component);
    delete component;
}


Component* Model::get_component(int component_id) {
    for (auto const &component : Model::get_components()) {
        if (component->get_id() == component_id) {
            return component;
        }
    }
    return nullptr;
}


std::list<Component *> Model::get_components() {
    return Model::components;
}


void Model::clean_workspace() {
    for (auto const &component : Model::components) {
        delete component;
    }
    Model::components.clear();
}


int Model::eval_one() {
    if (Model::eval_stack.empty()) {
        if (Model::components.empty()) {
            // Neni co pocitat
            return EXIT_SUCCESS;
        }
        if (!Model::get_component_to_eval()) {
            return EXIT_SUCCESS;
        }
    }
    int actual_state;

    while ((actual_state = (*Model::eval_stack.begin())->eval()) == eval_wait) {

        // Projdu zavislosti
        for (auto const &ports : Model::eval_stack.front()->get_ports_in()) {
            for (auto const &pin : ports.second) {

                if (Model::get_component(
                        pin.second->get_wire()->get_pin_out()->get_parent_id())->get_state() == component_undefined) {
                    Model::eval_stack.push_front(
                            Model::get_component(pin.second->get_wire()->get_pin_out()->get_parent_id()));

                }
            }
        }
    }

    Component * c = (*Model::eval_stack.begin());

    // odstrani vypocitany blok a prida dalsi nevypocitany
    if (actual_state == eval_ok) {
        Model::eval_stack.pop_front();
        return c->get_id();
    }

    return -c->get_id();
}


void Model::reset() {
    Model::eval_stack.clear();
    for (auto const &component : Model::components) {

        // Resetuji vstupni porty
        for (auto const &port : component->get_ports_in()) {
            for (auto const &pin : port.second) {
                pin.second->reset();
            }
        }

        // Resetuji vystupni porty
        for (auto const &port : component->get_ports_out()) {
            for (auto const &pin : port.second) {
                pin.second->reset();
            }
        }
    }
}


void Model::check_integrity() {
    std::list<int> visited;
    int actual;

    for (auto const &component : Model::components) {
        visited.clear();
        visited.emplace_front(component->get_id());
        while (!visited.empty()) {
            for (auto const &port : Model::get_component(visited.front())->get_ports_in()) {
                for (auto const &pin : port.second) {
                    if (pin.second->get_wire() == nullptr) {
                        continue;
                    }

                    if ((actual = pin.second->get_wire()->get_pin_out()->get_parent_id()) != component->get_id()) {
                        visited.emplace_back(actual);
                    } else {
                        throw "Cycle detected!";
                    }
                }
            }
            visited.pop_front();
        }
    }
}
