cmake_minimum_required(VERSION 3.28.3)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(incanti VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type (Debug or Release)" FORCE)
    message(STATUS "Build type not specified, defaulting to Release")
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(SOURCES
  src/test.cpp
)

set(HEADERS
  include/incanti.hpp
)

set(COMPILE_FLAGS_DEBUG -Wall -ggdb3 -O0)
set(COMPILE_FLAGS_RELEASE -Wall -O3)

function(add_project_target TARGET_NAME SOURCES HEADERS LIBS)
    add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})
    target_include_directories(${TARGET_NAME} PRIVATE include)

    target_compile_options(${TARGET_NAME} PRIVATE
        $<$<CONFIG:Debug>:${COMPILE_FLAGS_DEBUG}>
        $<$<CONFIG:Release>:${COMPILE_FLAGS_RELEASE}>
    )

    target_link_libraries(${TARGET_NAME} PRIVATE ${LIBS})

    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/bin/${CMAKE_BUILD_TYPE}"
    )
endfunction()

add_project_target(${PROJECT_NAME} "${SOURCES}" "${HEADERS}" "${LINK_LIBRARIES}")

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "Using GCC or Clang")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "Using MSVC")
    # MSVC-specific flags
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:/Od /Zi>
        $<$<CONFIG:Release>:/O2>
    )
endif()
