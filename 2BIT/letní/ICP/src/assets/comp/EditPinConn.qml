import QtQuick 2.0
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import "../js/generate.js" as Gen
import "../js/error.js" as Err

Rectangle {

    property var portName
    property var pinName
    property var pinType

    width: viewport.width
    height: viewport.height
    color: Qt.rgba(0,0,0,.75);

    id: editPinConn
    z: 200

    function map_enum(text) {
        if (text === 'Digital')
            return 0;
        else if (text === 'Analog')
            return 1;
        else if (text === 'Math')
            return 2
    }

    signal render()

    onRender: {
        var ports = viewport.blocks[String(activeBlock)].inPorts
        var arr = []

        var fn1Oper = ['sin()', 'cos()', 'tan()', '|']
        var fn2Oper = ["+", "-", "*", '/', '&', '|', '^']

        for (var port in ports) {
            //console.log(port)

            for (var pin in ports[port].pins) {
                var pinNameIn = ports[port].pins[pin].name
                var pinTypeIn = parseInt(ports[port].pins[pin].type)
                // nemuzeme spojovat nekompatibilni porty
                if (map_enum(pinType) === pinTypeIn)
                    arr.push(port + " - " + pinNameIn);
            }
        }

        operand_1_select.model = arr;
        operand_2_select.model = arr;
    }

    Rectangle {

        width: 300
        height: 400
        color: 'white'
        x: viewport.width / 2 - 150
        y: viewport.height / 2 - 200

        ColumnLayout {
            spacing: 10

            Text {
                Layout.preferredWidth: 260
                Layout.leftMargin: 20
                Layout.topMargin: 20
                text: "Selected operation"
                font.family: "Helvetica"
                font.pointSize: 16
                font.bold: true
                color: Qt.rgba(0,0,0,.75)
                //anchors.horizontalCenter: parent.horizontalCenter
            }

            ComboBox {
                Layout.preferredWidth: 260
                Layout.leftMargin: 20
                id: oper
                model: [ "+", "-", "*", '/', '&', '|', '^', 'sin()', 'cos()', 'tan()']
            }

            ColumnLayout {
                id: operand_1
                Text {
                    Layout.preferredWidth: 260
                    Layout.leftMargin: 20
                    Layout.topMargin: 20
                    text: "Operand 1"

                    font.family: "Helvetica"
                    font.pointSize: 14
                    color: Qt.rgba(0,0,0,.75)
                }

                ComboBox {
                    id: operand_1_select
                    Layout.preferredWidth: 260
                    Layout.leftMargin: 20
                    model: [ "+", "-", "*", '/']
                }
            }


            ColumnLayout {
                id: operand_2
                Text {
                    Layout.preferredWidth: 260
                    Layout.leftMargin: 20
                    Layout.topMargin: 20
                    text: "Operand 1"

                    font.family: "Helvetica"
                    font.pointSize: 14
                    color: Qt.rgba(0,0,0,.75)
                }

                ComboBox {
                    id: operand_2_select
                    Layout.preferredWidth: 260
                    Layout.leftMargin: 20
                    model: [ "+", "-", "*", '/']
                }
            }

            Button {
                text: "Close"
                Layout.preferredWidth: 260
                Layout.leftMargin: 20
                Layout.topMargin: 20

                onClicked: editPinConn.destroy()
            }

            Button {
                text: "Update value"
                Layout.preferredWidth: 260
                Layout.leftMargin: 20

                onClicked: {
                    var fn = oper.currentText
                    var oper1 = operand_1_select.currentText
                    var oper2 = operand_2_select.currentText

                    var index1 = oper1.indexOf(" - ");
                    var index2 = oper2.indexOf(" - ");

                    var port1 = oper1.slice(0, index1);
                    var pin1 = oper1.slice(index1+3);

                    var port2 = oper2.slice(0, index2);
                    var pin2 = oper2.slice(index2+3);

                    if (oper1 === oper2) {
                        Err.create_error_msg('Can\'t connect with two same pins');
                    } else {
                        Gen.block_pin_oper(portName, pinName, port1, port2, pin1, pin2, fn, map_enum(pinType));
                        editPinConn.destroy();
                    }
                }
            }
        }
    }
}
