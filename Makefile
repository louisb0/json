CXX = g++
CXXFLAGS = -Wall -Wextra -g
TARGET = json
BUILD_DIR = build

SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

$(TARGET): $(BUILD_DIR) $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: clean
