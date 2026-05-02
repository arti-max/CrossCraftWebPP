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
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/stb)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS 
    "src/*.hpp" 
    "src/*.h"
    "include/*.h" 
    "include/*.hpp"
)

message(STATUS "Source files: ${SOURCES}")

add_library(stb_vorbis STATIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/stb/stb_impl.c")
target_include_directories(stb_vorbis PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/stb")

add_executable(CrossCraftWebPP ${SOURCES} ${HEADERS})

if(EMSCRIPTEN)
    add_compile_options(-sUSE_ZLIB=1)
    target_link_libraries(CrossCraftWebPP PRIVATE 
        glfw
        websocket.js
        stb_vorbis
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/libgc.a"
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
            -s ASYNCIFY_STACK_SIZE=2097152 \
            -s ASYNCIFY_IMPORTS=[emscripten_sleep,emscripten_wget_data] \
            -s USE_ZLIB=1 \
            -s FETCH=1 \
            -s LEGACY_GL_EMULATION=0 \
            -s GL_FFP_ONLY=0 \
            -s ASSERTIONS=1 \
            -s EXPORTED_FUNCTIONS=['_main','_setAppletParams','_startApplet','_testAsyncify','_setServerParams','_showCrashScreen'] \
            -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap','allocate','intArrayFromString','UTF8ToString'] \
            -s ERROR_ON_UNDEFINED_SYMBOLS=0 \
            -s STACK_SIZE=134217728 \
            -s INITIAL_MEMORY=536870912 \
            -s NO_EXIT_RUNTIME=1 \
            -l openal \
            -fsanitize=undefined \
            -lidbfs.js \
            --allow-multiple-definition \
            --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/index.html \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/terrain.png@terrain.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/char.png@char.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/rock.png@rock.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/rock2.png@rock2.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/dirt.png@dirt.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/water.png@water.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/default.png@default.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/clouds.png@clouds.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/gui.png@gui.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/armor/plate.png@armor/plate.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/armor/chain.png@armor/chain.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/mob/zombie.png@mob/zombie.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/mob/skeleton.png@mob/skeleton.png \
            --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/mob/pig.png@mob/pig.png"
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
        clouds.png
        gui.png
        armor/plate.png
        armor/chain.png
        mob/zombie.png
        mob/skeleton.png
        mob/pig.png
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