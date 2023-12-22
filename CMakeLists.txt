cmake_minimum_required(VERSION 3.12...3.15)
set(DEVKITPRO "/opt/devkitpro")
set(DEVKITARM "/opt/devkitpro/devkitARM")
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/DevkitArm3DS.cmake")

project(moonlight-3ds 
    VERSION 1.0.0 
    DESCRIPTION "A 3DS Moonlight client"
    LANGUAGES CXX)
enable_language(ASM)
enable_language(C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(3DS TRUE) 
set(__3DS__ TRUE) 
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION DKA-3DS-52)
set(CMAKE_SYSTEM_PROCESSOR armv6k)
set(EXPAT_DEV_URANDOM ON)
set(EXPAT_BUILD_EXAMPLES OFF)
set(EXPAT_BUILD_DOCS OFF)
set(EXPAT_BUILD_TESTS OFF)


string(APPEND CMAKE_C_FLAGS " ${DKA_SUGGESTED_C_FLAGS} -Wall -Wextra ")
string(APPEND CMAKE_CXX_FLAGS " ${DKA_SUGGESTED_CXX_FLAGS} -Wno-psabi -Wall -Wextra")
add_link_options(-Wl,-Map,moonlight-3ds.map)
add_link_options(-Wl,--wrap,malloc,--wrap,realloc,--wrap,free,--wrap,_free_r,--wrap,memalign)
add_link_options(-Wl,--wrap,APT_GetAppCpuTimeLimit,--wrap,APT_SetAppCpuTimeLimit)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

include(Tools3DS)

include(ExternalProject)
ExternalProject_Add(moonlight-git
    GIT_REPOSITORY      https://github.com/Adi3000/moonlight-common-c.git
    BINARY_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/build/moonlight/lib
    GIT_TAG             origin/feat/3ds-compatibility
    PREFIX              external/moonlight
    CMAKE_ARGS          -DCMAKE_TOOLCHAIN_FILE=/opt/devkitpro/cmake/3DS.cmake -DUSE_MBEDTLS=ON
    INSTALL_COMMAND     cmake -E echo "Skipping install step."
)
ExternalProject_Get_Property(moonlight-git BINARY_DIR)
ExternalProject_Get_Property(moonlight-git SOURCE_DIR)
set(MOONLIGHT_SOURCE_DIR "${SOURCE_DIR}/src")
message("Will provide source moonlight here : ${MOONLIGHT_SOURCE_DIR}")
set(MOONLIGHT_BINARY_DIR ${BINARY_DIR})
message("Will provide lib moonlight here : ${MOONLIGHT_BINARY_DIR}")
add_library(moonlight STATIC IMPORTED)
add_library(enet STATIC IMPORTED)
set_target_properties(moonlight PROPERTIES 
    IMPORTED_LOCATION ${MOONLIGHT_BINARY_DIR}/libmoonlight-common-c.a)
set_target_properties(enet 
    PROPERTIES IMPORTED_LOCATION ${MOONLIGHT_BINARY_DIR}/enet/libenet.a)

ExternalProject_Add(pugixml-git
    GIT_REPOSITORY      https://github.com/zeux/pugixml.git
    BINARY_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/build/pugixml/lib
    GIT_TAG             origin/master
    PREFIX              external/pugixml
    CMAKE_ARGS          -DCMAKE_TOOLCHAIN_FILE=/opt/devkitpro/cmake/3DS.cmake -DPUGIXML_INSTALL=OFF
    INSTALL_COMMAND     cmake -E echo "Skipping install step."
)
ExternalProject_Get_Property(pugixml-git BINARY_DIR)
ExternalProject_Get_Property(pugixml-git SOURCE_DIR)
set(PUGIXML_SOURCE_DIR "${SOURCE_DIR}/src")
message("Will provide source pugixml here : ${PUGIXML_SOURCE_DIR}")
set(PUGIXML_BINARY_DIR ${BINARY_DIR})
message("Will provide lib pugixml here : ${PUGIXML_BINARY_DIR}")
add_library(pugixml STATIC IMPORTED)
set_target_properties(pugixml PROPERTIES 
    IMPORTED_LOCATION ${PUGIXML_BINARY_DIR}/libpugixml.a)

ExternalProject_Add(sdl2-git
    GIT_REPOSITORY      https://github.com/libsdl-org/SDL.git
    BINARY_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/build/sdl2/lib
    GIT_TAG             origin/main
    PREFIX              external/sdl2
    CMAKE_ARGS          -DCMAKE_TOOLCHAIN_FILE=/opt/devkitpro/cmake/3DS.cmake
    INSTALL_COMMAND     cmake -E echo "Skipping install step."
)

ExternalProject_Get_Property(sdl2-git BINARY_DIR)
ExternalProject_Get_Property(sdl2-git SOURCE_DIR)
set(SDL2_SOURCE_DIR "${SOURCE_DIR}/src")
message("Will provide source sdl2 here : ${SDL2_SOURCE_DIR}")
set(SDL2_BINARY_DIR ${BINARY_DIR})
message("Will provide lib sdl2 here : ${SDL2_BINARY_DIR}")
add_library(sdl2 STATIC IMPORTED)
set_target_properties(sdl2 PROPERTIES 
    IMPORTED_LOCATION ${SDL2_BINARY_DIR}/libpugixml.a)

link_directories(
    "${DEVKITPRO}/libctru/lib"
    "${DEVKITPRO}/portlibs/3ds/lib"
    "${DEVKITPRO}/extra_lib/lib"
)
include_directories(
    "${DEVKITPRO}/extra_lib/include"
    "${DEVKITPRO}/libctru/include"
    "${DEVKITPRO}/portlibs/3ds/include"
    "${MOONLIGHT_SOURCE_DIR}"
    "${MOONLIGHT_SOURCE_DIR}/enet/include"
    "${PUGIXML_SOURCE_DIR}"
# ??"${SDL2_SOURCE_DIR}"
)

set(TARGET_SOURCE_LIST "")
file(GLOB_RECURSE MAIN_SOURCE_LIST_CPP CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE MAIN_SOURCE_LIST_C "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
list(APPEND TARGET_SOURCE_LIST ${MAIN_SOURCE_LIST_C})
list(APPEND TARGET_SOURCE_LIST ${MAIN_SOURCE_LIST_CPP})
file(GLOB LIB_BASE64_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/library/base64/*.cpp")
list(APPEND TARGET_SOURCE_LIST ${LIB_BASE64_SOURCES})

set(TARGET_HEADER_LIST "")
file(GLOB_RECURSE MAIN_HEADER_LIST_HPP CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE MAIN_HEADER_LIST_H "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
list(APPEND TARGET_HEADER_LIST ${MAIN_HEADER_LIST_H})
list(APPEND TARGET_HEADER_LIST ${MAIN_HEADER_LIST_HPP})
set_source_files_properties(${MAIN_SOURCE_LIST_C} PROPERTIES LANGUAGE C)



set(ROMFS_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/romfs")
set(ICON_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/resource/icon.png")
add_executable(moonlight3ds ${TARGET_SOURCE_LIST} ${TARGET_HEADER_LIST})
set(AUTHOR "Adi3000 and to the moonlight-3ds project")
target_include_directories(moonlight3ds PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/library")
target_link_libraries(moonlight3ds 
    swresample 
    avformat 
    swscale 
    avcodec 
    avutil 
    citro2dd 
    citro3dd
    m 
    x264 
    mp3lame 
    dav1d 
    curl 
    mbedtls 
    mbedx509 
    mbedcrypto 
    z 
    ctrud
    moonlight
    enet
    sdl2
    pugixml
)
#set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/src/system/test.s" PROPERTY LANGUAGE ASM)
#set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/src/system/util/yuv420p_rgb565le.s" PROPERTY LANGUAGE ASM)
#set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/src/system/util/yuv420p_rgb888le.s" PROPERTY LANGUAGE ASM)
#target_sources(moonlight3ds PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/system/test.s")
#target_sources(moonlight3ds PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/system/util/yuv420p_rgb565le.s")
#target_sources(moonlight3ds PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/system/util/yuv420p_rgb888le.s")
#install(TARGETS moonlight3ds RUNTIME DESTINATION /mnt/g/Utils/CIAServer1.2/)


add_3dsx_target(moonlight3ds       
    ${CMAKE_PROJECT_NAME} # Name
    ${CMAKE_PROJECT_DESCRIPTION} # Description
    ${AUTHOR} # Author(s)
    ${ROMFS_LOCATION} # RomFS
    ${ICON_LOCATION} # Icon
)
add_cia_target(moonlight3ds 
    "${CMAKE_CURRENT_SOURCE_DIR}/resource/app.rsf" 
    "${CMAKE_CURRENT_SOURCE_DIR}/resource/banner.png"  
    "${CMAKE_CURRENT_SOURCE_DIR}/resource/banner.wav"  
    ${ROMFS_LOCATION} # RomFS
    ${CMAKE_PROJECT_NAME} # Name
    ${CMAKE_PROJECT_DESCRIPTION} # Description
    ${AUTHOR} # Author(s)
    "${CMAKE_CURRENT_SOURCE_DIR}/resource/icon.png"
)
