cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

find_package(spdlog REQUIRED)

function(add_example EXAMPLE_TARGET)
	add_executable(${EXAMPLE_TARGET} src/${EXAMPLE_TARGET}.cpp)
	target_link_libraries(${EXAMPLE_TARGET} PRIVATE timer::timer PRIVATE spdlog::spdlog)
	target_compile_features(${EXAMPLE_TARGET} PRIVATE cxx_std_17)
endfunction()

add_example(simple)
add_example(example_logger)