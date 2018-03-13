Name:           nxs-fw-libs-$$EDITION$$
Version:        $$VERSION$$
Release:        $$BUILD_VERSION$$%{?dist}
Summary:        Nixys Framework Libraries

Group:          Development/Tools
License:        GPL3
URL:            https://nixys.ru
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  make gcc rpm-build rpmdevtools libevent-nxs openssl-devel mariadb-devel ncurses-devel libxml2-devel jansson-devel libcurl-devel openssl-devel
Requires:       make gcc rpm-build rpmdevtools libevent-nxs openssl-devel mariadb-devel ncurses-devel libxml2-devel jansson-devel libcurl-devel openssl-devel


%description
Nixys Framework libraries, headers and config file for nxs-fw-ctl


%prep
%setup -q


%build
make


%install
$$HEADERS_DIR$$
$$DESC$$
$$HEADERS$$
$$STATIC_LIBS$$

%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
$$FILES$$

%changelog
