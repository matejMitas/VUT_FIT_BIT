import QtQuick 2.0
import QtQuick.Dialogs 1.3

FileDialog {
    id: fileDialog
    title: "Please choose a file"
    folder: shortcuts.home
    onAccepted: {
        console.log(fileDialog.fileUrls)
        // zavolat jirkovu funkci
    }
    Component.onCompleted: visible = true
}
