cmake_minimum_required(VERSION 2.8.3)
project(Exapmles)

# Set a default build type if and only if user did not define one as command
# line options and he did not give custom CFLAGS or CXXFLAGS. Otherwise, flags
# from default build type would overwrite user-defined ones.
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_C_FLAGS AND NOT CMAKE_CXX_FLAGS)
    set(CMAKE_BUILD_TYPE Release)
endif ()
string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)

# -----------------------------------------------------------------------------
# Default library type
# -----------------------------------------------------------------------------
set(2D_DIR "${CMAKE_SOURCE_DIR}/2d")
set(file_chooser_DIR "${CMAKE_SOURCE_DIR}/file_chooser")
set(input_DIR "${CMAKE_SOURCE_DIR}/input")
set(scripting_DIR "${CMAKE_SOURCE_DIR}/scripting")
set(url_loader_DIR "${CMAKE_SOURCE_DIR}/url_loader")

# -----------------------------------------------------------------------------
# Default target names (can be overrriden in Options${PORT}.cmake file)
# -----------------------------------------------------------------------------
set(2D_LIBRARY_NAME ppapi_example_paint_manager)
set(file_chooser_LIBRARY_NAME ppapi_example_file_chooser)
set(input_LIBRARY_NAME ppapi_example_input)
set(scripting_LIBRARY_NAME ppapi_example_scripting)
set(url_loader_LIBRARY_NAME ppapi_example_url_loader)

# -----------------------------------------------------------------------------
# Default library types
# -----------------------------------------------------------------------------

set(2D_LIBRARY_TYPE SHARED)
set(file_chooser_LIBRARY_TYPE SHARED)
set(input_LIBRARY_TYPE SHARED)
set(scripting_LIBRARY_TYPE SHARED)
set(url_loader_LIBRARY_TYPE SHARED)

# -----------------------------------------------------------------------------
# Add module directories
# -----------------------------------------------------------------------------
add_subdirectory(2d)
add_subdirectory(file_chooser)
add_subdirectory(input)
add_subdirectory(scripting)
add_subdirectory(url_loader)

