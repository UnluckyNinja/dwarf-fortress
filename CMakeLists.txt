cmake_minimum_required(VERSION 2.6.2)
include(GNUInstallDirs)

project(dwarf-fortress)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/debian/version" DWARF_FORTRESS_VERSION)
string(STRIP "${DWARF_FORTRESS_VERSION}" DWARF_FORTRESS_VERSION)
string(REGEX REPLACE "\\..*$" "" DWARF_FORTRESS_SOVERSION "${DWARF_FORTRESS_VERSION}")

if(NOT "$ENV{DEB_HOST_MULTIARCH}" STREQUAL "")
  set(CMAKE_INSTALL_LIBDIR_MULTIARCH "${CMAKE_INSTALL_LIBDIR}/$ENV{DEB_HOST_MULTIARCH}")
else()
  set(CMAKE_INSTALL_LIBDIR_MULTIARCH "${CMAKE_INSTALL_LIBDIR}")
endif()

set(CMAKE_CXX_FLAGS "-std=c++0x -Dlinux -Dunix")

find_package(PkgConfig REQUIRED)
find_package(GTK2      REQUIRED)
pkg_check_modules(GLEW REQUIRED glew)
find_package(OpenGL    REQUIRED)
find_package(SDL       REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_ttf   REQUIRED)
find_package(ZLIB      REQUIRED)

include_directories(${GTK2_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${SDL_INCLUDE_DIR} ${SDLIMAGE_INCLUDE_DIR} ${SDLTTF_INCLUDE_DIR} ${ZLIB_INCLUDE_DIRS})
link_directories(${GTK2_LIBRARY_DIRS} ${GLEW_LIBRARY_DIRS} ${OPENGL_LIBRARY_DIR} ${SDL_LIBRARY_DIR} ${SDLIMAGE_LIBRARY_DIR} ${SDLTTF_LIBRARY_DIR} ${ZLIB_LIBRARY_DIRS})

add_library(dwarf-fortress-graphics SHARED
  g_src/basics.cpp
  g_src/command_line.cpp
  g_src/files.cpp
  g_src/find_files_posix.cpp
  g_src/graphics.cpp
  g_src/enabler.cpp
  g_src/init.cpp
  g_src/interface.cpp
  g_src/keybindings.cpp
  g_src/music_and_sound_openal.cpp
  g_src/random.cpp
  g_src/textlines.cpp
  g_src/enabler_input.cpp
  g_src/ViewBase.cpp
  g_src/KeybindingScreen.cpp
  g_src/win32_compat.cpp
  g_src/textures.cpp
  g_src/resize++.cpp
  g_src/renderer_offscreen.cpp
  g_src/ttf_manager.cpp
)
set_target_properties(dwarf-fortress-graphics PROPERTIES
  SONAME dwarf-fortress-graphics
  VERSION ${DWARF_FORTRESS_VERSION}
  SOVERSION ${DWARF_FORTRESS_SOVERSION}
)
target_link_libraries(dwarf-fortress-graphics dl ${GTK2_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY} ${ZLIB_LIBRARIES})

add_executable(dwarf-fortress-tetris
  g_src/tetris.cpp
)
target_link_libraries(dwarf-fortress-tetris dwarf-fortress-graphics)

add_custom_target(dwarf-fortress-link
  COMMAND ln -sf "../libdwarf-fortress-graphics.so.0" ${CMAKE_CURRENT_BINARY_DIR}/libgraphics.so
)
add_dependencies(dwarf-fortress-graphics dwarf-fortress-link)

install(TARGETS dwarf-fortress-graphics dwarf-fortress-tetris
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/games
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR_MULTIARCH}"
)
install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/libs/Dwarf_Fortress
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/dwarf-fortress
  RENAME dwarf-fortress
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libgraphics.so
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/dwarf-fortress
)
