import QtQuick 2.0
import QtQuick.Layouts 1.3

Rectangle {

    property var parentPort
    property var parentBlock
    property var portType

    property var name
    property var value
    property var type

    property var hoverWrapper

    width: 25
    height: 5

    id: pin

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onEntered: {
            //console.log('Jsem pin ' + name + " s hodnotou " + value);

            var temp = Qt.createComponent("qrc:/assets/comp/BlockPinInfo.qml");
            var tempHover = temp.createObject(pin);
            tempHover.name = name;
            tempHover.portType = portType;
            // nastaveni hodnoty pinu
            if (value)
                tempHover.value = value;
            // nastaveni pozice okna
            tempHover.x = portType === 'in' ? -90 : 35;
            hoverWrapper = tempHover;
        }

        onExited:  {
            hoverWrapper.destroy();
        }

        onClicked: {
            onClicked: {
                var position = pin.mapToItem(linesCanvas, 0, 0)
                position.y += 2.5
                if (portType === 'out')
                    position.x += 25

                // zde je logika napojeni
                if (Object.keys(viewport.wire.from).length == 0) {
                    viewport.wire.from = {
                        'blockId': parentBlock,
                        'portName': parentPort,
                        'portType': portType,
                        'pinName': name,
                        'pos': [position.x, position.y]
                    }

                } else {
                    viewport.wire.to = {
                        'blockId': parentBlock,
                        'portName': parentPort,
                        'portType': portType,
                        'pinName': name,
                        'pos': [position.x, position.y]
                    }



                    // validace spojeni
                    linesCanvas.canvasStoreLine();
                    // vykresleni spojeni
                    linesCanvas.canvasDraw(viewport.wire.from.pos[0],
                                           viewport.wire.from.pos[1],
                                           position.x,
                                           position.y);
                    // reset vstupniho prvku
                    viewport.wire.from = {}
                    viewport.wire.to = {}
                }
            }
        }
    }
}
