import GPIO as GPIO
from flask import Flask, Response, request
app=Flask("Sunflower")

@app.route("/<path:path>")
def serve_static(p):
    return app.send_static_file(p)

@app.rounte("/")
def serve_homepage():
    return app.send_static_file("index.html")

