#
# spec file for package sumo
#
# Copyright (c) 2016 SUSE LINUX GmbH, Nuernberg, Germany.
# Copyright (c) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
Version:        svn
Release:        0
Summary:        Simulation of Urban Mobility - A Microscopic Traffic Simulation
License:        GPL-3.0+
Group:          Productivity/Scientific/Other
Url:            http://sumo.dlr.de/
Source0:        sumo-src-%{version}.tar.gz
Source1:        sumo-doc-%{version}.zip
Source2:        %{name}.desktop
Source3:        %{name}.png
Source4:        %{name}.xml
BuildRequires:  gcc-c++
BuildRequires:  help2man
BuildRequires:  pkgconfig
BuildRequires:  unzip
BuildRequires:  pkgconfig(fox)
BuildRequires:  pkgconfig(x11)
BuildRequires:  pkgconfig(xerces-c)
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
%if 0%{?fedora_version} || 0%{?suse_version}
BuildRequires:  fdupes
BuildRequires:  pkgconfig(gdal)
%endif
%if 0%{?fedora_version} || 0%{?centos_version} || 0%{?scientificlinux_version} || 0%{?suse_version}
BuildRequires:  pkgconfig(proj)
%endif
%if 0%{?fedora_version} || 0%{?centos_version} || 0%{?rhel_version} || 0%{?scientificlinux_version}
BuildRequires:  libGLU-devel
BuildRequires:  libjpeg-devel
BuildRequires:  libpng-devel
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

%prep
%setup -q
unzip -o %{SOURCE1} -d ..
mv docs/tutorial docs/examples
%if 0%{?sles_version}
find . -name "*.jar" | xargs rm
%endif

%build
%configure
make %{?_smp_mflags}
make %{?_smp_mflags} man

%install
%make_install
mkdir -p %{buildroot}%{_prefix}/lib/sumo
rm -rf tools/contributed/traci4j
cp -a tools data %{buildroot}%{_prefix}/lib/sumo
mkdir -p %{buildroot}%{_bindir}
ln -s ../../bin %{buildroot}%{_prefix}/lib/sumo
ln -s ../lib/sumo/tools/assign/duaIterate.py %{buildroot}%{_bindir}/duaIterate.py
ln -s ../lib/sumo/tools/osmWebWizard.py %{buildroot}%{_bindir}/osmWebWizard.py
ln -s ../lib/sumo/tools/randomTrips.py %{buildroot}%{_bindir}/randomTrips.py
ln -s ../lib/sumo/tools/traceExporter.py %{buildroot}%{_bindir}/traceExporter.py
install -d -m 755 %{buildroot}%{_mandir}/man1
install -p -m 644 docs/man/*.1 %{buildroot}%{_mandir}/man1
install -Dm644 %{SOURCE2} %{buildroot}%{_datadir}/applications/%{name}.desktop
install -Dm644 %{SOURCE3} %{buildroot}%{_datadir}/pixmaps/%{name}.png
%if 0%{?suse_version}
install -Dm644 %{SOURCE4} %{buildroot}%{_datadir}/mime/application/%{name}.xml
%fdupes -s docs
%fdupes %{buildroot}
%endif

%files
%defattr(-,root,root)
%{_bindir}/*
%{_prefix}/lib/sumo
%doc AUTHORS COPYING README ChangeLog docs/pydoc docs/userdoc docs/examples
%{_mandir}/man1/*
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/%{name}.png
%if 0%{?suse_version} > 1200
%{_datadir}/mime/application
%endif

%changelog
