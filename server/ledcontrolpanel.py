from flask import redirect, Flask
from serial import Serial, serialutil
from json import dumps


class App(Flask):
    def __init__(self, *args, **kwargs):
        self.port = '/dev/serial/by-id/usb-Arduino__www.arduino.cc__0043_5573132303635171A0A2-if00' if 'serport' not in kwargs.keys() else \
            kwargs['serport']
        try:
            del kwargs['serport']
        except KeyError:
            pass
        super().__init__(*args, **kwargs)
        self.register_error_handler(404, self.index)
        self.add_url_rule('/data/', view_func=self.initializePage)
        self.add_url_rule('/reset/', view_func=self.reset, methods=['POST'])
        self.add_url_rule('/led/<ledid>/<mode>', view_func=self.led, methods=['POST'])
        self.add_url_rule('/animation/<anim>/', view_func=self.animation, methods=['POST'])
        self.serial = None
        self.data = 0

    def __enter__(self):
        self.serial = Serial(port=self.port)
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.serial.close()

    def index(self, x):
        return redirect('/static/Led-controllor.html')

    def reset(self):
        self.data = 0
        self.serial.write(b's0\n')
        return ""

    def led(self, ledid, mode):
        ledid = int(ledid)
        if ledid < 16:
            if mode == 'fas':
                self.data &= ~(1 << ledid)
            if mode == 'far':
                self.data |= (1 << ledid)
        return self.initializePage()

    def initializePage(self):
        try:
            self.serial.write(b's' + str(self.data).encode() + b'\n')
        except (serialutil.SerialException, OSError):
            self.serial.close()
            self.serial.open()
            self.serial.write(b's' + str(self.data).encode() + b'\n')
        dt = bin(self.data)[2:]
        dt = '0' * (16 - len(dt)) + dt
        dct = {}
        for i in range(len(dt)):
            dct['#{} i'.format(15 - i)] = 'far' if dt[i] == '0' else 'fas'
        return dumps(dct)

    def animation(self, anim):
        if anim[:4] == 'anim':
            anim = anim[4:]
        try:
            self.serial.write(b'a' + anim.encode() + b'\n')
        except (serialutil.SerialException, OSError):
            self.serial.close()
            self.serial.open()
            self.serial.write(b'a' + anim.encode() + b'\n')
        return ''


with App(__name__) as app:
    app.run()
