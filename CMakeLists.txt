cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(GNUInstallDirs)
include(ParseVersion)
include(AddSubmodule)

project(dwarf-fortress-multi CXX)
parse_version(dwarf-fortress-multi DEBIAN)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

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
else()
  set(CMAKE_INSTALL_LIBDIR_MULTIARCH "${CMAKE_INSTALL_LIBDIR}")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Dlinux -Dunix")

find_package(PkgConfig REQUIRED)
pkg_check_modules(dwarf-fortress-multi_DEPS REQUIRED gtk+-2.0 sdl SDL_image SDL_ttf zlib libzmq)

include_directories(
  src/common
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${dwarf-fortress-multi_DEPS_INCLUDE_DIRS}
)
link_directories(${dwarf-fortress-multi_DEPS_LIBRARY_DIRS})

file(GLOB_RECURSE dwarf-fortress-multi_COMMON_SOURCES src/common/*.cpp)
file(GLOB_RECURSE dwarf-fortress-multi_CLIENT_SOURCES src/client/*.cpp)
file(GLOB_RECURSE dwarf-fortress-multi_SERVER_SOURCES src/server/*.cpp)

add_library(dwarf-fortress-server SHARED
  ${dwarf-fortress-multi_SERVER_SOURCES}
  ${dwarf-fortress-multi_COMMON_SOURCES}
)
set_target_properties(dwarf-fortress-server PROPERTIES
  VERSION   ${dwarf-fortress-multi_VERSION}
  SOVERSION ${dwarf-fortress-multi_SOVERSION}
  SONAME    dwarf-fortress-server
)
target_link_libraries(dwarf-fortress-server dwarf-fortress-graphics dl ${dwarf-fortress-multi_DEPS_LIBRARIES})

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

configure_file(src/dwarf-fortress        ${CMAKE_CURRENT_BINARY_DIR}/bin/dwarf-fortress        @ONLY)
configure_file(src/dwarf-fortress-server ${CMAKE_CURRENT_BINARY_DIR}/bin/dwarf-fortress-server @ONLY)
add_custom_target(dwarf-fortress-server-links
  COMMAND ln -sf "../libdwarf-fortress-server.so.0" ${CMAKE_CURRENT_BINARY_DIR}/lib/libgraphics.so
  COMMAND ln -sf "../dwarf-fortress/dwarf-fortress" ${CMAKE_CURRENT_BINARY_DIR}/lib/dwarf-fortress
)
add_dependencies(dwarf-fortress-server dwarf-fortress-server-links)


add_executable(dwarf-fortress-client
  ${dwarf-fortress-multi_CLIENT_SOURCES}
  ${dwarf-fortress-multi_COMMON_SOURCES}
)
target_link_libraries(dwarf-fortress-client gtulu ${dwarf-fortress-multi_DEPS_LIBRARIES})


install(TARGETS dwarf-fortress-server dwarf-fortress-client
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/games
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR_MULTIARCH}"
)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/bin/dwarf-fortress
                 ${CMAKE_CURRENT_BINARY_DIR}/bin/dwarf-fortress-server
  DESTINATION    ${CMAKE_INSTALL_PREFIX}/games
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/lib/libgraphics.so
              ${CMAKE_CURRENT_BINARY_DIR}/lib/dwarf-fortress
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/dwarf-fortress-server
)
