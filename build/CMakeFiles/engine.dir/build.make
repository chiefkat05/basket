# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/chiefkat/Documents/vscodeprojects/basket

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/chiefkat/Documents/vscodeprojects/basket/build

# Include any dependencies generated for this target.
include CMakeFiles/engine.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/engine.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/engine.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/engine.dir/flags.make

CMakeFiles/engine.dir/main.cpp.o: CMakeFiles/engine.dir/flags.make
CMakeFiles/engine.dir/main.cpp.o: /home/chiefkat/Documents/vscodeprojects/basket/main.cpp
CMakeFiles/engine.dir/main.cpp.o: CMakeFiles/engine.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/chiefkat/Documents/vscodeprojects/basket/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/engine.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/engine.dir/main.cpp.o -MF CMakeFiles/engine.dir/main.cpp.o.d -o CMakeFiles/engine.dir/main.cpp.o -c /home/chiefkat/Documents/vscodeprojects/basket/main.cpp

CMakeFiles/engine.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/engine.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chiefkat/Documents/vscodeprojects/basket/main.cpp > CMakeFiles/engine.dir/main.cpp.i

CMakeFiles/engine.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/engine.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chiefkat/Documents/vscodeprojects/basket/main.cpp -o CMakeFiles/engine.dir/main.cpp.s

CMakeFiles/engine.dir/multiplayer.cpp.o: CMakeFiles/engine.dir/flags.make
CMakeFiles/engine.dir/multiplayer.cpp.o: /home/chiefkat/Documents/vscodeprojects/basket/multiplayer.cpp
CMakeFiles/engine.dir/multiplayer.cpp.o: CMakeFiles/engine.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/chiefkat/Documents/vscodeprojects/basket/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/engine.dir/multiplayer.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/engine.dir/multiplayer.cpp.o -MF CMakeFiles/engine.dir/multiplayer.cpp.o.d -o CMakeFiles/engine.dir/multiplayer.cpp.o -c /home/chiefkat/Documents/vscodeprojects/basket/multiplayer.cpp

CMakeFiles/engine.dir/multiplayer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/engine.dir/multiplayer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chiefkat/Documents/vscodeprojects/basket/multiplayer.cpp > CMakeFiles/engine.dir/multiplayer.cpp.i

CMakeFiles/engine.dir/multiplayer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/engine.dir/multiplayer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chiefkat/Documents/vscodeprojects/basket/multiplayer.cpp -o CMakeFiles/engine.dir/multiplayer.cpp.s

CMakeFiles/engine.dir/events.cpp.o: CMakeFiles/engine.dir/flags.make
CMakeFiles/engine.dir/events.cpp.o: /home/chiefkat/Documents/vscodeprojects/basket/events.cpp
CMakeFiles/engine.dir/events.cpp.o: CMakeFiles/engine.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/chiefkat/Documents/vscodeprojects/basket/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/engine.dir/events.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/engine.dir/events.cpp.o -MF CMakeFiles/engine.dir/events.cpp.o.d -o CMakeFiles/engine.dir/events.cpp.o -c /home/chiefkat/Documents/vscodeprojects/basket/events.cpp

CMakeFiles/engine.dir/events.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/engine.dir/events.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chiefkat/Documents/vscodeprojects/basket/events.cpp > CMakeFiles/engine.dir/events.cpp.i

CMakeFiles/engine.dir/events.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/engine.dir/events.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chiefkat/Documents/vscodeprojects/basket/events.cpp -o CMakeFiles/engine.dir/events.cpp.s

CMakeFiles/engine.dir/gfx/shader.cpp.o: CMakeFiles/engine.dir/flags.make
CMakeFiles/engine.dir/gfx/shader.cpp.o: /home/chiefkat/Documents/vscodeprojects/basket/gfx/shader.cpp
CMakeFiles/engine.dir/gfx/shader.cpp.o: CMakeFiles/engine.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/chiefkat/Documents/vscodeprojects/basket/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/engine.dir/gfx/shader.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/engine.dir/gfx/shader.cpp.o -MF CMakeFiles/engine.dir/gfx/shader.cpp.o.d -o CMakeFiles/engine.dir/gfx/shader.cpp.o -c /home/chiefkat/Documents/vscodeprojects/basket/gfx/shader.cpp

CMakeFiles/engine.dir/gfx/shader.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/engine.dir/gfx/shader.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/chiefkat/Documents/vscodeprojects/basket/gfx/shader.cpp > CMakeFiles/engine.dir/gfx/shader.cpp.i

CMakeFiles/engine.dir/gfx/shader.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/engine.dir/gfx/shader.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/chiefkat/Documents/vscodeprojects/basket/gfx/shader.cpp -o CMakeFiles/engine.dir/gfx/shader.cpp.s

CMakeFiles/engine.dir/nms/glad/src/gl.c.o: CMakeFiles/engine.dir/flags.make
CMakeFiles/engine.dir/nms/glad/src/gl.c.o: /home/chiefkat/Documents/vscodeprojects/basket/nms/glad/src/gl.c
CMakeFiles/engine.dir/nms/glad/src/gl.c.o: CMakeFiles/engine.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/chiefkat/Documents/vscodeprojects/basket/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/engine.dir/nms/glad/src/gl.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/engine.dir/nms/glad/src/gl.c.o -MF CMakeFiles/engine.dir/nms/glad/src/gl.c.o.d -o CMakeFiles/engine.dir/nms/glad/src/gl.c.o -c /home/chiefkat/Documents/vscodeprojects/basket/nms/glad/src/gl.c

CMakeFiles/engine.dir/nms/glad/src/gl.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/engine.dir/nms/glad/src/gl.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/chiefkat/Documents/vscodeprojects/basket/nms/glad/src/gl.c > CMakeFiles/engine.dir/nms/glad/src/gl.c.i

CMakeFiles/engine.dir/nms/glad/src/gl.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/engine.dir/nms/glad/src/gl.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/chiefkat/Documents/vscodeprojects/basket/nms/glad/src/gl.c -o CMakeFiles/engine.dir/nms/glad/src/gl.c.s

# Object files for target engine
engine_OBJECTS = \
"CMakeFiles/engine.dir/main.cpp.o" \
"CMakeFiles/engine.dir/multiplayer.cpp.o" \
"CMakeFiles/engine.dir/events.cpp.o" \
"CMakeFiles/engine.dir/gfx/shader.cpp.o" \
"CMakeFiles/engine.dir/nms/glad/src/gl.c.o"

# External object files for target engine
engine_EXTERNAL_OBJECTS =

engine: CMakeFiles/engine.dir/main.cpp.o
engine: CMakeFiles/engine.dir/multiplayer.cpp.o
engine: CMakeFiles/engine.dir/events.cpp.o
engine: CMakeFiles/engine.dir/gfx/shader.cpp.o
engine: CMakeFiles/engine.dir/nms/glad/src/gl.c.o
engine: CMakeFiles/engine.dir/build.make
engine: CMAKE_BINARY_DIR/src/libglfw3.a
engine: /usr/lib/librt.a
engine: /usr/lib/libm.so
engine: CMakeFiles/engine.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/chiefkat/Documents/vscodeprojects/basket/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX executable engine"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/engine.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/engine.dir/build: engine
.PHONY : CMakeFiles/engine.dir/build

CMakeFiles/engine.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/engine.dir/cmake_clean.cmake
.PHONY : CMakeFiles/engine.dir/clean

CMakeFiles/engine.dir/depend:
	cd /home/chiefkat/Documents/vscodeprojects/basket/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/chiefkat/Documents/vscodeprojects/basket /home/chiefkat/Documents/vscodeprojects/basket /home/chiefkat/Documents/vscodeprojects/basket/build /home/chiefkat/Documents/vscodeprojects/basket/build /home/chiefkat/Documents/vscodeprojects/basket/build/CMakeFiles/engine.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/engine.dir/depend

