include(FetchContent)

# cmake_policy(SET CMP0135 NEW)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# find directories with includes
include_directories(../backend)
include_directories(../common)

# generate package
file(GLOB FRAMEWORK_SOURCES *.cpp *.h)
add_library(framework OBJECT ${FRAMEWORK_SOURCES})
target_link_libraries(framework PRIVATE nlohmann_json::nlohmann_json)

# get all necessary files
if(BUILD_SAMPLES)
    file(GLOB TEST_SOURCES tests/*.cpp tests/samples/*.cpp)
else()
    file(GLOB TEST_SOURCES tests/*.cpp)
endif()

foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE} $<TARGET_OBJECTS:common> $<TARGET_OBJECTS:framework>)
    target_include_directories(${TEST_NAME} PUBLIC .)
    target_link_libraries(${TEST_NAME} lc3core)
    # target_link_libraries(${TEST_NAME} PRIVATE nlohmann_json::nlohmann_json)
endforeach()

