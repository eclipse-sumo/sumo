export CPPFLAGS="$CPPFLAGS -I/opt/X11/include -I/usr/local/include"
export LDFLAGS="-L/opt/X11/lib -stdlib=libc++"

./configure CXX=clang++ CXXFLAGS="-stdlib=libc++ -std=gnu++11" --with-xerces=/usr/local --with-proj-gdal=/usr/local --enable-debug --with-python 
