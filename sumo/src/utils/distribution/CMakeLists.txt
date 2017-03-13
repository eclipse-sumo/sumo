message("entering distribution")

set(distribution_files
        Distribution.h
        Distribution_Parameterized.cpp Distribution_Parameterized.h
        Distribution_Points.cpp Distribution_Points.h
        DistributionCont.cpp DistributionCont.h
        RandomDistributor.h
        )

add_library(utdistirbutions STATIC ${distribution_files})
