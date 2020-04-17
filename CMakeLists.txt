cmake_minimum_required (VERSION 3.15)
project(pix_opencv CXX)

set(SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_athreshold.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_backgroundsubtractor.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_bgsubstract.cc
  # ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_blobtrack.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_calibration.cc

  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv.cc
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_utils.cc
)

set(HEADERS
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_athreshold.h
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_backgroundsubtractor.h
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_bgsubstract.h
  # ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_blobtrack.h
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_calibration.h

  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_utils.h
)

find_package(OpenCV 4 REQUIRED)

include_directories(/usr/local/include/Gem)

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
