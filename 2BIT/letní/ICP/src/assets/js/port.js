// typ pinu (IN, OUT) a barva
function createPort(dest, type, name) {
    // vytvorime u nas fyzickou komponentu
    var component = Qt.createComponent("qrc:/assets/comp/BlockPort.qml");
    var port = component.createObject(dest);

    port.type = type;
    port.name = name;

    // urcime jeji pozici podle typu pinu
    if (type === 'IN') {
        port.pinColor = 'magenta';
        port.x = -viewport.gridSize * 3;
        port.y = 30 * dest.inPorts.length;
        // nastavime sample jmeno
        port.name = "IN" + dest.inPorts.length;

        // pridame do naseho pole a pridame Jirkovi k bloku
        // TODO:
        dest.inPorts.push(port);
        // pro osetreni duplictiniho zvetsovani bloku
        var diffIn = dest.inPorts.length * 30 - dest.height;

        // porty na uz pretekaji
        // jestli je rozdil vyssi nez padding jednoho portu
        if (dest.inPorts.length > 3 && diffIn > 10)
            dest.height += 30

    } else if (type === 'OUT') {
        port.pinColor = 'navy';
        port.x = 80;
        port.y = 30 * dest.outPorts.length;
        // nastavime sample jmeno
        port.name = "OUT" + dest.outPorts.length;

        // pridame do naseho pole a pridame Jirkovi k bloku
        // TODO:
        dest.outPorts.push(port);
        // pro osetreni duplictiniho zvetsovani bloku
        var diffOut = dest.outPorts.length * 30 - dest.height;

        // porty na uz pretekaji
        // jestli je rozdil vyssi nez padding jednoho portu
        if (dest.outPorts.length > 3 && diffOut > 10)
            dest.height += 30
    }
}
