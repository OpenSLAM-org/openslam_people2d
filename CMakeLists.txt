# This is the main CMake configuration file, you should always do "cmake ." from here and not from subdirs

# Needed by Cmake versione 2.6 or later (Run "cmake --help-policy CMP0003" for more information.)"
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
IF(COMMAND CMAKE_POLICY)
	CMAKE_POLICY(SET CMP0003 OLD)
ENDIF(COMMAND CMAKE_POLICY)

SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};./build_tools")

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RELEASE CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)
 
# Some project related variables
PROJECT(PEOPLE2D CXX C)

SET(CMAKE_VERBOSE_MAKEFILE 0)
SET(VERBOSE 1)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PEOPLE2D_SOURCE_DIR}/lib")
SET(EXECUTABLE_OUTPUT_PATH   "${PEOPLE2D_SOURCE_DIR}")

# Set to build shared objects as option
OPTION(BUILD_SHARED_LIBS "Build package with shared libraries." ON)

# ---------------------------- Checking the OS ------------------------------
IF("${CMAKE_SYSTEM}" MATCHES "Linux")
	MESSAGE(STATUS "Compiling on Linux")
	SET(LINUX 1)
	EXEC_PROGRAM("uname" ARGS -m OUTPUT_VARIABLE myArch)
	IF("${myArch}" MATCHES "x86_64")
		MESSAGE(STATUS "  64-bit architecture detected")
		
		SET(LINUX64 1)
		SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}64")
		SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}64")
	ENDIF("${myArch}" MATCHES "x86_64")
ELSEIF(APPLE)
	SET(LINUX 0)
	MESSAGE(STATUS "Compiling on MacOSX")
ELSEIF(WIN32)
	SET(LINUX 0)
	IF(CYGWIN)
		MESSAGE(STATUS "Compiling on CygWin")
		SET(CYGWIN 1)
	ELSE(CYGWIN)
		MESSAGE(FATAL_ERROR "Compiling on Windows is not possible at the moment")
	ENDIF(CYGWIN)
ENDIF("${CMAKE_SYSTEM}" MATCHES "Linux")


# Library: GSL
INCLUDE (./build_tools/FindGSL.cmake)
IF(GSL_FOUND)
	MESSAGE(STATUS "GSL found")
	IF($ENV{VERBOSE})
		MESSAGE(STATUS "   GSL_LIBRARIES = ${GSL_LIBRARIES}")
		MESSAGE(STATUS "   GSL_INCLUDE_DIR = ${GSL_INCLUDE_DIR}")
		MESSAGE(STATUS "   CMAKE_GSL_CXX_FLAGS = ${CMAKE_GSL_CXX_FLAGS}")
		MESSAGE(STATUS "   GSL_LINK_DIRECTORIES = ${GSL_LINK_DIRECTORIES}")
		MESSAGE(STATUS "   GSL_EXE_LINKER_FLAGS = ${GSL_EXE_LINKER_FLAGS}")
	ENDIF($ENV{VERBOSE})
	INCLUDE_DIRECTORIES(${GSL_INCLUDE_DIR})
	LINK_DIRECTORIES(${GSL_LINK_DIRECTORIES})
ELSE(GSL_FOUND)
	MESSAGE(FATAL_ERROR "	GSL not found, it is REQUIRED.\n		GSL can be found on http://www.gnu.org/software/gsl")
ENDIF(GSL_FOUND)
 
INCLUDE_DIRECTORIES(${PRJ_SOURCE_DIR})

# build

#Optimization
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -march=native -O3 -funroll-loops -fomit-frame-pointer -Wall -W -pipe -ftree-vectorize")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native -O3 -funroll-loops -fomit-frame-pointer -Wall -W -pipe -ftree-vectorize")

#Debug (it can clash with optimization)
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g3 -ggdb3 -fno-inline")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -ggdb3 -fno-inline")
ADD_DEFINITIONS(-Wall)


#Recursive compilation

ADD_SUBDIRECTORY(utils)
ADD_SUBDIRECTORY(lib)
