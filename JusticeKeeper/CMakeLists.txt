cmake_minimum_required(VERSION 3.21)

project(plugin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)
set(FATHER ../)

file(GLOB_RECURSE SRC_FILES_DIR
    ${PROJECT_SOURCE_DIR}/src/*.c
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.h
)

include_directories(
    "${PROJECT_SOURCE_DIR}/${FATHER}/SDK/include"
    "${PROJECT_SOURCE_DIR}/${FATHER}/INC"
)

link_directories(
    ${PROJECT_SOURCE_DIR}/${FATHER}/SDK/lib
)

add_definitions(
    -D"NDEBUG"
    -D"NOMINMAX"
    -D"TEMPLATE_EXPORTS"
    -D"UNICODE"
    -D"WIN32_LEAN_AND_MEAN"
    -D"_AMD64_"
    -D"_CRT_SECURE_NO_WARNINGS"
    -D"_UNICODE"
    -D"_USRDLL"
    -D"_WINDLL"
    -D"_WINDOWS"
)

add_compile_options(
    /diagnostics:column
    /EHsc
    /FC
    /GL
    /MD
    /nologo
    /permissive-
    /sdl
    /utf-8
    /Zc:inline
    /Zi
)

add_link_options(
    /DEBUG
    /DELAYLOAD:"bedrock_server.dll"
    /DLL
    /IGNORE:4199
    /INCREMENTAL:NO
    /LTCG:INCREMENTAL
    /MANIFESTUAC:NO
    /MACHINE:X64
    /NOLOGO
    /OPT:ICF
    /OPT:REF
    /SUBSYSTEM:CONSOLE
)

add_library(plugin SHARED ${SRC_FILES_DIR})

target_link_libraries(plugin 
	"${PROJECT_SOURCE_DIR}/${FATHER}/SDK/lib/LiteLoader.lib"
	"${PROJECT_SOURCE_DIR}/${FATHER}/SDK/lib/bedrock_server_api.lib"
	"${PROJECT_SOURCE_DIR}/${FATHER}/SDK/lib/bedrock_server_var.lib"
	"${PROJECT_SOURCE_DIR}/${FATHER}/SDK/lib/SymDBHelper.lib"
)

add_custom_command(TARGET plugin PRE_BUILD
    COMMAND cmd /c ${PROJECT_SOURCE_DIR}/${FATHER}/prepare_libraries.cmd ${PROJECT_SOURCE_DIR}
)
