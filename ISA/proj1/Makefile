CXX = g++
CXXFLAGS = -g -Wall -Wextra
LFLAGS = -lpcap

PROJECT = flow
SRC = $(wildcard *.cpp)
SRC += $(wildcard src/*.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SRC))
DEPENDS := $(patsubst %.cpp, %.d, $(SRC))

TEST_DIR = test

.PHONY: all clean test tar

all: $(PROJECT)

$(PROJECT): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LFLAGS)

# generating dependency files
# source https://stackoverflow.com/a/52036564
-include $(DEPENDS)

%.o: %.cpp %.hpp Makefile
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	$(RM) $(PROJECT) $(DEPENDS) $(OBJ)
	$(RM) $(TEST_DIR)/out/*.pcap_out $(TEST_DIR)/out/*.pcap_fcache $(TEST_DIR)/out/*.pcap_inactive $(TEST_DIR)/out/*.pcap_active

test: $(PROJECT)
	cd $(TEST_DIR); ./test.sh

tar:
	tar -cf xzvara01.tar flow.1 flow.cpp flow.hpp Makefile README manual.pdf src/* test/*