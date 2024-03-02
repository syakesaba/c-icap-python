#!/usr/bin/env make
# encoding: utf-8

# apt install make gcc
# apt install python{2,3}-dev libicapapi-dev libssl-dev

# DEFINE VARIABLES
PYTHON_VERSION = 3
#PYTHON_VERSION = 2
DEBUG = 0
CC = gcc
RM = rm -f
RMDIR = rm -rf
SRCDIR = src

# DEFINE VARIABLE SET
SRCS = $(shell find $(SRCDIR) -type f -name "*.c")
OBJS = $(SRCS:.c=.o)
DEFS = -DPYTHON_VERSION=$(PYTHON_VERSION) -DDEBUG=$(DEBUG)

# PYTHON VERSION 2 OR 3
ifeq ($(PYTHON_VERSION),2)
	PYTHON_CONFIG = python2-config
endif
ifeq ($(PYTHON_VERSION),3)
	PYTHON_CONFIG = python3-config
endif

# COMPILE FLAGS
PYTHON_CFLAGS = $(shell $(PYTHON_CONFIG) --cflags)
PYTHON_LDFLAGS = $(shell $(PYTHON_CONFIG) --embed --ldflags)
C_ICAP_CFLAGS = $(shell c-icap-libicapapi-config --cflags)
C_ICAP_LDLAGS = $(shell c-icap-libicapapi-config --libs)
CFLAGS = $(PYTHON_CFLAGS) $(C_ICAP_CFLAGS) $(DEFS) -O4 -Wall
LDFLAGS = $(PYTHON_LDFLAGS) $(C_ICAP_LDLAGS)

# SHARED OBJECT NAME
TARGET = run

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
