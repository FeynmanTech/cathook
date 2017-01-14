EXE = cathook

SRC_DIR = src
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CPPFLAGS += -I../../ssdk/public -I../../ssdk/mathlib -I../../ssdk/common -I../../ssdk/public/tier1 -I../../ssdk/public/tier0
CFLAGS += --std=c++1y -DTF2 -O3 -Wall -c -shared -Wall -Wno-unknown-pragmas -fmessage-length=0 -m32 -fvisibility=hidden -fPIC
LDFLAGS += -Llib -m32 -fno-gnu-unique -D_GLIBCXX_USE_CXX11_ABI=0 -shared
LDLIBS += -lvstdlib -l:libstdc++.so.6 -ltier0 -l:libc.so.6

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
    $(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
    $(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
    $(RM) $(OBJ)