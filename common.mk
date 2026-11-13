# Shared build configuration used by every Makefile in this project.
# Keep all common variables here so the recursive makefiles stay small.

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

DEPFLAGS   := -MMD -MP

# Keep recursive output readable.
MAKEFLAGS  += --no-print-directory
