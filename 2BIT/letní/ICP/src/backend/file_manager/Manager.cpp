/**
 * Projekt ICP 2018
 * Implementace blokoveho editoru
 * @brief Modul pro spravu souboru
 * @author HanakJiri, xhanak33
 * @file Manager.cpp
 */

#include "Manager.h"

void Manager::load_schema(Model *model, std::string &file_name) {

    // priprava dokumentu
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(file_name.c_str());

    // Nepodarilo se otevrit soubor
    if (!result) {
        throw "00 - Load file error!";
    }

    // Zkontroluje verzi zapisu XML
    if (doc.child(IDENTIFICATION).attribute(VERSION).as_int() != ACTUAL_VERSION) {
        throw "01 - Load file error!";
    }

    // Vse co mame, zahodime
    model->clean_workspace();

    // Nacte vsechny komponenty
    for (pugi::xml_node component_node: doc.child(IDENTIFICATION)) {

        // Nastavi identifikator komponenty
        if (component_node.attribute(COMPONENT_ID).as_int() == 0) {
            throw "02 - Load file error!";
        }

        Component *component = model->new_block(component_node.attribute(COMPONENT_ID).as_int(),
                                                new Position(component_node.attribute(X_POS).as_int(),
                                                             component_node.attribute(Y_POS).as_int()));

        // Nastavi vsechny vstupni skupiny portu
        for (pugi::xml_node port_node: component_node.child(IN_PORT)) {

            // Prida vstupni port
            std::string port_name = port_node.attribute(PORT_NAME).as_string();
            component->add_in_port(port_name);
            for (pugi::xml_node pin: port_node.children(PIN)) {

                // Prida piny
                std::string pin_name = pin.attribute(PIN_NAME).as_string();
                component->add_in_pin(port_name, pin_name, pin.attribute(PIN_TYPE).as_int());
            }
        }
    }

    // Projde vsechny komponenty
    for (pugi::xml_node component_node: doc.child(IDENTIFICATION)) {
        Component *component = model->get_component(component_node.attribute(COMPONENT_ID).as_int());

        // Projde vsechny vystupni skupiny portu
        for (pugi::xml_node port_node: component_node.child(OUT_PORT)) {

            // Prida vystupni port
            std::string port_name = port_node.attribute(PORT_NAME).as_string();
            component->add_out_port(port_name);
            for (pugi::xml_node pin_node: port_node.children(PIN)) {

                // Prida pin
                std::string pin_name = pin_node.attribute(PIN_NAME).as_string();
                std::string operation = pin_node.attribute(OPERATION).as_string();
                Pin_out * pin_out = component->add_out_pin(port_name, pin_name, pin_node.attribute(PIN_TYPE).as_int());

                // Prida operandy
                std::vector<Pin_in *> operands;
                for (auto const &operand_node : pin_node.children(OPERAND)) {
                    std::string op_port_name = operand_node.attribute(PORT_NAME).as_string();
                    std::string op_pin_name = operand_node.attribute(PIN_NAME).as_string();

                    operands.emplace_back(component->get_pin_in(op_port_name, op_pin_name));
                }

                // Nastavi funkci
                pin_out->set_function(operands, operation);

                for (auto const &wire_node : pin_node.children(WIRE)) {
                    int wire_to_component_id = wire_node.attribute(COMPONENT_ID).as_int();
                    std::string wire_port_name = wire_node.attribute(PORT_NAME).as_string();
                    std::string wire_pin_name = wire_node.attribute(PIN_NAME).as_string();

                    std::vector<Position *> edges = {};

                    for (auto const &edge_node : wire_node.children(EDGE)) {
                        int x_pos = edge_node.attribute(X_POS).as_int();
                        int y_pos = edge_node.attribute(Y_POS).as_int();
                        edges.emplace_back(new Position(x_pos, y_pos));
                    }

                    pin_out->add_wire(model->get_component(wire_to_component_id)->
                            get_pin_in(wire_port_name, wire_pin_name), edges);
                }
            }
        }
    }


}

void Manager::save_schema(Model *model, std::string &file_name) {

    pugi::xml_document doc;
    pugi::xml_node root_node = doc.append_child(IDENTIFICATION);
    root_node.append_attribute(VERSION) = ACTUAL_VERSION;

    // Projde vsechny komponenty
    for (auto const &component : model->get_components()) {
        pugi::xml_node component_node = root_node.append_child(COMPONENT);
        component_node.append_attribute(COMPONENT_ID) = component->get_id();
        component_node.append_attribute(X_POS) = component->get_position()->get_x();
        component_node.append_attribute(Y_POS) = component->get_position()->get_y();

        // Projde vsechny vstupni porty komponenty
        pugi::xml_node in_ports_node = component_node.append_child(IN_PORT);
        for (auto const &in_port : component->get_ports_in()) {
            pugi::xml_node port_node = in_ports_node.append_child(PORT);
            port_node.append_attribute(PORT_NAME) = in_port.first.c_str();

            // Projde vsechny piny v portu
            for (auto const &pin : in_port.second) {
                pugi::xml_node pin_node = port_node.append_child(PIN);
                pin_node.append_attribute(PIN_NAME) = pin.first.c_str();
                pin_node.append_attribute(PIN_TYPE) = pin.second->get_type();
            }
        }

        // Projde vsechny vystupni porty komponenty
        pugi::xml_node out_ports_node = component_node.append_child(OUT_PORT);
        for (auto const &out_port : component->get_ports_out()) {
            pugi::xml_node port_node = out_ports_node.append_child(PORT);
            port_node.append_attribute(PORT_NAME) = out_port.first.c_str();

            // Projde vsechny piny v portu
            for (auto const &pin : out_port.second) {
                pugi::xml_node pin_node = port_node.append_child(PIN);
                pin_node.append_attribute(PIN_NAME) = pin.first.c_str();
                pin_node.append_attribute(PIN_TYPE) = pin.second->get_type();
                pin_node.append_attribute(OPERATION) = pin.second->get_function().c_str();

                // Projde vsechny operandy
                for (auto const &operand : pin.second->get_operands()) {
                    pugi::xml_node operand_node = pin_node.append_child(OPERAND);
                    operand_node.append_attribute(PORT_NAME) = operand->get_port_name().c_str();
                    operand_node.append_attribute(PIN_NAME) = operand->get_pin_name().c_str();
                }

                // Projde vsechny propojky
                for (auto const &wire : pin.second->get_wires()) {
                    pugi::xml_node wire_node = pin_node.append_child(WIRE);

                    wire_node.append_attribute(COMPONENT_ID) = wire->get_pin_in()->get_parent_id();
                    wire_node.append_attribute(PORT_NAME) = wire->get_pin_in()->get_port_name().c_str();
                    wire_node.append_attribute(PIN_NAME) = wire->get_pin_in()->get_pin_name().c_str();

                    // Projde vsechny rohy propojky
                    for (auto const &edge : wire->get_edges()) {
                        pugi::xml_node edge_node = wire_node.append_child(EDGE);
                        edge_node.append_attribute(X_POS) = edge->get_x();
                        edge_node.append_attribute(Y_POS) = edge->get_y();
                    }
                }
            }
        }
    }

    // Zapis dat do souboru
    if (!doc.save_file(file_name.c_str())) {
        throw "Save file error!";
    }
}
