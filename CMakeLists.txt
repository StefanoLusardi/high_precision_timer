cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

file(STRINGS "VERSION" VERSION_STR)
project(high_precision_timer 
	LANGUAGES CXX
	VERSION ${VERSION_STR}
)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/modules ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_library(timer INTERFACE)
add_library(timer::timer ALIAS timer)

target_compile_features(timer INTERFACE cxx_std_17)
target_include_directories(timer INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
	$<INSTALL_INTERFACE:CMAKE_INSTALL_INCLUDEDIR>)

option(HPT_ENABLE_EXAMPLES "Enable Examples" True)

if(HPT_ENABLE_EXAMPLES)
	add_subdirectory(examples)
endif()