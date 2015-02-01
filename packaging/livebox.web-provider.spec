#git:framework/web/web-provider
Name: livebox.web-provider
Summary: web framework for livebox 
Version: 1.100_w4
Release: 1
Group: main/app
License: Flora License, Version 1.1
Source0: %{name}-%{version}.tar.gz
BuildRequires: attr
BuildRequires: cmake, gettext-tools
BuildRequires: libcap, libcap-devel
BuildRequires: pkgconfig(aul)
BuildRequires: pkgconfig(appcore-efl)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(bundle)
BuildRequires: pkgconfig(eina)
BuildRequires: pkgconfig(ecore)
BuildRequires: pkgconfig(ecore-x)
BuildRequires: pkgconfig(evas)
BuildRequires: pkgconfig(ecore-evas)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(efl-assist)
BuildRequires: pkgconfig(ewebkit2)
BuildRequires: pkgconfig(wrt-core)
BuildRequires: pkgconfig(xmlsec1)
BuildRequires: pkgconfig(dpl-efl)
BuildRequires: pkgconfig(provider)
BuildRequires: pkgconfig(pkgmgr-info)
BuildRequires: pkgconfig(dynamicbox_service)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(json-glib-1.0)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(libsmack)

Requires(post): attr

%description
This is web framework responsible to manage liveboxes that consist of web contents

%package devel
Summary: Files for web provider devel.
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}

%description devel
Web Provider library (dev)

%prep
%setup -q

%build

cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DCMAKE_PROJECT_VERSION=%{version}

#-fpie  LDFLAGS="${LDFLAGS} -pie -O3"
CXXFLAGS="${CXXFLAGS} -Wall -Winline -Werror -fno-builtin-malloc" make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/share/license
cp LICENSE.Flora %{buildroot}/usr/share/license/%{name}
%make_install
%define app_data /opt/usr/apps/livebox.web-provider/data
mkdir -p %{buildroot}%{app_data}

%post
killall -9 web-provider
/usr/bin/web_provider_reset_db.sh
echo "smack setting..."
chsmack -a 'livebox.web-provider::db' /opt/usr/dbspace/.web_provider.db
chsmack -a 'livebox.web-provider::db' /opt/usr/dbspace/.web_provider.db-journal
setfattr -n security.capability -v 0sAQAAAgABAAAAAAAAAgAAAAAAAAA= %{_prefix}/apps/livebox.web-provider/bin/web-provider
chown 5000:5000 %{app_data}
chmod 755 %{app_data}

%files -n livebox.web-provider
%manifest livebox.web-provider.manifest
%defattr(-,root,root,-)
%{_libdir}/*.so*
%{_libdir}/web-provider/*.so*
%{_libdir}/web-provider/*.json
%{_datadir}/web-provider/*
%attr(755,root,root) %{_bindir}/web_provider_reset_db.sh
%{_prefix}/apps/livebox.web-provider/bin/web-provider
%{_datarootdir}/packages/livebox.web-provider.xml
%{_prefix}/share/locale/*
%{_sysconfdir}/smack/accesses.d/livebox.web-provider.efl
%{app_data}
%{_datadir}/license/%{name}
%attr(700,root,root) /etc/opt/upgrade/*.patch.sh

%files devel
%defattr(-,root,root,-)
%{_includedir}/web-provider/*
%{_libdir}/pkgconfig/*.pc
