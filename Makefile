# Top-level entry point. Delegates the actual work to the module makefiles.
#
#   make          build the executable
#   make main     build the executable (explicit alias)
#   make run      build and run the executable
#   make clean    remove build artefacts
#   make re       clean and rebuild

include common.mk

.PHONY: all clean re main run

all: main

# main.cpp is compiled and linked into $(BIN_DIR)/$(APP).
main:
	@$(MAKE) -C $(SRC_DIR) all

run: main
	@$(BIN_DIR)/$(APP)

clean:
	@$(MAKE) -C $(SRC_DIR) clean

re: clean all
