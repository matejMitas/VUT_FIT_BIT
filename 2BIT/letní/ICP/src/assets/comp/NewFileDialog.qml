import QtQuick 2.0
import QtQuick.Dialogs 1.3

MessageDialog {
    id: newFile
    title: "Overwrite?"
    text: "You're about to create new canvas, do you have everything saved?\nIf not, hit cancel and save it"
    standardButtons: StandardButton.Yes | StandardButton.No
    Component.onCompleted: visible = true
    onYes: {
        // vytvorime nove schema na backendu
        wrap.new_schema();
        // vymazame bloky z view
        for (var key in viewport.blocks)
            viewport.blocks[key].destroy()
        // zahodime uchovane informace o blocich
        viewport.blocks = {};

        newFile.destroy();
    }
    onNo: {
        newFile.destroy();
    }
}
