// MyItem.qml
import QtQuick 2.0
import "../js/generate.js" as Gen
import "../js/test.js" as Test

Item {

    function myQmlFunction(msg) {
        console.log("Got message:", msg)
        console.log(viewport);

        return "some return value"
    }
}
