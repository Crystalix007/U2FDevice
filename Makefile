#!/usr/bin/env make

SRC_DIR  := .
OBJ_DIR  := obj
LDFLAGS  := -lmbedcrypto
CXXFLAGS := --std=c++14

CXXFLAGS += -MMD -MP -Wall -Wfatal-errors -Wextra
MODULES  := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS  := $(MODULES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

U2FDevice: $(OBJECTS) libuECC.a libcppb64.a
	$(CXX) $(LDFLAGS) -o $@ $^

install: U2FDevice
	install -m775 -t /usr/bin U2FDevice
	install -m775 -t /etc/systemd/system Services/U2FDevice.service
	install -d /usr/share/U2FDevice/

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

-include $(OBJECTS:.o=.d)

clean:
	rm $(OBJ_DIR)/*
	rm U2FDevice libuECC.a libcppb64.a
	$(MAKE) -C micro-ecc clean
	$(MAKE) -C cpp-base64 clean

.PHONY: clean install

libuECC.a:
	$(MAKE) -C micro-ecc
	cp micro-ecc/libuECC.a libuECC.a

libcppb64.a:
	$(MAKE) -C cpp-base64
	cp cpp-base64/libcppb64.a libcppb64.a
