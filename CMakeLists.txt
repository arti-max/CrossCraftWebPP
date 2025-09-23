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
            -s EXPORTED_FUNCTIONS=['_main','_setAppletParams','_startApplet'] \
            -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap'] \
            -s ERROR_ON_UNDEFINED_SYMBOLS=0 \
            -s STACK_SIZE=1048576 \
            -s INITIAL_MEMORY=33554432 \
            --allow-multiple-definition \
            --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/index.html \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/terrain.png@terrain.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/char.png@char.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/rock.png@rock.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/rock2.png@rock2.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/dirt.png@dirt.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/water.png@water.png \
        "
    )
    
    set_target_properties(CrossCraftWebPP PROPERTIES OUTPUT_NAME "game")
    set_target_properties(CrossCraftWebPP PROPERTIES SUFFIX ".html")
    
    # Список файлов текстур для копирования
    set(TEXTURE_FILES
        terrain.png
        char.png 
        rock.png
        rock2.png
        dirt.png
        water.png
    )
    
    # Копируем текстуры в выходную директорию после сборки
    foreach(TEXTURE_FILE ${TEXTURE_FILES})
        add_custom_command(TARGET CrossCraftWebPP POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/${TEXTURE_FILE}"
                "${CMAKE_CURRENT_BINARY_DIR}/${TEXTURE_FILE}"
            COMMENT "Copying ${TEXTURE_FILE} to build directory"
        )
    endforeach()
    
    # Дополнительно создаем кастомную цель для копирования текстур
    add_custom_target(copy_textures
        DEPENDS ${TEXTURE_FILES}
        COMMENT "Copying texture files to build directory"
    )
    
    # Копируем каждую текстуру
    foreach(TEXTURE_FILE ${TEXTURE_FILES})
        add_custom_command(TARGET copy_textures PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/${TEXTURE_FILE}"
                "${CMAKE_CURRENT_BINARY_DIR}/${TEXTURE_FILE}"
        )
    endforeach()
    
    # Убеждаемся что текстуры копируются перед основной сборкой
    add_dependencies(CrossCraftWebPP copy_textures)
    
endif()
