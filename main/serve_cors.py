#! /usr/bin/env python3

from http.server import HTTPServer, BaseHTTPRequestHandler, SimpleHTTPRequestHandler, test

class MyHTTPRequestHandler(SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_my_headers()

        SimpleHTTPRequestHandler.end_headers(self)

    def send_my_headers(self):
        self.send_header("Access-Control-Allow-Origin", "*")


if __name__ == '__main__':
    test(HandlerClass=MyHTTPRequestHandler, port=8080)