import QtQuick 2.0
import QtGraphicalEffects 1.0

Image {
    property var menuImagePath: ""
    width: 24; height: 24
    x: 8
    y: 1
    source: menuImagePath
    anchors.horizontalCenter: parent.horizontalCenter
    opacity: 0.65
}
