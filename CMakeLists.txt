cmake_minimum_required(VERSION 3.16)

project(
  OpenGL-GLFW-GLEW
  LANGUAGES CXX C
  DESCRIPTION "OpenGL + GLFW + GLEW (TheCherno Series)"
  VERSION 0.1.0
)

if(CMAKE_GENERATOR MATCHES "Ninja")
  add_compile_options("-fdiagnostics-color=always")
endif()


# Language setting
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_VERBOSE_NINJA ON)



# ---< Dependencies >----------------------------------------------------------


include(FetchContent)
Set(FETCHCONTENT_QUIET FALSE)


# -- SDL
FetchContent_Declare(
  sdl
  URL_PROGRESS    TRUE
  GIT_REPOSITORY  git@github.com:libsdl-org/SDL.git
  GIT_TAG         release-2.28.5
  GIT_CONFIG      "advice.detachedhead=false"
  GIT_SHALLOW     TRUE
  GIT_PROGRESS    TRUE
)
  FetchContent_GetProperties(sdl)
if (NOT sdl_POPULATED)
  set(FETCHCONTENT_QUIET NO)
  FetchContent_Populate(sdl)
  add_subdirectory(${sdl_SOURCE_DIR} ${sdl_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()


# -- GLAD
# Configure GLAD loader generation variables (lines below pulled from the GLAD top-level
# CMakeLists.txt file, with slight modifications.)
# Refer to https://github.com/Dav1dde/glad for more information
# Source: https://github.com/Hoshiningen/OpenGL-Template/blob/master/CMakeLists.txt
set(GLAD_OUT_DIR        "${CMAKE_CURRENT_BINARY_DIR}" CACHE STRING "Output directory" FORCE)
set(GLAD_PROFILE        "core" CACHE STRING "OpenGL profile" FORCE)
set(GLAD_API            "gl=3.3" CACHE STRING "API type/version pairs, like \"gl=3.2,gles=\", no version means latest" FORCE)
set(GLAD_GENERATOR      "c" CACHE STRING "Language to generate the binding for" FORCE)
set(GLAD_EXTENSIONS     "" CACHE STRING "Path to extensions file or comma separated list of extensions, if missing all extensions are included" FORCE)
set(GLAD_SPEC           "gl" CACHE STRING "Name of the spec" FORCE)
set(GLAD_ALL_EXTENSIONS OFF CACHE BOOL "Include all extensions instead of those specified by GLAD_EXTENSIONS" FORCE)
set(GLAD_NO_LOADER      OFF CACHE BOOL "No loader" FORCE)
set(GLAD_REPRODUCIBLE   OFF CACHE BOOL "Reproducible build" FORCE)
FetchContent_Declare(
  glad
  GIT_REPOSITORY  https://github.com/Dav1dde/glad
  GIT_TAG         v0.1.36
  GIT_CONFIG      "advice.detachedhead=false"
  GIT_SHALLOW     TRUE
  GIT_PROGRESS    TRUE
)
FetchContent_GetProperties(glad)
if (NOT glad_POPULATED)
  FetchContent_Populate(glad)
  # This excludes glad from being rebuilt when ALL_BUILD is built
  # it will only be built when a target is built that has a dependency on glad
  add_subdirectory(${glad_SOURCE_DIR} ${glad_BINARY_DIR} EXCLUDE_FROM_ALL)
  # Set the target's folders
  #set_target_properties(glad PROPERTIES FOLDER ${glad_BINARY_DIR}/glad)
  #set_target_properties(glad-generate-files PROPERTIES FOLDER ${glad_BINARY_DIR}/glad)
endif()


# -- OpenGL
find_package(OpenGL REQUIRED) # Some platforms need OpenGL.
if(NOT OpenGL_FOUND)
  if(WIN32)
    message(FATAL_ERROR "OpenGL development files not found. Please install some version of the Windows SDK.")
  elseif(Linux)
    message(FATAL_ERROR "OpenGL development files not found. Please install them using your system package manager, for eg. libgl-dev")
  else(Apple)
    message(FATAL_ERROR "OpenGL development files not found.")
  endif()
endif()
if(NOT TARGET OpenGL::GLU)
  # GLU is a dependency of GLEW but it's not advertized as an OpenGL COMPONENT
  message(FATAL_ERROR "GLU is a dependency of GLEW but was not found.")
endif()


# -- DEPS: OpenGL + GLFW + GLEW
add_library(deps INTERFACE)
target_link_libraries(deps
  INTERFACE OpenGL::GL # On some platforms we still need to link directly.
  INTERFACE glad
  INTERFACE SDL2::SDL2-static # SDL2::SDL2 (shared) - SDL2::SDL2-static (static)
)
target_include_directories(deps
  INTERFACE ${glad_SOURCE_DIR}/include
  INTERFACE ${OPENGL_INCLUDE_DIRS}
)


# ---< App >-------------------------------------------------------------------

add_subdirectory(src)
