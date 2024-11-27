CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Werror -I./include
LDFLAGS :=

TEST_EXTRA_FLAGS := -g -Wno-nodiscard -Wno-unused-result

TEST_LIBS := -lgtest -lgtest_main -pthread

SRC_DIR := src
TEST_DIR := tests
EXAMPLES_DIR := examples
BUILD_DIR := build
DEP_DIR := $(BUILD_DIR)/deps

SRC_FILES := $(shell find $(SRC_DIR) -name '*.cpp')
TEST_FILES := $(shell find $(TEST_DIR) -name '*.cpp')
EXAMPLE_FILES := $(shell find $(EXAMPLES_DIR) -name '*.cpp')

SRC_OBJS := $(SRC_FILES:%.cpp=$(BUILD_DIR)/%.o)
TEST_OBJS := $(TEST_FILES:%.cpp=$(BUILD_DIR)/%.o)
EXAMPLE_OBJS := $(EXAMPLE_FILES:%.cpp=$(BUILD_DIR)/%.o)

SRC_DEPS := $(SRC_FILES:%.cpp=$(DEP_DIR)/%.d)
TEST_DEPS := $(TEST_FILES:%.cpp=$(DEP_DIR)/%.d)
EXAMPLE_DEPS := $(EXAMPLE_FILES:%.cpp=$(DEP_DIR)/%.d)

LIB_NAME := libjson.a
LIB := $(BUILD_DIR)/$(LIB_NAME)

TEST_EXE := $(BUILD_DIR)/run_tests

EXAMPLE_EXES := $(EXAMPLE_FILES:$(EXAMPLES_DIR)/%.cpp=$(BUILD_DIR)/$(EXAMPLES_DIR)/%)

.PHONY: all
all: $(LIB)

$(BUILD_DIR) $(DEP_DIR) $(DEP_DIR)/$(SRC_DIR) $(DEP_DIR)/$(TEST_DIR) $(DEP_DIR)/$(EXAMPLES_DIR) $(BUILD_DIR)/$(SRC_DIR) $(BUILD_DIR)/$(TEST_DIR) $(BUILD_DIR)/$(EXAMPLES_DIR):
	mkdir -p $@

$(BUILD_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR) $(DEP_DIR)
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEP_DIR)/$(SRC_DIR)/$*.d)
	$(CXX) $(CXXFLAGS) -MMD -MP -MF $(DEP_DIR)/$(SRC_DIR)/$*.d -c $< -o $@

$(BUILD_DIR)/$(TEST_DIR)/%.o: $(TEST_DIR)/%.cpp | $(BUILD_DIR) $(DEP_DIR)
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEP_DIR)/$(TEST_DIR)/$*.d)
	$(CXX) $(CXXFLAGS) $(TEST_EXTRA_FLAGS) -MMD -MP -MF $(DEP_DIR)/$(TEST_DIR)/$*.d -c $< -o $@

$(BUILD_DIR)/$(EXAMPLES_DIR)/%.o: $(EXAMPLES_DIR)/%.cpp | $(BUILD_DIR) $(DEP_DIR)
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEP_DIR)/$(EXAMPLES_DIR)/$*.d)
	$(CXX) $(CXXFLAGS) -MMD -MP -MF $(DEP_DIR)/$(EXAMPLES_DIR)/$*.d -c $< -o $@

$(LIB): $(SRC_OBJS)
	ar rcs $@ $^

.PHONY: build-tests
build-tests: $(TEST_EXE)

.PHONY: test
test: $(TEST_EXE)
	./$(TEST_EXE)

$(TEST_EXE): $(TEST_OBJS) $(LIB)
	$(CXX) $(LDFLAGS) $^ $(TEST_LIBS) -o $@

.PHONY: examples
examples: $(EXAMPLE_EXES)

$(BUILD_DIR)/$(EXAMPLES_DIR)/%: $(BUILD_DIR)/$(EXAMPLES_DIR)/%.o $(LIB)
	@mkdir -p $(dir $@)
	$(CXX) $(LDFLAGS) $^ -o $@

.PHONY: haversine
haversine: $(BUILD_DIR)/$(EXAMPLES_DIR)/haversine

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

-include $(SRC_DEPS) $(TEST_DEPS) $(EXAMPLE_DEPS)
