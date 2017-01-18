#!/usr/bin/env python

from flask import Flask, render_template
from flask_socketio import SocketIO, emit
import robotichand

app = Flask(__name__)
app.config['SECRET_KEY'] = 'Development key'        # Change this!
app.config['DEBUG'] = True                          # Change this!

socket = SocketIO(app)

robotic_hand = robotichand.RoboticHand()

@app.route('/')
def index():
    return render_template('index.html')

@socket.on('connect')
def connect():
    updateAll()

@socket.on('right')
def right():
    robotic_hand.move_right()

@socket.on('left')
def left():
    robotic_hand.move_left()

@socket.on('up')
def up():
    robotic_hand.move_up()

@socket.on('down')
def down():
    robotic_hand.move_down()

@socket.on('automatic')
def automatic(automatic):
    if automatic:
        robotic_hand.automatic(True)
    else:
        robotic_hand.automatic(False)
    updateAll();

@socket.on('rotate_up')
def rotate_up():
    robotic_hand.rotate_up()
    updateAll()

@socket.on('rotate_down')
def rotate_down():
    robotic_hand.rotate_down()
    updateAll();

@socket.on('extend')
def extend(extend):
    if extend:
        robotic_hand.extend(True)
    else:
        robotic_hand.extend(False)
    updateAll();

@socket.on('grab')
def pick(pick):
    if pick:
        robotic_hand.grab(True)
    else:
        robotic_hand.grab(False)
    updateAll();

def updateAll():
    emit('update', robotic_hand.states())

if __name__ == '__main__':
    socket.run(app, host = '0.0.0.0', port = 5001)

