file(GLOB SOURCE_FILES *.cpp)

add_executable(Http ${SOURCE_FILES})

target_link_libraries(Http PRIVATE netcpp::netcpp)