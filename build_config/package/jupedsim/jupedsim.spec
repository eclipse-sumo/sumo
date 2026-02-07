#
# spec file for package jupedsim
#
# Copyright (c) 2022 SUSE LLC
# Copyright (c) 2001-2025 DLR (http://www.dlr.de/) and contributors
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


Name:           jupedsim
Version:        1.3.1
Release:        0
Summary:        Juelich Pedestrian Simulator
License:        LGPL-3.0
Group:          Productivity/Scientific/Other
URL:            https://jupedsim.org/
Source0:        https://github.com/PedestrianDynamics/%{name}/archive/refs/tags/v%{version}.tar.gz
Patch1:         invalid_access.patch
BuildRequires:  gcc-c++
BuildRequires:  cmake
BuildRequires:  cmake
BuildRequires:  python3-devel

%description
JuPedSim is a Python package with a C++ core to simulate pedestrian dynamics.
This package only provides the C++ core library for SUMO to link against.

%prep
%setup -q
%patch -P1 -p1

%build
cmake -Bcmake-build -DCMAKE_INSTALL_PREFIX:PATH=/usr
cmake --build cmake-build %{?_smp_mflags}

%install
cd cmake-build
%make_install

%files
%defattr(-,root,root)
%{_prefix}/lib/libjupedsim.so
%{_prefix}/lib/cmake
%{_includedir}/jupedsim

%changelog
