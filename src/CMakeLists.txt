cmake_minimum_required(VERSION 3.2)
project(DigitalTopology)

################ GTEST ################
# Setup using ExternalProject + add_subdirectory.
#    from: http://crascit.com/2015/07/25/cmake-gtest/
# Download and unpack gtest at configure time
configure_file(./gtest-CMakeLists.txt gtest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/gtest-download )
execute_process(COMMAND ${CMAKE_COMMAND} --build .
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/gtest-download )

# Add gtest directly to our build
add_subdirectory(${CMAKE_BINARY_DIR}/gtest-src
    ${CMAKE_BINARY_DIR}/gtest-build
    EXCLUDE_FROM_ALL )

# Add the gtest include directory, since gtest
# doesn't add that dependency to its gtest target
target_include_directories(gtest INTERFACE
    "${CMAKE_BINARY_DIR}/gtest-src/include" )

########################################
enable_testing()

INCLUDE(Dart)
find_package(ITK REQUIRED)
# find_package(ITK REQUIRED COMPONENTS
#   ITKCommon
#   ITKImageFunction
#   ITKImageIntensity
#   ITKIOImageBase
#   ITKIOJPEG
#   ITKIOPNG
#   ITKIOTIFF
#   ITKIORAW
#   ITKIONRRD
#   # ITKIOAnalyzeImage
#   ITKIOBMP
#   ITKIOMeta
# )
# SET(Libraries ITKCommon ITKIO)
include_directories(${ITK_INCLUDE_DIRS})
include_directories(./Connectivity)
include_directories(./TopologicalNumberFunctions)
include(${ITK_USE_FILE})

add_library(SkeletonFilter itkSkeletonizeImageFilter.h)
set_target_properties(SkeletonFilter PROPERTIES LINKER_LANGUAGE CXX)

ADD_EXECUTABLE(test_bunny test_bunny.cxx)
TARGET_LINK_LIBRARIES(test_bunny SkeletonFilter)
TARGET_LINK_LIBRARIES(test_bunny ${ITK_LIBRARIES})

ADD_EXECUTABLE(Skeletonize main.cxx)
TARGET_LINK_LIBRARIES(Skeletonize SkeletonFilter)
TARGET_LINK_LIBRARIES(Skeletonize ${ITK_LIBRARIES})

ADD_TEST(Test test_bunny Data/bunnyPadded.hdr Data/output.hdr)


