/**
 * Projekt ICP 2018
 * Implementace blokoveho editoru
 * @brief Modul pro spravu pozic
 * @author HanakJiri, xhanak33
 * @file Position.cpp
 */

#include "Position.h"


Position::Position(int x, int y) {
    Position::x = x;
    Position::y = y;
}

void Position::set_position(int x, int y) {
    Position::x = x;
    Position::y = y;
}

int Position::get_x() const {
    return Position::x;
}

int Position::get_y() const {
    return Position::y;
}
