#==========================================================================================================
 
SET(ppl2Dlearn_SRC
    people2D_dumpfeats.cpp
    people2D_engine.cpp)

SET(ppl2Ddtc_SRC
    people2D_detect.cpp
    people2D_engine.cpp)
 

include_directories(../lib/)

ADD_EXECUTABLE(people2D_dumpfeats ${ppl2Dlearn_SRC})
TARGET_LINK_LIBRARIES(people2D_dumpfeats lfeatures gsl gslcblas)

ADD_EXECUTABLE(people2D_detect ${ppl2Ddtc_SRC})
TARGET_LINK_LIBRARIES(people2D_detect lfeatures gsl gslcblas)
 
