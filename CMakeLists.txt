cmake_minimum_required(VERSION 3.12)
project(tetris)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)

# Dependencies

# Project
file(GLOB Sources CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
add_executable(${PROJECT_NAME} ${Sources})

# Testing
enable_testing()

file(GLOB TestSources CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/tst/*.cxx)

foreach(Test ${TestSources})
    get_filename_component(Stem ${Test} NAME_WE)

    add_executable(${Stem} ${Test})
    set_target_properties(${Stem} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Testing)

    add_test(
        NAME ${Stem}
        COMMAND ${Stem})
endforeach()
