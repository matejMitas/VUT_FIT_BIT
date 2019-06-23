import QtQuick 2.0
import QtQuick.Controls 2.3

import "../js/port.js" as Port
import "../js/generate.js" as Gen
import "../js/error.js" as Err

Column {

    id: asideBlockWrap
    // ulozeni docasnych komponent pro aside
    // vstupni porty & vystupni porty
    property var inPortsTemp
    property var outPortsTemp

    Component.onCompleted: {
        // priradime vstupni a vystupni porty
        // pri generovani prazdne
        // pri editaci naplnime polozkami
        inPortsTemp = {}
        outPortsTemp = {}

        // zobrazovani placeholderu
        if (Object.keys(viewport.blocks[String(activeBlock)].inPorts).length) {
            portAsidePlaceholder.visible = false;
            portAsideSeparator.visible = true;
        }

        var component = Qt.createComponent("qrc:/assets/comp/AsidePort.qml");
        // naplnime vstupni porty
        for (var inPort in viewport.blocks[String(activeBlock)].inPorts) {
            var tempPort = component.createObject(portInAside);

            tempPort.color = 'darkRed';
            tempPort.portType = 'in';
            tempPort.portName = inPort;

            inPortsTemp[inPort] = tempPort;
        }
        // naplnime vystupni porty
        for (var outPort in viewport.blocks[String(activeBlock)].outPorts) {
            var tempOutPort = component.createObject(portOutAside);

            tempOutPort.color = 'navy';
            tempOutPort.portType = 'out';
            tempOutPort.portName = outPort;

            outPortsTemp[outPort] = tempPort;
        }
    }

    spacing: 20
    width: parent.width - 40
    height: parent.height - 40
    x: 20
    y: 20

    Rectangle {
        //color: Qt.rgba(1,1,1,0.5)
        color: "transparent"
        width: parent.width
        height: 30

        Text {
            text: "Edit Block"
            font.family: "Helvetica"
            font.pointSize: 16
            font.bold: true
            color: Qt.rgba(0,0,0,.75)
            //anchors.horizontalCenter: parent.horizontalCenter
        }


        Rectangle {
            width: parent.width
            height: 1
            color: Qt.rgba(0,0,0,.15)
            y: 30
            //anchors.bottom: parent.bottom
        }
    }


    TextField {
        id: portName
        width: parent.width
        placeholderText: qsTr("Port name")
        //anchors.centerIn: parent
    }

    Row {
        spacing: 10

        RadioButton {
            id: radioInAside
            text: "IN"
            checked: true
        }

        RadioButton {
            id: radioOutAside
            text: "OUT"
        }

        Button {
            text: "Add port"
            onClicked: {
                var text = portName.text
                if (!text) {
                    Err.create_error_msg('Port name can\'t be blank');
                } else {
                    // vyprazdnime input
                    portName.text = "";
                    // omezime delku nazvu portu
                    if (text.length > 16) {
                        Err.create_error_msg('Port name must be lesser or equal to 16');
                    } else {
                        // vytvoreni pinu
                        portAsidePlaceholder.visible = false;
                        portAsideSeparator.visible = true;

                        var component = Qt.createComponent("qrc:/assets/comp/AsidePort.qml");
                        var tempPort;
                        var err;

                        if (radioInAside.checked) {
                            err = Gen.block_port_gen(text, 'in');
                            if (err) {
                                Err.create_error_msg('IN Port name already in use');
                            } else {
                                // hodime do docasneho pole, je nutno mazat po zavreni
                                tempPort = component.createObject(portInAside);
                                tempPort.color = 'darkRed';
                                tempPort.portType = 'in';
                                asideBlockWrap.inPortsTemp[text] = tempPort;
                            }
                        } else {
                            err = Gen.block_port_gen(text, 'out');
                            if (err) {
                                Err.create_error_msg('OUT Port name already in use');
                            } else {
                                tempPort = component.createObject(portOutAside);
                                tempPort.color = 'navy';
                                tempPort.portType = 'out';
                                asideBlockWrap.outPortsTemp[text] = tempPort;
                            }
                        }
                        // zadame nazev portu
                        tempPort.portName = text;
                    }
                }
            }
        }
    }


    Rectangle {
        color: "transparent"
        width: parent.width
        height: 30

        Rectangle {
            width: parent.width
            height: 1
            color: Qt.rgba(0,0,0,.15)
            //anchors.top: parent.top
        }

        Text {
            y: 20
            text: "Available ports"
            font.family: "Helvetica"
            font.pointSize: 14
            color: Qt.rgba(0,0,0,.5)
            //anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    // blok pro generovani portu
    Row {
        width: parent.width
        height: 350
        spacing: 20

        Column {
            width: 110
            spacing: 20
            id: portInAside
        }

        Rectangle {
            id: portAsideSeparator
            width: 1
            height: parent.height
            color: Qt.rgba(0,0,0,.15)
            visible: false
        }

        Column {
            width: 110
            spacing: 20
            id: portOutAside
        }

        Rectangle {
            id: portAsidePlaceholder
            width: parent.width
            height: parent.height
            color: "transparent"
            Text {
                text: 'No ports yet'
                font.family: "Helvetica"
                font.pointSize: 32
                color: Qt.rgba(0,0,0,.25)
                anchors.centerIn: parent
            }
        }
    }

    // separator pro polozku
    Rectangle {
        width: parent.width
        height: 1
        color: Qt.rgba(0,0,0,.15)
    }


    Button {
        width: parent.width
        text: "OK"

        onClicked: {
            // chceme pokracovat v editaci
            if (viewport.mode !== 'edit')
                viewport.mode = 'select';
            // vymazeme polozky
            activeAsideCtx.destroy()
            activeAsideCtx = undefined
            // vymazeme aside
            activeAsideObj.destroy()
            activeAsideObj = undefined
        }
    }
}
