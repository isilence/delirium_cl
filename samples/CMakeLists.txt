if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
endif()

add_subdirectory(${DIR_SAMPLE}/benchmark)
add_subdirectory(${DIR_SAMPLE}/openclInfo)
add_subdirectory(${DIR_SAMPLE}/pool)
