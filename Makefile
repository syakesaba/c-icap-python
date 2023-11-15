#!/usr/bin/env make
# encoding: utf-8

DEBUG = 0

# apt install make gcc
# apt install python3-dev libicapapi-dev libssl-dev

CC = gcc
RM = rm -f
RMDIR = rm -rf

PYTHON_CFLAGS = $(shell python3-config --cflags)
PYTHON_LDFLAGS = $(shell python3-config --ldflags)
C_ICAP_CFLAGS = $(shell c-icap-libicapapi-config --cflags)
C_ICAP_LDLAGS = $(shell c-icap-libicapapi-config --libs)

SRCDIR = src
INCDIR = include

SRCS = $(shell find $(SRCDIR) -type f -name "*.c")
OBJS = $(SRCS:.c=.o)
LIBS = -lpython -licapapi
TARGET = libc-icap-python.so

CFLAGS = -fPIC -O4 -Wall -I$(INCDIR) $(PYTHON_CFLAGS) $(C_ICAP_CFLAGS)
LDFLAGS = -shared $(PYTHON_LDFLAGS) $(C_ICAP_LDLAGS)

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	-$(RM) $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

#apt install graphviz 
DOT := dot
DOT_TARGET := flow.dot
DOT_OPTIONS ?= -Tjpg

.PHONY: flow
flow: $(DOT_TARGET)
	$(DOT) $(DOT_OPTIONS) $(DOT_TARGET) -o $(DOT_TARGET:.dot=.jpg)

.PHONY: cleanflow
cleanflow:
	-$(RM) $(DOT_TARGET:.dot=.jpg)

#apt install doxygen
DOXYGEN := doxygen
DOXYFILE := c-icap-python.doxyfile
DOXYGEN_TARGET_SRC := $(SRCDIR)
DOXYGEN_TARGET := doc

install: $(TARGET)

.PHONY: doc
doc: $(DOXYGEN_TARGET_SRC) $(DOXYFILE)
	$(DOXYGEN) $(DOXYFILE)

.PHONY: cleandoc
cleandoc:
	$(RMDIR) $(DOXYGEN_TARGET)

# ALL

.PHONY: cleanall
cleanall:
	make clean
	make cleanflow
	make cleandoc
