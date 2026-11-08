CXX ?= g++
PLUGIN_NAME ?= $(notdir $(CURDIR))
UNAME_S := $(shell uname -s)
ifeq ($(OS),Windows_NT)
 TARGET=$(PLUGIN_NAME).dll
 SHARED=-shared
else ifeq ($(UNAME_S),Darwin)
 TARGET=$(PLUGIN_NAME).dylib
 SHARED=-dynamiclib -fPIC
else
 TARGET=$(PLUGIN_NAME).so
 SHARED=-shared -fPIC
endif
all: $(TARGET)
$(TARGET): src/main.cpp ../src/Security_Plugin_API.h
	$(CXX) -std=c++17 -Wall -Wextra $(SHARED) src/main.cpp -o $@
.PHONY: all
