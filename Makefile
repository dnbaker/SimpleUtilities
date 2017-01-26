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
CXXFLAGS=$(OPT) -std=c++11 $(WARNINGS)
LIB=-lz -pthread
LD=-L.
INCLUDE=-I.

EX=$(notdir $(patsubst %.cpp,%,$(wildcard src/*.cpp)))
OBJ=

all: $(EX)

%: src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LD) $< -o $@ $(LIB)

clean:
	rm -f $(OBJ) $(EX) || echo "EX is $(EX)" 
	&& cd hll && make clean && cd ..

mostlyclean: clean
