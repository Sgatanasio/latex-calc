# Builds the AST module into a static library.

include ../../common.mk

TARGET := $(BUILD_DIR)/ast/libast.a

SRCS   := tree.cpp
OBJS   := $(SRCS:%.cpp=$(BUILD_DIR)/ast/%.o)
DEPS   := $(OBJS:.o=.d)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	@printf "  AR      %s\n" "$@"
	@$(AR) rcs $@ $^

$(BUILD_DIR)/ast/%.o: %.cpp
	@mkdir -p $(dir $@)
	@printf "  CXX     %s\n" "$<"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPFLAGS) -c -o $@ $<

clean:
	@rm -rf $(BUILD_DIR)/ast
	@printf "  CLEAN   %s\n" "$(BUILD_DIR)/ast"

-include $(DEPS)
