CXX := clang++
CXXFLAGS := -std=c++2a -g -Wall -Wextra -pedantic
RM := rm -f
OBJECTS = run_on_modified.o
TARGET = run_on_modified

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

.PHONY: clean

$(OBJECTS): FileWatcher.hpp

clean:
	$(RM) $(OBJECTS)

distclean: clean
	$(RM) $(TARGET)
