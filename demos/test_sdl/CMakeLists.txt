if(UNIX)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
endif()

# Base frameworks
if(APPLE) # sarita's mac

  find_library(COCOA_LIBRARY Cocoa)
  find_library(OpenGL_LIBRARY OpenGL)
  find_library(IOKit_LIBRARY IOKit)
  find_library(QuartzCore_LIBRARY QuartzCore)

  set(DEMO_DEPS ${DEMO_DEPS} ${COCOA_LIBRARY} ${OpenGL_LIBRARY} ${IOKit_LIBRARY} ${QuartzCore_LIBRARY})

elseif(UNIX) # standard ubuntu

  set(FIND_LIBRARY_USE_LIB64_PATHS true)

  find_package(X11 REQUIRED)
  set(DEMO_DEPS ${DEMO_DEPS} ${X11_X11_LIB} ${X11_Xrandr_LIB} ${X11_Xinput_LIB} Xxf86vm)

  find_package(Threads REQUIRED)
  set(DEMO_DEPS ${DEMO_DEPS} ${CMAKE_THREAD_LIBS_INIT})

  find_library(RT_LIBRARY rt)
  set(DEMO_DEPS ${DEMO_DEPS} ${RT_LIBRARY})

  find_library(MATH_LIBRARY m)
  set(DEMO_DEPS ${DEMO_DEPS} ${MATH_LIBRARY})

  # TODO(rgayle): Find some better way of doing this.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L/usr/lib -L/usr/lib64 -L/usr/local/lib")

endif()

# GLEW
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

# SDL 2.0
include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(test_sdl test_sdl.cpp)
target_link_libraries(test_sdl ${DEMO_DEPS} ${SDL2_LIBRARIES})

add_executable(test_sdl_opengl test_sdl_opengl.cpp)
target_link_libraries(test_sdl_opengl ${DEMO_DEPS} ${SDL2_LIBRARIES} ${GLEW_LIBRARIES})

file(GLOB shaders "${CMAKE_CURRENT_SOURCE_DIR}/*.glsl")
install(FILES ${shaders} DESTINATION ${CMAKE_BINARY_DIR})

install(TARGETS test_sdl test_sdl_opengl
        RUNTIME DESTINATION ${CMAKE_BINARY_DIR}
        LIBRARY DESTINATION ${CMAKE_BINARY_DIR}
        ARCHIVE DESTINATION ${CMAKE_BINARY_DIR} 
       )
