#/usr/bin/env python3
# encoding: utf-8

from ctypes import CDLL
import os
import sys

fn = os.path.join(os.curdir,"libc-icap-python.so")
CDLL(fn)
