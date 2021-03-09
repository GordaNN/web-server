cmake_minimum_required(VERSION 3.8)

project(web-server)

# Assembly

find_library(WS2_32_LIBRARY ws2_32)

set(HEADERS
    include/config.h
    include/handler.h
    include/thread_pool.h
)

set(SOURCES
    src/main.cpp
    src/config.cpp
    src/handler.cpp
    src/thread_pool.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    $<$<CONFIG:Debug>:-MTd;-pthread;-DDEBUG>
    $<$<CONFIG:Release>:-O3;-MT;-pthread;-DNDEBUG>
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    ws2_32
)

# Installation

install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_NAME})
install(FILES server.config DESTINATION ${PROJECT_NAME}) 
install(DIRECTORY resources DESTINATION ${PROJECT_NAME}) 
