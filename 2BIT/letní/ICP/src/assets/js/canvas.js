function drawGrid() {
    var cols = viewport.width / viewport.gridSize;
    var rows = viewport.height / viewport.gridSize;
    // vygenerujeme sloupce
    for (var i = 0; i < cols; i++) {
        drawLine(
            gridCanvas,
            0,
            gridColor,
            i*viewport.gridSize,
            0,
            i*viewport.gridSize,
            viewport.height
        );
    }
    // vygenerujeme radky
    for (var j = 0; j < rows; j++) {
        drawLine(
            gridCanvas,
            0,
            gridColor,
            0,
            j*viewport.gridSize,
            viewport.width,
            j*viewport.gridSize
        );
    }
}



function drawLine(id, lineWidth, lineColor, x1, y1, x2, y2) {
    var ctx = id.getContext("2d");
    // reset platna pred prekresleni
    //ctx.reset();
    // samotna cesta
    ctx.beginPath();
    ctx.moveTo(x1, y1);
    ctx.lineTo(x2, y2);
    ctx.lineWidth = lineWidth;
    ctx.strokeStyle = lineColor;
    ctx.stroke();
    // prekresleni platna
    id.requestPaint();
}

function canvasInit(id) {
    var ctx = id.getContext("2d");
        ctx.clearRect(0,0,width,height);
}

function updateCanvas(id) {
    var ctx = id.getContext("2d");
    // reset platna pred prekresleni
    ctx.reset();
    // znovuprekresleni vsech car
    for (var key in connections) {
        var obj = connections[key].pos
        drawLine(id, 1, 'black', obj[0], obj[1], obj[2], obj[3])
    }
    // prekresleni platna
    id.requestPaint();
}


function size(a,b) {
   return Math.round(Math.sqrt(Math.pow(a.x-b.x, 2) + Math.pow(a.y-b.y, 2)))
}

function detect(a, b, c) {

    var click = size(a,c) + size(b,c)
    var line = size(a,b)
    var tolerance = 1

    if (click > (line - tolerance) && click < (line + tolerance)) {
       return true;
    } return false;
}
