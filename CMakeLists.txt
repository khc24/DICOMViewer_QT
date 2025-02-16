cmake_minimum_required(VERSION 3.16)

# Windows인 경우 vcpkg toolchain 사용
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "vcpkg toolchain file")
endif()

set(CMAKE_PREFIX_PATH "C:/vcpkg/installed/x64-windows" ${CMAKE_PREFIX_PATH})
set(CMAKE_PREFIX_PATH "C:/vcpkg/installed/x64-windows" CACHE PATH "VCPKG Installed Path")

project("DICOMViewer_QT")

# MSVC용 핫 리로드 설정
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Multi-Configuration 및 Single-Config 설정
if(CMAKE_GENERATOR MATCHES "Visual Studio")
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Supported build types" FORCE)
else()
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build" FORCE)
    endif()
endif()

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_CONFIGURATION_TYPES: ${CMAKE_CONFIGURATION_TYPES}")
message(STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")

# VTK 및 GDCM 경로 설정 (Debug/Release 구분)
if(CMAKE_GENERATOR MATCHES "Visual Studio")
    set(VTK_DIR_DEBUG "C:/Program Files/VTK/Debug/lib/cmake/vtk-9.4")
    set(VTK_DIR_RELEASE "C:/Program Files/VTK/Release/lib/cmake/vtk-9.4")
    set(GDCM_DIR_DEBUG "C:/Program Files/GDCM/Debug/lib/gdcm-3.0")
    set(GDCM_DIR_RELEASE "C:/Program Files/GDCM/Release/lib/gdcm-3.0")
    set(VTK_DIR ${VTK_DIR_RELEASE})
    set(GDCM_DIR ${GDCM_DIR_RELEASE})
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(VTK_DIR ${VTK_DIR_DEBUG})
        set(GDCM_DIR ${GDCM_DIR_DEBUG})
    endif()
else()
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(VTK_DIR "C:/Program Files/VTK/Debug/lib/cmake/vtk-9.4")
        set(GDCM_DIR "C:/Program Files/GDCM/Debug/lib/gdcm-3.0")
    else()
        set(VTK_DIR "C:/Program Files/VTK/Release/lib/cmake/vtk-9.4")
        set(GDCM_DIR "C:/Program Files/GDCM/Release/lib/gdcm-3.0")
    endif()
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_DISABLE_PRECOMPILE_HEADERS ON)

# VTK & GDCM 경로를 캐시에 저장하고 CMake가 찾도록 설정
set(VTK_DIR ${VTK_DIR} CACHE PATH "Path to VTK")
set(GDCM_DIR ${GDCM_DIR} CACHE PATH "Path to GDCM")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${VTK_DIR} ${GDCM_DIR})

# Qt6, VTK, GDCM, OpenGL, GLEW, glfw3 찾기
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(VTK REQUIRED)
find_package(GDCM REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
if(GLEW_FOUND)
    message(STATUS "✅ GLEW FOUND: ${GLEW_INCLUDE_DIRS} ${GLEW_LIBRARIES}")
else()
    message(FATAL_ERROR "🚨 GLEW NOT FOUND! vcpkg 설치 및 CMAKE_PREFIX_PATH 확인 필요.")
endif()

# glfw3 찾기 (vcpkg toolchain을 이용하여 자동 검색)
find_package(glfw3 CONFIG REQUIRED)
if(glfw3_FOUND)
    message(STATUS "✅ GLFW FOUND")
else()
    message(FATAL_ERROR "🚨 GLFW NOT FOUND! vcpkg 설치 및 CMAKE_PREFIX_PATH 확인 필요.")
endif()

message(STATUS "VTK_FOUND: ${VTK_FOUND}")
message(STATUS "VTK_DIR: ${VTK_DIR}")
message(STATUS "GDCM_FOUND: ${GDCM_FOUND}")
message(STATUS "GDCM_DIR: ${GDCM_DIR}")

# GDCM 라이브러리 설정 (Debug/Release 구분)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(GDCM_LIBRARIES 
        "C:/Program Files/GDCM/Debug/lib/gdcmMSFF.lib"
        "C:/Program Files/GDCM/Debug/lib/gdcmDSED.lib"
        "C:/Program Files/GDCM/Debug/lib/gdcmIOD.lib"
        "C:/Program Files/GDCM/Debug/lib/gdcmDICT.lib"
        "C:/Program Files/GDCM/Debug/lib/gdcmCommon.lib"
        "C:/Program Files/GDCM/Debug/lib/vtkgdcm-9.4d.lib"
    )
else()
    set(GDCM_LIBRARIES 
        "C:/Program Files/GDCM/Release/lib/gdcmMSFF.lib"
        "C:/Program Files/GDCM/Release/lib/gdcmDSED.lib"
        "C:/Program Files/GDCM/Release/lib/gdcmIOD.lib"
        "C:/Program Files/GDCM/Release/lib/gdcmDICT.lib"
        "C:/Program Files/GDCM/Release/lib/gdcmCommon.lib"
        "C:/Program Files/GDCM/Release/lib/vtkgdcm-9.4.lib"
    )
endif()

# 실행 파일 추가
qt_add_executable(DICOMViewer_QT
    DICOMViewer_QT.cpp
    DICOMViewer_QT.h
    DlgVtkView.h
    DlgVtkView.cpp
    ChildView.h
    ChildView.cpp
    DVManager.h
    DVManager.cpp
    test.cpp
)

# 라이브러리 연결 (GLEW는 find_package로 가져온 타겟 사용)
target_link_libraries(DICOMViewer_QT PRIVATE 
    OpenGL::GL
    GLEW::GLEW
    glfw
)

target_link_libraries(DICOMViewer_QT PRIVATE 
    Qt6::Widgets ${VTK_LIBRARIES} ${GDCM_LIBRARIES} gdcmMSFF
)

target_precompile_headers(DICOMViewer_QT PRIVATE ${CMAKE_SOURCE_DIR}/pch.h)

# Qt6의 MOC, UIC, RCC 자동화
set_target_properties(DICOMViewer_QT PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DICOMViewer_QT PROPERTY CXX_STANDARD 20)
endif()

include_directories(${VTK_INCLUDE_DIRS} ${GDCM_INCLUDE_DIRS})
include_directories(${GDCM_INCLUDE_DIRS}/vtk-9.4)

message(STATUS "GDCM_DIR: ${GDCM_DIR}")
message(STATUS "GDCM_LIBRARIES: ${GDCM_LIBRARIES}")
message(STATUS "GDCM_LIBRARY_DIRS: ${GDCM_LIBRARY_DIRS}")
message(STATUS "GDCM_INCLUDE_DIRS: ${GDCM_INCLUDE_DIRS}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# 실행 파일이 사용하는 DLL 복사 (VTK, GDCM, 그리고 GLEW)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(GDCM_BIN_DIR "C:/Program Files/GDCM/Debug/bin")
    set(VTK_BIN_DIR "C:/Program Files/VTK/Debug/bin")
    set(GLEW_DLL_PATH "C:/vcpkg/installed/x64-windows/debug/bin/glew32d.dll")
else()
    set(GDCM_BIN_DIR "C:/Program Files/GDCM/Release/bin")
    set(VTK_BIN_DIR "C:/Program Files/VTK/Release/bin")
    set(GLEW_DLL_PATH "C:/vcpkg/installed/x64-windows/bin/glew32.dll")
endif()

add_custom_command(TARGET DICOMViewer_QT POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${GDCM_BIN_DIR}"
    "$<TARGET_FILE_DIR:DICOMViewer_QT>"
)

add_custom_command(TARGET DICOMViewer_QT POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${VTK_BIN_DIR}"
    "$<TARGET_FILE_DIR:DICOMViewer_QT>"
)

add_custom_command(TARGET DICOMViewer_QT POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${GLEW_DLL_PATH}"
    "$<TARGET_FILE_DIR:DICOMViewer_QT>"
)

set(GLFW3_DLL_PATH "C:/vcpkg/installed/x64-windows/bin/glfw3.dll")

add_custom_command(TARGET DICOMViewer_QT POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${GLFW3_DLL_PATH}"
    "$<TARGET_FILE_DIR:DICOMViewer_QT>"
)
