#!/usr/bin/env python

from importlib import import_module
import os
from flask import Flask, render_template, Response
import socket
import sys
import cv2

HOST, PORT = "localhost", 1000
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# try:
    # Connect to server
sock.connect((HOST, PORT))

app = Flask(__name__)

# @app.route('/')
# def index():
#     """Video streaming home page."""
#     return render_template('index.html')

@app.route('/')
@app.route('/<cmd>')
def index(cmd=None):
    if cmd == 'go':
        data='g'
        sock.sendall(bytes(data + "\n", "utf-8"))
        print("go")
    elif cmd=='stop':
        data='s'
        sock.sendall(bytes(data + "\n", "utf-8"))
        print("stop")
    return render_template('index.html',cmd=cmd)


def gen():
    """Video streaming generator function."""
    # camera = cv2.VideoCapture(0)
    # if not camera.isOpened():
    #         raise RuntimeError('Could not start camera.')
    while True:
        # _, img = camera.read()
        img=cv2.imread("frame.jpg")
        frame = cv2.imencode('.jpg', img)[1].tobytes()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')


@app.route('/video_feed')
def video_feed():
    """Video streaming route. Put this in the src attribute of an img tag."""
    return Response(gen(),
                    mimetype='multipart/x-mixed-replace; boundary=frame')


if __name__ == '__main__':
    app.run(host='0.0.0.0', threaded=True)
