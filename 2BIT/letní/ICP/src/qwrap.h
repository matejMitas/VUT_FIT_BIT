#ifndef QWRAP_H
#define QWRAP_H

#include <QObject>
#include <QString>
#include <QPoint>

#include <QDebug>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QVariant>

// Jirkuv backend
#include "backend/model/Model.h"
#include "backend/file_manager/Manager.h"

#ifdef __unix__
#define INTERFACE toUtf8().constData()
#else
#define INTERFACE toLocal8Bit().constData()
#endif

enum QPinType {
     In,
     Out
};

class QWrap : public QObject {
    Q_OBJECT
    Q_PROPERTY(int id READ id NOTIFY idChanged)

public:
    explicit QWrap(QObject *parent = nullptr);

public slots:
     // Getters
     int id();
     // Setters
     void setId(int id);

     /**
      * @brief Vrati vsechny Q bloky
      * @return Q bloky
      */
     QVariantMap get_Q_blocks();

     /**
      * @brief Vytvori Q block
      * @param blockId blok id
      * @return Q blok
      */
     QVariantMap get_Q_block(int blockId);

     /**
      * @brief Vytvori jeden blok
      * @return ID noveho bloku
      */
     int add_block();

     /**
      * @brief Smaze blok
      * @param ID bloku
      */
     void delete_block(int blockId);

     /**
      * @brief Prida port
      * @param blockId identifikace bloku
      * @param portName jmeno portu
      * @param portType typ portu
      */
     QString add_port(int blockId, QString portName, int portType);

     /**
      * @brief Odstrani port
      * @param blockId identifikace bloku
      * @param portName jmeno portu
      * @param portType typ portu
      */
     void remove_port(int blockId, QString portName, int portType);

     /**
      * @brief Nastavi pozici bloku
      * @param blockId identifikace bloku
      * @param x souradnice X
      * @param y souradnice Y
      */
     void set_pos(int blockId, int x, int y);

     /**
      * @brief Prida pin
      * @param blockId identifikace bloku
      * @param portName jmeno portu
      * @param pinName jmeno pinu
      * @param portType typ portu
      * @param pinType typ pinu
      */
     QString add_pin(int blockId, QString portName, QString pinName, int portType, int pinType);

     /**
      * @brief Odstrani pin
      * @param blockId identifikace bloku
      * @param portName jmeno portu
      * @param pinName jmeno pinu
      * @param portType typ portu
      */
     void remove_pin(int blockId, QString portName, QString pinName, QPinType portType);

     /**
      * @brief Nastavi funkci pinu
      * @param blockId identifikace bloku
      * @param portName jmeno portu
      * @param pinName jmeno pinu
      * @param pinOperator identifikace operatoru
      */
     void set_function(int blockId, QString portName, QString pinName, QString pinOperator);

     /**
      * @brief Prida pin to vectoru operandu
      * @param blockId identifikace bloku
      * @param portName jmeno portu
      * @param pinName jmeno pinu
      */
     void add_operand_to_vector(int blockId, QString portName, QString pinName);

     /**
      * @brief Prida propojku
      * @param blockId1 identifikace prvniho bloku
      * @param portName1 jmeno portu prvniho bloku
      * @param portType1 typ portu prvniho bloku
      * @param pinName1 jmeno pinu prvniho bloku
      * @param blockId2 identifikace druheho bloku
      * @param portName2 jmeno portu druheho bloku
      * @param portType2 typ portu druheho bloku
      * @param pinName2 jmeno pinu druheho bloku
      */
     QString add_wire(int blockId1, QString portName1, int portType1, QString pinName1,
                      int blockId2, QString portName2, int portType2, QString pinName2);

     /**
      * @brief Odstrani propojku
      * @param blockId1 identifikace prvniho bloku
      * @param portName1 jmeno portu prvniho bloku
      * @param portType1 typ portu prvniho bloku
      * @param pinName1 jmeno pinu prvniho bloku
      * @param blockId2 identifikace druheho bloku
      * @param portName2 jmeno portu druheho bloku
      * @param portType2 typ portu druheho bloku
      * @param pinName2 jmeno pinu druheho bloku
      */
     void remove_wire(int blockId1, QString portName1, int portType1, QString pinName1,
                      int blockId2, QString portName2, int portType2, QString pinName2);

     /**
      * @brief Nastavi nove umisteni propojky
      * @param blockId1 identifikace prvniho bloku
      * @param portName1 jmeno portu prvniho bloku
      * @param portType1 typ portu prvniho bloku
      * @param pinName1 jmeno pinu prvniho bloku
      * @param blockId2 identifikace druheho bloku
      * @param portName2 jmeno portu druheho bloku
      * @param portType2 typ portu druheho bloku
      * @param pinName2 jmeno pinu druheho bloku
      */
     void set_wire_positions(int blockId1, QString portName1, int portType1, QString pinName1,
                             int blockId2, QString portName2, int portType2, QString pinName2);

     /**
      * @brief Prenastavi typ pinu
      * @param blockId identifikace bloku
      * @param portName jmeno portu
      * @param pinName jmeno pinu
      * @param portType typ portu
      * @param pinType typ pinu
      */
     void set_pin_type(int blockId, QString portName, QString pinName, int portType, int pinType);

     /**
      * @brief Nastavi hodnotu na pin
      * @param blockId identifikace bloku
      * @param portName jmeno portu
      * @param pinName jmeno pinu
      * @param value hodnota
      */
     void set_Value(int blockId, QString portName, QString pinName, double value);

     /**
      * @brief add_position
      * @param x souradnice X
      * @param y souradnice Y
      */
     void add_position(int x, int y);

     /**
      * @brief ulozi schema do souboru
      * @param path jmeno souboru
      */
     void save_schema(QString path);

     /**
      * @brief Nacte schema ze souboru
      * @param path jmeno souboru
      */
     void load_schema(QString path);

     /**
      * @brief Vytvoreni noveho schema
      */
     void new_schema();

     /**
      * @brief Provede vypocet jednoho bloku
      * @return 0 pro hotovy vypocet, ID pro korektni vypocet, -ID, pokud blok potrebuje doplnit informace
      */
     int eval_block();

     /**
      * @brief Resetuje vypocet
      */
     void eval_reset();

signals:
     // Signals
     void idChanged(int id);

private:
     // model pro prace s Backendem
     Model model;
     Manager manager;

     // refaktor, zde si nic nepomatuji
     // id bloku
     int m_id;

     std::vector<Pin_in *> pin_operands;
     std::vector<Position *> edges;
};

#endif // QWRAP_H
