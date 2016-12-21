#!/usr/bin/env python

from flask import Flask, render_template
from flask_socketio import SocketIO
import facom

app = Flask(__name__)
app.config['SECRET_KEY'] = 'Development key'        # Change this!
app.config['DEBUG'] = True                          # Change this!

socket = SocketIO(app)

plc = facom.Facom()
plc.open('/dev/ttyUSB0')
plc.set_discrete(facom.DISCRETE_M, 4, facom.ACTION_RESET)

@app.route('/')
def index():
    return render_template('index.html')

@socket.on('right')
def right():
    print plc.set_discrete(facom.DISCRETE_M, 42, facom.ACTION_SET)

@socket.on('left')
def left():
    print plc.set_discrete(facom.DISCRETE_M, 43, facom.ACTION_SET)

@socket.on('up')
def up():
    plc.set_discrete(facom.DISCRETE_M, 40, facom.ACTION_SET)

@socket.on('down')
def down():
    plc.set_discrete(facom.DISCRETE_M, 41, facom.ACTION_SET)

@socket.on('automatic')
def automatic(automatic):
    if automatic:
        plc.set_discrete(facom.DISCRETE_M, 4, facom.ACTION_SET)
    else:
        plc.set_discrete(facom.DISCRETE_M, 4, facom.ACTION_RESET)

@socket.on('rotate_up')
def rotate_up():
    plc.set_discrete(facom.DISCRETE_M, 44, facom.ACTION_SET)

@socket.on('rotate_down')
def rotate_down():
    plc.set_discrete(facom.DISCRETE_M, 45, facom.ACTION_SET)

@socket.on('extend')
def extend(extend):
    if extend:
        plc.set_discrete(facom.DISCRETE_M, 46, facom.ACTION_SET)
    else:
        plc.set_discrete(facom.DISCRETE_M, 47, facom.ACTION_SET)

@socket.on('grab')
def pick(pick):
    if pick:
        plc.set_discrete(facom.DISCRETE_M, 48, facom.ACTION_SET)
    else:
        plc.set_discrete(facom.DISCRETE_M, 49, facom.ACTION_SET)

def updateAll():
    emit('update', {
                        "automatic": "Manual"
                   })

if __name__ == '__main__':
    socket.run(app, host = '0.0.0.0', port = 5001)

