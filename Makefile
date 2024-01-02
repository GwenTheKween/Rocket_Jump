CC = g++
COMP_FLAGS = -Wall -c -O2 -std=c++20
LINK_FLAGS = -lraylib
ifeq ($(OS),Windows_NT)
	LINK_FLAGS += -lopengl32 -lgdi32 -lwinmm
endif
DEBUG_FLAGS = -O0 -g3 -DDEBUG_BUILD -std=c++20
OUTPUT = rj
OUTPUT_DEBUG = rj.debug
SRC_FILES = $(wildcard src/*.cpp)
HEADER_FILES = $(wildcard src/*.h)
OBJ_FILES = $(SRC_FILES:.cpp=.o)
SRC_DEPS = $(SRC_FILES:.cpp=.d)

all: $(OUTPUT)

$(OUTPUT): $(OBJ_FILES)
	$(CC) $(LINK_FLAGS) -o $(OUTPUT) $(OBJ_FILES)

-include $(SRC_DEPS)

# the move, sed and rm commands setup the .d file to have the rule name
# start with "src/", otherwise the rules wont work
%.o:%.cpp
	$(CC) $(COMP_FLAGS) $< -o $@
	$(CC) $(COMP_FLAGS) -MM $< -o $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp >$*.d
	@rm -f $*.d.tmp

debug:
	$(CC) $(DEBUG_FLAGS) $(LINK_FLAGS) $(SRC_FILES) -o $(OUTPUT_DEBUG)
	gdb $(OUTPUT_DEBUG)

clean:
	rm $(OBJ_FILES) $(OUTPUT)