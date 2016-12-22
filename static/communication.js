var socket;

$(document).ready(function() {
    socket = io.connect("http://" + document.domain + ":" + location.port);

    socket.on('update', function(data) {
        $("#btnModeSelect").text(data.automatic);
        $("#btnRotate").text(data.rotation);
        $("#btnExtend").text(data.extend);
        $("#btnGrab").text(data.grab);
    });

    $("#btnModeSelect").click(function() {
        if($(this).text() == "Automatic") {
            socket.emit("automatic", true);
        } else {
            socket.emit("automatic", false);
        }
    });

    $("#btnLeft").click(function() {
        socket.emit("left");
    });

    $("#btnRight").click(function() {
        socket.emit("right");
    });

    $("#btnUp").click(function () {
        socket.emit("up");
    });

    $("#btnDown").click(function() {
        socket.emit("down");
    });

    $("#btnRotate").click(function() {
        if($(this).text() == "Rotate Up") {
            socket.emit("rotate_up");
        } else {
            socket.emit("rotate_down");
        }
    });

    $("#btnExtend").click(function() {
        if($(this).text() == "Extend") {
            socket.emit("extend", true);
        } else {
            socket.emit("extend", false);
        }
    });

    $("#btnGrab").click(function() {
        if($(this).text() == "Pick") {
            socket.emit("grab", true);
        } else {
            socket.emit("grab", false);
        }
    });
});

