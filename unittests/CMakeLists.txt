enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

groupFilesRec(ALL_FILES ${DIR_UNIT_TESTS} "")

include_directories(${DIR_INC})
add_executable(unittests ${ALL_FILES})
target_link_libraries(unittests ${GTEST_BOTH_LIBRARIES})

GTEST_ADD_TESTS(AllTests "" ${ALL_FILES})
