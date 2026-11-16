# Builds the application entry point and links the final binary.

include ../common.mk

TARGET      := $(BIN_DIR)/$(APP)
MAIN_OBJ    := $(BUILD_DIR)/main.o
MAIN_DEP    := $(MAIN_OBJ:.o=.d)
TABLE_LIB   := $(BUILD_DIR)/table/libtable.a
TABLE_DIR   := $(SRC_DIR)/table
AST_LIB     := $(BUILD_DIR)/ast/libast.a
AST_DIR     := $(SRC_DIR)/ast
PARSER_OBJ  := $(PARSER_DIR)/parser.tab.o
LEXER_OBJ   := $(LEXER_DIR)/lex.yy.o
DEPS        := $(MAIN_DEP) $(PARSER_OBJ:.o=.d) $(LEXER_OBJ:.o=.d)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(MAIN_OBJ) $(TABLE_LIB) $(AST_LIB) $(PARSER_OBJ) $(LEXER_OBJ)
	@mkdir -p $(BIN_DIR)
	@printf "  LD      %s\n" "$@"
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(MAIN_OBJ): $(SRC_DIR)/main.cpp $(PARSER_H)
	@mkdir -p $(dir $@)
	@printf "  CXX     %s\n" "$<"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I$(PARSER_DIR) $(DEPFLAGS) -c -o $@ $<

# The table module owns its own incremental build. The parser header must
# exist first because init.cpp includes it.
$(TABLE_LIB): FORCE | $(PARSER_H)
	@$(MAKE) -s -C $(TABLE_DIR)

$(AST_LIB): FORCE
	@$(MAKE) -s -C $(AST_DIR)

$(PARSER_OBJ): $(PARSER_C)
	@mkdir -p $(dir $@)
	@printf "  CXX     %s\n" "$<"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I$(PARSER_DIR) $(DEPFLAGS) -c -o $@ $<

$(LEXER_OBJ): $(LEXER_C)
	@mkdir -p $(dir $@)
	@printf "  CXX     %s\n" "$<"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I$(PARSER_DIR) $(DEPFLAGS) -c -o $@ $<

.PHONY: FORCE
FORCE:

clean:
	@$(MAKE) -C $(TABLE_DIR) clean
	@$(MAKE) -C $(AST_DIR) clean
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@printf "  CLEAN   %s\n" "$(BUILD_DIR) $(BIN_DIR)"

-include $(DEPS)
