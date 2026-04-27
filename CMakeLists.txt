cmake_minimum_required(VERSION 3.22)

include(${CMAKE_CURRENT_SOURCE_DIR}/qpm_defines.cmake)

project(${COMPILE_ID} VERSION ${MOD_VERSION})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE CPP_FILES CONFIGURE_DEPENDS src/*.cpp)

add_library(${COMPILE_ID} SHARED ${CPP_FILES})
set_target_properties(${COMPILE_ID} PROPERTIES OUTPUT_NAME beatsinger)
target_sources(${COMPILE_ID} PRIVATE
    extern/includes/beatsaber-hook/shared/inline-hook/And64InlineHook.cpp
)

get_filename_component(PAPER2_SHARED_DIR extern/includes/paper2_scotland2/shared REALPATH)
get_filename_component(PAPER2_PACKAGE_DIR "${PAPER2_SHARED_DIR}/../.." ABSOLUTE)

target_include_directories(${COMPILE_ID} PRIVATE
    include
    extern/includes
    extern/includes/beatsaber-hook
    extern/includes/bs-cordl
    extern/includes/bs-cordl/include
    extern/includes/bsml
    extern/includes/custom-types
    extern/includes/fmt/fmt/include
    extern/includes/libil2cpp
    extern/includes/libil2cpp/il2cpp/libil2cpp
    extern/includes/libil2cpp/il2cpp/external/baselib/Include
    extern/includes/libil2cpp/il2cpp/external/baselib/Platforms/Android/Include
    extern/includes/paper2_scotland2
    extern/includes/scotland2
    extern/includes/songcore
    extern/includes/tinyxml2
)

target_compile_definitions(${COMPILE_ID} PRIVATE
    MOD_ID="${MOD_ID}"
    VERSION="${MOD_VERSION}"
    NEED_UNSAFE_CSHARP
    UNITY_2021
    HAS_CODEGEN
    FMT_HEADER_ONLY
)

target_compile_options(${COMPILE_ID} PRIVATE
    -fdeclspec
    -Wno-invalid-offsetof
    -Wno-extra-qualification
)

target_link_libraries(${COMPILE_ID} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/libs/libbeatsaber-hook.so
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/libs/libcustom-types.so
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/libs/libbsml.so
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/libs/libsongcore.so
    ${PAPER2_PACKAGE_DIR}/lib/libpaper2_scotland2.so
    log
)
