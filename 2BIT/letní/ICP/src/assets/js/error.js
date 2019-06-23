function create_error_msg(msg) {
    var tempError = Qt.createComponent("qrc:/assets/comp/Error.qml");
    var err = tempError.createObject(viewport);
    err.name = msg;
}
