var socket;

$(document).ready(function() {
    socket = io.connect("http://" + document.domain + ":" + location.port);

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
});

