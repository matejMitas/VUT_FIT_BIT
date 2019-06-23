/**
 * Projekt ICP 2018
 * Implementace blokoveho editoru
 * @brief Modul pro spravu souboru
 * @author HanakJiri, xhanak33
 * @file Manager.h
 */

#ifndef ICP18_FIT_2018_MANAGER_H
#define ICP18_FIT_2018_MANAGER_H

#include <string>
#include <iostream>
#include "../libs/pugixml-1.9/pugixml.hpp"
#include "../model/Model.h"

/**
 * Definice identifikace pro ukladani do XML
 */
#define IDENTIFICATION "ICP_BlockEditor"

/**
 * Definice komponenty pro ukladani do XML
 */
#define COMPONENT "Component"

/**
 * Definice vstupnich portu pro ukladani do XML
 */
#define IN_PORT "Input_ports"

/**
 * Definice vystupnich portu pro ukladani do XML
 */
#define OUT_PORT "Output_ports"

/**
 * Definice portu pro ukladani do XML
 */
#define PORT "Port"

/**
 * Definice pinu pro ukladani do XML
 */
#define PIN "Pin"

/**
 * Definice operace pro ukladani do XML
 */
#define OPERATION "operation"

/**
 * Definice operandu pro ukladani do XML
 */
#define OPERAND "Operand"

/**
 * Definice propojky pro ukladani do XML
 */
#define WIRE "Wire"

/**
 * Definice rohu pro ukladani do XML
 */
#define EDGE "Edge"

/**
 * Definice identifikace komponenty pro ukladani do XML
 */
#define COMPONENT_ID "component_ID"

/**
 * Definice verze pro ukladani do XML
 */
#define VERSION "version"

/**
 * Definice jmena portu pro ukladani do XML
 */
#define PORT_NAME "port_name"

/**
 * Definice jmena pinu pro ukladani do XML
 */
#define PIN_NAME "pin_name"

/**
 * Definice typu pinu
 */
#define PIN_TYPE "pin_type"

/**
 * Definice souradnice X pro ukladani do XML
 */
#define X_POS "x"

/**
 * Definice souradnice Y pro ukladani do XML
 */
#define Y_POS "y"

/**
 * Trida strarajici se o praci se soubory
 */
class Manager {
public:
    /**
     * Nacte schema ze souboru
     * @param model model schema
     * @param file_name jmeno souboru
     */
    void load_schema(Model *model, std::string &file_name);

    /**
     * Ulozi schema do souboru
     * @param model model schema
     * @param file_name jmeno souboru
     */
    void save_schema(Model *model, std::string &file_name);
};

#endif //ICP18_FIT_2018_MANAGER_H
