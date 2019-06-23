function createBlock() {
    // vytvoreni aside
    var asideCtx = Qt.createComponent("qrc:/assets/comp/AsideWrapBlock.qml");
    activeAsideCtx = asideCtx.createObject(aside);
}

function aside_gen(type) {

    if (activeAsideObj) {
        activeAsideObj.destroy()
        activeAsideCtx.destroy()
    }

    var asideTemp = Qt.createComponent("qrc:/assets/comp/Aside.qml");
    activeAsideObj = asideTemp.createObject(viewport);

    // podle typu vytvorime obsah
    asideTemp = Qt.createComponent("qrc:/assets/comp/AsideWrapBlock.qml");
    activeAsideCtx = asideTemp.createObject(activeAsideObj);

    if (type === 'add')
        viewport.mode = 'add'
    else if (type === 'edit')
        viewport.mode = 'edit'
}
