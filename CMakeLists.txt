cmake_minimum_required(VERSION 3.16)
project(BattleCity VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
# set(SDL2_DIR "C:/SDL2-2.30.6/cmake")
# find_package(SDL2 REQUIRED)
# find_package(SDL2_mixer REQUIRED)  # Temporarily disabled

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    src
    "C:/SDL2-2.30.6/include"
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    "C:/SDL2-2.30.6/lib/x64/SDL2.lib"
    "C:/SDL2-2.30.6/lib/x64/SDL2main.lib"
)

# Copy assets
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# Copy SDL2.dll to output directory
if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/SDL2-2.30.6/lib/x64/SDL2.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()

# Compiler flags
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)  # Removed /WX to allow warnings
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Debug/Release configurations
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
endif()