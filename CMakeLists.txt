cmake_minimum_required (VERSION 4.2.3)
project (froggy-jump)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- External libraries ---
set(GLFW_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/external/GLFW/glfw3.lib")
set(GLFW_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/GLFW/include")
set(GLM_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/")
set(STB_IMAGE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/stb_image")
set(GLTEXT_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/gltext")

add_library(glad STATIC "external/glad/src/glad.c")
target_include_directories(glad PUBLIC "external/glad/include")

add_library(miniaudio STATIC "external/miniaudio/miniaudio_impl.cpp")
target_include_directories(miniaudio PUBLIC "external/miniaudio")

# --- Sources ---
file(GLOB_RECURSE MY_SOURCE_FILES
    "src/*.cpp"
    "src/*rc")
file(GLOB_RECURSE MY_HEADER_FILES "src/*.h")

add_executable(${PROJECT_NAME}
    ${MY_HEADER_FILES}
    ${MY_SOURCE_FILES}
)

# --- Resources config (Symlinks) ---
set(DATA_SRC "${CMAKE_CURRENT_SOURCE_DIR}/data")

set(DATA_OUTPUT_DIR "$<TARGET_FILE_DIR:${PROJECT_NAME}>/data")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink "${DATA_SRC}" "${DATA_OUTPUT_DIR}"
    
    COMMENT "Creating symlinks for output data"
)

# --- Target config ---
target_include_directories(${PROJECT_NAME} PRIVATE 
    "src"
    "${GLFW_INCLUDE_DIR}"
    "${GLM_INCLUDE_DIR}"
    "${STB_IMAGE_INCLUDE_DIR}"
    "${GLTEXT_INCLUDE_DIR}"
)

target_link_libraries(${PROJECT_NAME}
    glad
    miniaudio
    "${GLFW_LIBRARY}"
    opengl32
    winmm
)