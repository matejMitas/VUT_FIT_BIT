import QtQuick 2.0
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import "../js/generate.js" as Gen
import "../js/error.js" as Err
import "../js/aside.js" as Aside

ColumnLayout{
    id: asidePortWrap

    // pro jmeno portu
    property var portName: ""
    property var portType
    property var pins

    spacing: 20
    width: parent.width - 40
    x: 20
    y: 20

    signal render();


    function enum_map(num) {
        if (num === 0)
            return 'Digital';
        else if (num === 1)
            return 'Analog';
        else if (num === 2)
            return 'Math';
    }

    onRender: {
        pins = {}

        var component = Qt.createComponent("qrc:/assets/comp/AsidePin.qml");
        var tempPin;
        var source;

        if (portType === 'in') {
            source  = viewport.blocks[String(activeBlock)].inPorts[String(portName)].pins;
        } else {
            source = viewport.blocks[String(activeBlock)].outPorts[String(portName)].pins;
        }

        for (var pin in source) {
            tempPin = component.createObject(pinAside);
            tempPin.pinName = source[pin].name;
            tempPin.pinType = enum_map(source[pin].type);
            tempPin.portType = portType;

            pins[tempPin.pinName] = tempPin;
        }
    }

    RowLayout {
        width: parent.width
        height: 50
        spacing: 20

        Text {
            text: '< back'
            color: '#2f8dc4'
            font.bold: true

            MouseArea {
                anchors.fill: parent
                onClicked: Aside.aside_gen('edit')
            }
        }

        Text {
            text: "Port: " + portName
            font.family: "Helvetica"
            font.pointSize: 16
            font.bold: true
            color: Qt.rgba(0,0,0,.75)
        }

    }


    ColumnLayout {
        width: parent.width
        spacing: 10

        TextField {
            id: pinName
            Layout.preferredWidth: parent.width
            placeholderText: qsTr("Pin name")
            //anchors.centerIn: parent
        }

        ComboBox {
            Layout.preferredWidth: parent.width
            id: tests
            model: [ "Digital", "Analog", "Math" ]
        }

        Button {
            text: "Add pin"
            Layout.preferredWidth: parent.width

            function map_enum(text) {
                if (text === 'Digital')
                    return 0;
                else if (text === 'Analog')
                    return 1;
                else if (text === 'Math')
                    return 2
            }

            onClicked: {
                var name = pinName.text
                // toz chyba soustruzi
                if (!name) {
                    Err.create_error_msg('Pin name can\'t be blank');
                } else {
                    var pinType = map_enum(tests.currentText)

                    if (portType === 'in')
                        Gen.block_pin_gen(portName, 'in', name, pinType);
                    else if (portType === 'out')
                        Gen.block_pin_gen(portName, 'out', name, pinType);

                    pinName.text = "";

                    var component = Qt.createComponent("qrc:/assets/comp/AsidePin.qml");
                    var tempPin;

                    tempPin = component.createObject(pinAside);
                    tempPin.pinName = name;
                    tempPin.pinType = tests.currentText;
                    tempPin.portType = portType;

                    pins[name] = tempPin;
                }
            }
        }
    }

    Column {
        id: pinAside
        width: parent.width
        spacing: 10
    }
}
