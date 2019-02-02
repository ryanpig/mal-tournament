# The name of the final executable
TARGET := mal 
# The directory in which to put compilation artifacts
OBJDIR := .objs

# Compiler choice and base flags
CXX := g++ -fdiagnostics-color=always
CXXFLAGS := -Wall -Wextra -std=c++14 -g -ffast-math -march=native -mtune=native -pthread
LDFLAGS :=

# Disable optimisation in debug mode.
# To enable debug mode, pass e.g. DEBUG=1 as an extra argument to 'make'.
ifeq ($(DEBUG),)
	CXXFLAGS += -O3 -flto
	CXXFLAGS += -fsanitize=address 
endif

# Detect platform and enable specific features.
UNAME_VAL := $(shell uname)
ifeq ($(UNAME_VAL),Darwin)
	CXXFLAGS += -DHAVE_SIGACTION
else ifeq ($(UNAME_VAL),Linux)
	CXXFLAGS += -DHAVE_SIGACTION
else
	# windows?
endif

# Find the SDL2 library
# SDL2_CXXFLAGS := $(shell pkg-config --cflags sdl2)
# SDL2_LDFLAGS := $(shell pkg-config --libs sdl2)
# CXXFLAGS += $(SDL2_CXXFLAGS)
# LDFLAGS += $(SDL2_LDFLAGS)
# Find the FREEIMAGE library
# FREEIMAGE_CXXFLAGS :=
# FREEIMAGE_LDFLAGS := -lfreeimage
# CXXFLAGS += $(FREEIMAGE_CXXFLAGS)
# LDFLAGS += $(FREEIMAGE_LDFLAGS)

# BOOST program_options library 
# BOOST_ROOT = /usr/local/Cellar/boost/1.67.0_1
BOOST_ROOT = ~/miscellaneous/boost_1_69_0/
# BOOST_CXXFLAGS := -I${BOOST_ROOT}/include
BOOST_CXXFLAGS := -I${BOOST_ROOT}/boost
# BOOST_LDFLAGS := -L${BOOST_ROOT}/lib -lboost_program_options
BOOST_LDFLAGS := -L${BOOST_ROOT}/stage/lib -lboost_program_options
CXXFLAGS += $(BOOST_CXXFLAGS)
LDFLAGS += $(BOOST_LDFLAGS)

# sqlite 3
SQLITE3_LDFLAGS := -lsqlite3
LDFLAGS += $(SQLITE3_LDFLAGS)
# Collect sources
CXX_SOURCES := $(wildcard *.cpp)

# Compute necessary compilation artifacts
OBJ_FILES := $(patsubst %.cpp,$(OBJDIR)/%.o,$(CXX_SOURCES))
DEP_FILES := $(patsubst %.cpp,$(OBJDIR)/%.d,$(CXX_SOURCES))


# Generic targets
.PHONY: all clean

all: $(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf $(OBJDIR)

# Compilation rules
$(TARGET): $(OBJ_FILES)
	@echo LD $@
	@$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo CXX $<
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/%.d: %.cpp
	@mkdir -p $(dir $@)
	@echo DEP $<
	@$(CXX) -MT $(OBJDIR)/$*.o -MM $(CXXFLAGS) $< >$@

# Make sure the dependency relations get read by 'make'
-include $(DEP_FILES)
