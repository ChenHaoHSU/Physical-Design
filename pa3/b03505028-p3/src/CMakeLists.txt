CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
PROJECT (place)
SET(CMAKE_C_FLAGS "-O3 -Wall -g -fopenmp -ffast-math -DUSE_OPENMP")
SET(CMAKE_CXX_FLAGS "-O3 -Wall -g -fopenmp -ffast-math -DUSE_OPENMP")
ADD_SUBDIRECTORY (Parser)
ADD_SUBDIRECTORY (PlaceCommon)
ADD_SUBDIRECTORY (Gnuplot)
ADD_SUBDIRECTORY (Placement)
ADD_SUBDIRECTORY (Legalizer)



ADD_EXECUTABLE (legalizer
main.cpp
)

INCLUDE_DIRECTORIES (
	${PROJECT_SOURCE_DIR}/Parser
        ${PROJECT_SOURCE_DIR}/Placement
	${PROJECT_SOURCE_DIR}/PlaceCommon
	${PROJECT_SOURCE_DIR}/Gnuplot
        ${PROJECT_SOURCE_DIR}/Legalizer
        ${PROJECT_SOURCE_DIR}
)
#TARGET_LINK_LIBRARIES (legalizer Legalizer Placement PlaceCommon Gnuplot Parser )
TARGET_LINK_LIBRARIES (legalizer Legalizer Placement Parser PlaceCommon Gnuplot )
