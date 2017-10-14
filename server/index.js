#!/usr/bin/env node
var express = require('express');
var WebSocketServer = require('websocket').server;
var http = require('http');
var requestURL = require('request');
var app = express();
var path = require('path');

var server = http.createServer(app, function (request, response) {
    console.log((new Date()) + ' Received request for ' + request.url);
    response.writeHead(404);
    response.end();
});

app.use("/public", express.static(__dirname + '/public'));
app.get('/ravi', function (req, res) {
    res.sendFile(path.join(__dirname + '/ravi.html'));
});


server.listen(3000, function () {
    console.log((new Date()) + ' Server is listening on port 3000');
});

wsServer = new WebSocketServer({
    httpServer: server,
    autoAcceptConnections: false
});

function originIsAllowed(origin) {
    // put logic here to detect whether the specified origin is allowed. 
    return true;
}

var connections = [];   // websocekts connected to this server

wsServer.on('request', function (request) {

    if (!originIsAllowed(request.origin)) {
        // Make sure we only accept requests from an allowed origin 
        request.reject();
        console.log((new Date()) + ' Connection from origin ' + request.origin + ' rejected.');
        return;
    }

    var connection = request.accept("", request.origin);

    connections.push(connection);

    console.log((new Date()) + ' Connection accepted.');

    connection.on('message', function (message) {
        if (message.type === 'utf8') {

            var msg = message.utf8Data;

            console.log('Received Message: ' + msg);

            if (msg == "resloved") {
                connections.forEach(function (conn) {
                    conn.sendUTF("resloved");
                });
            } else if (msg.startsWith("buzz")) {

                console.log("innn");

                connections.forEach(function (conn) {
                    var res = msg.split("-");

                    var pos = parseInt(res[1]) - 1;

                    conn.sendUTF(pos);
                });
            }
        }
        else if (message.type === 'binary') {
            console.log('Received Binary Message of ' + message.binaryData.length + ' bytes');
            //connection.sendBytes(message.binaryData);
        }
    });

    connection.on('close', function (reasonCode, description) {
        console.log((new Date()) + ' Peer ' + connection.remoteAddress + ' disconnected.');
    });

    connection.sendUTF("Hallo Client!");
});