cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(GNUInstallDirs)
include(DebianVersion)
include(AddSubmodule)

parse_debian_version(DWARF_FORTRESS_MULTI_VERSION)
string(REGEX REPLACE "\\..*$" "" DWARF_FORTRESS_MULTI_SOVERSION "${DWARF_FORTRESS_MULTI_VERSION}")
project(dwarf-fortress-multi CXX)

add_submodule(logging lib/logging
  INCLUDE_DIRS include lib/boost-log
)
add_submodule(gtulu lib/gtulu
  INCLUDE_DIRS include lib/registry/include lib/boost-gil-contributions
)
add_submodule(dwarf-fortress-graphics lib/dwarf-fortress-graphics
  INCLUDE_DIRS .
)

if(NOT "$ENV{DEB_HOST_MULTIARCH}" STREQUAL "")
  set(CMAKE_INSTALL_LIBDIR_MULTIARCH "${CMAKE_INSTALL_LIBDIR}/$ENV{DEB_HOST_MULTIARCH}")
  set(CMAKE_INSTALL_FULL_LIBDIR_MULTIARCH "${CMAKE_INSTALL_FULL_LIBDIR}/$ENV{DEB_HOST_MULTIARCH}")
else()
  set(CMAKE_INSTALL_LIBDIR_MULTIARCH "${CMAKE_INSTALL_LIBDIR}")
  set(CMAKE_INSTALL_FULL_LIBDIR_MULTIARCH "${CMAKE_INSTALL_FULL_LIBDIR}")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Dlinux -Dunix")

find_package(PkgConfig REQUIRED)
pkg_check_modules(DWARF_FORTRESS_MULTI_DEPS REQUIRED gtk+-2.0 gl glu glew sdl SDL_image SDL_ttf zlib)

include_directories(
  src/common
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${DWARF_FORTRESS_MULTI_DEPS_INCLUDE_DIRS}
)
link_directories(${DWARF_FORTRESS_MULTI_DEPS_LIBRARY_DIRS})

file(GLOB_RECURSE DWARF_FORTRESS_MULTI_COMMON_SOURCES src/common/*.cpp)
file(GLOB_RECURSE DWARF_FORTRESS_MULTI_CLIENT_SOURCES src/client/*.cpp)
file(GLOB_RECURSE DWARF_FORTRESS_MULTI_SERVER_SOURCES src/server/*.cpp)

add_library(dwarf-fortress-server SHARED
  ${DWARF_FORTRESS_MULTI_SERVER_SOURCES}
  ${DWARF_FORTRESS_MULTI_COMMON_SOURCES}
)
set_target_properties(dwarf-fortress-server PROPERTIES
  SONAME dwarf-fortress-server
  VERSION ${DWARF_FORTRESS_MULTI_VERSION}
  SOVERSION ${DWARF_FORTRESS_MULTI_SOVERSION}
)
target_link_libraries(dwarf-fortress-server dwarf-fortress-graphics dl ${DWARF_FORTRESS_MULTI_DEPS_LIBRARIES})


add_custom_target(dwarf-fortress-server-link
  COMMAND ln -sf "${CMAKE_INSTALL_FULL_LIBDIR_MULTIARCH}/libdwarf-fortress-server.so.0" ${CMAKE_CURRENT_BINARY_DIR}/libgraphics.so
)
add_dependencies(dwarf-fortress-server dwarf-fortress-server-link)


add_executable(dwarf-fortress-client
  ${DWARF_FORTRESS_MULTI_CLIENT_SOURCES}
  ${DWARF_FORTRESS_MULTI_COMMON_SOURCES}
)
target_link_libraries(dwarf-fortress-client gtulu zmq SDL SDL_image)


install(TARGETS dwarf-fortress-server dwarf-fortress-client
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/games
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR_MULTIARCH}"
)
install(PROGRAMS ${dwarf-fortress-graphics_SOURCE_DIR}/libs/Dwarf_Fortress
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/dwarf-fortress-server
  RENAME dwarf-fortress
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libgraphics.so
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/dwarf-fortress-server
)
