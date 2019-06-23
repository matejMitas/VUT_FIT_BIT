#include "qwrap.h"


QWrap::QWrap(QObject *parent) : QObject(parent) {
    m_id = 0;
    //qDebug() << ">Instancovana komunikacni sranda";
}

// 1 - in
// 0 - out


int QWrap::add_block() {

    return model.new_block(new Position(0, 0));
}


QVariantMap QWrap::get_Q_blocks() {
    QVariantMap blocks;

    int counter = 0;
    for (auto const &blok : model.get_components()) {
        blocks[QString::number(counter++)] = get_Q_block(blok->get_id());
    }
    return blocks;
}


QVariantMap QWrap::get_Q_block(int blockId) {
    Component * component = model.get_component(blockId);
    QVariantMap block;

    block["id"] = QVariant(component->get_id());
    block["x"] = QVariant(component->get_position()->get_x());
    block["y"] = QVariant(component->get_position()->get_y());


    // Projde vsechny vstupni porty komponenty
    QVariantMap in_ports_node;
    for (auto const &in_port : component->get_ports_in()) {
        QVariantMap port_node;
        port_node["port_name"] = in_port.first.c_str();

        // Projde vsechny piny v portu
        for (auto const &pin : in_port.second) {
            QVariantMap pin_node;
            pin_node["pin_name"] = pin.first.c_str();
            pin_node["pin_type"] = pin.second->get_type();
            pin_node["state"] = pin.second->get_status();
            pin_node["value"] = pin.second->get_value();
            port_node[pin.first.c_str()] = pin_node;
        }
        in_ports_node[in_port.first.c_str()] = port_node;
    }

    // Projde vsechny vystupni porty komponenty
    QVariantMap out_ports_node;
    for (auto const &out_port : component->get_ports_out()) {
        QVariantMap port_node;
        port_node["port_name"] = out_port.first.c_str();

        // Projde vsechny piny v portu
        for (auto const &pin : out_port.second) {
            QVariantMap pin_node;
            pin_node["pin_name"] = pin.first.c_str();
            pin_node["pin_type"] = pin.second->get_type();
            pin_node["operation"] = pin.second->get_function().c_str();
            pin_node["value"] = pin.second->get_value();
            pin_node["state"] = pin.second->get_status();

            // Projde vsechny operandy
            int operand_counter = 0;
            for (auto const &operand : pin.second->get_operands()) {
                QVariantMap operand_node;
                operand_node["port_name"] = operand->get_port_name().c_str();
                operand_node["pin_name"] = operand->get_pin_name().c_str();
                pin_node[QString::number(operand_counter++)] = operand_node;
            }

            // Projde vsechny propojky
            int wire_counter = 0;
            for (auto const &wire : pin.second->get_wires()) {
                QVariantMap wire_node;

                wire_node["component_id"] = wire->get_pin_in()->get_parent_id();
                wire_node["port_name"] = wire->get_pin_in()->get_port_name().c_str();
                wire_node["pin_name"] = wire->get_pin_in()->get_pin_name().c_str();

                // Projde vsechny rohy propojky
                int edge_counter = 0;
                for (auto const &edge : wire->get_edges()) {
                    QVariantMap edge_node;
                    edge_node["x"] = edge->get_x();
                    edge_node["y"] = edge->get_y();
                    wire_node[QString::number(edge_counter++)] = edge_node;
                }
                pin_node[QString::number(wire_counter++)] = wire_node;
            }
            port_node[pin.first.c_str()] = pin_node;
        }
        out_ports_node[out_port.first.c_str()] = port_node;
    }

    block["in_ports"] = in_ports_node;
    block["out_ports"] = out_ports_node;


    return block;
}


void QWrap::delete_block(int blockId) {
    model.del_block(blockId);
}


void QWrap::set_pos(int blockId, int x, int y) {
    model.get_component(blockId)->set_position(new Position(x, y));
}


QString QWrap::add_port(int blockId, QString portName, int portType) {
    std::string port_name = portName.INTERFACE;

    try {
        if (portType) {
            model.get_component(blockId)->add_in_port(port_name);
        } else {
            model.get_component(blockId)->add_out_port(port_name);
        }

        return "";
    } catch (const char* msg) {
        return msg;
    }
}


void QWrap::remove_port(int blockId, QString portName, int portType) {
    std::string port_name = portName.INTERFACE;

    if (portType) {
        model.get_component(blockId)->rm_in_port(port_name);
    } else {
        model.get_component(blockId)->rm_out_port(port_name);
    }
}


QString QWrap::add_pin(int blockId, QString portName, QString pinName, int portType, int pinType) {
    std::string port_name = portName.INTERFACE;
    std::string pin_name = pinName.INTERFACE;

    try {
        if (portType) {
            model.get_component(blockId)->add_in_pin(port_name, pin_name, pinType);
        } else {
            model.get_component(blockId)->add_out_pin(port_name, pin_name, pinType);
        }
        return "";
    } catch (const char* msg) {
        return msg;
    }
}


void QWrap::remove_pin(int blockId, QString portName, QString pinName, QPinType portType) {
    std::string port_name = portName.INTERFACE;
    std::string pin_name = pinName.INTERFACE;

    if (portType == In) {
        model.get_component(blockId)->rm_in_pin(port_name, pin_name);
    } else {
        model.get_component(blockId)->rm_out_pin(port_name, pin_name);
    }
}


void QWrap::set_function(int blockId, QString portName, QString pinName, QString pinOperator) {
    std::string port_name = portName.INTERFACE;
    std::string pin_name = pinName.INTERFACE;
    std::string pin_operator = pinOperator.INTERFACE;

    try {
        model.get_component(blockId)->get_pin_out(port_name, pin_name)->set_function(QWrap::pin_operands, pin_operator);
        QWrap::pin_operands.clear();
    } catch (const char *msg) {
        qDebug() << msg;
    }
}


void QWrap::add_operand_to_vector(int blockId, QString portName, QString pinName) {
    std::string port_name = portName.INTERFACE;
    std::string pin_name = pinName.INTERFACE;

    try {
        QWrap::pin_operands.emplace_back(model.get_component(blockId)->get_pin_in(port_name, pin_name));
    } catch (const char *msg) {
        qDebug() << msg;
    }
}


QString QWrap::add_wire(int blockId1, QString portName1, int portType1, QString pinName1,
                        int blockId2, QString portName2, int portType2, QString pinName2) {

    try{
        std::string port_name1 = portName1.INTERFACE;
        std::string pin_name1 = pinName1.INTERFACE;
        std::string port_name2 = portName2.INTERFACE;
        std::string pin_name2 = pinName2.INTERFACE;

        if (portType1 == 0 && portType2 == 1) {
            model.get_component(blockId2)->get_pin_out(port_name2, pin_name2)->
                    add_wire(model.get_component(blockId1)->get_pin_in(port_name1, pin_name1), QWrap::edges);
        } else if (portType1 == 1 && portType2 == 0){
            model.get_component(blockId1)->get_pin_out(port_name1, pin_name1)->
                    add_wire(model.get_component(blockId2)->get_pin_in(port_name2, pin_name2), QWrap::edges);
        } else {
            return "Bad pins!";
        }
        QWrap::edges.clear();

        model.check_integrity();
        return "";
    } catch (const char* msg) {
        return msg;
    }
}


void QWrap::remove_wire(int blockId1, QString portName1, int portType1, QString pinName1,
                        int blockId2, QString portName2, int portType2, QString pinName2) {

    std::string port_name1 = portName1.INTERFACE;
    std::string pin_name1 = pinName1.INTERFACE;
    std::string port_name2 = portName2.INTERFACE;
    std::string pin_name2 = pinName2.INTERFACE;

    if (portType1 == 0 && portType2 == 1) {
        model.get_component(blockId2)->get_pin_out(port_name2, pin_name2)->
                rm_wire(model.get_component(blockId1)->get_pin_in(port_name1, pin_name1));
    } else if (portType1 == 1 && portType2 == 0){
        model.get_component(blockId1)->get_pin_out(port_name1, pin_name1)->
                rm_wire(model.get_component(blockId2)->get_pin_in(port_name2, pin_name2));
    }
}


void QWrap::set_wire_positions(int blockId1, QString portName1, int portType1, QString pinName1,
                               int blockId2, QString portName2, int portType2, QString pinName2) {

    std::string port_name1 = portName1.INTERFACE;
    std::string pin_name1 = pinName1.INTERFACE;
    std::string port_name2 = portName2.INTERFACE;
    std::string pin_name2 = pinName2.INTERFACE;

    if (portType1 == 0 && portType2 == 1) {
        model.get_component(blockId2)->get_pin_out(port_name2, pin_name2)->
                get_wire(model.get_component(blockId1)->get_pin_in(port_name1, pin_name1))->set_edges(QWrap::edges);
    } else if (portType1 == 1 && portType2 == 0){
        model.get_component(blockId1)->get_pin_out(port_name1, pin_name1)->
                get_wire(model.get_component(blockId2)->get_pin_in(port_name2, pin_name2))->set_edges(QWrap::edges);
    }
    QWrap::edges.clear();
}


void QWrap::set_pin_type(int blockId, QString portName, QString pinName, int portType, int pinType) {
    std::string port_name = portName.INTERFACE;
    std::string pin_name = pinName.INTERFACE;

    if (portType == 0) {
        model.get_component(blockId)->get_pin_in(port_name, pin_name)->set_type(pinType);
    } else {
        model.get_component(blockId)->get_pin_out(port_name, pin_name)->set_type(pinType);
    }
}


void QWrap::set_Value(int blockId, QString portName, QString pinName, double value) {
    std::string port_name = portName.INTERFACE;
    std::string pin_name = pinName.INTERFACE;

    model.get_component(blockId)->get_pin_in(port_name, pin_name)->set_value(value);
}


void QWrap::add_position(int x, int y) {
    QWrap::edges.emplace_back(new Position(x, y));
}


void QWrap::save_schema(QString path) {
    std::string path_string = path.toUtf8().constData();
    manager.save_schema(&model, path_string);
}


void QWrap::load_schema(QString path) {
    std::string path_string = path.INTERFACE;
    manager.load_schema(&model, path_string);
}

void QWrap::new_schema() {
    model.clean_workspace();
}


int QWrap::eval_block() {
    return model.eval_one();
}


void QWrap::eval_reset() {
    model.reset();
}


int QWrap::id() {
    return m_id;
}

void QWrap::setId(int id) {
    if (m_id == id)
        return;

    m_id = id;
    emit idChanged(m_id);
}
