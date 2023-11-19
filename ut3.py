#/usr/bin/env python3
# encoding: utf-8

from ctypes import CDLL
from pathlib import Path

curdir = Path(__file__).resolve().parent
CDLL(curdir / "libc-icap-python.so")

