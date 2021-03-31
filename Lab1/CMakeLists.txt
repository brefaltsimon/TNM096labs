# Project name
project(Lab1)

# Add source files
file(GLOB_RECURSE SRC_FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Define the executable
add_executable(${PROJECT_NAME} ${SRC_FILES})