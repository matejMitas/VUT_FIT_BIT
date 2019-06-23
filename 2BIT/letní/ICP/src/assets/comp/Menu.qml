import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import QtQuick.Shapes 1.0

import "../js/generate.js" as Gen
import "../js/aside.js" as Aside
import "../js/error.js" as Err

Rectangle {
    // barva pro aktivni prvky
    property var menuActive: '#555'
    property var menuInactive: 'transparent'

    width: parent.width
    height: viewport.heightFull
    gradient: Gradient {
        GradientStop { position: 0.0; color: "#393a3f" }
        GradientStop { position: 1.0; color: "#29292d" }
    }


    // wrapper pro polozky v menu
    Rectangle {
        property int blockSize: 45
        //
        property int sectionSep: 40
        property int blockSep: 10
        //
        property int blockActive: 0

        // pro nastavovani modu programu a vizualu ikon
        function handModes() {
            // TODO
        }

        id: menuContainer
        y: 10
        x: 20
        width: parent.width-40
        height: viewport.heightRedc
        color: 'transparent'

        Row  {
            spacing: 40

            /**
             * CANVAS
             * prace s platnem
             */

            Row {
                spacing: menuContainer.blockSep;

                Rectangle {
                    width: menuContainer.blockSize
                    height: viewport.heightRedc
                    color: 'transparent'
                    id: menuNew

                    MenuImage { menuImagePath: "../menu/new.svg"; id: menuNewImage }
                    MenuText { menuItemText: "NEW" }

                    MouseArea {
                        anchors.fill: parent
                        // na hover
                        hoverEnabled: true
                        onEntered: { menuNewImage.opacity = 1}
                        onExited: { menuNewImage.opacity = 0.65}

                        onClicked: {
                            // vytvorime nove platno, ale musi byt co zahazovat
                            var temp = Qt.createComponent("qrc:/assets/comp/NewFileDialog.qml");
                            temp.createObject(viewport);
                        }
                    }
                }

                Rectangle {
                    width: menuContainer.blockSize
                    height: viewport.heightRedc
                    color: 'transparent'
                    id: menuOpen

                    property var items: []

                    MenuImage { menuImagePath: "../menu/open.svg"; id: menuOpenImage }
                    MenuText { menuItemText: "OPEN" }

                    MouseArea {
                        anchors.fill: parent
                        // na hover
                        hoverEnabled: true
                        onEntered: { menuOpenImage.opacity = 1}
                        onExited: { menuOpenImage.opacity = 0.65}

                        onClicked: {
                            var temp = Qt.createComponent("qrc:/assets/comp/OpenFileDialog.qml");
                            temp.createObject(viewport);
                        }
                    }
                }

                Rectangle {
                    width: menuContainer.blockSize
                    height: viewport.heightRedc
                    color: 'transparent'
                    id: menuSave

                    property var items: []

                    MenuImage { menuImagePath: "../menu/save.svg"; id: menuSaveImage }
                    MenuText { menuItemText: "SAVE" }

                    MouseArea {
                        anchors.fill: parent
                        // na hover
                        hoverEnabled: true
                        onEntered: { menuSaveImage.opacity = 1}
                        onExited: { menuSaveImage.opacity = 0.65}

                        onClicked: {
                            var temp = Qt.createComponent("qrc:/assets/comp/SaveFileDialog.qml");
                            temp.createObject(viewport);
                        }
                    }
                }
            }

            /**
             * BLOCK
             * prace s blokem
             */

            Row {
                spacing: menuContainer.blockSep;

                Rectangle {
                    width: menuContainer.blockSize
                    height: viewport.heightRedc
                    color: 'transparent'
                    id: menuAdd

                    MenuImage { menuImagePath: "../menu/add.svg"; id: menuAddImage }
                    MenuText { menuItemText: "ADD" }

                    MouseArea {
                        anchors.fill: parent
                        // na hover
                        hoverEnabled: true
                        onEntered: { menuAddImage.opacity = 1}
                        onExited: { menuAddImage.opacity = 0.65}

                        onClicked: {
                            // vytvorime novy blok
                            activeBlock = Gen.block_gen(false);
                            // vysleme signal pro otevreni aside
                            Aside.aside_gen('add');

                            // reset zakliknuti mazani
                            menuDeleteImage.opacity = 0.65;
                            menuDelete.clicked = false;
                            menuEditImage.opacity = 0.65;
                            menuEdit.clicked = false;

                            // TODO: zakazat pridavani bloku
                        }
                    }
                }


                Rectangle {
                    width: menuContainer.blockSize
                    height: viewport.heightRedc

                    color: 'transparent'
                    id: menuDelete
                    property bool clicked: false

                    MenuImage { menuImagePath: "../menu/delete.svg"; id: menuDeleteImage }
                    MenuText { menuItemText: "DELETE" }

                    MouseArea {
                        anchors.fill: parent
                        // na hover
                        hoverEnabled: true

                        onClicked: {
                            // tohle
                            menuDelete.clicked = !menuDelete.clicked;

                            if (menuDelete.clicked) {
                                menuDeleteImage.opacity = 1;
                                viewport.mode = 'delete'
                            } else {
                                menuDeleteImage.opacity = 0.65;
                                viewport.mode = 'select'
                            }

                            menuEditImage.opacity = 0.65;
                            menuEdit.clicked = false;
                        }
                    }
                }

                Rectangle {
                    width: menuContainer.blockSize
                    height: viewport.heightRedc

                    color: 'transparent'
                    id: menuEdit
                    property bool clicked: false

                    MenuImage { menuImagePath: "../menu/edit.svg"; id: menuEditImage }
                    MenuText { menuItemText: "EDIT" }

                    MouseArea {
                        anchors.fill: parent
                        // na hover
                        hoverEnabled: true

                        onClicked: {

                            // tohle
                            menuEdit.clicked = !menuEdit.clicked;

                            if (menuEdit.clicked) {
                                menuEditImage.opacity = 1;
                                viewport.mode = 'edit'
                            } else {
                                menuEditImage.opacity = 0.65;
                                viewport.mode = 'select'
                            }

                            menuDeleteImage.opacity = 0.65;
                            menuDelete.clicked = false;
                        }
                    }
                }
            }

            /**
             * WIRE
             * prace s dratem
             */

            Row {
                spacing: menuContainer.blockSep;

                Rectangle {
                    width: menuContainer.blockSize
                    height: viewport.heightRedc
                    color: 'transparent'
                    id: connectionAdd

                    MenuImage { menuImagePath: "../menu/add_conn.svg"; id: connectionAddImage }
                    MenuText { menuItemText: "ADD" }

                    MouseArea {
                        anchors.fill: parent
                        // na hover
                        hoverEnabled: true
                        onEntered: { connectionAddImage.opacity = 1}
                        onExited: { connectionAddImage.opacity = 0.65}

                        onClicked: {
                            viewport.mode = 'addWire'
                        }
                    }
                }


                Rectangle {
                    width: menuContainer.blockSize
                    height: viewport.heightRedc

                    color: 'transparent'
                    id: connectionDelete
                    property bool clicked: false

                    MenuImage { menuImagePath: "../menu/delete_conn.svg"; id: connectionDeleteImage}
                    MenuText { menuItemText: "DELETE" }

                    MouseArea {
                        anchors.fill: parent
                        // na hover
                        hoverEnabled: true
                        onEntered: { connectionDeleteImage.opacity = 1}
                        onExited: { connectionDeleteImage.opacity = 0.65}

                        onClicked: {
                            // tohle
                            viewport.mode = 'deleteWire'
                        }
                    }
                }
            }


            /**
             * DEBUG
             * prace s debuggem
             */

            Row {
                spacing: menuContainer.blockSep;

                Rectangle {
                    width: menuContainer.blockSize
                    height: viewport.heightRedc
                    color: 'transparent'
                    id: debugStart

                    MenuImage { menuImagePath: "../menu/start.svg"; id: debugStartImage }
                    MenuText { menuItemText: "START" }

                    MouseArea {
                        anchors.fill: parent
                        // na hover
                        /*
                        hoverEnabled: true
                        onEntered: { connectionAddImage.opacity = 1}
                        onExited: { connectionAddImage.opacity = 0.65}*/

                        onClicked: {
                            var ret = wrap.eval_block();
                            // chyba
                            if (ret < 0) {
                                Err.create_error_msg('Missing values in block ' + Math.abs(ret));
                            } else if (ret > 0) {
                                for (var key in wrap.get_Q_block(ret).out_ports) {
                                    for (var key2 in wrap.get_Q_block(ret).out_ports[key]) {
                                        if (key2 !== 'port_name') {
                                            viewport.blocks[String(ret)].
                                            outPorts[key].pins[key2].
                                            value = wrap.get_Q_block(ret).out_ports[key][key2].value;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    width: menuContainer.blockSize
                    height: viewport.heightRedc
                    color: 'transparent'
                    id: debugStep

                    MenuImage { menuImagePath: "../menu/step.svg"; id: debugStepImage }
                    MenuText { menuItemText: "STEP" }

                    MouseArea {
                        anchors.fill: parent
                        // na hover
                        /*
                        hoverEnabled: true
                        onEntered: { connectionAddImage.opacity = 1}
                        onExited: { connectionAddImage.opacity = 0.65}*/

                        onClicked: {

                        }
                    }
                }

                Rectangle {
                    width: menuContainer.blockSize
                    height: viewport.heightRedc
                    color: 'transparent'
                    id: debugSkip

                    MenuImage { menuImagePath: "../menu/skip.svg"; id: debugSkipImage }
                    MenuText { menuItemText: "SKIP" }

                    MouseArea {
                        anchors.fill: parent
                        // na hover
                        /*
                        hoverEnabled: true
                        onEntered: { connectionAddImage.opacity = 1}
                        onExited: { connectionAddImage.opacity = 0.65}*/

                        onClicked: {

                        }
                    }
                }

                Rectangle {
                    width: menuContainer.blockSize
                    height: viewport.heightRedc
                    color: 'transparent'
                    id: debugAbort

                    MenuImage { menuImagePath: "../menu/abort.svg"; id: debugAbortImage }
                    MenuText { menuItemText: "ABORT" }

                    MouseArea {
                        anchors.fill: parent
                        // na hover
                        /*
                        hoverEnabled: true
                        onEntered: { connectionAddImage.opacity = 1}
                        onExited: { connectionAddImage.opacity = 0.65}*/
                    }
                }
            }
        }
    }
}
