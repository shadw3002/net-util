TARGET  := main

CC      := clang++
LIBS    := -lpthread
INCLUDE:= -I./usr/local/lib
CFLAGS  := -std=c++17 -g -Wall -O3 -D_PTHREADS
CXXFLAGS:= $(CFLAGS)

SOURCE  := $(wildcard *.cpp)
OBJS    := $(patsubst %.cpp,%.o,$(SOURCE))

.PHONY : all clean

all : $(TARGET)

clean :
	find . -name '*.o' | xargs rm -f

$(TARGET) : $(OBJS) main.o
	$(CC) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)