# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

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

# Suppress display of executed commands.
$$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /tmp/tmp.DKyhPZMIc8

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /tmp/tmp.DKyhPZMIc8/cmake-build-debug-default

# Include any dependencies generated for this target.
include CMakeFiles/daemon_process.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/daemon_process.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/daemon_process.dir/flags.make

CMakeFiles/daemon_process.dir/main.cpp.o: CMakeFiles/daemon_process.dir/flags.make
CMakeFiles/daemon_process.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.DKyhPZMIc8/cmake-build-debug-default/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/daemon_process.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/daemon_process.dir/main.cpp.o -c /tmp/tmp.DKyhPZMIc8/main.cpp

CMakeFiles/daemon_process.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/daemon_process.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.DKyhPZMIc8/main.cpp > CMakeFiles/daemon_process.dir/main.cpp.i

CMakeFiles/daemon_process.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/daemon_process.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.DKyhPZMIc8/main.cpp -o CMakeFiles/daemon_process.dir/main.cpp.s

CMakeFiles/daemon_process.dir/doorLock.cpp.o: CMakeFiles/daemon_process.dir/flags.make
CMakeFiles/daemon_process.dir/doorLock.cpp.o: ../doorLock.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.DKyhPZMIc8/cmake-build-debug-default/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/daemon_process.dir/doorLock.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/daemon_process.dir/doorLock.cpp.o -c /tmp/tmp.DKyhPZMIc8/doorLock.cpp

CMakeFiles/daemon_process.dir/doorLock.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/daemon_process.dir/doorLock.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.DKyhPZMIc8/doorLock.cpp > CMakeFiles/daemon_process.dir/doorLock.cpp.i

CMakeFiles/daemon_process.dir/doorLock.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/daemon_process.dir/doorLock.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.DKyhPZMIc8/doorLock.cpp -o CMakeFiles/daemon_process.dir/doorLock.cpp.s

CMakeFiles/daemon_process.dir/camera.cpp.o: CMakeFiles/daemon_process.dir/flags.make
CMakeFiles/daemon_process.dir/camera.cpp.o: ../camera.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.DKyhPZMIc8/cmake-build-debug-default/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/daemon_process.dir/camera.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/daemon_process.dir/camera.cpp.o -c /tmp/tmp.DKyhPZMIc8/camera.cpp

CMakeFiles/daemon_process.dir/camera.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/daemon_process.dir/camera.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.DKyhPZMIc8/camera.cpp > CMakeFiles/daemon_process.dir/camera.cpp.i

CMakeFiles/daemon_process.dir/camera.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/daemon_process.dir/camera.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.DKyhPZMIc8/camera.cpp -o CMakeFiles/daemon_process.dir/camera.cpp.s

CMakeFiles/daemon_process.dir/terminal.cpp.o: CMakeFiles/daemon_process.dir/flags.make
CMakeFiles/daemon_process.dir/terminal.cpp.o: ../terminal.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.DKyhPZMIc8/cmake-build-debug-default/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/daemon_process.dir/terminal.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/daemon_process.dir/terminal.cpp.o -c /tmp/tmp.DKyhPZMIc8/terminal.cpp

CMakeFiles/daemon_process.dir/terminal.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/daemon_process.dir/terminal.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.DKyhPZMIc8/terminal.cpp > CMakeFiles/daemon_process.dir/terminal.cpp.i

CMakeFiles/daemon_process.dir/terminal.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/daemon_process.dir/terminal.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.DKyhPZMIc8/terminal.cpp -o CMakeFiles/daemon_process.dir/terminal.cpp.s

CMakeFiles/daemon_process.dir/logger.cpp.o: CMakeFiles/daemon_process.dir/flags.make
CMakeFiles/daemon_process.dir/logger.cpp.o: ../logger.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.DKyhPZMIc8/cmake-build-debug-default/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/daemon_process.dir/logger.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/daemon_process.dir/logger.cpp.o -c /tmp/tmp.DKyhPZMIc8/logger.cpp

CMakeFiles/daemon_process.dir/logger.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/daemon_process.dir/logger.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.DKyhPZMIc8/logger.cpp > CMakeFiles/daemon_process.dir/logger.cpp.i

CMakeFiles/daemon_process.dir/logger.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/daemon_process.dir/logger.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.DKyhPZMIc8/logger.cpp -o CMakeFiles/daemon_process.dir/logger.cpp.s

# Object files for target daemon_process
daemon_process_OBJECTS = \
"CMakeFiles/daemon_process.dir/main.cpp.o" \
"CMakeFiles/daemon_process.dir/doorLock.cpp.o" \
"CMakeFiles/daemon_process.dir/camera.cpp.o" \
"CMakeFiles/daemon_process.dir/terminal.cpp.o" \
"CMakeFiles/daemon_process.dir/logger.cpp.o"

# External object files for target daemon_process
daemon_process_EXTERNAL_OBJECTS =

daemon_process: CMakeFiles/daemon_process.dir/main.cpp.o
daemon_process: CMakeFiles/daemon_process.dir/doorLock.cpp.o
daemon_process: CMakeFiles/daemon_process.dir/camera.cpp.o
daemon_process: CMakeFiles/daemon_process.dir/terminal.cpp.o
daemon_process: CMakeFiles/daemon_process.dir/logger.cpp.o
daemon_process: CMakeFiles/daemon_process.dir/build.make
daemon_process: /usr/local/lib/libopencv_stitching.so.3.4.0
daemon_process: /usr/local/lib/libopencv_superres.so.3.4.0
daemon_process: /usr/local/lib/libopencv_videostab.so.3.4.0
daemon_process: /usr/local/lib/libopencv_aruco.so.3.4.0
daemon_process: /usr/local/lib/libopencv_bgsegm.so.3.4.0
daemon_process: /usr/local/lib/libopencv_bioinspired.so.3.4.0
daemon_process: /usr/local/lib/libopencv_ccalib.so.3.4.0
daemon_process: /usr/local/lib/libopencv_dpm.so.3.4.0
daemon_process: /usr/local/lib/libopencv_face.so.3.4.0
daemon_process: /usr/local/lib/libopencv_freetype.so.3.4.0
daemon_process: /usr/local/lib/libopencv_fuzzy.so.3.4.0
daemon_process: /usr/local/lib/libopencv_img_hash.so.3.4.0
daemon_process: /usr/local/lib/libopencv_line_descriptor.so.3.4.0
daemon_process: /usr/local/lib/libopencv_optflow.so.3.4.0
daemon_process: /usr/local/lib/libopencv_reg.so.3.4.0
daemon_process: /usr/local/lib/libopencv_rgbd.so.3.4.0
daemon_process: /usr/local/lib/libopencv_saliency.so.3.4.0
daemon_process: /usr/local/lib/libopencv_stereo.so.3.4.0
daemon_process: /usr/local/lib/libopencv_structured_light.so.3.4.0
daemon_process: /usr/local/lib/libopencv_surface_matching.so.3.4.0
daemon_process: /usr/local/lib/libopencv_tracking.so.3.4.0
daemon_process: /usr/local/lib/libopencv_xfeatures2d.so.3.4.0
daemon_process: /usr/local/lib/libopencv_ximgproc.so.3.4.0
daemon_process: /usr/local/lib/libopencv_xobjdetect.so.3.4.0
daemon_process: /usr/local/lib/libopencv_xphoto.so.3.4.0
daemon_process: /usr/local/lib/libopencv_shape.so.3.4.0
daemon_process: /usr/local/lib/libopencv_photo.so.3.4.0
daemon_process: /usr/local/lib/libopencv_datasets.so.3.4.0
daemon_process: /usr/local/lib/libopencv_plot.so.3.4.0
daemon_process: /usr/local/lib/libopencv_text.so.3.4.0
daemon_process: /usr/local/lib/libopencv_dnn.so.3.4.0
daemon_process: /usr/local/lib/libopencv_ml.so.3.4.0
daemon_process: /usr/local/lib/libopencv_video.so.3.4.0
daemon_process: /usr/local/lib/libopencv_calib3d.so.3.4.0
daemon_process: /usr/local/lib/libopencv_features2d.so.3.4.0
daemon_process: /usr/local/lib/libopencv_highgui.so.3.4.0
daemon_process: /usr/local/lib/libopencv_videoio.so.3.4.0
daemon_process: /usr/local/lib/libopencv_phase_unwrapping.so.3.4.0
daemon_process: /usr/local/lib/libopencv_flann.so.3.4.0
daemon_process: /usr/local/lib/libopencv_imgcodecs.so.3.4.0
daemon_process: /usr/local/lib/libopencv_objdetect.so.3.4.0
daemon_process: /usr/local/lib/libopencv_imgproc.so.3.4.0
daemon_process: /usr/local/lib/libopencv_core.so.3.4.0
daemon_process: CMakeFiles/daemon_process.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/tmp/tmp.DKyhPZMIc8/cmake-build-debug-default/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX executable daemon_process"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/daemon_process.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/daemon_process.dir/build: daemon_process

.PHONY : CMakeFiles/daemon_process.dir/build

CMakeFiles/daemon_process.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/daemon_process.dir/cmake_clean.cmake
.PHONY : CMakeFiles/daemon_process.dir/clean

CMakeFiles/daemon_process.dir/depend:
	cd /tmp/tmp.DKyhPZMIc8/cmake-build-debug-default && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/tmp.DKyhPZMIc8 /tmp/tmp.DKyhPZMIc8 /tmp/tmp.DKyhPZMIc8/cmake-build-debug-default /tmp/tmp.DKyhPZMIc8/cmake-build-debug-default /tmp/tmp.DKyhPZMIc8/cmake-build-debug-default/CMakeFiles/daemon_process.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/daemon_process.dir/depend
