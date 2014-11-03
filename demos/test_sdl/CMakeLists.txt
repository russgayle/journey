if(UNIX)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
endif()

include(FindPkgConfig)

pkg_search_module(SDL2 REQUIRED sdl2)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(test_sdl test_sdl.cpp)
target_link_libraries(test_sdl ${SDL2_LIBRARIES})
        
install(TARGETS test_sdl 
        RUNTIME DESTINATION ${CMAKE_BINARY_DIR}
        LIBRARY DESTINATION ${CMAKE_BINARY_DIR}
        ARCHIVE DESTINATION ${CMAKE_BINARY_DIR} 
       )
