# The name of the final executable
TARGET := mal 
# The directory in which to put compilation artifacts
OBJDIR := .objs
OBJTEST := .objstest
SRC_DIR = src

# Compiler choice and base flags
CXX := g++ -fdiagnostics-color=always
CXXFLAGS := -Wall -Wextra -std=c++14 -ffast-math -march=native -mtune=native -pthread
LDFLAGS :=

# Setting for UnitTest
TARGET_TEST := unittest 

# Disable optimisation in debug mode. 
# To enable debug mode, pass DEBUG=1 as an extra argument to 'make'.
ifeq ($(DEBUG), 1)
	CXXFLAGS += -g -fsanitize=address 
else 
  CXXFLAGS += -O3
endif

# Detect platform and enable specific features.
UNAME_VAL := $(shell uname)
ifeq ($(UNAME_VAL),Darwin)
	CXXFLAGS += -DHAVE_SIGACTION -flto
else ifeq ($(UNAME_VAL),Linux)
	CXXFLAGS += -DHAVE_SIGACTION
else
	# windows?
endif

# BOOST program_options library 
ifeq ($(UNAME_VAL),Darwin)
	BOOST_ROOT = /usr/local/opt/boost
	BOOST_CXXFLAGS := -I${BOOST_ROOT}/include
	BOOST_LDFLAGS := -L${BOOST_ROOT}/lib -lboost_program_options
else ifeq ($(UNAME_VAL),Linux)
	# BOOST_ROOT = $(shell ls -d ~/miscellaneous/boost_* | tail -1)
	BOOST_ROOT = ~/miscellaneous/boost_1_69_0
	BOOST_CXXFLAGS := -I${BOOST_ROOT}/boost
	BOOST_LDFLAGS := -L${BOOST_ROOT}/stage/lib -lboost_program_options
endif

CXXFLAGS += $(BOOST_CXXFLAGS)
LDFLAGS += $(BOOST_LDFLAGS)

# sqlite 3
SQLITE3_LDFLAGS := -lsqlite3
LDFLAGS += $(SQLITE3_LDFLAGS)

# Flags for testing
CXXFLAGS_TEST := $(CXXFLAGS)
LDFLAGS_TEST  := $(LDFLAGS)
# Google Test libraries/headers
# GTEST_LIBS = libgtest.a libgtest_main.a
GTEST_LIBS = libgtest.a 
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

# Collect sources
SRC_DIR_TEST = tests
CXX_SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
CXX_SOURCES_TEST := $(wildcard $(SRC_DIR_TEST)/*.cpp) 
# CXX_SOURCES_TEST := $(wildcard tests/*.cpp) 

# Compute necessary compilation artifacts
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJDIR)/%.o,$(CXX_SOURCES))
DEP_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJDIR)/%.d,$(CXX_SOURCES))

# OBJ_FILES_TEST := $(patsubst $(SRC_DIR_TEST)/%.cpp,$(OBJTEST)/%.o,$(CXX_SOURCES_TEST))
OBJ_FILES_TEST := $(patsubst tests/%.cpp,$(OBJTEST)/%.o,$(CXX_SOURCES_TEST))


# Generic targets
.PHONY: all clean

all: $(TARGET)

# testing target
.PHONY: test
test: $(TARGET_TEST) 

# cleaning after building
clean:
	rm -f $(TARGET)
	rm -f $(TARGET_TEST)
	rm -rf $(OBJDIR)
	rm -rf $(OBJTEST)
	rm -f *.o
	rm -f error_report.txt
	rm -f paramInfo.txt

# Build Gtest library 
GTEST_DIR = gtest
GTEST_LIB_DIR = .
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)
CPPFLAGS += -isystem $(GTEST_DIR)/include # for gtest

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

# gtest_main.o : $(GTEST_SRCS_)
#   $(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
#             $(GTEST_DIR)/src/gtest_main.cc

libgtest.a : gtest-all.o
	@echo flags  $(ARFLAGS)
	$(AR) $(ARFLAGS) $@ $^

# libgtest_main.a : gtest-all.o gtest_main.o
#   $(AR) $(ARFLAGS) $@ $^

# Compilation rules
$(TARGET): $(OBJ_FILES)
	@echo LD $@
	@$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo CXX $<
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/%.d: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo DEP $<
	@$(CXX) -MT $(OBJDIR)/$*.o -MM $(CXXFLAGS) $< >$@


# Compilation rules for UnitTest 
BASIC_HEADER = $(SRC_DIR)/*.h
SRC_TEST = tests
OBJ_FILTER := $(filter-out $(OBJDIR)/main.o, $(OBJ_FILES)) 

$(TARGET_TEST): $(OBJ_FILES_TEST) $(OBJ_FILTER) $(GTEST_LIBS) 
	@echo link objects: $^
	@echo LD $@
	# @$(CXX) $(CPPFLAGS) $(CXXFLAGS_TEST) -L$(GTEST_LIB_DIR) -lgtest_main -lpthread -o $@ $^ $(LDFLAGS_TEST)
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS_TEST) -L$(GTEST_LIB_DIR) -lpthread -o $@ $^ $(LDFLAGS_TEST)
	


$(OBJTEST)/%.o : $(SRC_TEST)/%.cpp 
	@mkdir -p $(dir $@)
	@echo CXX $<
	@echo compiled *.cpp to *.o : $^
	$(CXX) $(CPPFLAGS) $(CXXFLAGS_TEST) -I$(SRC_DIR) -c -o $@ $< 

# Make sure the dependency relations get read by 'make'
-include $(DEP_FILES)
