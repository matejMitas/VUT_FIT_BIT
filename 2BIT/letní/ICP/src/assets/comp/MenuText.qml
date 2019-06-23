import QtQuick 2.0

Text {
    property var menuItemText: ""
    text: menuItemText
    color: '#888'
    font.pointSize: 9
    font.bold: true
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
}
