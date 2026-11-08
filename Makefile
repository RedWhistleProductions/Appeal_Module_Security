CXX ?= g++
UNAME_S := $(shell uname -s)
ifeq ($(OS),Windows_NT)
 TARGET=Appeal_Module_Security.dll
 SHARED=-shared
else ifeq ($(UNAME_S),Darwin)
 TARGET=Appeal_Module_Security.dylib
 SHARED=-dynamiclib -fPIC
else
 TARGET=Appeal_Module_Security.so
 SHARED=-shared -fPIC
 LIBS=-ldl
endif
all: $(TARGET)
$(TARGET): $(wildcard src/*.cpp) $(wildcard src/*.h)
	$(CXX) -std=c++17 -Wall -Wextra $(SHARED) $(wildcard src/*.cpp) -o $@ $(LIBS)
.PHONY: all
