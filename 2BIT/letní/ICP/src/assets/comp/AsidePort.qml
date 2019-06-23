import QtQuick 2.0

import "../js/generate.js" as Gen
import "../js/aside.js" as Aside


Rectangle {
    // nastaveni textu a typu
    property var portName: ""
    property var portType: ""

    id: asidePort
    width: 110
    height: 50

    Text {
        width: parent.width - 10
        font.family: "Helvetica"
        font.bold: true
        font.pointSize: 12
        text: portName
        y: 5
        x: 5
        color: Qt.rgba(1,1,1,1)
    }

    Rectangle {
        width: parent.width
        height: 1
        color: Qt.rgba(1,1,1,.5)
        y: 20
    }

    Rectangle {
        width: 1
        height: 30
        z: 2
        color: Qt.rgba(1,1,1,.5)
        x: 55
        y: 20
    }

    Image {
        width: 55
        height: 30
        y: 20
        opacity: 0.65
        source: "../port/port_edit.svg"

        MouseArea {
            anchors.fill: parent
            onClicked: {
                // vyhod prechodzi info
                activeAsideObj.destroy()

                var asideTemp = Qt.createComponent("qrc:/assets/comp/Aside.qml");
                activeAsideObj = asideTemp.createObject(viewport);

                // podle typu vytvorime obsah
                asideTemp = Qt.createComponent("qrc:/assets/comp/AsideWrapPort.qml");
                activeAsideCtx = asideTemp.createObject(activeAsideObj);
                activeAsideCtx.portName = portName;
                activeAsideCtx.portType = portType;
                activeAsideCtx.render();
            }
        }
    }

    Image {
        width: 55
        height: 30
        y: 20
        x: 55
        opacity: 0.65
        source: "../port/port_delete.svg"

        MouseArea {
            anchors.fill: parent
            onClicked: {
                // odstraneni portu z view
                asidePort.destroy();

                // odstraneni z naseho uchovani bloku
                // odstraneni z globalniho pole bloku
                if (portType == 'in')
                    delete activeAsideCtx.inPortsTemp[portName];
                else
                    delete activeAsideCtx.outPortsTemp[portName];

                // odstraneni portu
                Gen.block_port_delete(activeBlock, portName, portType);
            }
        }
    }
}
