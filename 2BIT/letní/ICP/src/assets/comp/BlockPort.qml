import QtQuick 2.0
import QtQuick.Layouts 1.3

RowLayout {

    id: port
    // pro jmeno pinu
    property var name
    property var type
    property var portColor: '#000'
    // pro uchovani si rodice
    property int parentBlockId

    spacing: 1

    property var pins

    signal createPin(string pinName, int pinType)

    Component.onCompleted: {
        pins = {}
        port.createPin.connect(onCreatePin);
    }

    function onCreatePin(pinName, pinType) {
        var temp = Qt.createComponent("qrc:/assets/comp/BlockPin.qml");
        var tempPin;
        // ukazeme vhodnou komponentu
        if (type === 'in') {
            pinsWrapperIn.visible = true;
            pinsPortIn.visible = true;

            tempPin = temp.createObject(pinsWrapperIn);
            tempPin.color = 'red'

        } else if (type === 'out') {
            pinsWrapperOut.visible = true;
            pinsPortOut.visible = true;

            tempPin = temp.createObject(pinsWrapperOut);
            tempPin.color = 'blue'
        }

        tempPin.name = pinName
        tempPin.value = undefined
        tempPin.type = pinType
        tempPin.portType = type
        // pro cary
        tempPin.parentPort = name
        tempPin.parentBlock = parentBlockId;
        // pridame pin do vsech pinu
        pins[pinName] = tempPin;
    }

    // pro vstupni porty
    ColumnLayout {

        Layout.fillHeight: true
        Layout.preferredWidth: 25
        Layout.alignment: Qt.AlignLeft

        spacing: 5

        id: pinsWrapperIn
        visible: false
    }

    Rectangle {
        Layout.fillHeight: true
        Layout.minimumHeight: 20
        Layout.alignment: Qt.AlignRight
        id: pinsPortIn
        visible: false

        width: 25

        height: parent.height
        color: 'darkRed'

        MouseArea {
            anchors.fill: parent
        }
    }


    // pro vystupni porty
    Rectangle {
        Layout.fillHeight: true
        Layout.minimumHeight: 20
        id: pinsPortOut
        visible: false

        width: 25

        height: parent.height
        color: 'navy'

        MouseArea {
            anchors.fill: parent
        }
    }

    ColumnLayout {
        Layout.fillHeight: true
        Layout.preferredWidth: 25
        Layout.alignment: Qt.AlignLeft

        spacing: 5

        id: pinsWrapperOut
        visible: false

   }
}
