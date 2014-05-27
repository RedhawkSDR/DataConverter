# By default, the RPM will install to the standard REDHAWK SDR root location (/var/redhawk/sdr)
# You can override this at install time using --prefix /new/sdr/root when invoking rpm (preferred method, if you must)
%bcond_with intel
%define _sdrroot /var/redhawk/sdr
%define _prefix %{_sdrroot}
Prefix: %{_prefix}

# Point install paths to locations within our target SDR root
%define _sysconfdir    %{_prefix}/etc
%define _localstatedir %{_prefix}/var
%define _mandir        %{_prefix}/man
%define _infodir       %{_prefix}/info
Name: DataConverter
Summary: Component %{name}
Version: 3.0.0
Release: 1
License: None
Group: REDHAWK/Components
Source: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-root

Requires: redhawk >= 1.10
BuildRequires: redhawk-devel >= 1.10
BuildRequires: autoconf automake libtool

# Interface requirements
Requires: bulkioInterfaces
BuildRequires: bulkioInterfaces

# C++ requirements
Requires: fftw >= 3.2.0
BuildRequires: fftw-devel >= 3.2.0

%if %{with intel}
Requires: compat-libstdc++-33
BuildRequires: compat-libstdc++-33
%endif

%description
Component %{name}

%prep
%setup

%build
# Implementation cpp
pushd cpp
./reconf
%define _bindir %{_prefix}/dom/components/DataConverter/cpp
%configure %{?_with_intel} --with-intelopts
make
popd

%install
rm -rf $RPM_BUILD_ROOT
# Implementation cpp
pushd cpp
%define _bindir %{_prefix}/dom/components/DataConverter/cpp 
make install DESTDIR=$RPM_BUILD_ROOT
popd

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,redhawk,redhawk)
%dir %{_prefix}/dom/components/%{name}
%{_prefix}/dom/components/%{name}/DataConverter.spd.xml
%{_prefix}/dom/components/%{name}/DataConverter.prf.xml
%{_prefix}/dom/components/%{name}/DataConverter.scd.xml
%{_prefix}/dom/components/%{name}/cpp
