CXXFLAGS := -g -std=c++11 -I. $(shell pkg-config --cflags sdl2)
LDFLAGS := -lGLEW -lGL $(shell pkg-config --libs sdl2) -lpthread

CPP_FILES = $(shell find . -type f -name '*.cpp')
OBJ_FILES := $(addprefix obj/,$(CPP_FILES:.cpp=.o))

all: dsb

dsb: $(OBJ_FILES)
	$(CXX) -o $@ $^ $(LDFLAGS)

obj/%.o: %.cpp
	mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) -o $@ -c $*.cpp

.PHONY: clean
clean:
	rm -rf obj ./dsb
