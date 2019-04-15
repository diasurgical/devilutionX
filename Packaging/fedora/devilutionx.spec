%define debug_package %{nil}

Name:		devilutionx
Version:	0.3.1
Release:	1%{?dist}
Summary:	Diablo I engine for modern operating systems


#Group:		
License:	Unlicensed
URL:		https://github.com/diasurgical/devilutionX
Source0:	https://github.com/diasurgical/devilutionX/archive/%{version}.tar.gz
Source1:	devilutionx.desktop

BuildRequires:	cmake gcc gcc-c++ libstdc++-static glibc desktop-file-utils
BuildRequires:  glibc-devel SDL2-devel SDL2_ttf-devel SDL2_mixer-devel libsodium-devel libasan
Requires:	SDL2_ttf SDL2_mixer libsodium

%description
Diablo devolved - magic behind the 1996 computer game
Note, Devilution requires an original copy of diabdat.mpq. None of the Diablo 1 game assets are provided by this package. 

%prep
%setup -q -n devilutionX-%{version}


%build
#%{__rm} makefile
cmake -DBINARY_RELEASE=ON -DDEBUG=OFF
make %{?_smp_mflags}

%install
make INSTALL_ROOT=%{buildroot}
#according to Fedora's Games Packaging guidelines the binary should go in %{_bindir}
#However, the problem is that the mpq files do actually not belong in this directory.
#That's why we gonna use /var/games/%{name} for the moment, but this needs to be addressed though.
#mkdir -p %{buildroot}%{_bindir}
#install -m 755 devilutionx %{buildroot}%{_bindir}

mkdir -p %{buildroot}/var/games/%{name}
install -m 755 devilutionx %{buildroot}/var/games/%{name}
desktop-file-install --remove-category="Qt" --dir=%{buildroot}%{_datadir}/applications %{SOURCE1} 


%files
/var/games/%{name}
/var/games/%{name}/%{name}
%{_datadir}/applications/%{name}.desktop


%changelog
* Mon Apr 15 2019 Michael Seevogel <michael (at) michaelseevogel.de> - 0.3.1-1
- Initial release for Fedora
