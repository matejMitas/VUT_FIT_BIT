/**
 * Projekt ICP 2018
 * Implementace blokoveho editoru
 * @brief Modul pro spravu operaci a funkci
 * @author HanakJiri, xhanak33
 * @file Operations.h
 */

#ifndef ICP18_FIT_2018_OPERATIONS_H
#define ICP18_FIT_2018_OPERATIONS_H

#include <map>
#include <string>
#include <vector>
#include <cmath>
#include <climits>


class Ops;

/**
 * Trida zastresujici vsechny podporovane operace
 */
class Operations {
private:
    std::vector<Ops *> operators;
public:
    Operations();

    /**
     * Vrati vsechny dostupne operace
     * @return vektor operaci
     */
    std::vector<Ops *> get_operations();

    /**
     * Vycisti operace
     */
    void clear();
};

/**
 * Bazova trida operaci
 */
class Ops {
public:

    /**
     * Vypocita vysledek na zakade operatoru a operandu
     * @param operands operandy
     * @return vysledek operace
     */
    virtual double eval(std::vector<double> operands) = 0;

    /**
     * Textova reprezentace operace
     * @return String
     */
    virtual std::string repr() = 0;

    virtual ~Ops() {};
};

/**
 * Operace scitani
 */
class Op_add: public Ops {
public:
    double eval(std::vector<double> values) final;
    std::string repr() final;
};

/**
 * Operace odcitani
 */
class Op_sub: public Ops {
public:
    double eval(std::vector<double> values) final;
    std::string repr() final;
};

/**
 * Operace nasobeni
 */
class Op_mul: public Ops {
public:
    double eval(std::vector<double> values) final;
    std::string repr() final;
};

/**
 * Operace deleni
 */
class Op_div: public Ops {
public:
    double eval(std::vector<double> values) final;
    std::string repr() final;
};

/**
 * Operace and
 */
class Op_and: public Ops {
public:
    double eval(std::vector<double> values) final;
    std::string repr() final;
};

/**
 * Operace or
 */
class Op_or: public Ops {
public:
    double eval(std::vector<double> values) final;
    std::string repr() final;
};

/**
 * Operace not
 */
class Op_not: public Ops {
public:
    double eval(std::vector<double> values) final;
    std::string repr() final;
};

/**
 * Operace xor
 */
class Op_xor: public Ops {
public:
    double eval(std::vector<double> values) final;
    std::string repr() final;
};

/**
 * Funkce sinus
 */
class Op_sin: public Ops {
public:
    double eval(std::vector<double> values) final;
    std::string repr() final;
};

/**
 * Funkce kosinus
 */
class Op_cos: public Ops {
public:
    double eval(std::vector<double> values) final;
    std::string repr() final;
};

/**
 * Funkce tangens
 */
class Op_tan: public Ops {
public:
    double eval(std::vector<double> values) final;
    std::string repr() final;
};


/**
 * Vyhodnoti, zda li se jedna o bitovou hodnotu
 * @param value zkoumana hodnota
 * @return true pro binarni hodnotu
 */
bool is_binary(double value);

#endif //ICP18_FIT_2018_OPERATIONS_H
