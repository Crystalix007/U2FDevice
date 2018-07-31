#!/usr/bin/env make

SRC_DIR  := .
OBJ_DIR  := obj
LDFLAGS  := -lmbedcrypto -lb64
CPPFLAGS :=
CXXFLAGS := --std=c++14

CXXFLAGS += -MMD -MP
MODULES  := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS  := $(MODULES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

monitor: $(OBJECTS) libuECC.o
	g++ $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

-include $(OBJECTS:.o=.d)

clean:
	rm $(OBJ_DIR)/*
	rm monitor

.PHONY: libuECC
libuECC.o:
	$(MAKE) -C micro-ecc
	cp micro-ecc/libuECC.o libuECC.o
