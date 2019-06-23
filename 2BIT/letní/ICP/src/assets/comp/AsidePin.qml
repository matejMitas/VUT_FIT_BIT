import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4

import "../js/generate.js" as Gen

Rectangle {

    property var pinName: ""
    property var pinType: ""
    property var portType

    width: parent.width
    height: 40
    color: '#bbb'
    id: asidePin

    RowLayout {
        width: parent.width
        spacing: 0

        Column {
            Layout.leftMargin: 10
            y: 5
            spacing: 2

            Text {
                font.family: "Helvetica"
                font.pointSize: 16
                text: pinName
            }

            Text {
                font.family: "Helvetica"
                font.pointSize: 12
                text: pinType
            }
        }

        Row {
            Layout.alignment : Qt.AlignRight
            Rectangle {
                width: 40
                height: 40
                color: '#777'


                Image {
                    width: 40
                    height: 40
                    opacity: 0.65
                    source: "../port/pin-edit.svg"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        var component
                        if (portType === 'in') {
                            component = Qt.createComponent("qrc:/assets/comp/EditPinValue.qml");
                            var edit = component.createObject(viewport);
                            edit.portName = portName
                            edit.pinName = pinName
                        } else {
                            component = Qt.createComponent("qrc:/assets/comp/EditPinConn.qml");
                            var conn = component.createObject(viewport);
                            conn.portName = portName
                            conn.pinType = pinType
                            conn.pinName = pinName

                            conn.render();
                        }
                    }
                }
            }

            Rectangle {
                width: 40
                height: 40
                color: '#777'
                //Layout.alignment : Qt.AlignRight

                Image {
                    width: 40
                    height: 40
                    opacity: 0.65
                    source: "../port/pin-delete.svg"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        asidePin.destroy();
                        delete asidePortWrap[pinName];
                        Gen.block_pin_delete(portName, portType, pinName);
                    }
                }
            }
        }
    }
}
