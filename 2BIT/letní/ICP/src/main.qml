import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import QtQuick.Dialogs 1.3
// rozhrani komunikacni srandy
import QWrap 1.0
// nacteme si komponenty pro program
import "assets/comp"
// import JS souboru
import "assets/js/canvas.js" as Canvas
import "assets/js/generate.js" as Gen


ApplicationWindow {
    id: viewport
    visible: true
    // standartni velikost okna, samozrejme resizable
    width: 1200
    height: 800
    // nastaveni minimalni velikosti
    minimumWidth: 800
    minimumHeight: 600

    // nastaveni
    title: qsTr("ICP18 Block Editor")
    // pro mod programu, drag, delete, select
    property var mode: 'select'

    // pro mrizku
    property var gridColor: '#efefef'
    property int gridSize: 10

    // pro menu
    property int heightFull: 75
    property int heightRedc: 50

    // pro vytvoreni cary, jinak jsou cary uchovavane u pinu
    property var wire
    property var connections

    // pro praci s aktivnimo blokem
    property var blocks
    property int activeBlock: 0
    // pro zalohu polozek v menu
    property var activeAsideObj
    // aktualni zobrazeny view
    property var activeAsideCtx

    // inicializujeme komunikacni nastroj a
    // nastavime mod prace s programem
    QWrap {
        id: wrap
    }

    //Aside {}

    // horni menicko
    Menu {
        id: menuMain
    }

    // object pro view
    Rectangle {
         width: viewport.width
         height: viewport.height - 75
         y: 75

         MouseArea {
             anchors.fill: parent
         }

         Canvas {
             id: linesCanvas
             width: parent.width
             height: parent.height

             property int lineCounter: 1

             signal canvasDraw(int x1, int y1, int x2, int y2)
             signal canvasStoreLine()
             signal canvasUpdate()

             MouseArea {
                 anchors.fill: parent
                 onClicked: {
                    // zkontrolujeme pozici pro kazdou caru
                    for (var key in connections) {
                         var A = {
                            'x': connections[key].pos[0],
                            'y': connections[key].pos[1],
                         };
                         var B = {
                             'x': connections[key].pos[2],
                             'y': connections[key].pos[3],
                         }
                         var C = {
                             'x': Math.round(mouseX),
                             'y': Math.round(mouseY)
                         }
                        // nasli jsme shodu
                        if (Canvas.detect(A,B,C)) {
                            console.log(key);
                            break;
                        }
                    }
                 }
             }

             onCanvasDraw: { Canvas.drawLine(linesCanvas, 1, 'black', x1, y1, x2, y2) }
             onCanvasStoreLine: {
                 // ulozime vsechna data to skvelho
                 var tempWire = {
                     'source': [wire.from.blockId, wire.from.portName, wire.from.portType, wire.from.pinName],
                     'dest': [wire.to.blockId, wire.to.portName, wire.to.portType, wire.to.pinName],
                     'pos': [wire.from.pos[0], wire.from.pos[1], wire.to.pos[0], wire.to.pos[1]]
                 }
                 // prevedem typ portu
                 var portType1 = wire.from.portType === 'in' ? 1 : 0;
                 var portType2 = wire.to.portType === 'in' ? 1 : 0;

                 wrap.add_position(wire.from.pos[0], wire.from.pos[1])
                 wrap.add_position(wire.to.pos[0], wire.to.pos[1])

                 console.log(wire.from.blockId)
                 console.log(wire.from.portName)
                 console.log(portType1)
                 console.log(wire.from.pinName)
                 console.log(wire.to.blockId)
                 console.log(wire.to.portName)
                 console.log(portType2)
                 console.log(wire.to.pinName)


                 console.log(JSON.stringify(tempWire))

                 // ulozime
                 wrap.add_wire(wire.from.blockId, wire.from.portName,
                               portType1, wire.from.pinName,
                               wire.to.blockId, wire.to.portName,
                               portType2, wire.to.pinName)


                 // pridame vytvorene spojeni
                 connections["Wire"+lineCounter] = tempWire
                 lineCounter++;
             }

             onCanvasUpdate: {
                Canvas.updateCanvas(linesCanvas);
             }
         }
    }

    Component.onCompleted: {
       // inicializace bloku
       blocks = {};
       connections = {};
       // inicializace mozneho spojeni
       wire = {
           'from': {},
           'to': {}
       };

       Gen.create_sample_data();
    }
}
