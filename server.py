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

if __name__ == '__main__':
    socket.run(app, host = '0.0.0.0', port = 5001)

