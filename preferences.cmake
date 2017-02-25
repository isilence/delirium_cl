set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(POLICY CMP0020)
    cmake_policy(SET CMP0020 OLD)
endif()

if(MSVC)
  # Force to always compile with W4
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    # Update if necessary
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()


# Checking for out-of-source build
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(
    FATAL_ERROR
    "In-source builds are not allowed. Please create "
    "a directory and run cmake from there, passing the "
    "path to this source directory as the last argument. "
    "This process created the file `CMakeCache.txt' and "
    "the directory `CMakeFiles'. Please delete them.")
endif()


# Enable C++11 support
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -Wno-unknown-pragmas -Wno-attributes")
endif()
# MSVC does not require any special flags

macro(listDirsRel result curdir)
  file(GLOB children RELATIVE ${curdir} ${curdir}/*)
  set(dirlist "")
  foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
        list(APPEND dirlist ${child})
    endif()
  endforeach()
  set(${result} ${dirlist})
endmacro()

macro(listSrcFilesRel result curdir)
  file(GLOB files RELATIVE ${curdir} "${curdir}/*.c" "${curdir}/*.cc" "${curdir}/*.cpp" "${curdir}/*.h" "${curdir}/*.hpp")
  set(${result} ${files})
endmacro()

function(prepend result prefix)
  set(listVar "")
    foreach(f ${ARGN})
      list(APPEND listVar "${prefix}/${f}")
    endforeach(f)
  set(${result} "${listVar}" PARENT_SCOPE)
endfunction(prepend)

function(groupFiles result dir groupName)
    listSrcFilesRel(REL_FILES ${dir})
    prepend(ABS_FILES ${dir} ${REL_FILES})

    if("${groupName}" STREQUAL "")
        source_group("root" FILES ${ABS_FILES})
    else()
        source_group("${groupName}" FILES ${ABS_FILES})
    endif()

    list(APPEND SRC_FILES ${ABS_FILES})
    set(${result} "${SRC_FILES}" PARENT_SCOPE)
endfunction()

function(groupFilesRec result dir groupName)
    groupFiles(FILES ${dir} "${groupName}")
    listDirsRel(DIRS ${dir})

    foreach(subdir ${DIRS})
        groupFilesRec(IT_FILES ${dir}/${subdir} "${groupName}\\${subdir}")
        list(APPEND FILES ${IT_FILES})
    endforeach()

    set(${result} "${FILES}" PARENT_SCOPE)
endfunction()
