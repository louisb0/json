# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Werror -I./include
LDFLAGS :=

# Test-specific flags (disable nodiscard warnings)
TEST_EXTRA_FLAGS := -g -Wno-nodiscard -Wno-unused-result

# Google Test flags
TEST_LIBS := -lgtest -lgtest_main -pthread

# Directories
SRC_DIR := src
TEST_DIR := tests
BUILD_DIR := build
DEP_DIR := $(BUILD_DIR)/deps

# Find all source files
SRC_FILES := $(shell find $(SRC_DIR) -name '*.cpp')
TEST_FILES := $(shell find $(TEST_DIR) -name '*.cpp')

# Generate object file names
SRC_OBJS := $(SRC_FILES:%.cpp=$(BUILD_DIR)/%.o)
TEST_OBJS := $(TEST_FILES:%.cpp=$(BUILD_DIR)/%.o)

# Generate dependency file names
SRC_DEPS := $(SRC_FILES:%.cpp=$(DEP_DIR)/%.d)
TEST_DEPS := $(TEST_FILES:%.cpp=$(DEP_DIR)/%.d)

# Library name
LIB_NAME := libjson.a
LIB := $(BUILD_DIR)/$(LIB_NAME)

# Test executable
TEST_EXE := $(BUILD_DIR)/run_tests

# Default target (just builds the library)
.PHONY: all
all: $(LIB)

# Create build directories
$(BUILD_DIR) $(DEP_DIR) $(DEP_DIR)/$(SRC_DIR) $(DEP_DIR)/$(TEST_DIR) $(BUILD_DIR)/$(SRC_DIR) $(BUILD_DIR)/$(TEST_DIR) $(BUILD_DIR)/$(TEST_DIR)/value:
	mkdir -p $@

# Compile source files (library)
$(BUILD_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR) $(DEP_DIR)
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEP_DIR)/$(SRC_DIR)/$*.d)
	$(CXX) $(CXXFLAGS) -MMD -MP -MF $(DEP_DIR)/$(SRC_DIR)/$*.d -c $< -o $@

# Compile test files (with additional flags)
$(BUILD_DIR)/$(TEST_DIR)/%.o: $(TEST_DIR)/%.cpp | $(BUILD_DIR) $(DEP_DIR)
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEP_DIR)/$(TEST_DIR)/$*.d)
	$(CXX) $(CXXFLAGS) $(TEST_EXTRA_FLAGS) -MMD -MP -MF $(DEP_DIR)/$(TEST_DIR)/$*.d -c $< -o $@

# Build the library
$(LIB): $(SRC_OBJS)
	ar rcs $@ $^

# Build tests (without running)
.PHONY: build-tests
build-tests: $(TEST_EXE)

# Build and run tests
.PHONY: test
test: $(TEST_EXE)
	./$(TEST_EXE)

# Link test executable (now with Google Test libraries)
$(TEST_EXE): $(TEST_OBJS) $(LIB)
	$(CXX) $(LDFLAGS) $^ $(TEST_LIBS) -o $@

# Clean build files
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

# Include dependency files
-include $(SRC_DEPS) $(TEST_DEPS)
