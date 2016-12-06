Name:       iotcon
Summary:    Tizen IoT Connectivity
Version:    0.3.4
Release:    0
Group:      Network & Connectivity/Service
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1001: %{name}.manifest
Source1002: %{name}-old.manifest
Source1003: %{name}-test.manifest
Source1004: %{name}-test-old.manifest
Source1005: %{name}-network-get
Source1006: %{name}-internet
BuildRequires:  cmake
BuildRequires:  boost-devel
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(capi-system-info)
BuildRequires:  pkgconfig(capi-system-system-settings)
BuildRequires:  pkgconfig(capi-network-wifi)
BuildRequires:  pkgconfig(iotivity)
BuildRequires:  pkgconfig(uuid)
BuildRequires:  pkgconfig(cynara-client)

%if "%{tizen}" == "2.3"
BuildRequires:  python-xml
%endif
%if 0%{?tizen_version_major} >= 3
Requires(post): /usr/bin/chgrp, /usr/bin/chmod, /usr/bin/chsmack
%endif
Requires(post): /sbin/ldconfig, /usr/bin/systemctl
Requires(postun): /sbin/ldconfig, /usr/bin/systemctl

%define _unitdir /usr/lib/systemd/system
%define _dbus_interface org.tizen.iotcon.dbus

%description
Tizen IoT Connectivity Service & Library(Client) based on Iotivity


%package devel
Summary:    TizenIoT Connectivity(devel)
Group:      Network & Connectivity/Development
Requires:   %{name} = %{version}

%description devel
IoT Connectivity Manager development Kit


%package test
Summary:    Tizen IoT Connectivity(test)
Group:      Network & Connectivity/Testing
Requires:   %{name} = %{version}

%description test
Tizen IoT Connectivity Test Programs


%prep
%setup -q
chmod g-w %_sourcedir/*
%if 0%{?tizen_version_major} < 3
cp %{SOURCE1002} ./%{name}.manifest
cp %{SOURCE1004} ./%{name}-test.manifest
%else
cp %{SOURCE1001} ./%{name}.manifest
cp %{SOURCE1003} ./%{name}-test.manifest
cp %{SOURCE1005} ./%{name}-network-get
cp %{SOURCE1006} ./%{name}-internet
%endif


%build
%if 0%{?tizen_version_major} < 3
TZ_VER_3=0
%else
TZ_VER_3=1
%endif

# for aarch64, x86_64
%define BUILD_ARCH %{_arch}

%ifarch armv7l armv7hl armv7nhl armv7tnhl armv7thl
%define BUILD_ARCH "arm"
%endif

%ifarch %{ix86}
%define BUILD_ARCH "x86"
%endif

MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
%cmake . -DMAJORVER=${MAJORVER} -DFULLVER=%{version} -DBIN_INSTALL_DIR:PATH=%{_bindir} \
		-DTZ_VER_3=${TZ_VER_3} -DARCH=%{BUILD_ARCH}


%install
rm -rf %{buildroot}
%make_install

%if 0%{?tizen_version_major} < 3
mkdir -p %{buildroot}/%{_datadir}/license
cp LICENSE.APLv2 %{buildroot}/%{_datadir}/license/%{name}
cp LICENSE.APLv2 %{buildroot}/%{_datadir}/license/%{name}-test
cp LICENSE.APLv2 %{buildroot}/%{_datadir}/license/%{name}-devel
%else
mkdir -p %{buildroot}/usr/share/%{name}
cp %{name}-network-get %{buildroot}/usr/share/%{name}/%{name}-network-get
cp %{name}-internet %{buildroot}/usr/share/%{name}/%{name}-internet
%endif

%post
%if 0%{?tizen_version_major} >= 3
chgrp priv_internet /usr/share/%{name}/%{name}-internet
chmod g+r /usr/share/%{name}/%{name}-internet
chmod o= /usr/share/%{name}/%{name}-internet
chsmack -a "*" /usr/share/%{name}/%{name}-internet

chgrp priv_network_get /usr/share/%{name}/%{name}-network-get
chmod g+r /usr/share/%{name}/%{name}-network-get
chmod o= /usr/share/%{name}/%{name}-network-get
chsmack -a "*" /usr/share/%{name}/%{name}-network-get
%endif
/sbin/ldconfig

%postun
/sbin/ldconfig


%files
%manifest %{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/lib%{name}.so.*
%if 0%{?tizen_version_major} < 3
%{_datadir}/license/%{name}
%else
%license LICENSE.APLv2
/usr/share/%{name}/%{name}-network-get
/usr/share/%{name}/%{name}-internet
%endif

%files devel
%defattr(-,root,root,-)
%if 0%{?tizen_version_major} < 3
%{_datadir}/license/%{name}
%else
%license LICENSE.APLv2
%endif
%{_libdir}/lib%{name}.so
%{_libdir}/pkgconfig/%{name}.pc
%{_includedir}/%{name}/*.h

%files test
%manifest %{name}-test.manifest
%defattr(-,root,root,-)
%{_bindir}/iotcon-test-*
%if 0%{?tizen_version_major} < 3
%{_datadir}/license/%{name}-test
%else
%license LICENSE.APLv2
%endif
