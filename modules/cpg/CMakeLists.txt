cmake_minimum_required(VERSION 2.8)

set(PROJECTNAME cpg)
project(${PROJECTNAME})



FIND_PACKAGE(YARP)
FIND_PACKAGE(ICUB)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
list(APPEND CMAKE_MODULE_PATH ${ICUB_MODULE_PATH})

SET(EIGEN3_INCLUDE_DIR "$ENV{EIGEN3_INCLUDE_DIR}" )
IF(NOT EIGEN3_INCLUDE_DIR )
    MESSAGE(FATAL_ERROR "Please point the environment variable EIGEN3_INCLUDE_DIR to the include directory of your Eigen3 installation.")
ENDIF()
INCLUDE_DIRECTORIES ( "${EIGEN3_INCLUDE_DIR}" )

SET(NN2_INCLUDE_DIR "$ENV{NN2_INCLUDE_DIR}" )
IF(NOT NN2_INCLUDE_DIR )
    MESSAGE(FATAL_ERROR "Please point the environment variable NN2_INCLUDE_DIR to the include directory of your NN2 installation.")
ENDIF()
INCLUDE_DIRECTORIES ( "${NN2_INCLUDE_DIR}" )



INCLUDE_DIRECTORIES("${EIGEN3_INCLUDE_DIR}" "${NN2_INCLUDE_DIR}" ${iKin_INCLUDE_DIRS} ${ICUB_INCLUDE_DIRS} ${YARP_INCLUDE_DIRS})


FILE(GLOB folder_source *.cpp)
FILE(GLOB folder_header *.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})


IF(folder_source)
    ADD_EXECUTABLE(${PROJECTNAME}
        ${folder_source}
        ${folder_header}
    )
ELSE (folder_source)
    MESSAGE(FATAL_ERROR "No source code files found. Please add something")
ENDIF (folder_source)

SOURCE_GROUP("Source Files" FILES ${folder_source})

# activate c++-11
# target_compile_features(${PROJECTNAME} PRIVATE cxx_range_for)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


TARGET_LINK_LIBRARIES(${PROJECTNAME} ${YARP_LIBRARIES} ${ICUB_LIBRARIES})
INSTALL(TARGETS ${PROJECTNAME} DESTINATION ${ICUB_DIR}/bin)
