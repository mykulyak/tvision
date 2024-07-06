file(GLOB_RECURSE TEST_SRC "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_executable(${PROJECT_NAME}-test ${TEST_SRC})
target_include_directories(${PROJECT_NAME}-test PRIVATE "${CMAKE_CURRENT_LIST_DIR}")

tv_add_private_includes(${PROJECT_NAME}-test)
tv_set_warnings(${PROJECT_NAME}-test)

find_package(GTest CONFIG REQUIRED)

target_include_directories(${PROJECT_NAME}-test PUBLIC "/opt/homebrew/include")
target_link_libraries(${PROJECT_NAME}-test PRIVATE
    ${PROJECT_NAME}
    GTest::gtest
    GTest::gtest_main
)

add_test(NAME ${PROJECT_NAME}-test COMMAND ${PROJECT_NAME}-test)
