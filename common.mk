# Shared build configuration used by every Makefile in this project.
# Keep all common variables here so the recursive makefiles stay small.

.DEFAULT_GOAL := all

ROOT_DIR   := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
SRC_DIR    := $(ROOT_DIR)/src
BUILD_DIR  := $(ROOT_DIR)/build
BIN_DIR    := $(ROOT_DIR)/bin

APP        := latex-calc

CXX        ?= g++
CXXFLAGS   ?= -std=c++17 -Wall -Wextra -pedantic
CPPFLAGS   += -I$(SRC_DIR)
LDFLAGS    ?=
LDLIBS     ?=

BISON      ?= bison
FLEX       ?= flex

PARSER_DIR := $(BUILD_DIR)/parser
PARSER_C   := $(PARSER_DIR)/parser.tab.c
PARSER_H   := $(PARSER_DIR)/parser.tab.h
LEXER_DIR  := $(BUILD_DIR)/lexer
LEXER_C    := $(LEXER_DIR)/lex.yy.c

DEPFLAGS   := -MMD -MP

# Keep recursive output readable.
MAKEFLAGS  += --no-print-directory

# Generated sources. Both parser files come from a single bison invocation,
# hence the grouped target to keep parallel builds race-free.
$(PARSER_C) $(PARSER_H) &: $(SRC_DIR)/parser/parser.y
	@mkdir -p $(PARSER_DIR)
	@printf "  BISON   %s\n" "$<"
	@$(BISON) -d -o $(PARSER_C) $<

$(LEXER_C): $(SRC_DIR)/lexer/lexer.l $(PARSER_H)
	@mkdir -p $(LEXER_DIR)
	@printf "  FLEX    %s\n" "$<"
	@$(FLEX) -o $@ $<
