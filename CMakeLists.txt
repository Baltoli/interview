cmake_minimum_required(VERSION 3.28)
project(MonadCompilerInterview)

function(set_compile_options target)
    set_property(
        TARGET ${target}
        PROPERTY CXX_STANDARD 23
    )

    set_property(
        TARGET ${target}
        PROPERTY POSITION_INDEPENDENT_CODE ON
    )

    set_property(
        TARGET ${target}
        PROPERTY ENABLE_EXPORTS ON
    )
endfunction()

add_library(compiler
    src/compiler.cpp
    src/host.cpp
    src/instructions.cpp
    src/vm.cpp
    src/runtime.cpp
)
target_include_directories(compiler
    PUBLIC include/
)
set_compile_options(compiler)

add_executable(monad-run src/main.cpp)
set_compile_options(monad-run)
target_link_libraries(monad-run compiler)

add_executable(unit-tests src/unit_tests.cpp)
set_compile_options(unit-tests)
target_link_libraries(unit-tests compiler)