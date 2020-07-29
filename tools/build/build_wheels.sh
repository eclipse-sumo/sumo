#!/bin/bash
yum-config-manager --add-repo=https://download.opensuse.org/repositories/home:/behrisch/CentOS_7/
yum install -y --nogpgcheck cmake3 xerces-c-devel proj-devel fox16-devel bzip2-devel swig3
/opt/python/cp38-cp38/bin/pip install scikit-build

/opt/python/cp38-cp38/bin/python tools/build/setup-sumo.py --cmake-executable cmake3 -j 8 bdist_wheel
mv dist/sumo-* `echo dist/sumo-* | sed 's/cp38-cp38/py2.py3-none/'`
auditwheel repair dist/sumo*.whl
for py in /opt/python/*; do
    rm dist/*.whl
    $py/bin/pip install scikit-build
    $py/bin/python tools/build/setup-sumo.py --cmake-executable cmake3 -j 8 bdist_wheel
    $py/bin/python tools/build/setup-libsumo.py bdist_wheel
    auditwheel repair dist/libsumo*.whl
done

# TODO:
# - fix version (sumo still shows "unknown")
# - add bzip2-devel as dependency for fox16-devel
# - add and test entry points
