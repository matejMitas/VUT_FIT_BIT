/**
 * Projekt ICP 2018
 * Implementace blokoveho editoru
 * @brief Modul zastitujici spravu dat
 * @author HanakJiri, xhanak33
 * @file Model.h
 */

#ifndef ICP18_FIT_2018_MODEL_H
#define ICP18_FIT_2018_MODEL_H

#include <list>
#include "Component.h"
#include "Operations.h"

/**
 * Verze programu
 */
#define ACTUAL_VERSION 1

/**
 * Trida zastresujici logiku
 */
class Model {
private:
    std::list<Component *> components;
    std::list<Component *> eval_stack;
    int Component_id_counter = 1;

    /**
     * Do eval_stack pushne komponentu k vypoctu
     * @return true, pokud je co pocitat
     */
    bool get_component_to_eval();
public:
    Model();
    ~Model();

    /**
     * Vytvori novy vypocetni blok
     * @param position pozice komponenty
     * @return identifikace noveho bloku
     */
    int new_block(Position *position);

    /**
     * Vytvori novy vypocetni blok
     * @param component_id identifikator bloku
     * @param position pozice komponenty
     * @return nova komponenta
     */
    Component* new_block(int component_id, Position *position);

    /**
     * Odstrani vypocetni blok
     * @param component_id blok k odstraneni
     */
    void del_block(int component_id);

    /**
     * Vrati pozadovanou komponentu
     * @param component_id identifikace komponenty
     * @return komponenta
     */
    Component * get_component(int component_id);

    /**
     * Vrati vsechny vypocetni bloky
     * @return vektor bloku
     */
    std::list<Component *> get_components();

    /**
     * Vymaze smaze bloky
     */
    void clean_workspace();

    /**
     * Provede vypocet jednoho bloku, ktery ma vsechny parametry
     * @return identifikace bloku, ktery provedl vypocet, nebo ktery ceka na zadani vstupnich hodnot
     */
    int eval_one();

    /**
     * Resetuje vysechny komponenty
     */
    void reset();

    /**
     * Zkontroluje, zda li schema neobsahuje cykly
     */
    void check_integrity();
};



#endif //ICP18_FIT_2018_MODEL_H
