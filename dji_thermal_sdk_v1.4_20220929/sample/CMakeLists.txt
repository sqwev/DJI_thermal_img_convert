# Cmake for sample codes
#
# @Copyright (c) 2020-2023 DJI. All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

CMAKE_MINIMUM_REQUIRED (VERSION 3.0)

SET (CMAKE_CXX_STANDARD 11)

if (${TARGET_CPU} STREQUAL "x64")
    MESSAGE (STATUS "compile 64bit application")
    SET (CMAKE_CXX_FLAGS "-DPLATFORM_X64")
    SET (CMAKE_C_FLAGS "-DPLATFORM_X64")
else ()
    MESSAGE (STATUS "compile 32bit application")
    SET (CMAKE_CXX_FLAGS "-DPLATFORM_X86")
    SET (CMAKE_C_FLAGS "-DPLATFORM_X86")

    if (NOT CMAKE_HOST_WIN32)
        SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -msse2 -mfpmath=sse")
        SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -msse2 -mfpmath=sse")
    endif ()
endif ()

if (CMAKE_HOST_WIN32)
    MESSAGE (STATUS "Windows Version")

    SET (LIBRARY_NAME_DIRP libdirpd)

    if (DEFINED CMAKE_BUILD_TYPE)
        MESSAGE (STATUS "Configuration : " ${CMAKE_BUILD_TYPE})
        SET (CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)
        if ((${CMAKE_BUILD_TYPE} STREQUAL "Release") OR (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo"))
            SET (LIBRARY_NAME_DIRP libdirp)
        endif()
    endif()
else ()
    MESSAGE (STATUS "Linux Version")

    SET (LIBRARY_NAME_DIRP dirpd)

    if (DEFINED CMAKE_BUILD_TYPE)
        MESSAGE (STATUS "Configuration : " ${CMAKE_BUILD_TYPE})
        SET (CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)
        if ((${CMAKE_BUILD_TYPE} STREQUAL "Release") OR (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo"))
            SET (LIBRARY_NAME_DIRP dirp)
        endif ()
    endif()
endif()

# dji_irp app
PROJECT (dji_irp LANGUAGES C CXX)

SET (CMAKE_PLATFORM "${CMAKE_BUILD_TYPE}_${TARGET_CPU}")
STRING (TOLOWER ${CMAKE_PLATFORM} CMAKE_PLATFORM)

if (CMAKE_HOST_WIN32)
    SET (SAMPLE_LIBRARY_PATH "${PROJECT_SOURCE_DIR}/../tsdk-core/lib/windows/${CMAKE_PLATFORM}")
    SET (SAMPLE_DEPLOY_PATH "${PROJECT_SOURCE_DIR}/bin/windows/${CMAKE_PLATFORM}")
else ()
    SET (SAMPLE_LIBRARY_PATH "${PROJECT_SOURCE_DIR}/../tsdk-core/lib/linux/${CMAKE_PLATFORM}")
    SET (SAMPLE_DEPLOY_PATH "${PROJECT_SOURCE_DIR}/bin/linux/${CMAKE_PLATFORM}")
endif ()

LINK_DIRECTORIES (${SAMPLE_LIBRARY_PATH})

INCLUDE_DIRECTORIES (
    ${PROJECT_SOURCE_DIR}/../tsdk-core/api
    ${PROJECT_SOURCE_DIR}/argparse
)

SET (CMAKE_CXX_STACK_SIZE "104857600")

ADD_EXECUTABLE (${PROJECT_NAME} dji_irp.cpp)

if (CMAKE_HOST_WIN32)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/EHsc")
endif ()

TARGET_LINK_LIBRARIES (${PROJECT_NAME} ${LIBRARY_NAME_DIRP})

# dji_ircm app
PROJECT (dji_ircm  LANGUAGES C CXX)

SET (CMAKE_CXX_STACK_SIZE "104857600")

ADD_EXECUTABLE (${PROJECT_NAME} dji_ircm.cpp)

if (CMAKE_HOST_WIN32)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/EHsc")
endif ()

TARGET_LINK_LIBRARIES (${PROJECT_NAME} ${LIBRARY_NAME_DIRP})

# dji_irp_omp app
PROJECT (dji_irp_omp  LANGUAGES C CXX)

SET (CMAKE_CXX_STACK_SIZE "104857600")

FIND_PACKAGE (OpenMP REQUIRED)
if (OPENMP_FOUND)
    SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif ()

ADD_EXECUTABLE (${PROJECT_NAME} dji_irp_omp.cpp)

if (CMAKE_HOST_WIN32)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/EHsc")
endif ()

TARGET_LINK_LIBRARIES (${PROJECT_NAME} ${LIBRARY_NAME_DIRP})

# libv_cirp library
if (CMAKE_HOST_WIN32)
    MESSAGE (STATUS "Windows Version")
    SET (LIBRARY_VENDOR_NAME_CIRP libv_cirpd)

    if (DEFINED CMAKE_BUILD_TYPE)
        MESSAGE (STATUS "Configuration : " ${CMAKE_BUILD_TYPE})
        if ((${CMAKE_BUILD_TYPE} STREQUAL "Release") OR (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo"))
            SET (LIBRARY_VENDOR_NAME_CIRP libv_cirp)
        endif ()
    endif ()
else ()
    MESSAGE (STATUS "Linux Version")
    SET (LIBRARY_VENDOR_NAME_CIRP v_cirpd)

    if (DEFINED CMAKE_BUILD_TYPE)
        MESSAGE (STATUS "Configuration : " ${CMAKE_BUILD_TYPE})
        SET (CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)
        if ((${CMAKE_BUILD_TYPE} STREQUAL "Release") OR (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo"))
            SET (LIBRARY_VENDOR_NAME_CIRP v_cirp)
        endif ()
    endif ()
endif ()

PROJECT (${LIBRARY_VENDOR_NAME_CIRP} LANGUAGES C CXX)

ADD_LIBRARY (${PROJECT_NAME} SHARED libv_cirp_api.cpp)

if (CMAKE_HOST_WIN32)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/EHsc")
    ADD_DEFINITIONS(-DBUILD_SHARED_LIBS=TRUE)
endif ()

INSTALL (TARGETS
    dji_irp
    dji_ircm
    dji_irp_omp
    ${LIBRARY_VENDOR_NAME_CIRP}
    RUNTIME DESTINATION ${SAMPLE_DEPLOY_PATH}
    LIBRARY DESTINATION ${SAMPLE_DEPLOY_PATH}
    ARCHIVE DESTINATION ${SAMPLE_DEPLOY_PATH}
)

if (CMAKE_HOST_WIN32)
    FILE (GLOB dll_files "${SAMPLE_LIBRARY_PATH}/*.dll")
else()
    FILE (GLOB dll_files "${SAMPLE_LIBRARY_PATH}/*.so")
endif()

INSTALL (FILES ${dll_files}
    DESTINATION ${SAMPLE_DEPLOY_PATH}
)

INSTALL (FILES "libv_list.ini"      DESTINATION ${SAMPLE_DEPLOY_PATH})
