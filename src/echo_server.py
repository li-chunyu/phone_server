#!/usr/bin/python3
import tornado.web
import tornado.websocket
import tornado.httpserver
import tornado.ioloop

import wave
import time


class WebSocketHandler(tornado.websocket.WebSocketHandler):
    def __init__(self, application, request, **kwargs):
        super().__init__(application, request, **kwargs)
        self.buf = bytes()

    def on_message(self, message):
        self.buf += message
        print('got data of:', len(message), 'bytes.')

    def on_close(self):
        t = time.ctime().split()[3].split(':')
        t = ''.join(t)
        f = wave.open(t+'.wav', 'wb')
        f.setnchannels(1)
        f.setsampwidth(2)
        f.setframerate(8000)
        f.writeframes(self.buf)
        f.close()
        print('>>>>>>>>>closed<<<<<<<<')
    
    def open(self):
        print("connection open")

class IndexPageHandler(tornado.web.RequestHandler):
    def get(self):
        self.render("websockets.html")


class Application(tornado.web.Application):
    def __init__(self):
        handlers = [
            (r'/', IndexPageHandler),
            (r'/ws', WebSocketHandler)
        ]

        settings = {
            'template_path': 'static'
        }
        tornado.web.Application.__init__(self, handlers, **settings)


if __name__ == '__main__':
    ws_app = Application()
    server = tornado.httpserver.HTTPServer(ws_app)
    server.listen(8888)
    tornado.ioloop.IOLoop.instance().start()
