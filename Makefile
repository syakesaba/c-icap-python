#!/usr/bin/env make
#encoding: utf-8

# apt install make gcc
# apt install python3-dev libicapapi-dev libssl-dev

CC = gcc
RM = rm -f
RMDIR = rm -rf

# Depends
PYTHON_CFLAGS = $(shell python3-config --cflags)
PYTHON_LDFLAGS = $(shell python3-config --ldflags)
C_ICAP_CFLAGS = $(shell c-icap-libicapapi-config --cflags)
C_ICAP_LDLAGS = $(shell c-icap-libicapapi-config --libs)

CFLAGS = -fPIC -O4 -Wall $(PYTHON_CFLAGS) $(C_ICAP_CFLAGS)
LDFLAGS = -shared $(PYTHON_LDFLAGS) $(C_ICAP_LDLAGS)

TARGET = libc-icap-python.so
LIBS = -lpython -licapapi
SRCDIR = src
SRCS = $(shell find $(SRCDIR) -name "*.c")
INCS = $(shell find $(SRCDIR) -name "*.h")
OBJS = $(SRCS:.c=.o)

.PHONY: all
all: $(TARGET)

.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	-$(RM) $(TARGET) $(OBJS)

$(TARGET): $(SOURCE)
	$(CC) $(FLAGS) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $< $(INCLUDES)

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

#doc need doxygen
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