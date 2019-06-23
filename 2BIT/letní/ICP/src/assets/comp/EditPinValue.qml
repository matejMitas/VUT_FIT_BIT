import QtQuick 2.0
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import "../js/generate.js" as Gen
import "../js/error.js" as Err

Rectangle {

    property var portName
    property var pinName

    width: viewport.width
    height: viewport.height
    color: Qt.rgba(0,0,0,.75);

    id: editPinValue
    z: 200

    Rectangle {

        width: 300
        height: 170
        color: 'white'
        x: viewport.width / 2 - 150
        y: viewport.height / 2 - 85

        ColumnLayout {
            spacing: 10

            TextField {
                id: valuePin
                Layout.preferredWidth: 260
                Layout.leftMargin: 20
                Layout.topMargin: 20
                placeholderText: qsTr("New value")
                //anchors.centerIn: parent
            }

            Button {
                text: "Close"
                Layout.preferredWidth: 260
                Layout.leftMargin: 20
                Layout.topMargin: 20

                onClicked: editPinValue.destroy()
            }

            Button {
                text: "Finish operation"
                Layout.preferredWidth: 260
                Layout.leftMargin: 20

                onClicked: {
                    Gen.block_pin_value(portName, 'in', pinName, valuePin.text)
                    editPinValue.destroy();
                }
            }
        }
    }
}
