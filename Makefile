#!/usr/bin/env make

SRC_DIR  := .
KEY_DIR  := Keys
OBJ_DIR  := obj
CXXFLAGS := -std=c++11 -MMD -MP -Wall -Wfatal-errors -Wextra -fPIE
LDFLAGS  := -fPIE

ifdef CROSS_COMPILE
$(info Doing a cross-compilation)
STATIC := -static
CXX := $(CROSS_COMPILE)g++
CC := $(CROSS_COMPILE)gcc
LDFLAGS += -L /opt/lib
CROSS_CFLAGS := -I /opt/include
CXXFLAGS += $(CROSS_CFLAGS)
export CROSS_CFLAGS
export CXX
export CC
export STATIC
endif

override LDFLAGS += -lmbedcrypto
MODULES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(MODULES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

U2FDevice: $(OBJECTS) libuECC.a libcppb64.a
	$(CXX) -o $@ $^ $(LDFLAGS) $(STATIC)

install: U2FDevice
	install -m775 -t /usr/bin U2FDevice
	install -m775 -t /etc/systemd/system Services/U2FDevice.service
	install -d /usr/share/U2FDevice/

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR) $(SRC_DIR)/Certificates.hpp
	$(CXX) $(STATIC) $(CXXFLAGS) -c -o $@ $<

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

-include $(OBJECTS:.o=.d)

clean:
	rm -f $(OBJ_DIR)/*
	rm -f U2FDevice libuECC.a libcppb64.a
	$(MAKE) -C micro-ecc clean
	$(MAKE) -C cpp-base64 clean

clean-certificates:
	rm -f $(KEY_DIR)/*
	rm -f Certificates.cpp Certificates.hpp

$(SRC_DIR)/Certificates.hpp: $(SRC_DIR)/Certificates.hpp.template
	$(error "Please run the GenCertificates.sh script to generate certificate before building\n")

.PHONY: clean clean-certificates install

libuECC.a:
	$(MAKE) -C micro-ecc
	cp micro-ecc/libuECC.a libuECC.a

libcppb64.a:
	$(MAKE) -C cpp-base64
	cp cpp-base64/libcppb64.a libcppb64.a
