var socket;

$(document).ready(function() {
    socket = io.connect("http://" + document.domain + ":" + location.port);

    $("#btnModeSelect").click(function() {
        if($(this).text() == "Automatic") {
            socket.emit("automatic", true);
            $(this).text("Manual");
        } else {
            socket.emit("automatic", false);
            $(this).text("Automatic");
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
            $(this).text("Rotate Down");
        } else {
            socket.emit("rotate_down");
            $(this).text("Rotate Up");
        }
    });

    $("#btnExtend").click(function() {
        if($(this).text() == "Extend") {
            socket.emit("extend", true);
            $(this).text("Un Extend");
        } else {
            socket.emit("extend", false);
            $(this).text("Extend");
        }
    });

    $("#btnGrab").click(function() {
        if($(this).text() == "Pick") {
            socket.emit("grab", true);
            $(this).text("Place");
        } else {
            socket.emit("grab", false);
            $(this).text("Pick");
        }
    });
});

