#
# spec file for package sumo (Version 0.9.9)
#
# Copyright (c) 2008.
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.


Name:           sumo
Summary:        Microscopic Traffic Simulation
Version:        0.9.9
Release:        1
Url:            http://sumo.sourceforge.net/
Source0:        %{name}-%{version}.tar.bz2
License:        GPL v2.1 or later
Group:          Productivity/Scientific/Other
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
"Simulation of Urban MObility" (SUMO) is an open source,
highly portable, microscopic road traffic simulation package
designed to handle large road networks.

%prep
%setup -q

%build
%configure  --prefix=%{_prefix}
%{__make}

%install
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(0755,root,root)
%{_bindir}/*
