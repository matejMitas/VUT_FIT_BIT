import QtQuick 2.4
import QtQuick.Dialogs 1.3

import "../js/generate.js" as Gen

FileDialog {
    id: fileDialog
    title: "Please choose a file"
    folder: shortcuts.home
    selectExisting: false
    onAccepted: {
        // zavolat jirkovu funkci
        Gen.save_to_file(fileDialog.fileUrls);
    }
    Component.onCompleted: visible = true
}
