CC = gcc
CXX = g++

INC_DIR = ../inc

CFLAGS = -c -W
CFLAGS += -pipe -fomit-frame-pointer -fpic -D_MP_MODE 
CFLAGS += -I$(INC_DIR)
CXXFLAGS = $(CFLAGS)
SOCK_LIB = 
LFLAGS=-L.. -liop -lpthread

HEADS = $(wildcard $(INC_DIR)/*.h)

SOURCES = iop_test.cpp
OBJS = $(SOURCES:.cpp=.o)
TARGET = test

all:$(TARGET)
$(TARGET):$(OBJS) $(SOURCES) $(HEADS)
	$(CXX) -o $@ $(OBJS) $(LFLAGS)

$(OBJS):%.o:%.cpp $(HEADS) 
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJS) ./$(TARGET)
