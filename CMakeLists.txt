cmake_minimum_required(VERSION 3.16)

project(emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Subdirectories (libraries)
add_subdirectory(src/cpu)
add_subdirectory(src/memory)
add_subdirectory(src/disasm)


# Emulator executable

add_executable(emulator
    src/main.cpp
)

target_link_libraries(emulator
    PRIVATE
        cpu
        memory
)


# Disassembler executable

add_executable(disassembler
    src/disassembler.cpp
)

target_link_libraries(disassembler
    PRIVATE
        disasm
        cpu        # opcode table lives here
        memory     # optional but useful
)
