#!/bin/sh
gunicorn app:app -w 2 -b 0.0.0.0:8080 -t 30