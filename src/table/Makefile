# Builds the symbol table module into a static library.

include ../../common.mk

TARGET := $(BUILD_DIR)/table/libtable.a

# init.cpp needs the token constants generated from the parser grammar.
# common.mk knows how to build $(PARSER_H) from parser.y.
SRCS   := symbol.cpp variable.cpp table.cpp init.cpp
OBJS   := $(SRCS:%.cpp=$(BUILD_DIR)/table/%.o)
DEPS   := $(OBJS:.o=.d)

CPPFLAGS += -I$(PARSER_DIR)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	@printf "  AR      %s\n" "$@"
	@$(AR) rcs $@ $^

$(BUILD_DIR)/table/%.o: %.cpp
	@mkdir -p $(dir $@)
	@printf "  CXX     %s\n" "$<"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPFLAGS) -c -o $@ $<

# Only init.cpp includes the generated token header.
$(BUILD_DIR)/table/init.o: $(PARSER_H)

clean:
	@rm -rf $(BUILD_DIR)/table
	@printf "  CLEAN   %s\n" "$(BUILD_DIR)/table"

-include $(DEPS)
