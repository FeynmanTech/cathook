CC=g++
CFLAGS=-std=c++1y -DTF2 -O3 -Wall -c -shared -Wall -Wno-unknown-pragmas -fmessage-length=0 -m32 -fvisibility=hidden -fPIC
CINCLUDES=-I../../ssdk/public -I../../ssdk/mathlib -I../../ssdk/common -I../../ssdk/public/tier1 -I../../ssdk/public/tier0
LDFLAGS=-Llib -m32 -fno-gnu-unique -D_GLIBCXX_USE_CXX11_ABI=0 -shared
LDLIBS=-l:libvstdlib.so -l:libstdc++.so.6 -l:libc.so.6 -l:libtier0.so
OBJ_DIR := obj
BIN_DIR := bin
SRC_DIR := src
OUT_NAME := libcathook.so
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
SOURCES += $(wildcard $(SRC_DIR)/copypasted/*.cpp)
SOURCES += $(wildcard $(SRC_DIR)/hacks/*.cpp)
SOURCES += $(wildcard $(SRC_DIR)/hooks/*.cpp)
SOURCES += $(wildcard $(SRC_DIR)/sdk/*.cpp)
SOURCES += $(wildcard $(SRC_DIR)/gui/*.cpp)
SOURCES += $(wildcard $(SRC_DIR)/ipc/*.cpp)
SOURCES += $(wildcard $(SRC_DIR)/segvcatch/*.cpp)
SOURCES += $(wildcard $(SRC_DIR)/targeting/*.cpp)
OBJECTS := $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(patsubst %.cpp,%.o,$(SOURCES)))

.PHONY: clean directories

all: directories cathook

directories:
	mkdir bin
	mkdir obj
	mkdir obj/copypasted
	mkdir obj/hacks
	mkdir obj/hooks
	mkdir obj/sdk
	mkdir obj/gui
	mkdir obj/ipc
	mkdir obj/segvcatch
	mkdir obj/targeting

echo: $(OBJECTS)
	echo $(OBJECTS)

$(OBJECTS):
	echo Compiling $(patsubst %.o,%.cpp,$(patsubst $(OBJ_DIR)/%,$(SRC_DIR)/%,$@))
	$(CC) $(CFLAGS) $(CINCLUDES) $(patsubst %.o,%.cpp,$(patsubst $(OBJ_DIR)/%,$(SRC_DIR)/%,$@)) -o $@

cathook: $(OBJECTS)
	$(CC) -o $(BIN_DIR)/$(OUT_NAME) $(LDFLAGS) $(LDLIBS) $(OBJECTS)
	strip --strip-all $(BIN_DIR)/$(OUT_NAME)

clean:
	rm $(BIN_DIR)/*
	rm $(OBJ_DIR)/*