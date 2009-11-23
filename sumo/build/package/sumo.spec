#
# spec file for package sumo (Version svn)
#
# norootforbuild
Name:           sumo
Summary:        Microscopic Traffic Simulation
Version:        svn
Release:        1
Url:            http://sumo.sourceforge.net/
Source0:        %{name}-%{version}.tar.bz2
License:        GPL v2.1 or later
Group:          Productivity/Scientific/Other
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
BuildRequires:  gcc-c++ libproj-devel libgdal-devel fox16-devel
%if 0%{?suse_version} > 1110 || 0%{?centos_version} || 0%{?rhel_version}
BuildRequires: libxerces-c-devel
%else
BuildRequires: libXerces-c-devel
%endif
%if 0%{?mandriva_version}
BuildRequires:  XFree86-devel postgresql-devel libmesaglu1-devel
%else
BuildRequires:  xorg-x11-devel xorg-x11-Mesa-devel
%endif
%if 0%{?fedora_version} || 0%{?centos_version} || 0%{?rhel_version}
BuildRequires:  libGLU-devel libXext-devel libXft-devel
%endif
Autoreqprov: on

%description
"Simulation of Urban MObility" (SUMO) is an open source,
highly portable, microscopic road traffic simulation package
designed to handle large road networks.

%if 0%{?suse_version}
%debug_package
%endif
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
