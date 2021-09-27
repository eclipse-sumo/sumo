#
# spec file for package sumo
#
# Copyright (c) 2018 SUSE LINUX GmbH, Nuernberg, Germany.
# Copyright (c) 2001-2020 DLR (http://www.dlr.de/) and contributors
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#


Name:           sumo
Version:        git
Release:        0
# The Epoch line makes sure the nightly build is always considered newer than a release build.
# It should be removed for release builds.
Epoch:          2
Summary:        Eclipse Simulation of Urban Mobility - A Microscopic Traffic Simulation
License:        EPL-2.0
Group:          Productivity/Scientific/Other
URL:            https://sumo.dlr.de/
Source0:        https://sumo.dlr.de/daily/sumo-all-%{version}.tar.gz
BuildRequires:  gcc-c++
%if 0%{?centos_version}
BuildRequires:  cmake3
%else
BuildRequires:  cmake
%endif
BuildRequires:  python3
BuildRequires:  python3-setuptools
BuildRequires:  python3-devel
BuildRequires:  swig
BuildRequires:  help2man
BuildRequires:  pkgconfig
BuildRequires:  unzip
BuildRequires:  pkgconfig(fox)
BuildRequires:  pkgconfig(x11)
BuildRequires:  pkgconfig(xerces-c)
BuildRequires:  fdupes
BuildRequires:  pkgconfig(gdal)
BuildRequires:  pkgconfig(proj)
BuildRequires:  gl2ps-devel
%if 0%{?fedora_version} || 0%{?centos_version} || 0%{?rhel_version} || 0%{?scientificlinux_version}
BuildRequires:  libGLU-devel
BuildRequires:  libjpeg-devel
BuildRequires:  libpng-devel
BuildRequires:  proj-devel
BuildRequires:  pkgconfig(libtiff-4)
BuildRequires:  pkgconfig(xcursor)
BuildRequires:  pkgconfig(xext)
BuildRequires:  pkgconfig(xft)
BuildRequires:  pkgconfig(xi)
BuildRequires:  pkgconfig(xrandr)
%endif

%description
"Simulation of Urban MObility" (SUMO) is an open source,
highly portable, microscopic traffic simulation package
designed to handle large road networks.

%package -n libsumocpp
Summary:        Eclipse SUMO - Microscopic Traffic Simulation Library
Group:          Development/Libraries/C and C++

%description -n libsumocpp
libsumocpp provides the C++-API for adding traffic simulation
functionality to your own application.

%package -n libsumocpp-devel
Summary:        Development files for libsumocpp
Group:          Development/Libraries/C and C++
Requires:       libsumocpp = %{version}

%description -n libsumocpp-devel
This package provides development libraries and headers needed to build
software using libsumocpp.

%package -n python3-libsumo
Summary:        libsumo Python3 module
Requires:       %{name} = %{version}-%{release}
Provides:       python3-%{name} = %{version}
Obsoletes:      python3-%{name} < %{version}

%description -n python3-libsumo
The libsumo python module provides support to connect to and remote control a running sumo simulation.

%if 0%{?fedora_version} || 0%{?centos_version}
%global debug_package %{nil}
%endif

%prep
%setup -q
# Use real shebang
find . -name "*.py" -o -name "*.pyw" | xargs sed -i 's,^#!%{_bindir}/env python$,#!%{_bindir}/python3,'
find . -name "*.py" -o -name "*.pyw" | xargs sed -i 's,^#!%{_bindir}/env python3$,#!%{_bindir}/python3,'

%build
mkdir cmake-build
cd cmake-build
%if 0%{?centos_version}
cmake3 -DCMAKE_INSTALL_PREFIX:PATH=/usr -DPYTHON_EXECUTABLE=/usr/bin/python3 ..
%else
cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr -DPYTHON_EXECUTABLE=/usr/bin/python3 ..
%endif
make %{?_smp_mflags}
make %{?_smp_mflags} man


%install
cd cmake-build
%make_install
cd ..
rm -rf %{buildroot}%{_datadir}/sumo/tools/libsumo %{buildroot}%{_datadir}/sumo/tools/libtraci
ln -s %{_datadir}/sumo/tools/assign/duaIterate.py %{buildroot}%{_bindir}/duaIterate.py
ln -s %{_datadir}/sumo/tools/osmWebWizard.py %{buildroot}%{_bindir}/osmWebWizard.py
ln -s %{_datadir}/sumo/tools/randomTrips.py %{buildroot}%{_bindir}/randomTrips.py
ln -s %{_datadir}/sumo/tools/traceExporter.py %{buildroot}%{_bindir}/traceExporter.py
install -d -m 755 %{buildroot}%{_mandir}/man1
install -p -m 644 docs/man/*.1 %{buildroot}%{_mandir}/man1
install -d -m 755 %{buildroot}%{_sysconfdir}/profile.d
install -p -m 644 build/package/*sh %{buildroot}%{_sysconfdir}/profile.d
install -d -m 755 %{buildroot}%{_datadir}/applications
install -p -m 644 build/package/%{name}.desktop %{buildroot}%{_datadir}/applications
install -d -m 755 %{buildroot}%{_datadir}/pixmaps
install -p -m 644 build/package/%{name}.png %{buildroot}%{_datadir}/pixmaps
%if 0%{?suse_version}
install -d -m 755 %{buildroot}%{_datadir}/mime/application
install -p -m 644 build/package/%{name}.xml %{buildroot}%{_datadir}/mime/application/%{name}.xml
%endif
%fdupes %{buildroot}%{_datadir}

%check
cd cmake-build
#make %{?_smp_mflags} test

%post -n libsumocpp -p /sbin/ldconfig
%postun -n libsumocpp -p /sbin/ldconfig

%files
%defattr(-,root,root)
%{_bindir}/*
%{_datadir}/sumo
%doc AUTHORS README.md ChangeLog CONTRIBUTING.md NOTICE.md docs/pydoc docs/userdoc docs/examples docs/tutorial
%if 0%{?suse_version} < 1500
%doc LICENSE
%else
%license LICENSE
%endif
%{_mandir}/man1/*
%config %{_sysconfdir}/profile.d/%{name}.*sh
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/%{name}.png
%if 0%{?suse_version}
%{_datadir}/mime/application
%endif

%files -n libsumocpp
%if 0%{?suse_version} < 1500
%doc LICENSE
%else
%license LICENSE
%endif
%{_libdir}/libsumocpp.so
%{_libdir}/libtracicpp.so

%files -n libsumocpp-devel
%if 0%{?suse_version} < 1500
%doc LICENSE
%else
%license LICENSE
%endif
%{_includedir}/libsumo

%files -n python3-libsumo
%if 0%{?suse_version} < 1500
%doc LICENSE
%else
%license LICENSE
%endif
%{python3_sitelib}/sumolib*/
%{python3_sitelib}/traci*/
%{python3_sitearch}/libsumo*/
%{python3_sitearch}/libtraci*/

%changelog
