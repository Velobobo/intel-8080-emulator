add_library(cpu
    cpu.cpp
    instructions.cpp
    flags.cpp
    opcodes.cpp
    load.cpp
)

target_include_directories(cpu
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)
