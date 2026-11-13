# Builds the application entry point and links the final binary.

include ../common.mk

TARGET      := $(BIN_DIR)/$(APP)
MAIN_OBJ    := $(BUILD_DIR)/main.o
MAIN_DEP    := $(MAIN_OBJ:.o=.d)
TABLE_LIB   := $(BUILD_DIR)/table/libtable.a
TABLE_DIR   := $(SRC_DIR)/table

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(MAIN_OBJ) $(TABLE_LIB)
	@mkdir -p $(BIN_DIR)
	@printf "  LD      %s\n" "$@"
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(MAIN_OBJ): $(SRC_DIR)/main.cpp
	@mkdir -p $(dir $@)
	@printf "  CXX     %s\n" "$<"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPFLAGS) -c -o $@ $<

# The table module owns its own incremental build.
$(TABLE_LIB): FORCE
	@$(MAKE) -s -C $(TABLE_DIR)

.PHONY: FORCE
FORCE:

clean:
	@$(MAKE) -C $(TABLE_DIR) clean
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@printf "  CLEAN   %s\n" "$(BUILD_DIR) $(BIN_DIR)"

-include $(MAIN_DEP)
