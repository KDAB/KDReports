Name:           qt5-kdreports
Version:        2.2.0
Release:        1
Summary:        A Qt library for creating printable reports
Source:         %{name}-%{version}.tar.gz
Source1:        %{name}-rpmlintrc
Source2:        %{name}-%{version}.tar.gz.asc
URL:            https://github.com/KDAB/KDReports
Group:          System/Libraries
License:        MIT
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Vendor:         Klaralvdalens Datakonsult AB (KDAB)
Packager:       Klaralvdalens Datakonsult AB (KDAB) <info@kdab.com>

BuildRequires: cmake
%if %{defined suse_version}
BuildRequires:  libqt5-qtbase-devel
%endif

%if %{defined fedora}
BuildRequires:  gcc-c++ qt5-qtbase-devel desktop-file-utils
%endif

%if %{defined rhel}
BuildRequires:  gcc-c++ qt5-qtbase-devel desktop-file-utils
%endif

%description
KDAB's KD Reports is the Qt tool that lets you easily create printable reports
by providing all of the necessary features for a variety of applications.

Authors:
--------
      Klaralvdalens Datakonsult AB (KDAB) <info@kdab.com>

%define debug_package %{nil}
%global __debug_install_post %{nil}

%package devel
Summary:        Development files for %{name}
Group:          Development/Libraries/C and C++
Requires:       %{name} = %{version}

%description devel
This package contains header files and associated tools and libraries to
develop programs using kdreports.

%prep
%setup -q

%build
cmake . -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release
%__make %{?_smp_mflags}

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%install
%make_install

%clean
%__rm -rf "%{buildroot}"

%files
%defattr(-,root,root)
%{_prefix}/share/doc/KDReports
%{_libdir}/libkdreports.so.*

%files devel
%defattr(-,root,root,-)
%if 0%{?sle_version} >= 150200 && 0%{?is_opensuse}
%{_libdir}/qt5/mkspecs/modules/*
%endif
%if 0%{?suse_version} > 1500
%{_libdir}/qt5/mkspecs/modules/*
%endif
%if 0%{?fedora} > 28
%{_libdir}/qt5/mkspecs/modules/*
%endif
%if %{defined rhel}
%{_libdir}/qt5/mkspecs/modules/*
%endif
%dir %{_includedir}/KDReports
%{_includedir}/KDReports/*
%dir %{_libdir}/cmake/KDReports
%{_libdir}/cmake/KDReports/*
%{_libdir}/libkdreports.so

%changelog
* Sat Sep 03 2022 Allen Winter <allen.winter@kdab.com> 2.2.0
  2.2.0 final
* Tue Jan 11 2022 Allen Winter <allen.winter@kdab.com> 2.1.0
  2.1.0 final
* Sat Aug 21 2021 Allen Winter <allen.winter@kdab.com> 2.0.0
  2.0.0 final
* Wed Nov 04 2020 Allen Winter <allen.winter@kdab.com> 1.9.0
  1.9.0 final
* Thu Oct 15 2020 Allen Winter <allen.winter@kdab.com> 1.8.2
  1.8.2 final
* Sun Oct 11 2020 Allen Winter <allen.winter@kdab.com> 1.8.1
  1.8.1 final
* Mon Apr 02 2018 Allen Winter <allen.winter@kdab.com> 1.8.0
  1.8.0 final
* Thu Aug 11 2016 Allen Winter <allen.winter@kdab.com> 1.7.1
  1.7.1 final
