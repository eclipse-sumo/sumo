#
# spec file for package sumo
#
# Copyright (c) 2022 SUSE LLC
# Copyright (c) 2001-2026 DLR (http://www.dlr.de/) and contributors
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
License:        EPL-2.0 OR GPL-2.0-or-later
Group:          Productivity/Scientific/Other
URL:            https://sumo.dlr.de/
Source0:        https://sumo.dlr.de/daily/sumo_%{version}.orig.tar.gz
BuildRequires:  gcc-c++
%if 0%{?centos_version} && 0%{?centos_version} < 800
BuildRequires:  cmake3
%else
BuildRequires:  cmake
BuildRequires:  java-devel
%endif
%if 0%{?fedora_version} > 36 || 0%{?suse_version} >= 1600
BuildRequires:  pkgconfig(geos)
BuildRequires:  jupedsim
%endif
%if 0%{?fedora_version} > 36
BuildRequires:  libarrow-devel
BuildRequires:  parquet-libs-devel
%endif
BuildRequires:  python3-devel
%if 0%{?fedora_version} || 0%{?suse_version}
BuildRequires:  python3-matplotlib
%endif
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
BuildRequires:  eigen3-devel
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

%if 0%{?fedora_version} || 0%{?centos_version}
%global debug_package %{nil}
%endif

%prep
%setup -q
# Use real shebang
find . -name "*.py" -o -name "*.pyw" | xargs sed -i 's,^#!%{_bindir}/env python$,#!%{_bindir}/python3,'
find . -name "*.py" -o -name "*.pyw" | xargs sed -i 's,^#!%{_bindir}/env python3$,#!%{_bindir}/python3,'
%if 0%{?fedora_version}
rm -rf tools/contributed/sumopy
%endif

%build
mkdir cmake-build
cd cmake-build
%if 0%{?centos_version} && 0%{?centos_version} < 800
cmake3 -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_SYSCONFDIR=/etc -DPYTHON_EXECUTABLE=/usr/bin/python3 ..
%else
%if 0%{?suse_version}
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_SYSCONFDIR=/etc -DEIGEN3_INCLUDE_DIR=/usr/include/eigen3 -DPYTHON_EXECUTABLE=/usr/bin/python3 ..
%else
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_SYSCONFDIR=/etc -DPYTHON_EXECUTABLE=/usr/bin/python3 ..
%endif
%endif
make %{?_smp_mflags}
make %{?_smp_mflags} man

%install
cd cmake-build
%make_install
%if 0%{?centos_version} && 0%{?centos_version} < 800
DESTDIR=%{buildroot} cmake3 --install . --component linux_integration
%else
DESTDIR=%{buildroot} cmake --install . --component linux_integration
%endif
cd ..
rm -rf %{buildroot}%{_datadir}/sumo/tools/libsumo %{buildroot}%{_datadir}/sumo/tools/libtraci
ln -s %{_datadir}/sumo/tools/assign/duaIterate.py %{buildroot}%{_bindir}/duaIterate.py
ln -s %{_datadir}/sumo/tools/osmWebWizard.py %{buildroot}%{_bindir}/osmWebWizard.py
ln -s %{_datadir}/sumo/tools/randomTrips.py %{buildroot}%{_bindir}/randomTrips.py
ln -s %{_datadir}/sumo/tools/traceExporter.py %{buildroot}%{_bindir}/traceExporter.py
%fdupes %{buildroot}%{_datadir}

%check
cd cmake-build
make %{?_smp_mflags} test

%post -n libsumocpp -p /sbin/ldconfig
%postun -n libsumocpp -p /sbin/ldconfig

%files
%defattr(-,root,root)
%{_bindir}/*
%if 0%{?fedora_version} || 0%{?sle_version} >= 150400 || 0%{?suse_version} >= 1600
%{_libdir}/libsumocs.so
%{_libdir}/libtracics.so
%{_libdir}/liblibsumojni.so
%{_libdir}/liblibtracijni.so
%endif
%{_datadir}/sumo
%doc AUTHORS README.md ChangeLog CONTRIBUTING.md NOTICE.md docs/pydoc docs/userdoc docs/examples docs/tutorial
%license LICENSE
%{_mandir}/man1/*
%config %{_sysconfdir}/profile.d/%{name}.*sh
%{_datadir}/applications/*.desktop
%{_datadir}/pixmaps/*.png
%{_datadir}/mime/application

%files -n libsumocpp
%license LICENSE
%{_libdir}/libsumocpp.so
%{_libdir}/libtracicpp.so

%files -n libsumocpp-devel
%license LICENSE
%{_includedir}/libsumo

%changelog
