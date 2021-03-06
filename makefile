CC=g++
CFLAGS=-std=gnu++11 -D_POSIX=1 -DRAD_TELEMETRY_DISABLED -DLINUX=1 -D_LINUX=1 -DPOSIX=1 -DGNUC=1 -D_DEVELOPER=1 -DNO_MALLOC_OVERRIDE -O3 -g3 -ggdb -w -c -shared -Wall -Wno-unknown-pragmas -fmessage-length=0 -m32 -fvisibility=hidden -fPIC
SDKFOLDER=$(realpath source-sdk-2013/mp/src)
CINCLUDES=-I$(SDKFOLDER)/public -I$(SDKFOLDER)/mathlib -I$(SDKFOLDER)/common -I$(SDKFOLDER)/public/tier1 -I$(SDKFOLDER)/public/tier0 -I$(SDKFOLDER)
LDFLAGS=-m32 -fno-gnu-unique -D_GLIBCXX_USE_CXX11_ABI=0 -shared -L$(realpath lib)
LDLIBS=-lvstdlib -l:libstdc++.so.6 -l:libc.so.6 -ltier0
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

all:
	$(MAKE) clean
	$(MAKE) directories
	$(MAKE) cathook

directories:
	mkdir -p bin
	mkdir -p obj
	mkdir -p obj/copypasted
	mkdir -p obj/hacks
	mkdir -p obj/hooks
	mkdir -p obj/sdk
	mkdir -p obj/gui
	mkdir -p obj/ipc
	mkdir -p obj/segvcatch
	mkdir -p obj/targeting

echo: $(OBJECTS)
	echo $(OBJECTS)

$(OBJECTS):
	echo Compiling $(patsubst %.o,%.cpp,$(patsubst $(OBJ_DIR)/%,$(SRC_DIR)/%,$@))
	$(CC) $(CFLAGS) $(CINCLUDES) $(patsubst %.o,%.cpp,$(patsubst $(OBJ_DIR)/%,$(SRC_DIR)/%,$@)) -o $@

cathook: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(BIN_DIR)/$(OUT_NAME) $(OBJECTS) $(LDLIBS)

clean:
	rm -rf bin
	rm -rf obj