CC = gcc
CXX = g++

INC_DIR = ./inc

CFLAGS = -c -Wall
CFLAGS += -g
CFLAGS += -pipe -fomit-frame-pointer -fpic -D_MP_MODE 
CFLAGS += -I$(INC_DIR)
CXXFLAGS = $(CFLAGS)

LFLAGS=-shared

HEADS = $(wildcard $(INC_DIR)/*.h)

SOURCES = $(wildcard ./src/*.c)
OBJS = $(SOURCES:.c=.o)
TARGET = ./libiop.so

all:$(TARGET)
$(TARGET):$(OBJS) $(SOURCES) $(HEADS)
	$(CC) -o $@ $(OBJS) $(LFLAGS)

$(OBJS):%.o:%.c $(HEADS) 
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS) ./$(TARGET)
