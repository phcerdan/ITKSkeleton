PROJECT(DigitalTopology)


ENABLE_TESTING()
INCLUDE(Dart)

find_package(ITK REQUIRED)
# INCLUDE(${CMAKE_SOURCE_DIR}/IJMacros.txt)
# SET(Required_Packages ITK)

# FOREACH(Package ${Required_Packages})
#   LOADPACKAGE(${Package})
# ENDFOREACH(Package)

# SET(Libraries ITKCommon ITKIO)
include_directories(${ITK_INCLUDE_DIRS})
include(${ITK_USE_FILE})
set(Libraries ${ITK_LIBRARIES})

SET(CurrentExe "DigitalTopology")
FILE(GLOB DigitalTopologyFiles *.cxx *.h *.txx)
ADD_EXECUTABLE(${CurrentExe} ${DigitalTopologyFiles} ) 
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

ADD_TEST(Test DigitalTopology Data/bunnyPadded.hdr Data/output.hdr)


