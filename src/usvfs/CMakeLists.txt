cmake_minimum_required (VERSION 3.9 FATAL_ERROR)
project(usvfs VERSION 0.1 LANGUAGES CXX)
set(library_name "usvfs")

## Files ##
# Main
SET(usvfs_HDRS
    include/${library_name}/usvfs.h
    include/${library_name}/dllimport.h
    include/${library_name}/hookcontext.h
    include/${library_name}/hookcallcontext.h
    include/${library_name}/hookmanager.h
    include/${library_name}/hooks/kernel32.h
    include/${library_name}/hooks/ntdll.h
    include/${library_name}/hooks/ole32.h
    include/${library_name}/hooks/cogetserverpid.h
    include/${library_name}/hooks/sharedids.h
    include/${library_name}/loghelpers.h
    include/${library_name}/redirectiontree.h
    include/${library_name}/semaphore.h
    include/${library_name}/stringcast_boost.h
    include/${library_name}/usvfs.h
)

SET(usvfs_SRCS
    src/hookcontext.cpp
    src/hookcallcontext.cpp
    src/hookmanager.cpp
    src/hooks/kernel32.cpp
    src/hooks/ntdll.cpp
    src/hooks/ole32.cpp
    src/loghelpers.cpp
    src/redirectiontree.cpp
    src/semaphore.cpp
    src/stringcast_boost.cpp
    src/usvfs.cpp
)

SET(usvfs_RCS src/version.rc)
# Helpers
SET(usvfsHelper_HDRS
    include/${library_name}/inject.h
    include/${library_name}/usvfsparameters.h
)
SET(usvfsHelper_SRCS
    src/inject.cpp
    src/usvfsparameters.cpp
)

## Source Groups ##
source_group(Helpers FILES ${usvfsHelper_HDRS} ${usvfsHelper_SRCS})

## Library ##
add_library(${library_name} STATIC ${usvfs_HDRS} ${usvfs_SRCS} ${usvfs_RCS} ${usvfsHelper_SRCS} ${usvfsHelper_HDRS})
# Headers
target_include_directories(${library_name} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src
)
# Dependencies
target_link_libraries(${library_name} PUBLIC fmt::fmt-header-only spdlog usvfs_shared)
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
add_executable(${library_name}Tests
    test/main.cpp
)
target_include_directories(${library_name}Tests PRIVATE test)
target_link_libraries(${library_name}Tests PRIVATE ${library_name} spdlog GTest)
add_test(${library_name}_Tests Tests)

#### OLD ####
#[[

SET_PROPERTY(SOURCE logger.cpp PROPERTY COMPILE_DEFINITIONS A DEFAULT_LOG_FILE=L"usvfs.log")


INCLUDE_DIRECTORIES("${Boost_INCLUDE_DIRS}"
                    "${CMAKE_SOURCE_DIR}/shared"
                    "${CMAKE_SOURCE_DIR}/thooklib"
                    "${CMAKE_SOURCE_DIR}/tinjectlib"
                    "${CMAKE_SOURCE_DIR}/usvfs_helper"
                    "${ASMJIT_SOURCE_DIR}/AsmJit"
                    "${SPDLOG_INCLUDE_DIR}"
                    "${CPPFORMAT_SOURCE_DIR}"
                    )

LINK_DIRECTORIES(${LINK_DIRECTORIES}
                 "${Boost_LIBRARY_DIRS}"
                 "${ASMJIT_BINARY_DIR}"
                 "${CMAKE_BINARY_DIR}/shared"
                 "${CMAKE_BINARY_DIR}/usvfs"
                 "${CMAKE_BINARY_DIR}/usvfs_helper"
                 "${UDIS86_BINARY_DIR}"
  )

ADD_DEFINITIONS(-D_WINDLL -DBOOST_LIB_DIAGNOSTIC -DNOMINMAX)

SET(libname usvfs_${PROJ_ARCH})

ADD_LIBRARY(${libname} SHARED ${usvfs_HDRS} ${usvfs_SRCS} ${usvfs_RCS})
TARGET_LINK_LIBRARIES(${libname} asmjit shared libudis86 thooklib tinjectlib Shlwapi usvfs_helper Version)
#]]
