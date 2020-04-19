cmake_minimum_required (VERSION 3.15)
project(pix_opencv CXX)

set(CMAKE_CXX_STANDARD 11)

set(SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_athreshold.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_backgroundsubtractor.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_bgsubstract.cpp
  # ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_blobtrack.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_calibration.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_camshift.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_clahe.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_colorfilt.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_contours.cpp

  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_utils.cpp
)

set(HEADERS
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_athreshold.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_backgroundsubtractor.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_bgsubstract.hpp
  # ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_blobtrack.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_calibration.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_camshift.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_clahe.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_colorfilt.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_contours.hpp

  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_utils.hpp
)

find_package(OpenCV 4 REQUIRED)

set(PUREDATA_INCLUDE_DIRS "" CACHE PATH "Path to folder containing m_pd.h" )
set(GEM_INCLUDE_DIRS "" CACHE PATH "Path to Gem header folder")
find_package(PkgConfig)
pkg_check_modules(PUREDATA pd)
pkg_check_modules(GEM Gem)

include_directories(${PUREDATA_INCLUDE_DIRS} ${GEM_INCLUDE_DIRS})

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

# set library binary file name and extension
set_target_properties(${PROJECT_NAME}
  PROPERTIES
    PREFIX ""
    OUTPUT_NAME ${PROJECT_NAME})

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pd_darwin")
elseif(UNIX)
    if(${OSSIA_ARCHITECTURE} MATCHES "arm")
        set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".l_arm")
    elseif(${OSSIA_ARCHITECTURE} MATCHES "x86")
        set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".l_i386")
    else()
        set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pd_linux")
    endif()
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})
