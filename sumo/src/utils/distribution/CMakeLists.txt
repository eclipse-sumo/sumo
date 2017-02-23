message("entering distribution")

set(distribution_files
        Distribution.h
        Distribution_MeanDev.cpp Distribution_MeanDev.h
        Distribution_Points.cpp Distribution_Points.h
        DistributionCont.cpp DistributionCont.h
        )

add_library(utdistirbutions STATIC ${distribution_files})
