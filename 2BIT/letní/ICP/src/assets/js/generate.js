function block_size() {
    // pocitani vysky bloku
    var block = viewport.blocks[String(activeBlock)];

    var inCount = (Object.keys(block.inPorts).length - 1) * 10;
    for (var port in block.inPorts) {
        var pins = 0;
        for (var pin in block.inPorts[port].pins)
            pins++;
        inCount += pins < 3 ? 20 : (5 * pins) + (5 * (pins - 1));
    }

    var outCount = (Object.keys(block.outPorts).length - 1) * 10;

    for (var outPort in block.outPorts) {
        var outPins = 0;
        for (var outPin in block.outPorts[outPort].pins) {
            outPins++;
        }
        outCount += outPins < 3 ? 20 : (5 * outPins) + (5 * (outPins - 1));
    }
    //console.log("Vyska vstupu " + inCount + " a vystupu " + outCount)
    var newSize = (outCount > inCount) ? outCount : inCount;
    block.height = (newSize > 80) ? newSize : 80;
}

function save_to_file(inPath) {
    var p = String(inPath);
    wrap.save_schema(p.slice(7))
}

function block_gen(generator) {
    // vytvorime v Backendu blok, jedine co nas zajima je jeho id
    var id = wrap.add_block();
    // vytvorime u nas fyzickou komponentu
    var tempBlock = Qt.createComponent("qrc:/assets/comp/Block.qml");
    viewport.blocks[String(id)] = tempBlock.createObject(viewport);
    // vytvorime pozici
    var x, y;

    // nastaveni pozice bud generovane pri kliku na add
    // nebo pouziti
    if (!generator) {
        x = (viewport.width - 300) / 2 - 40;
        y = viewport.height / 2 - 40;
    } else {
        x = generator.x
        y = generator.y
    }

    // ulozime ji do backendu
    wrap.set_pos(id, x, y);
    //
    viewport.blocks[String(id)].x = x;
    viewport.blocks[String(id)].y = y;
    viewport.blocks[String(id)].blockId = id;
    // vratime id a podle nej budeme indexovat
    return id;
}

function block_port_gen(portName, portType) {
    var block = viewport.blocks[String(activeBlock)];
    var retVal;

    block.createPort(portName, portType);
    if (portType === 'in')
        retVal = wrap.add_port(activeBlock, portName, 1)
    else
        retVal = wrap.add_port(activeBlock, portName, 0)
    // pocitani vysky bloku
    block_size();

    return retVal;
}

function block_pin_gen(portName, portType, pinName, pinType) {
    if (portType === 'in') {
        viewport.
        blocks[String(activeBlock)].
        inPorts[String(portName)].
        createPin(pinName, pinType);
        // pridam vstupni pin, neni co vymyslet
        wrap.add_pin(activeBlock, portName, pinName, 1, pinType)
    } else if (portType === 'out') {
        viewport.
        blocks[String(activeBlock)].
        outPorts[String(portName)].
        createPin(pinName, pinType);
        // vystupni pin muze byt pridan az po pripojeni operace, BlockPin.qml
    }
    // pocitani vysky bloku
    block_size();
}

function block_pin_value(portName, portType, pinName, value) {
    if (portType === 'in') {
        viewport.blocks[String(activeBlock)].inPorts[String(portName)].pins[pinName].value = value
        wrap.set_Value(activeBlock, portName, pinName, parseFloat(value));
    } else if (portType === 'out') {
        viewport.blocks[String(activeBlock)].outPorts[String(portName)].pins[pinName].value = value
    }
}

function block_pin_oper(sourcePort, sourcePin, port1, port2, pin1, pin2, oper, t) {
    wrap.add_pin(activeBlock, sourcePort, sourcePin, 0, t);
    wrap.add_operand_to_vector(activeBlock, port1, pin1);
    wrap.add_operand_to_vector(activeBlock, port2, pin2);
    wrap.set_function(activeBlock, sourcePort, sourcePin, oper);
}


function block_delete(blockId) {
    // vymazeme QML objekt a uklidime v nasem containeru
    viewport.blocks[String(blockId)].destroy();
    delete viewport.blocks[String(blockId)];
    // vymazeme v backendu
    wrap.delete_block(blockId);
}

function block_port_delete(blockId, portName, portType) {
    if (portType === 'in') {
        // vymazeme QML objekt a uklidime v nasem containeru
        viewport.blocks[String(blockId)].inPorts[portName].destroy();
        delete viewport.blocks[String(blockId)].inPorts[portName];
        // vymazeme v backendu
        wrap.remove_port(blockId, portName, 1);
    } else if (portType === 'out') {
        // vymazeme QML objekt a uklidime v nasem containeru
        viewport.blocks[String(blockId)].outPorts[portName].destroy();
        delete viewport.blocks[String(blockId)].outPorts[portName];
        // vymazeme v backendu
        wrap.remove_port(blockId, portName, 0);
    }
    // pocitani vysky bloku
    block_size();
}

function block_pin_delete(portName, portType, pinName) {
    if (portType === 'in') {
        // vymazeme QML objekt a uklidime v nasem containeru
        viewport.blocks[String(activeBlock)].inPorts[portName].pins[pinName].destroy();
        delete viewport.blocks[String(activeBlock)].inPorts[portName].pins[pinName];

    } else if (portType === 'out') {
        // vymazeme QML objekt a uklidime v nasem containeru
        viewport.blocks[String(activeBlock)].outPorts[portName].pins[pinName].destroy();
        delete viewport.blocks[String(activeBlock)].outPorts[portName].pins[pinName];
    }
    // pocitani vysky bloku
    block_size();
}

function save_file(path) {

}

//function block_edit_value
function create_sample_data() {
    // vygenerujeme blok
    activeBlock = Gen.block_gen({'x': 200, 'y': 150});

    Gen.block_port_gen('in_plus', 'in');
    Gen.block_pin_gen('in_plus', 'in', 'in_1', 1);
    Gen.block_pin_gen('in_plus', 'in', 'in_2', 1);

    Gen.block_pin_value('in_plus', 'in', 'in_1', '5')
    Gen.block_pin_value('in_plus', 'in', 'in_2', '15')

    Gen.block_port_gen('out_plus', 'out');
    Gen.block_pin_gen('out_plus', 'out', 'out', 1);

    activeBlock = Gen.block_gen({'x': 200, 'y': 350});

    Gen.block_port_gen('sub_plus', 'in');
    Gen.block_pin_gen('sub_plus', 'in', 'in_1', 1);
    Gen.block_pin_gen('sub_plus', 'in', 'in_2', 1);

    Gen.block_pin_value('sub_plus', 'in', 'in_1', '7')
    Gen.block_pin_value('sub_plus', 'in', 'in_2', '8')

    Gen.block_port_gen('out_sub', 'out');
    Gen.block_pin_gen('out_sub', 'out', 'out', 1);

    activeBlock = Gen.block_gen({'x': 700, 'y': 150});

    Gen.block_port_gen('in', 'in');
    Gen.block_pin_gen('in', 'in', 'in_1', 1);
    Gen.block_pin_gen('in', 'in', 'in_2', 1);

    Gen.block_port_gen('out', 'out');
    Gen.block_pin_gen('out', 'out', 'out', 1);
}

function print_blocks() {
    // vytisteni vsech bloku
    for (var blockKey in blocks) {
        console.log("Blok: "+blockKey);

        console.log(" Vstupni porty:")
        for (var inPortKey in blocks[blockKey].inPorts) {
            console.log("   " + inPortKey);

            for (var inPortKeyPin in blocks[blockKey].inPorts[inPortKey].pins) {
                console.log("     " + blocks[blockKey].inPorts[inPortKey].pins[inPortKeyPin].name);
                console.log("       Hodnota: " + blocks[blockKey].inPorts[inPortKey].pins[inPortKeyPin].value);
                console.log("       Typ:     " + blocks[blockKey].inPorts[inPortKey].pins[inPortKeyPin].type);
            }
        }

        console.log(" Vystupni porty:")
        for (var outPortKey in blocks[blockKey].outPorts) {
            console.log("   " + outPortKey);

            for (var outPortKeyPin in blocks[blockKey].outPorts[outPortKey].pins) {
                console.log("     " + blocks[blockKey].outPorts[outPortKey].pins[outPortKeyPin].name);
                console.log("       Hodnota: " + blocks[blockKey].outPorts[outPortKey].pins[outPortKeyPin].value);
                console.log("       Typ:     " + blocks[blockKey].outPorts[outPortKey].pins[outPortKeyPin].type);
            }
        }

        console.log("------------------------------------");
    }
}
