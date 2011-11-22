Name: sp-mem-throughput
Version: 0.3.0
Release: 1%{?dist}
Summary: Memory throughput testing tool
Group: Development/Tools
License: GPLv2+
URL: http://www.gitorious.org/+maemo-tools-developers/maemo-tools/sp-mem-throughput
Source: %{name}_%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build
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
%defattr(-,root,root,-)
%{_bindir}/sp-mem-throughput
%{_mandir}/man1/sp-mem-throughput.1.gz
%doc COPYING 


%changelog
* Fri Jun 10 2011 Eero Tamminen <eero.tamminen@nokia.com> 0.3
  * Change in reported numbers for memcpy() and strcpy(): take both read and
    written bytes in account when reporting throughput.
  * New routine categories: strcmp, strncmp, strchr, strncpy
  * Include ARM strcmp() and strcpy() from bionic.git for benchmarking
    purposes.
  *  - memory throughput test should be extended to have
    str-specific tests

* Tue Oct 19 2010 Eero Tamminen <eero.tamminen@nokia.com> 0.2
  * Include memset() routine from the cortex-strings / Linaro project.
  * Extend manual page: explain the console output.
  * Fix executable stack.

* Fri Oct 01 2010 Eero Tamminen <eero.tamminen@nokia.com> 0.1
  * Initial version of sp-mem-throughput released. 
