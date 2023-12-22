set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION DKA-3DS-52)
set(CMAKE_SYSTEM_PROCESSOR armv6k)
set(EXPAT_DEV_URANDOM ON)
set(EXPAT_BUILD_EXAMPLES OFF)
set(EXPAT_BUILD_DOCS OFF)
set(EXPAT_BUILD_TESTS OFF)
set(DEVKITPRO "/opt/devkitpro")
set(DEVKITARM "/opt/devkitpro/devkitARM")
set(CMAKE_TOOLCHAIN_FILE "/opt/devkitpro/cmake/3DS.cmake")

set(3DS TRUE) # To be used for multiplatform projects
set(__3DS__ TRUE) # To be used for multiplatform projects
add_definitions(-DARM11 -D__3DS__)

if(NOT IS_DIRECTORY ${DEVKITPRO})
    message(FATAL_ERROR "Please set DEVKITPRO in your environment  ${DEVKITPRO} ")
endif()

if(NOT IS_DIRECTORY ${DEVKITARM})
    message(WARNING "DEVKITARM not found : using set DEVKITARM in your environment ${DEVKITPRO}/devkitARM as default")
    set(DEVKITARM ${DEVKITPRO}/devkitARM)
endif()

set(CMAKE_C_COMPILER "${DEVKITARM}/bin/arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "${DEVKITARM}/bin/arm-none-eabi-g++")
set(CMAKE_LINKER "${DEVKITARM}/bin/arm-none-eabi-ld")
set(CMAKE_AR "${DEVKITARM}/bin/arm-none-eabi-gcc-ar" CACHE STRING "")
set(CMAKE_AS "${DEVKITARM}/bin/arm-none-eabi-as" CACHE STRING "")
set(CMAKE_NM "${DEVKITARM}/bin/arm-none-eabi-gcc-nm" CACHE STRING "")
set(CMAKE_RANLIB "${DEVKITARM}/bin/arm-none-eabi-gcc-ranlib" CACHE STRING "")

set(WITH_PORTLIBS ON CACHE BOOL "use portlibs ?")

if(WITH_PORTLIBS)
    set(CMAKE_FIND_ROOT_PATH ${DEVKITARM} ${DEVKITPRO} ${DEVKITPRO}/portlibs/3ds ${DEVKITPRO}/portlibs/armv6k)
else()
    set(CMAKE_FIND_ROOT_PATH ${DEVKITARM} ${DEVKITPRO})
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Technically, the 3DS does support it (CROs), but the toolchain doesn't.
set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS FALSE)


set(ARCH "-march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft")
set(CMAKE_C_FLAGS "-mword-relocations ${ARCH}" CACHE STRING "C flags")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "C++ flags")
set(CMAKE_ASM_FLAGS "-x assembler-with-cpp ${ARCH}" CACHE STRING "ASM flags")
set(CMAKE_EXE_LINKER_FLAGS "" CACHE STRING "Executable LD flags")
set(CMAKE_STATIC_LINKER_FLAGS "" CACHE STRING "Library LD flags")
set(DKA_SUGGESTED_C_FLAGS "-fomit-frame-pointer -ffunction-sections")
set(DKA_SUGGESTED_CXX_FLAGS "${DKA_SUGGESTED_C_FLAGS} -fno-rtti -fno-exceptions")