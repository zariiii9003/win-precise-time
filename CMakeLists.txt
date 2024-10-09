cmake_minimum_required(VERSION 3.15...3.26)
project(${SKBUILD_PROJECT_NAME} LANGUAGES C)

find_package(Python
             REQUIRED
             COMPONENTS
             Interpreter
             Development.Module
             ${SKBUILD_SABI_COMPONENT})

python_add_library(_t
                   MODULE
                   ${CMAKE_SOURCE_DIR}/src/win_precise_time/_t.c
                   WITH_SOABI)

install(TARGETS _t LIBRARY DESTINATION win_precise_time)
