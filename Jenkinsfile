def notify(status){
	emailext (
		body: '$DEFAULT_CONTENT', 
		recipientProviders: [
			[$class: 'CulpritsRecipientProvider'],
			[$class: 'DevelopersRecipientProvider'],
			[$class: 'RequesterRecipientProvider']
		], 
		replyTo: '$DEFAULT_REPLYTO', 
		subject: '$DEFAULT_SUBJECT',
		to: '$DEFAULT_RECIPIENTS'
	)
}

@NonCPS
def killall_jobs() {
	def jobname = env.JOB_NAME
	def buildnum = env.BUILD_NUMBER.toInteger()
	def killnums = ""
	def job = Jenkins.instance.getItemByFullName(jobname)
	def fixed_job_name = env.JOB_NAME.replace('%2F','/')

	for (build in job.builds) {
		if (!build.isBuilding()) { continue; }
		if (buildnum == build.getNumber().toInteger()) { continue; println "equals" }
		if (buildnum < build.getNumber().toInteger()) { continue; println "newer" }
		
		echo "Kill task = ${build}"
		
		killnums += "#" + build.getNumber().toInteger() + ", "
		
		build.doStop();
	}
	
	if (killnums != "") {
		slackSend color: "danger", channel: "#jenkins", message: "Killing task(s) ${fixed_job_name} ${killnums} in favor of #${buildnum}, ignore following failed builds for ${killnums}"
	}
	echo "Done killing"
}


def get_libs() {
	echo "============= Getting Libs ============="

	sh "curl -O https://www.zlib.net/zlib-1.2.11.tar.gz"
	sh "curl -O https://www.libsdl.org/release/SDL2-2.0.9.tar.gz"
	sh "curl -O https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.4.tar.gz"
	sh "curl -O https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.15.tar.gz"
	sh "curl -SLO https://download.savannah.gnu.org/releases/freetype/freetype-2.10.1.tar.gz"
	sh "curl -SLO https://github.com/glennrp/libpng/archive/v1.6.36.tar.gz"
	sh "curl -SLO https://github.com/jedisct1/libsodium/archive/1.0.17.tar.gz"
	sh "wget https://raw.githubusercontent.com/Kitware/CMake/v3.10.0/Modules/FindFreetype.cmake -O CMake/FindFreetype.cmake"
	sh "wget https://raw.githubusercontent.com/Kitware/CMake/v3.10.0/Modules/SelectLibraryConfigurations.cmake -O CMake/SelectLibraryConfigurations.cmake"
	sh "wget https://raw.githubusercontent.com/Kitware/CMake/master/Modules/FindZLIB.cmake -O CMake/FindZLIB.cmake"
}

def decompress_libs() {
	echo "============= Unzip Libs ============="

	sh "tar -xvf zlib-1.2.11.tar.gz"
	sh "tar -xvf SDL2-2.0.9.tar.gz"
	sh "tar -xvf SDL2_mixer-2.0.4.tar.gz"
	sh "tar -xvf SDL2_ttf-2.0.15.tar.gz"
	sh "tar -xvf v1.6.36.tar.gz"
	sh "tar -xvf freetype-2.10.1.tar.gz"
	sh "tar -xvf 1.0.17.tar.gz"
}

def build_zlib(TARGET, SYSROOT) {
	echo "============= Build ZLIB ============="

	sh "mkdir -p zlib-1.2.11/build"
	sh "sudo rm -rfv zlib-1.2.11/build/*"

	sh "cd zlib-1.2.11/build && cmake .. -DCMAKE_INSTALL_PREFIX=${SYSROOT}"
	sh "cd zlib-1.2.11/build && cmake --build . --config Release --target install -- -j8"
}

def build_sdl2(TARGET, SYSROOT) {
	echo "============= Build SDL2 ============="

	sh "cd SDL2-2.0.9/ && ./autogen.sh"
	sh "cd SDL2-2.0.9/ && ./configure --host=${TARGET} --enable-sdl2-config --prefix=${SYSROOT}"
	sh "cd SDL2-2.0.9/ && make clean"
	sh "cd SDL2-2.0.9/ && make -j8"
	sh "cd SDL2-2.0.9/ && make install"
}

def build_sdl2_mixer(TARGET, SYSROOT) {
	echo "============= Build SDL2_mixer ============="

	sh "cd SDL2_mixer-2.0.4/ && ./autogen.sh"
	sh "cd SDL2_mixer-2.0.4/ && ./configure --host=${TARGET} --prefix=${SYSROOT}"
	sh "cd SDL2_mixer-2.0.4/ && make clean"
	sh "cd SDL2_mixer-2.0.4/ && make -j8"
	sh "cd SDL2_mixer-2.0.4/ && make install"
}

def build_libpng(TARGET, SYSROOT) {
	echo "============= Build libpng ============="

	dir("libpng-1.6.36") {
		sh "mkdir -p build"
		sh "sudo rm -rfv build/*"

		sh "cd build && cmake .. -DCMAKE_INSTALL_LIBDIR=${SYSROOT}/lib -DCMAKE_INSTALL_INCLUDEDIR=${SYSROOT}/include -DCMAKE_INSTALL_PREFIX=${SYSROOT}"
		sh "cd build && cmake --build . --config Release --target install -- -j8"
	}
}

def build_freetype(TARGET, SYSROOT) {
	echo "============= Build Freetype ============="

	dir("freetype-2.10.1") {
		sh "mkdir -p build"
		sh "sudo rm -rfv build/*"

		sh "cd build/ && cmake .. -DCMAKE_INSTALL_PREFIX=${SYSROOT} -DUNIX=1" // -DCMAKE_INSTALL_LIBDIR=${SYSROOT}/lib -DCMAKE_INSTALL_INCLUDEDIR=${SYSROOT}/include
		sh "cd build/ && cmake --build . --config Release --target install -- -j8"
	}
}

def build_sdl2_ttf(TARGET, SYSROOT) {
	echo "============= Build SDL2_ttf ============="

	def ZLIB_FILE = ""
	if (SYSROOT.contains('mingw')) {
		ZLIB_FILE = "zlibstatic"
	}
	else {
		ZLIB_FILE = "z"
	}

	dir("SDL2_ttf-2.0.15") {
		sh "./autogen.sh"
		sh "FT2_CFLAGS=\"-I${SYSROOT}/include/freetype2\" FT2_LIBS=\"-lfreetype -lpng -l${ZLIB_FILE}\" ./configure --disable-shared --enable-static --host=${TARGET} --prefix=${SYSROOT}" //FT2_CONFIG=${SYSROOT}/include/freetype2/freetype/config/ftconfig.h
		sh "make clean"
		sh "make -j8"
		sh "make install"
	}
}

def build_libsodium(TARGET, SYSROOT) {
	echo "============= Build Libsodium ============="

	dir("libsodium-1.0.17") {
		sh "./autogen.sh"
		sh "./configure --host=${TARGET} --prefix=${SYSROOT}"
		sh "make clean"
		sh "make -j8"
		sh "make install"
	}
}


def buildStep(dockerImage, generator, os, defines) {
	def split_job_name = env.JOB_NAME.split(/\/{1}/)  
	def fixed_job_name = split_job_name[1].replace('%2F',' ')
	def fixed_os = os.replace(' ','-')
	try{
		stage("Building on \"${dockerImage}\" with \"${generator}\" for \"${os}\"...") {
			properties([pipelineTriggers([githubPush()])])
			def commondir = env.WORKSPACE + '/../' + fixed_job_name + '/'

			docker.image("${dockerImage}").inside("-u 0:0 -e BUILDER_UID=1001 -e BUILDER_GID=1001 -e BUILDER_USER=gserver -e BUILDER_GROUP=gserver") {

				sh "sudo apt update"
				sh "sudo apt install -y gcc-multilib curl automake autoconf libtool unzip"
				
				checkout scm

				if (env.CHANGE_ID) {
					echo 'Trying to build pull request'
				}

				if (!env.CHANGE_ID) {
					sh "rm -rfv publishing/deploy/*"
					sh "mkdir -p publishing/deploy/devilutionx"
				}

				def TARGET = sh (
					script: '$CC -dumpmachine',
					returnStdout: true
				).trim()

				def SYSROOT

				if (os.contains('Web')) {
					SYSROOT = "/emsdk_portable/sdk/system"
				}
				else {
					SYSROOT = sh (
						script: '$CC -print-sysroot',
						returnStdout: true
					).trim()
				}
				
				get_libs()
				decompress_libs()
				build_zlib(TARGET, SYSROOT)
				build_sdl2(TARGET, SYSROOT)
				build_sdl2_mixer(TARGET, SYSROOT)
				build_libpng(TARGET, SYSROOT)
				build_freetype(TARGET, SYSROOT)
				build_sdl2_ttf(TARGET, SYSROOT)
				build_libsodium(TARGET, SYSROOT)

				sh "mkdir -p build/"
				sh "mkdir -p lib/"
				sh "sudo rm -rfv build/*"

				slackSend color: "good", channel: "#jenkins", message: "Starting ${os} build target..."
				dir("build") {
					sh "cmake -G\"${generator}\" ${defines} -DVER_EXTRA=\"-${fixed_os}-${fixed_job_name}\" .."
					sh "VERBOSE=1 cmake --build . --config Release -- -j 8"

					if (os.contains('Windows')) {
						sh "mv devilutionx.exe devilutionx-${fixed_os}-${fixed_job_name}.exe"
						archiveArtifacts artifacts: 'devilutionx-${fixed_os}-${fixed_job_name}.exe'
					} else {
						sh "mv devilutionx devilutionx-${fixed_os}-${fixed_job_name}"
						archiveArtifacts artifacts: 'devilutionx-${fixed_os}-${fixed_job_name}'
					}

					//sh "cmake --build . --config Release --target package -- -j 8"
					//archiveArtifacts artifacts: '*.zip,*.tar.gz,*.tgz'
				}
				
				slackSend color: "good", channel: "#jenkins", message: "Build ${fixed_job_name} #${env.BUILD_NUMBER} Target: ${os} DockerImage: ${dockerImage} Generator: ${generator} successful!"
			}
		}
	} catch(err) {
		slackSend color: "danger", channel: "#jenkins", message: "Build Failed: ${fixed_job_name} #${env.BUILD_NUMBER} Target: ${os} DockerImage: ${dockerImage} Generator: ${generator} (<${env.BUILD_URL}|Open>)"
		currentBuild.result = 'FAILURE'
		notify('Build failed')
		throw err
	}
}

node('master') {
	killall_jobs()
	def fixed_job_name = env.JOB_NAME.replace('%2F','/')
	slackSend color: "good", channel: "#jenkins", message: "Build Started: ${fixed_job_name} #${env.BUILD_NUMBER} (<${env.BUILD_URL}|Open>)"
	parallel (
		'Win32': {
			node {			
				buildStep('dockcross/windows-static-x86:latest', 'Unix Makefiles', 'Windows x86', '')
			}
		},
		'Win64': {
			node {			
				buildStep('dockcross/windows-static-x64:latest', 'Unix Makefiles', 'Windows x86_64', '')
			}
		},
		'Linux x86': {
			node {			
				buildStep('desertbit/crossbuild:linux-x86', 'Unix Makefiles', 'Linux x86', '')
			}
		},
		'Linux x86_64': {
			node {			
				buildStep('desertbit/crossbuild:linux-x86_64', 'Unix Makefiles', 'Linux x86_64', '')
			}
		},
		'Linux ARMv7': {
			node {
				buildStep('desertbit/crossbuild:linux-armv7', 'Unix Makefiles', 'Linux RasPi', '')
			}
		},
		'WebASM': {
			node {			
				buildStep('dockcross/web-wasm:latest', 'Unix Makefiles', 'Web assembly', '')
			}
		}
    )
}