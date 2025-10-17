cmake_minimum_required(VERSION 3.16)
project(CrossCraftWebPP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/level)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/phys)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/render)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/util)

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
    add_compile_options(-sUSE_ZLIB=1)
    target_link_libraries(CrossCraftWebPP PRIVATE 
        glfw
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/libGLU.a"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/libGL.a"
    )

    set_target_properties(CrossCraftWebPP PROPERTIES 
        COMPILE_FLAGS "-sUSE_ZLIB=1"
        LINK_FLAGS "\
            -s WASM=1 \
            -s USE_GLFW=3 \
            -s FULL_ES2=1 \
            -s ALLOW_MEMORY_GROWTH=1 \
            -s ASYNCIFY \
            -s ASYNCIFY_STACK_SIZE=262144 \
            -s ASYNCIFY_IMPORTS=[emscripten_sleep,emscripten_wget_data] \
            -s USE_ZLIB=1 \
            -s FETCH=1 \
            -s ASSERTIONS=1 \
            -s EXPORTED_FUNCTIONS=['_main','_setAppletParams','_startApplet','_testAsyncify'] \
            -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap'] \
            -s ERROR_ON_UNDEFINED_SYMBOLS=0 \
            -s STACK_SIZE=2097152 \
            -s INITIAL_MEMORY=67108864 \
            --allow-multiple-definition \
            --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/index.html \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/terrain.png@terrain.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/char.png@char.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/rock.png@rock.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/rock2.png@rock2.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/dirt.png@dirt.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/water.png@water.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/default.png@default.png \
        "
    )
    
    set_target_properties(CrossCraftWebPP PROPERTIES OUTPUT_NAME "game")
    set_target_properties(CrossCraftWebPP PROPERTIES SUFFIX ".html")
    
    set(TEXTURE_FILES
        terrain.png
        char.png 
        rock.png
        rock2.png
        dirt.png
        water.png
        default.png
    )
    
    foreach(TEXTURE_FILE ${TEXTURE_FILES})
        add_custom_command(TARGET CrossCraftWebPP POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/${TEXTURE_FILE}"
                "${CMAKE_CURRENT_BINARY_DIR}/${TEXTURE_FILE}"
            COMMENT "Copying ${TEXTURE_FILE} to build directory"
        )
    endforeach()
    
    add_custom_target(copy_textures DEPENDS ${TEXTURE_FILES})
    
    foreach(TEXTURE_FILE ${TEXTURE_FILES})
        add_custom_command(TARGET copy_textures PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/${TEXTURE_FILE}"
                "${CMAKE_CURRENT_BINARY_DIR}/${TEXTURE_FILE}"
        )
    endforeach()
    
    add_dependencies(CrossCraftWebPP copy_textures)
endif()
