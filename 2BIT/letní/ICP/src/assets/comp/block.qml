import QtQuick 2.0
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
// import JS souboru
import "../js/port.js" as Port
import "../js/aside.js" as Aside

Rectangle {
    width: 80
    height: 80
    color: 'white'
    border.color: "#888"
    border.width: 1
    id: block
    //opacity: 0.5

    // defaultni hodnota id
    property int blockId
    // nastaveni moznosti posuvu
    property var draggable: block
    property bool dragging: false
    // ukladani portu
    property var inPorts
    property var outPorts

    // signal pro tvoreni a mazani portu
    signal createPort(string portName, string portType)
    signal deletePort(string portName, string portType)

    Component.onCompleted: {
        inPorts = {}
        outPorts = {}
        // presmerovani signalu
        block.createPort.connect(onCreatePort);
    }

    function onCreatePort(portName, portType) {
        var temp = Qt.createComponent("qrc:/assets/comp/BlockPort.qml");
        var tempPort;

        if (portType === 'in') {
            tempPort = temp.createObject(inPortsWrapper);
            tempPort.portColor = 'darkRed';
            inPorts[portName] = tempPort;
        } else if (portType === 'out') {
            tempPort = temp.createObject(outPortsWrapper);
            tempPort.portColor = 'navy';
            outPorts[portName] = tempPort;
        }

        tempPort.name = portName;
        tempPort.type = portType;
        tempPort.parentBlockId = blockId;
    }

    // Textove oznaceni bloku, obsolete, pouze pro debug
    Text {
        text: blockId
        font.family: "Helvetica"
        font.pointSize: 14
        color: "black"
        x: 10
        y: 10
    }

    // Vstupni Porty
    ColumnLayout {
        id: inPortsWrapper
        spacing: 10
        width: 45
        x: -50
    }

    // Vystupni Porty
    ColumnLayout {
        id: outPortsWrapper
        spacing: 10
        width: 45
        x: 80
    }


    MouseArea {
        anchors.fill: parent

        onClicked: {
            // nastavime aktivitu bloku
            activeBlock = blockId;
            var m = viewport.mode;
            // at si vzajemne nerusime akce

            if (m === 'select') {
                console.log('Vyberme');
            }
            else if (m === 'edit') {
                Aside.aside_gen('edit');
            }
            else if (m === 'delete') {
                wrap.delete_block(blockId);
                block.destroy();
                console.log('Mazeme');
            }
        }

        drag {
            // drag
            onActiveChanged: {
                dragging = !dragging;
                // po ukonceni tahu
                if (!dragging) {
                    var x = viewport.gridSize * Math.round(block.x / viewport.gridSize);
                    var y = (viewport.gridSize * Math.round(block.y / viewport.gridSize) - 5);
                    // zde zpracujeme uchyceni k mrizce
                    block.x = x;
                    // pozice vrsku nam dava offset do mrizky
                    block.y = y;
                    // zde zavolej Jirasovu metodu pro pozici bloku
                    wrap.set_pos(blockId, x, y);

                    console.log('konec dragu')

                    if (connections) {
                        // v pripade ze neni treba prekreslit canvas
                        var changed = false;
                        // projedeme vsechny spojeni a najdeme ty, ktere znaci
                        // protnuti s nasim blokem a jejich pozici nastavime
                        for (var key in connections) {
                            // pro kazdou caru nacteme ovlinene bloky
                            var block1 = connections[key].source[0];
                            var block2 = connections[key].dest[0];
                            // pro kazdou caru nacteme porty, at mame k cemu prichytavat
                            var port1 = connections[key].source[1];
                            var port2 = connections[key].dest[1];
                            //
                            var portType1 = connections[key].source[2];
                            var portType2 = connections[key].dest[2];
                            //
                            var pin1 = connections[key].source[3];
                            var pin2 = connections[key].dest[3];

                            // preklopime nastaveni hodnoty
                            if (!changed) {
                                changed = !changed
                            }
                            var pos;

                            // je treba updatovat source
                            if (block1 === blockId) {
                                //nacteme novou pozici portu vzhledem k bloku
                                if (portType1 == 'in')
                                    pos = inPorts[port1].pins[pin1].mapToItem(linesCanvas, 0, 0)
                                else
                                    pos = outPorts[port1].pins[pin1].mapToItem(linesCanvas, 0, 0)
                                //
                                connections[key].pos[0] = pos.x + 25
                                connections[key].pos[1] = pos.y + 2.5
                            }
                            // je treba updatova dest
                            else if (block2 === blockId) {
                                //nacteme novou pozici portu vzhledem k bloku
                                if (portType2 == 'in')
                                    pos = inPorts[port2].pins[pin2].mapToItem(linesCanvas, 0, 0)
                                else
                                    pos = outPorts[port2].pins[pin2].mapToItem(linesCanvas, 0, 0)
                                //
                                connections[key].pos[2] = pos.x
                                connections[key].pos[3] = pos.y + 2.5
                            }
                        }

                        if (changed) {
                            // zavolame canvas redraw
                            linesCanvas.canvasUpdate();
                        }
                    }
                }
            }

            // moznost posouvani
            target: block.draggable
            axis: Drag.XAndYAxis
            // maximum posouvani x
            minimumX: 0
            maximumX: viewport.width - block.width
            // rozmery posouvani y
            minimumY: 75
            maximumY: viewport.height - block.height
        }
    }
}
