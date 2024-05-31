import serial
from flask import Flask, render_template
from flask_socketio import SocketIO, emit

last="calc"

app = Flask(__name__)
socketio = SocketIO(app)
arduino=serial.Serial('/dev/ttyACM0',9600,timeout=0)

@app.route("/<path:p>", methods=['GET', 'POST'])
def serve_static(p):
    return app.send_static_file(p)

@app.route("/")
def serve_homepage():
    return app.send_static_file("index2.html")

@app.route('/jcp')
def jcp():
    if(arduino.in_waiting>0):
        ret = arduino.readline()
        last=ret.decode()
        print(ret.decode())
        return render_template('index.html', result=ret.decode())
    else:
        return render_template('index.html', result=last)

def send_string_to_html(result_text):
    socketio.emit('update_string', {'data': result_text})

@socketio.on('connect')
def handle_connect():
    print('Client connected')

if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', debug=True)

