#!/usr/bin/env make
# encoding: utf-8

# apt install make gcc
# apt install python{2,3}-dev libicapapi-dev libssl-dev

# DEFINE VARIABLES
#PYTHON_VERSION = 3
PYTHON_VERSION = 2
DEBUG = 0
CC = gcc
RM = rm -f
RMDIR = rm -rf
SRCDIR = src
INCDIR = include

# DEFINE VARIABLE SET
SRCS = $(shell find $(SRCDIR) -type f -name "*.c")
OBJS = $(SRCS:.c=.o)
DEFS = -DPYTHON_VERSION=$(PYTHON_VERSION) -DDEBUG=$(DEBUG)
INCS = -I$(INCDIR)

# PYTHON VERSION 2 OR 3
ifeq ($(PYTHON_VERSION),2)
	PYTHON_CONFIG = python2-config
endif
ifeq ($(PYTHON_VERSION),3)
	PYTHON_CONFIG = python3-config
endif

# COMPILE FLAGS
PYTHON_CFLAGS = $(shell $(PYTHON_CONFIG) --cflags)
PYTHON_LDFLAGS = $(shell $(PYTHON_CONFIG) --ldflags)
C_ICAP_CFLAGS = $(shell c-icap-libicapapi-config --cflags)
C_ICAP_LDLAGS = $(shell c-icap-libicapapi-config --libs)
CFLAGS = $(PYTHON_CFLAGS) $(C_ICAP_CFLAGS) $(INCS) $(DEFS) -fPIC -O4 -Wall
LDFLAGS = $(PYTHON_LDFLAGS) $(C_ICAP_LDLAGS) -shared

# SHARED OBJECT NAME
TARGET = libc-icap-python.so

.PHONY: all
all: $(TARGET)

.PHONY: clean
clean:
	-$(RM) $(TARGET) $(OBJS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

.PHONY: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

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

#apt install docker-ce

.PHONY: docker
docker: Dockerfile
	make cleanall
	docker build . -t c-icap-python --build-arg pv=$(PYTHON_VERSION)
	yes | docker system prune


# ALL

.PHONY: cleanall
cleanall:
	make clean
	make cleanflow
	make cleandoc
