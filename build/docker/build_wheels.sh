yum-config-manager --add-repo=https://download.opensuse.org/repositories/home:/behrisch/CentOS_7/
yum install --nogpgcheck cmake3 xerces-c-devel proj-devel fox16-devel bzip2-devel swig3
/opt/python/cp35-cp35m/bin/pip install scikit-build

# RUN curl -OL http://sumo.dlr.de/daily/sumo-src-git.tar.gz
# RUN tar xzf sumo-src-git.tar.gz
# RUN cd sumo-git
cd $GITHUB_WORKSPACE/sumo
/opt/python/cp35-cp35m/bin/python tools/build/setup-sumo.py --cmake-executable cmake3 -j 8 bdist_wheel
auditwheel repair dist/sumo*.whl
/opt/python/cp35-cp35m/bin/python tools/build/setup-libsumo.py bdist_wheel
auditwheel repair dist/libsumo*.whl

# TODO:
# - fix version (sumo still shows "unknown")
# - add bzip2-devel as dependency for fox16-devel
# - add and test entry points
# - label sumo as python version independent package
