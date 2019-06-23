/**
 * Projekt ICP 2018
 * Implementace blokoveho editoru
 * @brief Modul pro spravu pozic
 * @author HanakJiri, xhanak33
 * @file Position.h
 */

#ifndef ICP18_FIT_2018_POSITION_H
#define ICP18_FIT_2018_POSITION_H

/**
 * Reprezentace pozice v 2D modelu
 */
class Position {
private:
    int x = 0;
    int y = 0;
public:
    /**
     * Konstruktor pozice
     * @param x souradnice X
     * @param y souradnice Y
     */
    Position(int x, int y);

    /**
     * Nastavi novou pozici
     * @param x souradnice x
     * @param y souradnice y
     */
    void set_position(int x, int y);

    /**
     * Vrati posun v ose x
     * @return posun
     */
    int get_x() const;

    /**
     * Vrati posun v ose y
     * @return posun
     */
    int get_y() const;
};
#endif //ICP18_FIT_2018_POSITION_H
