# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.23

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.23.2/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.23.2/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/scott/Downloads/quazip-1.3

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/scott/Downloads/quazip-1.3

# Include any dependencies generated for this target.
include quazip/CMakeFiles/QuaZip.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include quazip/CMakeFiles/QuaZip.dir/compiler_depend.make

# Include the progress variables for this target.
include quazip/CMakeFiles/QuaZip.dir/progress.make

# Include the compile flags for this target's objects.
include quazip/CMakeFiles/QuaZip.dir/flags.make

quazip/CMakeFiles/QuaZip.dir/QuaZip_autogen/mocs_compilation.cpp.o: quazip/CMakeFiles/QuaZip.dir/flags.make
quazip/CMakeFiles/QuaZip.dir/QuaZip_autogen/mocs_compilation.cpp.o: quazip/QuaZip_autogen/mocs_compilation.cpp
quazip/CMakeFiles/QuaZip.dir/QuaZip_autogen/mocs_compilation.cpp.o: quazip/CMakeFiles/QuaZip.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/scott/Downloads/quazip-1.3/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object quazip/CMakeFiles/QuaZip.dir/QuaZip_autogen/mocs_compilation.cpp.o"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT quazip/CMakeFiles/QuaZip.dir/QuaZip_autogen/mocs_compilation.cpp.o -MF CMakeFiles/QuaZip.dir/QuaZip_autogen/mocs_compilation.cpp.o.d -o CMakeFiles/QuaZip.dir/QuaZip_autogen/mocs_compilation.cpp.o -c /Users/scott/Downloads/quazip-1.3/quazip/QuaZip_autogen/mocs_compilation.cpp

quazip/CMakeFiles/QuaZip.dir/QuaZip_autogen/mocs_compilation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QuaZip.dir/QuaZip_autogen/mocs_compilation.cpp.i"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/scott/Downloads/quazip-1.3/quazip/QuaZip_autogen/mocs_compilation.cpp > CMakeFiles/QuaZip.dir/QuaZip_autogen/mocs_compilation.cpp.i

quazip/CMakeFiles/QuaZip.dir/QuaZip_autogen/mocs_compilation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QuaZip.dir/QuaZip_autogen/mocs_compilation.cpp.s"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/scott/Downloads/quazip-1.3/quazip/QuaZip_autogen/mocs_compilation.cpp -o CMakeFiles/QuaZip.dir/QuaZip_autogen/mocs_compilation.cpp.s

quazip/CMakeFiles/QuaZip.dir/unzip.c.o: quazip/CMakeFiles/QuaZip.dir/flags.make
quazip/CMakeFiles/QuaZip.dir/unzip.c.o: quazip/unzip.c
quazip/CMakeFiles/QuaZip.dir/unzip.c.o: quazip/CMakeFiles/QuaZip.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/scott/Downloads/quazip-1.3/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object quazip/CMakeFiles/QuaZip.dir/unzip.c.o"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT quazip/CMakeFiles/QuaZip.dir/unzip.c.o -MF CMakeFiles/QuaZip.dir/unzip.c.o.d -o CMakeFiles/QuaZip.dir/unzip.c.o -c /Users/scott/Downloads/quazip-1.3/quazip/unzip.c

quazip/CMakeFiles/QuaZip.dir/unzip.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/QuaZip.dir/unzip.c.i"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/scott/Downloads/quazip-1.3/quazip/unzip.c > CMakeFiles/QuaZip.dir/unzip.c.i

quazip/CMakeFiles/QuaZip.dir/unzip.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/QuaZip.dir/unzip.c.s"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/scott/Downloads/quazip-1.3/quazip/unzip.c -o CMakeFiles/QuaZip.dir/unzip.c.s

quazip/CMakeFiles/QuaZip.dir/zip.c.o: quazip/CMakeFiles/QuaZip.dir/flags.make
quazip/CMakeFiles/QuaZip.dir/zip.c.o: quazip/zip.c
quazip/CMakeFiles/QuaZip.dir/zip.c.o: quazip/CMakeFiles/QuaZip.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/scott/Downloads/quazip-1.3/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object quazip/CMakeFiles/QuaZip.dir/zip.c.o"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT quazip/CMakeFiles/QuaZip.dir/zip.c.o -MF CMakeFiles/QuaZip.dir/zip.c.o.d -o CMakeFiles/QuaZip.dir/zip.c.o -c /Users/scott/Downloads/quazip-1.3/quazip/zip.c

quazip/CMakeFiles/QuaZip.dir/zip.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/QuaZip.dir/zip.c.i"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/scott/Downloads/quazip-1.3/quazip/zip.c > CMakeFiles/QuaZip.dir/zip.c.i

quazip/CMakeFiles/QuaZip.dir/zip.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/QuaZip.dir/zip.c.s"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/scott/Downloads/quazip-1.3/quazip/zip.c -o CMakeFiles/QuaZip.dir/zip.c.s

quazip/CMakeFiles/QuaZip.dir/JlCompress.cpp.o: quazip/CMakeFiles/QuaZip.dir/flags.make
quazip/CMakeFiles/QuaZip.dir/JlCompress.cpp.o: quazip/JlCompress.cpp
quazip/CMakeFiles/QuaZip.dir/JlCompress.cpp.o: quazip/CMakeFiles/QuaZip.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/scott/Downloads/quazip-1.3/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object quazip/CMakeFiles/QuaZip.dir/JlCompress.cpp.o"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT quazip/CMakeFiles/QuaZip.dir/JlCompress.cpp.o -MF CMakeFiles/QuaZip.dir/JlCompress.cpp.o.d -o CMakeFiles/QuaZip.dir/JlCompress.cpp.o -c /Users/scott/Downloads/quazip-1.3/quazip/JlCompress.cpp

quazip/CMakeFiles/QuaZip.dir/JlCompress.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QuaZip.dir/JlCompress.cpp.i"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/scott/Downloads/quazip-1.3/quazip/JlCompress.cpp > CMakeFiles/QuaZip.dir/JlCompress.cpp.i

quazip/CMakeFiles/QuaZip.dir/JlCompress.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QuaZip.dir/JlCompress.cpp.s"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/scott/Downloads/quazip-1.3/quazip/JlCompress.cpp -o CMakeFiles/QuaZip.dir/JlCompress.cpp.s

quazip/CMakeFiles/QuaZip.dir/qioapi.cpp.o: quazip/CMakeFiles/QuaZip.dir/flags.make
quazip/CMakeFiles/QuaZip.dir/qioapi.cpp.o: quazip/qioapi.cpp
quazip/CMakeFiles/QuaZip.dir/qioapi.cpp.o: quazip/CMakeFiles/QuaZip.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/scott/Downloads/quazip-1.3/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object quazip/CMakeFiles/QuaZip.dir/qioapi.cpp.o"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT quazip/CMakeFiles/QuaZip.dir/qioapi.cpp.o -MF CMakeFiles/QuaZip.dir/qioapi.cpp.o.d -o CMakeFiles/QuaZip.dir/qioapi.cpp.o -c /Users/scott/Downloads/quazip-1.3/quazip/qioapi.cpp

quazip/CMakeFiles/QuaZip.dir/qioapi.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QuaZip.dir/qioapi.cpp.i"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/scott/Downloads/quazip-1.3/quazip/qioapi.cpp > CMakeFiles/QuaZip.dir/qioapi.cpp.i

quazip/CMakeFiles/QuaZip.dir/qioapi.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QuaZip.dir/qioapi.cpp.s"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/scott/Downloads/quazip-1.3/quazip/qioapi.cpp -o CMakeFiles/QuaZip.dir/qioapi.cpp.s

quazip/CMakeFiles/QuaZip.dir/quaadler32.cpp.o: quazip/CMakeFiles/QuaZip.dir/flags.make
quazip/CMakeFiles/QuaZip.dir/quaadler32.cpp.o: quazip/quaadler32.cpp
quazip/CMakeFiles/QuaZip.dir/quaadler32.cpp.o: quazip/CMakeFiles/QuaZip.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/scott/Downloads/quazip-1.3/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object quazip/CMakeFiles/QuaZip.dir/quaadler32.cpp.o"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT quazip/CMakeFiles/QuaZip.dir/quaadler32.cpp.o -MF CMakeFiles/QuaZip.dir/quaadler32.cpp.o.d -o CMakeFiles/QuaZip.dir/quaadler32.cpp.o -c /Users/scott/Downloads/quazip-1.3/quazip/quaadler32.cpp

quazip/CMakeFiles/QuaZip.dir/quaadler32.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QuaZip.dir/quaadler32.cpp.i"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/scott/Downloads/quazip-1.3/quazip/quaadler32.cpp > CMakeFiles/QuaZip.dir/quaadler32.cpp.i

quazip/CMakeFiles/QuaZip.dir/quaadler32.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QuaZip.dir/quaadler32.cpp.s"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/scott/Downloads/quazip-1.3/quazip/quaadler32.cpp -o CMakeFiles/QuaZip.dir/quaadler32.cpp.s

quazip/CMakeFiles/QuaZip.dir/quachecksum32.cpp.o: quazip/CMakeFiles/QuaZip.dir/flags.make
quazip/CMakeFiles/QuaZip.dir/quachecksum32.cpp.o: quazip/quachecksum32.cpp
quazip/CMakeFiles/QuaZip.dir/quachecksum32.cpp.o: quazip/CMakeFiles/QuaZip.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/scott/Downloads/quazip-1.3/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object quazip/CMakeFiles/QuaZip.dir/quachecksum32.cpp.o"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT quazip/CMakeFiles/QuaZip.dir/quachecksum32.cpp.o -MF CMakeFiles/QuaZip.dir/quachecksum32.cpp.o.d -o CMakeFiles/QuaZip.dir/quachecksum32.cpp.o -c /Users/scott/Downloads/quazip-1.3/quazip/quachecksum32.cpp

quazip/CMakeFiles/QuaZip.dir/quachecksum32.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QuaZip.dir/quachecksum32.cpp.i"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/scott/Downloads/quazip-1.3/quazip/quachecksum32.cpp > CMakeFiles/QuaZip.dir/quachecksum32.cpp.i

quazip/CMakeFiles/QuaZip.dir/quachecksum32.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QuaZip.dir/quachecksum32.cpp.s"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/scott/Downloads/quazip-1.3/quazip/quachecksum32.cpp -o CMakeFiles/QuaZip.dir/quachecksum32.cpp.s

quazip/CMakeFiles/QuaZip.dir/quacrc32.cpp.o: quazip/CMakeFiles/QuaZip.dir/flags.make
quazip/CMakeFiles/QuaZip.dir/quacrc32.cpp.o: quazip/quacrc32.cpp
quazip/CMakeFiles/QuaZip.dir/quacrc32.cpp.o: quazip/CMakeFiles/QuaZip.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/scott/Downloads/quazip-1.3/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object quazip/CMakeFiles/QuaZip.dir/quacrc32.cpp.o"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT quazip/CMakeFiles/QuaZip.dir/quacrc32.cpp.o -MF CMakeFiles/QuaZip.dir/quacrc32.cpp.o.d -o CMakeFiles/QuaZip.dir/quacrc32.cpp.o -c /Users/scott/Downloads/quazip-1.3/quazip/quacrc32.cpp

quazip/CMakeFiles/QuaZip.dir/quacrc32.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QuaZip.dir/quacrc32.cpp.i"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/scott/Downloads/quazip-1.3/quazip/quacrc32.cpp > CMakeFiles/QuaZip.dir/quacrc32.cpp.i

quazip/CMakeFiles/QuaZip.dir/quacrc32.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QuaZip.dir/quacrc32.cpp.s"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/scott/Downloads/quazip-1.3/quazip/quacrc32.cpp -o CMakeFiles/QuaZip.dir/quacrc32.cpp.s

quazip/CMakeFiles/QuaZip.dir/quagzipfile.cpp.o: quazip/CMakeFiles/QuaZip.dir/flags.make
quazip/CMakeFiles/QuaZip.dir/quagzipfile.cpp.o: quazip/quagzipfile.cpp
quazip/CMakeFiles/QuaZip.dir/quagzipfile.cpp.o: quazip/CMakeFiles/QuaZip.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/scott/Downloads/quazip-1.3/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object quazip/CMakeFiles/QuaZip.dir/quagzipfile.cpp.o"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT quazip/CMakeFiles/QuaZip.dir/quagzipfile.cpp.o -MF CMakeFiles/QuaZip.dir/quagzipfile.cpp.o.d -o CMakeFiles/QuaZip.dir/quagzipfile.cpp.o -c /Users/scott/Downloads/quazip-1.3/quazip/quagzipfile.cpp

quazip/CMakeFiles/QuaZip.dir/quagzipfile.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QuaZip.dir/quagzipfile.cpp.i"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/scott/Downloads/quazip-1.3/quazip/quagzipfile.cpp > CMakeFiles/QuaZip.dir/quagzipfile.cpp.i

quazip/CMakeFiles/QuaZip.dir/quagzipfile.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QuaZip.dir/quagzipfile.cpp.s"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/scott/Downloads/quazip-1.3/quazip/quagzipfile.cpp -o CMakeFiles/QuaZip.dir/quagzipfile.cpp.s

quazip/CMakeFiles/QuaZip.dir/quaziodevice.cpp.o: quazip/CMakeFiles/QuaZip.dir/flags.make
quazip/CMakeFiles/QuaZip.dir/quaziodevice.cpp.o: quazip/quaziodevice.cpp
quazip/CMakeFiles/QuaZip.dir/quaziodevice.cpp.o: quazip/CMakeFiles/QuaZip.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/scott/Downloads/quazip-1.3/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object quazip/CMakeFiles/QuaZip.dir/quaziodevice.cpp.o"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT quazip/CMakeFiles/QuaZip.dir/quaziodevice.cpp.o -MF CMakeFiles/QuaZip.dir/quaziodevice.cpp.o.d -o CMakeFiles/QuaZip.dir/quaziodevice.cpp.o -c /Users/scott/Downloads/quazip-1.3/quazip/quaziodevice.cpp

quazip/CMakeFiles/QuaZip.dir/quaziodevice.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QuaZip.dir/quaziodevice.cpp.i"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/scott/Downloads/quazip-1.3/quazip/quaziodevice.cpp > CMakeFiles/QuaZip.dir/quaziodevice.cpp.i

quazip/CMakeFiles/QuaZip.dir/quaziodevice.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QuaZip.dir/quaziodevice.cpp.s"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/scott/Downloads/quazip-1.3/quazip/quaziodevice.cpp -o CMakeFiles/QuaZip.dir/quaziodevice.cpp.s

quazip/CMakeFiles/QuaZip.dir/quazip.cpp.o: quazip/CMakeFiles/QuaZip.dir/flags.make
quazip/CMakeFiles/QuaZip.dir/quazip.cpp.o: quazip/quazip.cpp
quazip/CMakeFiles/QuaZip.dir/quazip.cpp.o: quazip/CMakeFiles/QuaZip.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/scott/Downloads/quazip-1.3/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object quazip/CMakeFiles/QuaZip.dir/quazip.cpp.o"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT quazip/CMakeFiles/QuaZip.dir/quazip.cpp.o -MF CMakeFiles/QuaZip.dir/quazip.cpp.o.d -o CMakeFiles/QuaZip.dir/quazip.cpp.o -c /Users/scott/Downloads/quazip-1.3/quazip/quazip.cpp

quazip/CMakeFiles/QuaZip.dir/quazip.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QuaZip.dir/quazip.cpp.i"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/scott/Downloads/quazip-1.3/quazip/quazip.cpp > CMakeFiles/QuaZip.dir/quazip.cpp.i

quazip/CMakeFiles/QuaZip.dir/quazip.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QuaZip.dir/quazip.cpp.s"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/scott/Downloads/quazip-1.3/quazip/quazip.cpp -o CMakeFiles/QuaZip.dir/quazip.cpp.s

quazip/CMakeFiles/QuaZip.dir/quazipdir.cpp.o: quazip/CMakeFiles/QuaZip.dir/flags.make
quazip/CMakeFiles/QuaZip.dir/quazipdir.cpp.o: quazip/quazipdir.cpp
quazip/CMakeFiles/QuaZip.dir/quazipdir.cpp.o: quazip/CMakeFiles/QuaZip.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/scott/Downloads/quazip-1.3/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object quazip/CMakeFiles/QuaZip.dir/quazipdir.cpp.o"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT quazip/CMakeFiles/QuaZip.dir/quazipdir.cpp.o -MF CMakeFiles/QuaZip.dir/quazipdir.cpp.o.d -o CMakeFiles/QuaZip.dir/quazipdir.cpp.o -c /Users/scott/Downloads/quazip-1.3/quazip/quazipdir.cpp

quazip/CMakeFiles/QuaZip.dir/quazipdir.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QuaZip.dir/quazipdir.cpp.i"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/scott/Downloads/quazip-1.3/quazip/quazipdir.cpp > CMakeFiles/QuaZip.dir/quazipdir.cpp.i

quazip/CMakeFiles/QuaZip.dir/quazipdir.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QuaZip.dir/quazipdir.cpp.s"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/scott/Downloads/quazip-1.3/quazip/quazipdir.cpp -o CMakeFiles/QuaZip.dir/quazipdir.cpp.s

quazip/CMakeFiles/QuaZip.dir/quazipfile.cpp.o: quazip/CMakeFiles/QuaZip.dir/flags.make
quazip/CMakeFiles/QuaZip.dir/quazipfile.cpp.o: quazip/quazipfile.cpp
quazip/CMakeFiles/QuaZip.dir/quazipfile.cpp.o: quazip/CMakeFiles/QuaZip.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/scott/Downloads/quazip-1.3/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building CXX object quazip/CMakeFiles/QuaZip.dir/quazipfile.cpp.o"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT quazip/CMakeFiles/QuaZip.dir/quazipfile.cpp.o -MF CMakeFiles/QuaZip.dir/quazipfile.cpp.o.d -o CMakeFiles/QuaZip.dir/quazipfile.cpp.o -c /Users/scott/Downloads/quazip-1.3/quazip/quazipfile.cpp

quazip/CMakeFiles/QuaZip.dir/quazipfile.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QuaZip.dir/quazipfile.cpp.i"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/scott/Downloads/quazip-1.3/quazip/quazipfile.cpp > CMakeFiles/QuaZip.dir/quazipfile.cpp.i

quazip/CMakeFiles/QuaZip.dir/quazipfile.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QuaZip.dir/quazipfile.cpp.s"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/scott/Downloads/quazip-1.3/quazip/quazipfile.cpp -o CMakeFiles/QuaZip.dir/quazipfile.cpp.s

quazip/CMakeFiles/QuaZip.dir/quazipfileinfo.cpp.o: quazip/CMakeFiles/QuaZip.dir/flags.make
quazip/CMakeFiles/QuaZip.dir/quazipfileinfo.cpp.o: quazip/quazipfileinfo.cpp
quazip/CMakeFiles/QuaZip.dir/quazipfileinfo.cpp.o: quazip/CMakeFiles/QuaZip.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/scott/Downloads/quazip-1.3/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building CXX object quazip/CMakeFiles/QuaZip.dir/quazipfileinfo.cpp.o"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT quazip/CMakeFiles/QuaZip.dir/quazipfileinfo.cpp.o -MF CMakeFiles/QuaZip.dir/quazipfileinfo.cpp.o.d -o CMakeFiles/QuaZip.dir/quazipfileinfo.cpp.o -c /Users/scott/Downloads/quazip-1.3/quazip/quazipfileinfo.cpp

quazip/CMakeFiles/QuaZip.dir/quazipfileinfo.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QuaZip.dir/quazipfileinfo.cpp.i"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/scott/Downloads/quazip-1.3/quazip/quazipfileinfo.cpp > CMakeFiles/QuaZip.dir/quazipfileinfo.cpp.i

quazip/CMakeFiles/QuaZip.dir/quazipfileinfo.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QuaZip.dir/quazipfileinfo.cpp.s"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/scott/Downloads/quazip-1.3/quazip/quazipfileinfo.cpp -o CMakeFiles/QuaZip.dir/quazipfileinfo.cpp.s

quazip/CMakeFiles/QuaZip.dir/quazipnewinfo.cpp.o: quazip/CMakeFiles/QuaZip.dir/flags.make
quazip/CMakeFiles/QuaZip.dir/quazipnewinfo.cpp.o: quazip/quazipnewinfo.cpp
quazip/CMakeFiles/QuaZip.dir/quazipnewinfo.cpp.o: quazip/CMakeFiles/QuaZip.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/scott/Downloads/quazip-1.3/CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building CXX object quazip/CMakeFiles/QuaZip.dir/quazipnewinfo.cpp.o"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT quazip/CMakeFiles/QuaZip.dir/quazipnewinfo.cpp.o -MF CMakeFiles/QuaZip.dir/quazipnewinfo.cpp.o.d -o CMakeFiles/QuaZip.dir/quazipnewinfo.cpp.o -c /Users/scott/Downloads/quazip-1.3/quazip/quazipnewinfo.cpp

quazip/CMakeFiles/QuaZip.dir/quazipnewinfo.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/QuaZip.dir/quazipnewinfo.cpp.i"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/scott/Downloads/quazip-1.3/quazip/quazipnewinfo.cpp > CMakeFiles/QuaZip.dir/quazipnewinfo.cpp.i

quazip/CMakeFiles/QuaZip.dir/quazipnewinfo.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/QuaZip.dir/quazipnewinfo.cpp.s"
	cd /Users/scott/Downloads/quazip-1.3/quazip && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/scott/Downloads/quazip-1.3/quazip/quazipnewinfo.cpp -o CMakeFiles/QuaZip.dir/quazipnewinfo.cpp.s

# Object files for target QuaZip
QuaZip_OBJECTS = \
"CMakeFiles/QuaZip.dir/QuaZip_autogen/mocs_compilation.cpp.o" \
"CMakeFiles/QuaZip.dir/unzip.c.o" \
"CMakeFiles/QuaZip.dir/zip.c.o" \
"CMakeFiles/QuaZip.dir/JlCompress.cpp.o" \
"CMakeFiles/QuaZip.dir/qioapi.cpp.o" \
"CMakeFiles/QuaZip.dir/quaadler32.cpp.o" \
"CMakeFiles/QuaZip.dir/quachecksum32.cpp.o" \
"CMakeFiles/QuaZip.dir/quacrc32.cpp.o" \
"CMakeFiles/QuaZip.dir/quagzipfile.cpp.o" \
"CMakeFiles/QuaZip.dir/quaziodevice.cpp.o" \
"CMakeFiles/QuaZip.dir/quazip.cpp.o" \
"CMakeFiles/QuaZip.dir/quazipdir.cpp.o" \
"CMakeFiles/QuaZip.dir/quazipfile.cpp.o" \
"CMakeFiles/QuaZip.dir/quazipfileinfo.cpp.o" \
"CMakeFiles/QuaZip.dir/quazipnewinfo.cpp.o"

# External object files for target QuaZip
QuaZip_EXTERNAL_OBJECTS =

quazip/libquazip1-qt6.1.3.dylib: quazip/CMakeFiles/QuaZip.dir/QuaZip_autogen/mocs_compilation.cpp.o
quazip/libquazip1-qt6.1.3.dylib: quazip/CMakeFiles/QuaZip.dir/unzip.c.o
quazip/libquazip1-qt6.1.3.dylib: quazip/CMakeFiles/QuaZip.dir/zip.c.o
quazip/libquazip1-qt6.1.3.dylib: quazip/CMakeFiles/QuaZip.dir/JlCompress.cpp.o
quazip/libquazip1-qt6.1.3.dylib: quazip/CMakeFiles/QuaZip.dir/qioapi.cpp.o
quazip/libquazip1-qt6.1.3.dylib: quazip/CMakeFiles/QuaZip.dir/quaadler32.cpp.o
quazip/libquazip1-qt6.1.3.dylib: quazip/CMakeFiles/QuaZip.dir/quachecksum32.cpp.o
quazip/libquazip1-qt6.1.3.dylib: quazip/CMakeFiles/QuaZip.dir/quacrc32.cpp.o
quazip/libquazip1-qt6.1.3.dylib: quazip/CMakeFiles/QuaZip.dir/quagzipfile.cpp.o
quazip/libquazip1-qt6.1.3.dylib: quazip/CMakeFiles/QuaZip.dir/quaziodevice.cpp.o
quazip/libquazip1-qt6.1.3.dylib: quazip/CMakeFiles/QuaZip.dir/quazip.cpp.o
quazip/libquazip1-qt6.1.3.dylib: quazip/CMakeFiles/QuaZip.dir/quazipdir.cpp.o
quazip/libquazip1-qt6.1.3.dylib: quazip/CMakeFiles/QuaZip.dir/quazipfile.cpp.o
quazip/libquazip1-qt6.1.3.dylib: quazip/CMakeFiles/QuaZip.dir/quazipfileinfo.cpp.o
quazip/libquazip1-qt6.1.3.dylib: quazip/CMakeFiles/QuaZip.dir/quazipnewinfo.cpp.o
quazip/libquazip1-qt6.1.3.dylib: quazip/CMakeFiles/QuaZip.dir/build.make
quazip/libquazip1-qt6.1.3.dylib: /opt/homebrew/lib/QtCore5Compat.framework/Versions/A/QtCore5Compat
quazip/libquazip1-qt6.1.3.dylib: /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX12.3.sdk/usr/lib/libz.tbd
quazip/libquazip1-qt6.1.3.dylib: /opt/homebrew/lib/QtCore.framework/Versions/A/QtCore
quazip/libquazip1-qt6.1.3.dylib: quazip/CMakeFiles/QuaZip.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/scott/Downloads/quazip-1.3/CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Linking CXX shared library libquazip1-qt6.dylib"
	cd /Users/scott/Downloads/quazip-1.3/quazip && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/QuaZip.dir/link.txt --verbose=$(VERBOSE)
	cd /Users/scott/Downloads/quazip-1.3/quazip && $(CMAKE_COMMAND) -E cmake_symlink_library libquazip1-qt6.1.3.dylib libquazip1-qt6.1.3.0.dylib libquazip1-qt6.dylib

quazip/libquazip1-qt6.1.3.0.dylib: quazip/libquazip1-qt6.1.3.dylib
	@$(CMAKE_COMMAND) -E touch_nocreate quazip/libquazip1-qt6.1.3.0.dylib

quazip/libquazip1-qt6.dylib: quazip/libquazip1-qt6.1.3.dylib
	@$(CMAKE_COMMAND) -E touch_nocreate quazip/libquazip1-qt6.dylib

# Rule to build all files generated by this target.
quazip/CMakeFiles/QuaZip.dir/build: quazip/libquazip1-qt6.dylib
.PHONY : quazip/CMakeFiles/QuaZip.dir/build

quazip/CMakeFiles/QuaZip.dir/clean:
	cd /Users/scott/Downloads/quazip-1.3/quazip && $(CMAKE_COMMAND) -P CMakeFiles/QuaZip.dir/cmake_clean.cmake
.PHONY : quazip/CMakeFiles/QuaZip.dir/clean

quazip/CMakeFiles/QuaZip.dir/depend:
	cd /Users/scott/Downloads/quazip-1.3 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/scott/Downloads/quazip-1.3 /Users/scott/Downloads/quazip-1.3/quazip /Users/scott/Downloads/quazip-1.3 /Users/scott/Downloads/quazip-1.3/quazip /Users/scott/Downloads/quazip-1.3/quazip/CMakeFiles/QuaZip.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : quazip/CMakeFiles/QuaZip.dir/depend
