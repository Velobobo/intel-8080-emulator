add_library(disasm
    disasm.cpp
)

target_include_directories(disasm
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)
