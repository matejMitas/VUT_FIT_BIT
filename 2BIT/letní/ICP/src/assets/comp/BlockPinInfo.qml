import QtQuick 2.0

Rectangle {
    width: 80
    height: 30
    x: -85
    y: -7.5
    color: '#ccc'

    property var name: "nazev portu"
    property var value: "undefined"
    property var portType

    Column {
        spacing: 2
        width: parent.width

        Text {
            text: name
            font.family: "Helvetica"
            font.pointSize: 10
            color: "black"
            x: 5
        }

        Rectangle {
            width: parent.width
            height: 1
            color: 'white'
        }

        Text {
            text: value
            x: 5
        }
    }
}
