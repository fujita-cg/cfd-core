cmake_minimum_required(VERSION 3.13)

# 絶対パス->相対パス変換
cmake_policy(SET CMP0076 NEW)
#cmake_policy(SET CMP0015 NEW)

####################
# options
####################
option(CFDCORE_SHARED "force shared build (ON or OFF. default:OFF)" OFF)
if(CMAKE_JS_INC)
option(ENABLE_SHARED "enable shared library (ON or OFF. default:ON)" ON)
else()
option(ENABLE_SHARED "enable shared library (ON or OFF. default:OFF)" OFF)
endif()
option(ENABLE_DEBUG "enable debugging (ON or OFF. default:OFF)" OFF)
option(ENABLE_ELEMENTS "enable elements code (ON or OFF. default:ON)" ON)
option(ENABLE_BITCOIN  "enable bitcoin code (ON or OFF. default:ON)" ON)

if(NOT WIN32)
option(TARGET_RPATH "target rpath list (separator is ';') (default:@rpath;./)" OFF)
endif()

if(ENABLE_DEBUG)
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:DEBUGBUILD>)
set(STACK_PROTECTOR_OPT  $<IF:$<CXX_COMPILER_ID:MSVC>,/GS,-fstack-check -fstack-protector>)
else()
set(STACK_PROTECTOR_OPT  "")
endif()

if(CFDCORE_SHARED AND (WIN32 OR APPLE))
set(USE_CFDCORE_SHARED  TRUE)
else()
set(USE_CFDCORE_SHARED  FALSE)
endif()

####################
# common setting
####################
set(WORK_WINDOWS_BINARY_DIR_NAME  $<IF:$<CONFIG:Debug>,Debug,Release>)
if(NOT CFD_OBJ_BINARY_DIR)
set(CFD_OBJ_BINARY_DIR   ${CMAKE_BINARY_DIR}/${WORK_WINDOWS_BINARY_DIR_NAME})
set(CFD_ROOT_BINARY_DIR
  $<IF:$<PLATFORM_ID:Windows>,${CFD_OBJ_BINARY_DIR},${CMAKE_BINARY_DIR}>)
endif()
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${CFD_OBJ_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CFD_OBJ_BINARY_DIR})
# set(WORK_BINARY_DIR  ${CFD_ROOT_BINARY_DIR})
# set(BINARY_OUTPUT_DIR  ${CFD_ROOT_BINARY_DIR})

if(NOT ENABLE_BITCOIN)
set(CFD_BITCOIN_USE   CFD_DISABLE_BITCOIN)
else()
set(CFD_BITCOIN_USE   "")
endif()

if(NOT ENABLE_ELEMENTS)
set(ELEMENTS_COMP_OPT "")
set(CFD_ELEMENTS_USE   CFD_DISABLE_ELEMENTS)
else()
set(ELEMENTS_COMP_OPT  BUILD_ELEMENTS)
set(CFD_ELEMENTS_USE   "")
endif()

if(NOT WIN32)
if(APPLE)
set(CMAKE_MACOSX_RPATH 1)
endif()
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(TARGET_RPATH)
set(CMAKE_INSTALL_RPATH "${TARGET_RPATH};./;@rpath")
else()
set(CMAKE_INSTALL_RPATH "./;@rpath")
endif()
endif()

####################
# cfdcore setting
####################
transform_makefile_srclist("Makefile.srclist" "${CMAKE_CURRENT_BINARY_DIR}/Makefile.srclist.cmake")
include(${CMAKE_CURRENT_BINARY_DIR}/Makefile.srclist.cmake)

if(NOT CFDCORE_DEBUG)
set(CFDCORE_DEBUG_OPT "")
else()
set(CFDCORE_DEBUG_OPT  $<IF:$<C_COMPILER_ID:MSVC>,/DCFDCORE_DEBUG,CFDCORE_DEBUG>)
endif()

if(NOT CFDCORE_LOG_CONSOLE)
set(CFDCORE_LOG_CONSOLE_OPT "")
else()
set(CFDCORE_LOG_CONSOLE_OPT  $<IF:$<C_COMPILER_ID:MSVC>,/DCFDCORE_LOG_CONSOLE,CFDCORE_LOG_CONSOLE>)
endif()

####################
# cfdcore test
####################
if(ENABLE_TESTS)
enable_testing()
include(CTest)
project(cfdcore_test CXX)

if(MSVC)
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} /Od /Zi")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Od /Zi")
else()
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -O0 -ggdb")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb")
endif()

set(LIBWALLY_LIBRARY wally)
set(UNIVALUE_LIBRARY univalue)
set(CFDCORE_LIBRARY cfdcore)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(WIN32 AND ENABLE_SHARED)
add_executable(${PROJECT_NAME} ${TEST_CFDCORE_SOURCES})
else()
add_executable(${PROJECT_NAME} ${TEST_CFDCORE_SOURCES} ${TEST_CFDCORE_STATIC_SOURCES})
endif()

target_compile_options(${PROJECT_NAME}
  PRIVATE
    $<IF:$<CXX_COMPILER_ID:MSVC>,
      /source-charset:utf-8 /Wall 
      /wd4061 /wd4244 /wd4251 /wd4365 /wd4464 /wd4514 /wd4571 /wd4574 /wd4623 /wd4625 /wd4626 /wd4668 /wd4710 /wd4711 /wd4774 /wd4820 /wd4946 /wd5026 /wd5027 /wd5039 /wd5045 /wd5052
      ${STACK_PROTECTOR_OPT},
      -Wall -Wextra
    >
    $<$<BOOL:$<CXX_COMPILER_ID:GNU>>:${STACK_PROTECTOR_OPT}>
)

if(ENABLE_SHARED OR USE_CFDCORE_SHARED)
target_compile_definitions(${PROJECT_NAME}
  PRIVATE
    CFD_CORE_SHARED=1
    ${ELEMENTS_COMP_OPT}
    ${CFD_BITCOIN_USE}
    ${CFD_ELEMENTS_USE}
)
else()
target_compile_definitions(${PROJECT_NAME}
  PRIVATE
    ${ELEMENTS_COMP_OPT}
    ${CFD_BITCOIN_USE}
    ${CFD_ELEMENTS_USE}
)
endif()

target_include_directories(${PROJECT_NAME}
  PRIVATE
    .
    ../src
    ../src/include
)

target_link_directories(${PROJECT_NAME}
  PRIVATE
    ./
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE $<$<BOOL:$<CXX_COMPILER_ID:MSVC>>:winmm.lib>
  PRIVATE $<$<BOOL:$<CXX_COMPILER_ID:MSVC>>:ws2_32.lib>
  PRIVATE $<IF:$<OR:$<PLATFORM_ID:Darwin>,$<PLATFORM_ID:Windows>>,,rt>
  PRIVATE $<$<BOOL:$<CXX_COMPILER_ID:GNU>>:-static-libgcc>
  PRIVATE $<$<BOOL:$<CXX_COMPILER_ID:GNU>>:-static-libstdc++>
  PRIVATE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:pthread>
  PRIVATE
    ${LIBWALLY_LIBRARY}
    ${UNIVALUE_LIBRARY}
    ${CFDCORE_LIBRARY}
    gtest_main
    gmock
)


add_test(
  NAME ${PROJECT_NAME}
  COMMAND $<TARGET_FILE:${PROJECT_NAME}>
  WORKING_DIRECTORY ${CFD_OBJ_BINARY_DIR}
)

endif()		# ENABLE_TESTS
