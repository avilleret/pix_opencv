cmake_minimum_required (VERSION 3.15)
project(pix_opencv CXX)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV 4 REQUIRED
  COMPONENTS core imgproc video
  OPTIONAL_COMPONENTS bgsegm objdetect)

set(PD_INSTALL_FOLDER ${PROJECT_NAME} CACHE PATH "Pure Data package installation directory")
message(STATUS "'${PROJECT_NAME}' will be installed in '${PD_INSTALL_FOLDE}'")

set(SOURCES
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_athreshold.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_bgsubstract.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_calibration.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_camshift.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_clahe.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_colorfilt.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_contours.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_dft.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_distrans.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_edge.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_findchessboardcorners.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_floodfill.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_hist_compare.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_hough_circles.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_hough_lines.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_hu_compare.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_hu_moments.cpp
  # ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_knear.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_laplace.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_lk.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_opticalflow.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_pgh_compare.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_surf.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_threshold.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_warpperspective.cpp

  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_utils.cpp
)

set(HEADERS
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_athreshold.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_bgsubstract.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_calibration.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_camshift.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_clahe.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_colorfilt.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_contours.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_dft.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_distrans.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_edge.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_findchessboardcorners.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_floodfill.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_hist_compare.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_hough_circles.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_hough_lines.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_hu_compare.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_hu_moments.hpp
  # ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_knear.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_laplace.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_lk.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_opticalflow.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_pgh_compare.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_surf.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_threshold.hpp
  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_warpperspective.hpp

  ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_utils.hpp
)

if(OpenCV_bgsegm_FOUND)
  add_definitions("-DHAVE_BGSEGM")
  list(APPEND SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_backgroundsubtractor.cpp)
  list(APPEND HEADERS ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_backgroundsubtractor.hpp)
endif()

if(OpenCV_objdetect_FOUND)
  add_definitions("-DHAVE_OBJDETECT")
  add_subdirectory(FaceTracker)
  list(APPEND SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_facetracker.cpp)
  list(APPEND HEADERS ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_facetracker.hpp)


  list(APPEND SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_haarcascade.cpp)
  list(APPEND HEADERS ${CMAKE_CURRENT_LIST_DIR}/src/pix_opencv_haarcascade.hpp)
endif()

set(PUREDATA_INCLUDE_DIRS "" CACHE PATH "Path to folder containing m_pd.h" )
set(GEM_INCLUDE_DIRS "" CACHE PATH "Path to Gem header folder")
find_package(PkgConfig)
pkg_check_modules(PUREDATA pd)
pkg_check_modules(GEM Gem)

include_directories(${PUREDATA_INCLUDE_DIRS} ${GEM_INCLUDE_DIRS})

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang$")
  set(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -undefined dynamic_lookup")
endif()

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

target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS} FaceTracker)

install(TARGETS ${PROJECT_NAME}
    DESTINATION ${PD_INSTALL_FOLDER}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                GROUP_READ GROUP_EXECUTE
                WORLD_READ WORLD_EXECUTE
)

file(GLOB HELP_FILES ${CMAKE_CURRENT_LIST_DIR}/help/*)

install(FILES
  ${HELP_FILES}
  ${CMAKE_CURRENT_LIST_DIR}/LICENSE.txt
    DESTINATION ${PD_INSTALL_FOLDER}
)
