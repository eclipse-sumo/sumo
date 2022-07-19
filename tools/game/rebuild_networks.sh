#!/bin/bash
netgenerate -c square/square.netgcfg
netgenerate -c kuehne/kuehne.netgcfg
netconvert -c cross/cross.netccfg
netconvert -c grid6/grid6.netccfg
