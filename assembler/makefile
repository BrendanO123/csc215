#Flags and Compilers
CXX = g++
CXXFLAGS = -Wall -g
CPPVERSION = -std=c++17

#.out file name
TARGET = assembler.out

#include directories, libraries, and library directory
INCLUDE = -I$(SRC_PATH)

#source .cpp files
SRC_PATH = src/main/
MAIN = assembler
SRCS_RAW_CPP = LOCFileManager.cpp LOCTokenProcessor.cpp LOCBitStream.cpp LOCInstructionLoader.cpp
SRCS_CPP = $(SRCS_RAW_CPP:LOC%=$(SRC_PATH)%) #LOC -> path to source files

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

#Rule to generate the main target from the .o files (depends on .h files to ensure it recompiles often enough)
$(TARGET) : $(OBJS) $(ENTRY) $(SRCS_H)
	@$(CXX) $(CXXFLAGS) $(CPPVERSION) -o $(TARGET) $(OBJS) $(ENTRY) -I$(INCLUDE)
	@echo "RECOMPILED"

#rules for generating .o files
-include $(DEPS)

#Rule to generate app.o (seperate bc no .h for the .cpp)
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


#recompile .out if neccessary and run
run:
	@make -s
	@./$(TARGET)

#clean .o and .out files
clean:
	@rm -f $(TARGET) $(OBJS) $(ENTRY) $(DEPS) $(GL_OBJ)
	@rm -f $(TEST_OBJS)
	@rm -rf $(TARGET).dSYM
	@echo "CLEANED"

#clean .out file
delete_exc:
	@rm -f $(TARGET)
	@rm -rf $(TARGET).dSYM
	@echo "CLEANED EXECUTABLE"
