cmake_minimum_required(VERSION 3.16)
project(CrossCraftWebPP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавляем корневые директории для поиска заголовочных файлов
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Добавляем все подпапки src для удобства
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/level)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/phys)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/render)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/util)

# Автоматически находим все файлы
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS 
    "src/*.hpp" 
    "src/*.h"
    "include/*.h" 
    "include/*.hpp"
)

message(STATUS "Source files: ${SOURCES}")

add_executable(CrossCraftWebPP ${SOURCES} ${HEADERS})

if(EMSCRIPTEN)
    target_link_libraries(CrossCraftWebPP PRIVATE 
        glfw
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/libGLU.a"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/libGL.a"
    )

    set_target_properties(CrossCraftWebPP PROPERTIES 
        LINK_FLAGS "\
            -s WASM=1 \
            -s USE_GLFW=3 \
            -s FULL_ES2=1 \
            -s ALLOW_MEMORY_GROWTH=1 \
            -s EXPORT_ALL=1 \
            -s ERROR_ON_UNDEFINED_SYMBOLS=0 \
            --allow-multiple-definition \
            --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/index.html \
        "
    )
    
    set_target_properties(CrossCraftWebPP PROPERTIES OUTPUT_NAME "game")
    set_target_properties(CrossCraftWebPP PROPERTIES SUFFIX ".html")
endif()
