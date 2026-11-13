# Top-level entry point. Delegates the actual work to the module makefiles.
#
#   make          build everything
#   make clean    remove build artefacts
#   make re       clean and rebuild

include common.mk

.PHONY: all clean re

all:
	@$(MAKE) -C $(SRC_DIR) all

clean:
	@$(MAKE) -C $(SRC_DIR) clean

re: clean all
