.PHONY=all clean mostlyclean
CXX=g++
CC=gcc
WARNINGS=-Wall -Wextra -Wno-char-subscripts \
         -Wpointer-arith -Wwrite-strings -Wdisabled-optimization \
         -Wformat -Wcast-align -Wno-unused-function -Wno-unused-parameter

DBG= -DNDEBUG # -fno-inline

OPT= $(DBG) -O3 -funroll-loops \
			-pipe -fno-strict-aliasing -march=native # -flto#  -mavx512f # -msse2
OS:=$(shell uname)
ifeq ($(OS),Darwin)
    OPT := $(OPT) -Wa,-q
endif
CFLAGS=$(OPT) $(WARNINGS)
CXXFLAGS=$(OPT) -std=c++14 $(WARNINGS)
LIB=-lz -pthread
LD=-L.
INCLUDE=-I. -Iklib -Iinclude

EX=$(notdir $(patsubst %.cpp,%,$(wildcard src/*.cpp)))
OBJ=klib/kstring.o

all: $(EX)

%: src/%.cpp klib/kstring.o
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LD) klib/kstring.o $< -o $@ $(LIB)

%: src/%.c klib/kstring.o
	$(CC) $(CFLAGS) $(INCLUDE) $(LD) klib/kstring.o $< -o $@ $(LIB)

clean:
	rm -f $(OBJ) $(EX) || echo "EX is $(EX)" 
	&& cd hll && make clean && cd ..

mostlyclean: clean
