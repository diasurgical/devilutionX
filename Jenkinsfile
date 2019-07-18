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

def buildStep(ext, hostFlags = '', sysRoot = true) {
	def fixed_job_name = env.JOB_NAME.replace('%2F','/')
	def sysRootEnv = ''
	try{
		stage("Building ${ext}...") {
			properties([pipelineTriggers([githubPush()])])
			if (env.CHANGE_ID) {
				echo 'Trying to build pull request'
			}
			
			def commondir = env.WORKSPACE + '/../' + fixed_job_name + '/'

			checkout scm

			if (!env.CHANGE_ID) {
				sh "rm -rfv ${env.WORKSPACE}/publishing/deploy/*"
				sh "mkdir -p ${env.WORKSPACE}/publishing/deploy/devilutionx"
			}

			slackSend color: "good", channel: "#jenkins", message: "Starting ${ext} build target..."

			freshUpRoot(ext)

			if (!env.CHANGE_ID) {
				sh "mkdir -p ${env.WORKSPACE}/publishing/deploy/devilutionx/${ext}/"
			}

			sh "cd ${env.WORKSPACE}/build-${ext} && cmake -DCMAKE_TOOLCHAIN_FILE=/opt/toolchains/cmake$ext .."
			
			sh "cd ${env.WORKSPACE}/build-${ext} && cmake --build . --config Release -- -j8"

			sh "cd ${env.WORKSPACE}/build-${ext} && mv -fv devilutionx ${env.WORKSPACE}/publishing/deploy/devilutionx/${ext}/"
			sh "cd ${env.WORKSPACE}/ && cp -fvr devilutionx.info ${env.WORKSPACE}/publishing/deploy/devilutionx/${ext}/"

			if (!env.CHANGE_ID) {
				sh "cd ${env.WORKSPACE}/publishing/deploy/devilutionx/${ext}/ && lha c ../devilutionx-${ext}.lha *"
				sh "rm -rfv ${env.WORKSPACE}/publishing/deploy/devilutionx/${ext}/*"
				sh "echo '${env.BUILD_NUMBER}|${env.BUILD_URL}' > ${env.WORKSPACE}/publishing/deploy/devilutionx/${ext}/BUILD"
				sh "mv -fv ${env.WORKSPACE}/publishing/deploy/devilutionx/devilutionx-${ext}.lha ${env.WORKSPACE}/publishing/deploy/devilutionx/${ext}/"
			}

			if (env.TAG_NAME) {
				sh "echo $TAG_NAME > ${env.WORKSPACE}/publishing/deploy/STABLE"
				sh "ssh $DEPLOYHOST mkdir -p public_html/downloads/releases/devilutionx/$TAG_NAME"
				sh "scp -r ${env.WORKSPACE}/publishing/deploy/devilutionx/* $DEPLOYHOST:~/public_html/downloads/releases/devilutionx/$TAG_NAME/"
				sh "scp ${env.WORKSPACE}/publishing/deploy/STABLE $DEPLOYHOST:~/public_html/downloads/releases/devilutionx/"
			} else if (env.BRANCH_NAME.equals('master')) {
				def deploy_url = sh (
				    script: 'echo "nightly/devilutionx/`date +\'%Y\'`/`date +\'%m\'`/`date +\'%d\'`/"',
				    returnStdout: true
				).trim()
				sh "date +'%Y-%m-%d %H:%M:%S' > ${env.WORKSPACE}/publishing/deploy/BUILDTIME"
				sh "ssh $DEPLOYHOST mkdir -p public_html/downloads/nightly/devilutionx/`date +'%Y'`/`date +'%m'`/`date +'%d'`/"
				sh "scp -r ${env.WORKSPACE}/publishing/deploy/devilutionx/* $DEPLOYHOST:~/public_html/downloads/nightly/devilutionx/`date +'%Y'`/`date +'%m'`/`date +'%d'`/"
				sh "scp ${env.WORKSPACE}/publishing/deploy/BUILDTIME $DEPLOYHOST:~/public_html/downloads/nightly/devilutionx/"

				slackSend color: "good", channel: "#jenkins", message: "Deploying ${fixed_job_name} #${env.BUILD_NUMBER} Target: ${ext} to web (<https://dl.amigadev.com/${deploy_url}|https://dl.amigadev.com/${deploy_url}>)"
			} else {
				slackSend color: "good", channel: "#jenkins", message: "Build ${fixed_job_name} #${env.BUILD_NUMBER} Target: ${ext} successful!"
			}
		}
	} catch(err) {
		slackSend color: "danger", channel: "#jenkins", message: "Build Failed: ${fixed_job_name} #${env.BUILD_NUMBER} Target: ${ext} (<${env.BUILD_URL}|Open>)"
		currentBuild.result = 'FAILURE'
		notify('Build failed')
		throw err
	}
}

def freshUpRoot(ext) {
	def commondir = env.WORKSPACE + '/../' + env.JOB_NAME.replace('%2F','/') + '/'
	sh "rm -rfv ${env.WORKSPACE}/build-${ext}"
	
	sh "mkdir -p ${env.WORKSPACE}/build-${ext}"

}

def postCoreBuild(ext) {

}

node('master') {
	killall_jobs()
	def fixed_job_name = env.JOB_NAME.replace('%2F','/')
	slackSend color: "good", channel: "#jenkins", message: "Build Started: ${fixed_job_name} #${env.BUILD_NUMBER} (<${env.BUILD_URL}|Open>)"
	parallel (
		'Build AmigaOS3.x version': {
			node {			
				buildStep('68k')
			}
		},
		'Build AmigaOS4.x version': {
			node {			
				buildStep('os4')
			}
		},
		'Build MorphOS version': {
			node {			
				buildStep('mos')
			}
		},
		'Build WarpOS version': {
			node {
				buildStep('wos')
			}
		}
	)
}
