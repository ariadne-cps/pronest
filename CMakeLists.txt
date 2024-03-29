cmake_minimum_required(VERSION 3.9)

project(ProNest VERSION 1.0)

option(COVERAGE "Enable coverage reporting" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.20)
        message(FATAL_ERROR "MSVC version must be at least 19.20!")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.2)
        message(FATAL_ERROR "GCC version must be at least 10.2!")
    endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.0)
        message(FATAL_ERROR "Clang version must be at least 11.0!")
    endif()
else()
    message(WARNING "You are using an unsupported compiler! MSVC, GCC and Clang are supported.")
endif()

if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(LIBRARY_KIND STATIC)
elseif(COVERAGE)
    set(LIBRARY_KIND STATIC)
else()
    set(LIBRARY_KIND SHARED)	
endif()

set(CMAKE_MACOSX_RPATH 1)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT WIN32)
    set(ANY_TARGET_WARN all extra pedantic sign-conversion cast-qual disabled-optimization
        init-self missing-include-dirs sign-promo switch-default undef redundant-decls
        strict-aliasing unused-parameter shadow error)
    foreach(WARN ${ANY_TARGET_WARN})
        add_compile_options(-W${WARN})
    endforeach()
else()
    add_compile_options(/WX)
endif()

find_package(Git)
if(GIT_FOUND)
    if (NOT EXISTS ${PROJECT_SOURCE_DIR}/.git) # Manages the case when an archive is used
        execute_process(COMMAND git init WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_QUIET ERROR_QUIET)
        execute_process(COMMAND rm -Rf ${PROJECT_SOURCE_DIR}/submodules/conclog OUTPUT_QUIET ERROR_QUIET)
        execute_process(COMMAND git submodule add -f https://github.com/ariadne-cps/helper submodules/helper WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_QUIET ERROR_QUIET)
    endif()
    message(STATUS "Getting all submodules if not downloaded already...")
    execute_process(COMMAND git submodule update --init --recursive WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} OUTPUT_QUIET ERROR_QUIET)
    if(NOT EXISTS ${PROJECT_SOURCE_DIR}/submodules/helper/CMakeLists.txt)
        message(ERROR "Helper download failure")
    else()
        message(STATUS "Helper submodule found")
    endif()
else()
    message(FATAL_ERROR "Git not found, no submodules could be downloaded")
endif()

if (NOT EXISTS ${PROJECT_SOURCE_DIR}/submodules/.symbolic)
    execute_process(COMMAND mkdir ${PROJECT_SOURCE_DIR}/submodules/.symbolic)
    message(STATUS "Created symbolic links directory for submodules")
endif()

if (NOT EXISTS ${PROJECT_SOURCE_DIR}/submodules/.symbolic/helper)
    execute_process(COMMAND ln -s ${PROJECT_SOURCE_DIR}/submodules/helper/include ${PROJECT_SOURCE_DIR}/submodules/.symbolic/helper)
    message(STATUS "Created symbolic link to Helper submodule")
endif()

include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/submodules/.symbolic)

if(NOT TARGET pronest)
    add_subdirectory(src)

    add_library(pronest ${LIBRARY_KIND} $<TARGET_OBJECTS:PRONEST_SRC>)

    if(NOT TARGET tests)

        if(COVERAGE)
            include(CodeCoverage)
            append_coverage_compiler_flags()
        endif()

        enable_testing()
        add_subdirectory(test)
    endif()

    add_subdirectory(submodules)
    target_link_libraries(pronest helper)

endif()
