add_library(memory
    memory.cpp
)

target_include_directories(memory
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)