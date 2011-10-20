Name: sp-mem-throughput
Version: 0.3.0
Release: 1%{?dist}
Summary: Memory throughput testing tool
Group: Development/Tools
License: GPLv2+
URL: http://www.gitorious.org/+maemo-tools-developers/maemo-tools/sp-mem-throughput
Source: %{name}_%{version}.tar.gz
BuildRoot: {_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires: python

%description
 This is a tool for benchmarking memory throughput by different access
 patterns, such as read only, write only (similar to the memset function from
 the C library), or copy (similar to memcpy from the C library). For each of
 these access patterns, various implementations can be benchmarked, including
 those found in the C library.
 .
 sp-mem-throughput has various parameters, that can be set to measure different
 kind of workloads, for example to measure the memory throughput when writing
 to very small or very large memory areas.
 .
 Results can be stored in CSV format for later analysis.
 
%prep
%setup -q -n %{name}

%build
make 

%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}

%clean
rm -rf %{buildroot}

%files
%defattr(755,root,root,-)
%{_bindir}/sp-mem-throughput
%defattr(644,root,root,-)
%{_mandir}/man1/sp-mem-throughput.1.gz
%doc COPYING 

