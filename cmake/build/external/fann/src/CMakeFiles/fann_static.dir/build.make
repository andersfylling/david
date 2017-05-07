# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.7

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/anders/Projects/chess_ann

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/anders/Projects/chess_ann/cmake/build

# Include any dependencies generated for this target.
include external/fann/src/CMakeFiles/fann_static.dir/depend.make

# Include the progress variables for this target.
include external/fann/src/CMakeFiles/fann_static.dir/progress.make

# Include the compile flags for this target's objects.
include external/fann/src/CMakeFiles/fann_static.dir/flags.make

external/fann/src/CMakeFiles/fann_static.dir/floatfann.c.o: external/fann/src/CMakeFiles/fann_static.dir/flags.make
external/fann/src/CMakeFiles/fann_static.dir/floatfann.c.o: ../../external/fann/src/floatfann.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/anders/Projects/chess_ann/cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object external/fann/src/CMakeFiles/fann_static.dir/floatfann.c.o"
	cd /home/anders/Projects/chess_ann/cmake/build/external/fann/src && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/fann_static.dir/floatfann.c.o   -c /home/anders/Projects/chess_ann/external/fann/src/floatfann.c

external/fann/src/CMakeFiles/fann_static.dir/floatfann.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/fann_static.dir/floatfann.c.i"
	cd /home/anders/Projects/chess_ann/cmake/build/external/fann/src && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/anders/Projects/chess_ann/external/fann/src/floatfann.c > CMakeFiles/fann_static.dir/floatfann.c.i

external/fann/src/CMakeFiles/fann_static.dir/floatfann.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/fann_static.dir/floatfann.c.s"
	cd /home/anders/Projects/chess_ann/cmake/build/external/fann/src && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/anders/Projects/chess_ann/external/fann/src/floatfann.c -o CMakeFiles/fann_static.dir/floatfann.c.s

external/fann/src/CMakeFiles/fann_static.dir/floatfann.c.o.requires:

.PHONY : external/fann/src/CMakeFiles/fann_static.dir/floatfann.c.o.requires

external/fann/src/CMakeFiles/fann_static.dir/floatfann.c.o.provides: external/fann/src/CMakeFiles/fann_static.dir/floatfann.c.o.requires
	$(MAKE) -f external/fann/src/CMakeFiles/fann_static.dir/build.make external/fann/src/CMakeFiles/fann_static.dir/floatfann.c.o.provides.build
.PHONY : external/fann/src/CMakeFiles/fann_static.dir/floatfann.c.o.provides

external/fann/src/CMakeFiles/fann_static.dir/floatfann.c.o.provides.build: external/fann/src/CMakeFiles/fann_static.dir/floatfann.c.o


# Object files for target fann_static
fann_static_OBJECTS = \
"CMakeFiles/fann_static.dir/floatfann.c.o"

# External object files for target fann_static
fann_static_EXTERNAL_OBJECTS =

external/fann/src/libfann.a: external/fann/src/CMakeFiles/fann_static.dir/floatfann.c.o
external/fann/src/libfann.a: external/fann/src/CMakeFiles/fann_static.dir/build.make
external/fann/src/libfann.a: external/fann/src/CMakeFiles/fann_static.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/anders/Projects/chess_ann/cmake/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libfann.a"
	cd /home/anders/Projects/chess_ann/cmake/build/external/fann/src && $(CMAKE_COMMAND) -P CMakeFiles/fann_static.dir/cmake_clean_target.cmake
	cd /home/anders/Projects/chess_ann/cmake/build/external/fann/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/fann_static.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
external/fann/src/CMakeFiles/fann_static.dir/build: external/fann/src/libfann.a

.PHONY : external/fann/src/CMakeFiles/fann_static.dir/build

external/fann/src/CMakeFiles/fann_static.dir/requires: external/fann/src/CMakeFiles/fann_static.dir/floatfann.c.o.requires

.PHONY : external/fann/src/CMakeFiles/fann_static.dir/requires

external/fann/src/CMakeFiles/fann_static.dir/clean:
	cd /home/anders/Projects/chess_ann/cmake/build/external/fann/src && $(CMAKE_COMMAND) -P CMakeFiles/fann_static.dir/cmake_clean.cmake
.PHONY : external/fann/src/CMakeFiles/fann_static.dir/clean

external/fann/src/CMakeFiles/fann_static.dir/depend:
	cd /home/anders/Projects/chess_ann/cmake/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/anders/Projects/chess_ann /home/anders/Projects/chess_ann/external/fann/src /home/anders/Projects/chess_ann/cmake/build /home/anders/Projects/chess_ann/cmake/build/external/fann/src /home/anders/Projects/chess_ann/cmake/build/external/fann/src/CMakeFiles/fann_static.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : external/fann/src/CMakeFiles/fann_static.dir/depend

