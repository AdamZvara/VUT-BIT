CC = g++
CPPFLAGS = -std=c++11 -O2 -Wall -Wextra -pedantic

TARGET = hinfosvc

$(TARGET): $(TARGET).cpp
	$(CC) $(CPPFLAGS) $^ -o $@

clean:
	rm -rf $(TARGET)

.phony: clean 
