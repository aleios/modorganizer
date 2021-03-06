cmake_minimum_required (VERSION 3.9 FATAL_ERROR)
project(tinjectlib VERSION 0.1 LANGUAGES CXX)
set(library_name "tinjectlib")

## Files ##
# Main
SET(tinjectlib_HDRS
    include/${library_name}/asmjit_sane.h
    include/${library_name}/injectlib.h
)

SET(tinjectlib_SRCS
    src/injectlib.cpp
)

## Library ##
add_library(${library_name} STATIC ${tinjectlib_HDRS} ${tinjectlib_SRCS})
# Headers
target_include_directories(${library_name} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src
)
# Dependencies
target_link_libraries(${library_name} PUBLIC spdlog AsmJit usvfs_shared common)
# Install Targets
install(TARGETS ${library_name} EXPORT ${library_name}Config
    ARCHIVE  DESTINATION lib # <prefix>/lib. DLL import libs.
    LIBRARY  DESTINATION lib # <prefix>/lib. Shared Libs, non DLL.
    RUNTIME  DESTINATION bin # <prefix>/bin. EXEs, DLLs and shit.
)
install(DIRECTORY include/ DESTINATION include)
# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
install(EXPORT ${library_name}Config DESTINATION cmake)
# This makes the project importable from the build directory
export(TARGETS ${library_name} FILE ${library_name}Config.cmake)

## Tests ##
add_executable(${library_name}TestExe EXCLUDE_FROM_ALL test/testexe.cpp)
add_library(${library_name}TestDll SHARED EXCLUDE_FROM_ALL
    test/testdll.cpp
    test/testdll.h
)

add_executable(${library_name}Tests EXCLUDE_FROM_ALL
    test/main.cpp
)
set_target_properties(${library_name}Tests PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD ON)
set_target_properties(${library_name}TestExe PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD ON)
set_target_properties(${library_name}TestDll PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD ON)
target_include_directories(${library_name}Tests PRIVATE test)
target_link_libraries(${library_name}Tests PRIVATE ${library_name} spdlog GTest common)
target_link_libraries(${library_name}TestExe PRIVATE ${library_name} spdlog GTest common)
target_link_libraries(${library_name}TestDll PRIVATE ${library_name} spdlog GTest common)
add_test(${library_name}_Tests ${library_name}Tests)
add_test(${library_name}_Tests ${library_name}TestExe)
add_test(${library_name}_Tests ${library_name}TestDll)









