#
# spec file for package sumo (Version 0.9.10)
#
# norootforbuild
Name:           sumo
Summary:        Microscopic Traffic Simulation
Version:        0.9.10
Release:        1
Url:            http://sumo.sourceforge.net/
Source0:        %{name}-%{version}.tar.bz2
License:        GPL v2.1 or later
Group:          Productivity/Scientific/Other
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
BuildRequires: gcc-c++ libproj-devel libgdal-devel Xerces-c-devel fox14-devel
Requires:      libproj libgdal Xerces-c fox14

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
