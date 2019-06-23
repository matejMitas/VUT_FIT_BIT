import QtQuick 2.2
import QtQuick.Dialogs 1.1

MessageDialog {
    id: messageDialog

    property var name: ""

    title: "Error"
    text: name
    onAccepted: {messageDialog.destroy()}

    Component.onCompleted: visible = true
}
