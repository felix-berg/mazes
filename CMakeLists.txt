cmake_minimum_required(VERSION 3.24)
project(mazes)

set(CMAKE_CXX_STANDARD 23)

# update modules (git submodule update --init --recursive)
find_package(Git QUIET)
IF (GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if (GIT_SUBMODULE)
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if (NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update failed")
        endif()
    endif()
endif()

add_executable(mazes
        main.cpp src/mazegraph.cpp)

target_include_directories(mazes
        PRIVATE include
        PRIVATE external/fbg/include)

add_subdirectory(external/fbg)
target_link_libraries(mazes
        PRIVATE fbg)
