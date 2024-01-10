# MIT License
#
# Copyright (c) 2023 William Quelho Ferreira
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
#
#
# ===============================================
# |                                             |
# |  If using for the first time in a project,  |
# | run "make tree" to initialize the directory |
# |                 structure.                  |
# |                                             |
# +---------------------------------------------+
# |                                             |
# |   More information on make targets can be   |
# |                 found below.                |
# |                                             |
# ===============================================
#
#
#
# ================
# |              |
# |     ABOUT    |
# |              |
# ================
#
# This Makefile was created to try and minimize the effort
# required to compile most C projects (although it could be
# extended to projects of other languages) by automating
# compile rules and adding a few other functions (e.g. `tree`).
# Most commands are easily customizable by altering the values
# on some variables, but work in their default state, so you
# could use this Makefile as it is for a C/C++ project.

# Compiler
CC := g++

# Expressions which describe source and compiled files
SRC_FILE := cpp
COMP_FILE := o

SRC_PTRN := "*."$(SRC_FILE)

# Output file name
OUT := rj
ifeq ($(OS),Windows_NT)
	OUT := $(OUT).exe
endif

# Library names without `-l` prefix
# e.g.: if you use math.h and GL/gl.h, instead of `-lm -lGL`,
# set the definition to
#     LIBS := m GL
LIBS := raylib box2d
ifeq ($(OS),Windows_NT)
	LIBS += opengl32 gdi32 winmm
endif


# Compile flags
C_FLAGS = -L$(LIB_DIR) -I$(INC_DIR) $(addprefix -l,$(LIBS)) -std=c++20
DEBUG_FLAGS = -g -DDEBUG=1


#
# Flags for running gdb
#

GDB_FLAGS := 

#
# Tree customization
# Default tree structure:
#
# - Project directory
#   \_ Makefile - This file
#   \_ src/ - Source files (subdirectories included)
#   |  \_ .obj/ - Object files
#   |  \_ .dep/ - Dependency files
#   \_ include/ - Header files
#   \_ lib/ - Libraries
#


#
# ===============
# |             |
# |   TARGETS   |
# |             |
# ===============
#
# The main targets of this Makefile are:
#   - tree:        Initialize project tree.
#               Must be run before any other commands.
#
#     - all:        Compile all modified/uncompiled files to create an executable.
#
#     - rebuild:    Recompile all files regardless of modifications date.
#
#     - clean:    Cleanup compiled files and intermediate Makefile files.
#
#     - run:        Run generated executable.
#
#     - arun:        Same as running `all`, followed by `run`.
#
#     - rebrun:    Same as running `rebuild`, followed by `run`.
#
#     - gdb:        Same as `run`, except executes over gdb.
#
#     - destroy-tree-yes-i-am-sure:    THERE IS NO WAY TO REVERSE THIS.
#                                      All files, directories and subdirectories
#                                      are removed, except for this file.


# +=========================+
# |                         |
# | DIR TREE CUSTOMIZATION  |
# |                         |
# +=========================+
SRC_DIR := ./src
OBJ_DIR := $(SRC_DIR)/.obj
DEP_DIR := $(SRC_DIR)/.dep
INC_DIR := ./include
LIB_DIR := ./lib

# File where the output of the last execution is saved to
STDOUT_LOG := out.log

# ===========================
# END OF CUSTOM STUFF
#
# If you want to fully understand how this Makefile works, feel free too look
# down there, but DON'T TOUCH ANYTHING.
#
#
# SERIOUSLY, DON'T DO IT.
#
#
#
# IF YOU MESS WITH ANYTHING DOWN HERE IT COULD VERY WELL STOP WORKING, SO DON'T.
#
#
#
#
#
#
#
#
#
#
# YOU HAVE BEEN WARNED.


.PHONY: all run clean arun rebrun rebuild tree\
        destroy-tree-yes-i-am-sure gdb .gitignore

# Find all source files
SOURCES := $(shell find $(SRC_DIR) -name $(SRC_PTRN) 2> /dev/null)

OBJECTS := $(addsuffix .$(COMP_FILE), $(basename $(SOURCES)))
DEPS := $(addsuffix .d, $(basename $(SOURCES)))

# Add $(OBJ_DIR)/ as a prefix to each object file bare name
OBJECTS := $(addprefix $(OBJ_DIR)/,$(notdir $(OBJECTS)))
DEPS := $(addprefix $(DEP_DIR)/,$(notdir $(DEPS)))

# Search path for make
# Allows use of pattern rules in
# directories discovered in runtime
SRC_SUBDIR := $(shell find $(SRC_DIR) -type d 2> /dev/null)
VPATH += $(SRC_SUBDIR)

RUN_CMD := ./$(OUT)

debug_mode = 

ifneq (,$(DEBUG))
debug_mode = yep
endif

ifneq (,$(findstring gdb,$(MAKECMDGOALS)))
RUN_CMD := gdb $(RUN_CMD)
debug_mode = yep
endif

RUN_CMD := $(RUN_CMD) $(ARGS)

ifdef debug_mode
C_FLAGS += $(DEBUG_FLAGS)
endif

all: $(OUT)

run:
	@printf "====================\n"
	@$(RUN_CMD) | tee $(STDOUT_LOG)
	@printf "====================\n"

gdb: run


clean:
	-@rm -f $(ZIP).zip
	-@rm -f $(OBJ_DIR)/*.$(COMP_FILE)
	-@rm -f $(DEP_DIR)/*.d
	-@rm -f $(OUT)

arun: all run

rebrun: rebuild run

rebuild: clean all

$(OUT): $(OBJECTS)
	@printf "Linking object files... "
	@$(CC) $^ $(C_FLAGS) $(CFLAGS) -o $(OUT)
	@printf "\n"
	@printf "====================\n"
	@printf " COMPILING COMPLETE \n"

$(OBJ_DIR)/%.$(COMP_FILE):
	@printf "Building -%s-... " $(notdir $(basename $<))
	@$(CC) $(C_FLAGS) $(CFLAGS) -c -o $@ $<
	@printf "Done.\n"

$(DEP_DIR)/%.d: %.$(SRC_FILE)
	@$(CC) $(C_FLAGS) -MM -MT'$(OBJ_DIR)/$(notdir $(@:%.d=%.$(COMP_FILE)))' $< > $@

ifeq (,$(findstring clean,$(MAKECMDGOALS)))
-include $(DEPS)
endif

.gitignore:
	@echo "/$(OBJ_DIR:./%=%)/" >> .gitignore
	@echo "/$(DEP_DIR:./%=%)/" >> .gitignore
	@echo "/$(LIB_DIR:./%=%)/" >> .gitignore
	@echo "/$(OUT)" >> .gitignore
	@echo "/$(STDOUT_LOG)" >> .gitignore
	@echo "!**/.gitkeep" >> .gitignore

tree: .gitignore
	@printf "Creating project tree...\n"
	-@mkdir -p $(SRC_DIR)
	-@touch $(SRC_DIR)/.gitkeep
	@printf "%s directory created.\n" "Source"
	-@mkdir -p $(INC_DIR)
	-@touch $(INC_DIR)/.gitkeep
	@printf "%s directory created.\n" "Include"
	-@mkdir -p $(DEP_DIR)
	-@touch $(DEP_DIR)/.gitkeep
	@printf "%s directory created.\n" "Dependency"
	-@mkdir -p $(OBJ_DIR)
	-@touch $(OBJ_DIR)/.gitkeep
	@printf "%s directory created.\n" "Object"
	-@mkdir -p $(LIB_DIR)
	-@touch $(LIB_DIR)/.gitkeep
	@printf "%s directory created.\n" "Library"
	@printf "Project tree complete.\n"
	@printf "======================\n\n"

destroy-tree-yes-i-am-sure:
	-@rm --preserve-root -rf $(SRC_DIR) $(INC_DIR) $(DEP_DIR) $(OBJ_DIR) $(LIB_DIR)
	-@rm -f $(STDOUT_LOG) .gitignore
	@printf "Too late to change your mind.\n"
	@printf "Goodbye project!\n"
