#
# spec file for package sumo
#
# norootforbuild
Name:           sumo
Summary:        Microscopic Traffic Simulation
Version:        svn
Release:        1
Url:            http://sumo.sourceforge.net/
Source0:        %{name}-src-%{version}.tar.gz
Source1:        %{name}-doc-%{version}.zip
License:        GPL v2.1 or later
Group:          Productivity/Scientific/Other
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
BuildRequires:  gcc-c++ libproj-devel libgdal-devel fox16-devel unzip
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
%if 0%{?fedora_version} >= 15
BuildRequires:  hdf5 javamail
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
%doc AUTHORS COPYING README ChangeLog docs/pydoc docs/userdoc docs/tutorial

%changelog
