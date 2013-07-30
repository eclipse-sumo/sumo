#
# spec file for package sumo
#
# Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
#  This file is part of SUMO.
#  SUMO is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
# norootforbuild
Name:           sumo
Summary:        Simulation of Urban Mobility - A Microscopic Traffic Simulation
Version:        svn
Release:        1
Url:            http://sumo.sourceforge.net/
Source0:        %{name}-src-%{version}.tar.gz
Source1:        %{name}-doc-%{version}.zip
License:        GPL-3.0+
Group:          Productivity/Scientific/Other
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
BuildRequires:  gcc-c++ libproj-devel libgdal-devel fox16-devel unzip
BuildRequires:  libxerces-c-devel
%if 0%{?mandriva_version}
BuildRequires:  XFree86-devel postgresql-devel libmesaglu1-devel
%else
BuildRequires:  xorg-x11-devel xorg-x11-Mesa-devel
%endif
%if 0%{?fedora_version} || 0%{?centos_version} || 0%{?rhel_version} || 0%{?scientificlinux_version}
BuildRequires:  libGLU-devel libXext-devel libXft-devel
%if 0%{?fedora_version} >= 15
BuildRequires:  netcdf hdf5 javamail
%endif
%endif
Autoreqprov: on

%description
"Simulation of Urban MObility" (SUMO) is an open source,
highly portable, microscopic road traffic simulation package
designed to handle large road networks.

%prep
%setup -q
unzip -o %{SOURCE1} -d ..
mv docs/tutorial docs/examples
%if 0%{?sles_version}
find . -name "*.jar" | xargs rm
%endif

%build
%configure
%{__make}

%install
%makeinstall
%__mkdir_p %{buildroot}%{_prefix}/lib/sumo
cp -a tools/* %{buildroot}%{_prefix}/lib/sumo
%__mkdir_p %{buildroot}%{_bindir}
%__ln_s ../lib/sumo/assign/duaIterate.py %{buildroot}%{_bindir}/duaIterate.py

%files
%defattr(-,root,root)
%{_bindir}/*
%{_prefix}/lib/sumo
%doc AUTHORS COPYING README ChangeLog docs/pydoc docs/userdoc docs/examples

%changelog
