#Flags and Compilers
CXX = g++
CXXFLAGS = -Wall -g
CPPVERSION = -std=c++17

#.out file name
TARGET = assembler.out
TEST_EXEC = tests.out

#include directories, libraries, and library directory
INCLUDE = $(SRC_PATH) -IDependencies/

#source .cpp files
SRC_PATH = src/main/
MAIN = assembler
SRCS_RAW_CPP = LOCFileManager.cpp LOCTokenProcessor.cpp LOCBitStream.cpp LOCInstructionLoader.cpp LOCPushableBitSequence.cpp
SRCS_CPP = $(SRCS_RAW_CPP:LOC%=$(SRC_PATH)%) #LOC -> path to source files

TEST_PATH = src/test/
TEST_OBJ_PATH = Dependencies/bin/test/
TESTS_CPP = $(wildcard $(TEST_PATH)*.cpp)
TESTS_OBJ := $(patsubst $(TEST_PATH)%.cpp,$(TEST_OBJ_PATH)%.o,$(TESTS_CPP))

#source .h files (same path)
SRCS_RAW_H = LOCFileManager.h LOCTokenProcessor.h LOCBitStream.h LOCPushableBitSequence.h LOCInstructionLoader.h
SRCS_H = $(SRCS_RAW_H:LOC%=$(SRC_PATH)%) #LOC -> path to source files

#object .o files
OBJ_PATH = Dependencies/bin/
OBJS = $(SRCS_RAW_CPP:LOC%.cpp=$(OBJ_PATH)%.o) #.cpp -> .o
DEPS = $(OBJS:%.o=%.d)
ENTRY = $(OBJ_PATH)$(MAIN).o

#default compilation rule
ALL: $(TARGET)
	@mkdir -p $(OBJ_PATH)

#Rule to generate the main target from the .o files (depends on .h files to ensure it recompiles often enough)
$(TARGET) : $(OBJS) $(ENTRY) $(SRCS_H)
	@$(CXX) $(CXXFLAGS) $(CPPVERSION) -o $(TARGET) $(OBJS) $(ENTRY) -I$(INCLUDE)
	@echo "RECOMPILED"

#rules for generating .o files
-include $(DEPS)

#Rule to generate app.o (separate bc no .h for the .cpp)
$(ENTRY) : $(SRC_PATH)$(MAIN).cpp
	@$(CXX) $(CXXFLAGS) $(CPPVERSION) -c -o $@ $< -I$(INCLUDE)

#rule to generate .d files
$(OBJ_PATH)%.d : $(SRC_PATH)%.cpp
	@$(CXX) $(CXXFLAGS) $(CPPVERSION) -MM -o $(patsubst %.o, %.d, $@) $< -I$(INCLUDE)

DEPENDS : $(DEPS)
	@echo "MADE DEPENDENCY FILES"

#Rule to generate .o files from corresponding .cpp and .h files
$(OBJ_PATH)%.o : $(SRC_PATH)%.cpp
	@$(CXX) $(CXXFLAGS) $(CPPVERSION) -c -o $@ $< -I$(INCLUDE)

build:
	@make -s
	@rm -f $(OBJS) $(ENTRY)

#recompile .out if necessary and run
run_%:
	@make -s
	@./$(TARGET) $(strip $*)

#clean .o and .out files
clean:
	@rm -f $(TARGET) $(OBJS) $(ENTRY) $(DEPS) $(TESTS_OBJ) $(TEST_EXEC)
	@rm -rf $(TARGET).dSYM $(TEST_EXEC).dSYM
	@echo "CLEANED"

#clean .out file
delete_exc:
	@rm -f $(TARGET) $(TEST_EXEC)
	@rm -rf $(TARGET).dSYM $(TEST_EXEC).dSYM
	@echo "CLEANED EXECUTABLES"

test_cases: $(TEST_EXEC)
	@mkdir -p $(TEST_OBJ_PATH)
TEST_CASES: $(TEST_EXEC)
	@mkdir -p $(TEST_OBJ_PATH)

$(TEST_EXEC): $(TESTS_OBJ)  $(OBJS) $(ENTRY) $(SRCS_H)
	@$(CXX) $(CXXFLAGS) $(CPPVERSION) -o $(TEST_EXEC) $(OBJS) $(TESTS_OBJ) -I$(INCLUDE)
	@echo "RECOMPILED TESTS"

$(TEST_OBJ_PATH)%.o : $(TEST_PATH)%.cpp
	@$(CXX) $(CXXFLAGS) $(CPPVERSION) -c -o $@ $< -I$(INCLUDE)

CHECK_TESTS:
	@make TEST_CASES -s
	@./$(TEST_EXEC)
check_tests:
	@make TEST_CASES -s
	@./$(TEST_EXEC)