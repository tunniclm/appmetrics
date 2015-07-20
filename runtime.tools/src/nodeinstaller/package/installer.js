var fs = require('fs');
var util = require('util');
var http = require('http');
var exec = require('child_process').exec;
var child_process = require('child_process');
var url = require('url');
var path = require('path');

var OS = process.platform; // e.g. linux
var ROOT_FOLDER = process.cwd().substring(0, process.cwd().indexOf('node_modules'));
var INSTALLER_FILE = 'installer.js';
var LICENSE_FILE = 'license.txt';
var INSTALLER_DIR = path.join(ROOT_FOLDER, 'node_modules', 'healthcenter');
var WINSTALLER_DIR = path.join(ROOT_FOLDER, 'node_modules', 'healthcenter-installer');
var INSTALL_DIR = path.join(ROOT_FOLDER, 'node_modules', 'healthcenter');
var BASE_DOWNLOAD_URL = 'http://public.dhe.ibm.com/ibmdl/export/pub/software/websphere/runtimes/tools/healthcenter/agents/nodejs/healthcenter-npm';
var TARBALL_FILE_NAME = 'healthcenter-3.0.3.tgz';

var logFile = path.join(ROOT_FOLDER, 'node_modules', 'install.log');
var log_file = fs.createWriteStream(logFile, {flags : 'a'});

console.log = function(info) { //
  log_file.write(util.format(info) + '\n');
  process.stdout.write(util.format(info) + '\n');
};

var downloadPackage = function() {
	/*
	 * Check for unsupported operating systems and fail fast
	 */
	if (OS == 'darwin') {
		console.log('Mac OS is not a currently supported platform. Exiting');
		process.exit(1);
	}
	if (OS == 'sunos') {
		console.log('Smart OS is not a currently supported platform. Exiting');
		process.exit(1);
	}
	/*
	 * Gather system information
	 */
	var ARCH = process.arch; // e.g. ia32
	var ENDIANNESS = process.config.variables.node_byteorder; // e.g. 'little'
	var NODE_VERSION;

	if (process.version.indexOf('v0.10') === 0) {
		NODE_VERSION='0.10';
	}
	else if (process.version.indexOf('v0.12') === 0) {
		NODE_VERSION='0.12';
	}

	/* Taking a note of the folder where node_modules is by stripping down the cwd
	 * of anything that is past node_modules. */
	//var ROOT_FOLDER = process.cwd().substring(0, process.cwd().indexOf('node_modules'));

	/* Going up the path tree - from node_modules/healthcenter to the root */
	//process.chdir(ROOT_FOLDER);

	var platformFolder;
	if (ARCH === 'ppc64' && ENDIANNESS === 'little') {
	    platformFolder = 'linux-ppc64le';
	} else {
	    platformFolder = OS + '-' + ARCH;
	}

	var final_download_url = [BASE_DOWNLOAD_URL, NODE_VERSION, platformFolder, TARBALL_FILE_NAME].join('/');

	/* Downloading the healthcenter tarball */
	var file = fs.createWriteStream(path.join(ROOT_FOLDER, TARBALL_FILE_NAME));

	/* Legal warning */
	console.log('********************************************************************************');
	console.log('You are installing the Health Center monitoring and profiling module.');
	console.log('This package includes the IBM Monitoring and Diagnostic Tools - Health Center ');
	console.log('monitoring agent for Node.js, which is automatically downloaded as the module is');
	console.log('installed on your system/device. This is released under a proprietary IBM');
	console.log('licence. The license agreement for IBM Monitoring and Diagnostic Tools - Health');
	console.log('Center is available in the following location:');
	console.log('node_modules/healthcenter/licenses');
	console.log('Your use of the components of the package and dependencies constitutes your ');
	console.log('acceptance of this license agreement. If you do not accept the terms of the ');
	console.log('license agreement(s), delete the relevant component(s) immediately from your ');
	console.log('device.');
	console.log('********************************************************************************');

	var req = http.get(final_download_url, function(response) {
		console.log('Downloading binaries from ' + final_download_url + ' to ' + path.join(ROOT_FOLDER, TARBALL_FILE_NAME));

		if (response.statusCode != 200) {
			console.log('ERROR: Unable to download ' + TARBALL_FILE_NAME + ' from ' + final_download_url);
			process.exit(1);
		}

	    response.pipe(file);

	    file.on('finish', function() {
	        console.log('Download finished.');
	        file.close();
	        installPackage();
	    });
	}).on('error', function(e) {
	    console.log('Got an error: ' + e.message);
	});	
};


/*
 * Check if we're on Windows. If so, we need to work around overwriting the healthcenter
 * install package with the real healthcenter platform package.
 * 
 * This consists of:
 * 1) Creating a healthcenter-installer directory
 * 2) Copying installer.js and the license files to it
 * 3) Re-launching installer.js from the new location using an async process
 */
var winstallPackage = function() {
	console.log('Installing for Windows. This is done asynchronously so errors may not be reported');
	console.log('Setting up healthcenter-installer directory');
	if (!fs.existsSync(WINSTALLER_DIR)) {
		fs.mkdirSync(WINSTALLER_DIR);
	};
	console.log('Copying installer.js');
	fs.writeFileSync(path.join(WINSTALLER_DIR, INSTALLER_FILE), fs.readFileSync(path.join(INSTALL_DIR, INSTALLER_FILE)));
	console.log('Copying license files');
	if (!fs.existsSync(path.join(WINSTALLER_DIR, 'licenses'))) {
		fs.mkdirSync(path.join(WINSTALLER_DIR, 'licenses'));
	};
	copyLicenses(path.join(INSTALL_DIR, 'licenses'), path.join(WINSTALLER_DIR, 'licenses'));
	
	var spawn = require('child_process').spawn;
	var installerPath = path.join('node_modules', 'healthcenter-installer', INSTALLER_FILE);
	console.log('Running ' + installerPath + ' from ' + ROOT_FOLDER);
	
	/*
	 * Flush out the installer log and wait till its flushed before starting the async task
	 */ 
	log_file.end();
	log_file.on('finish', function() {
		var child = spawn('cmd', ['/s', '/c', 'node ' + installerPath + ' winstall'], {cwd: ROOT_FOLDER, detached: true, stdio: [ 'ignore', 'ignore', 'ignore' ]});
		child.unref();
		process.exit(0);
	});
};


/* 
 * Install the platform specific healthcenter package using npm and copying over the 
 * licenses and the install log 
 */
var installPackage = function () {
	console.log('Installing platform specific healthcenter module...');
	/*
	 * If we're on windows we need to call this asynchronously to avoid file usage collisions. We
	 * do this by re-launching ourselves in an asynchronous child process (denoted by passing "winstall"
	 * as an argument).
	 */
	if (OS == 'win32'){
		INSTALLER_DIR = WINSTALLER_DIR;
		if (winstall == false) {
			winstallPackage();
			return;
		}
	}
	
	/*
	 * Load licenses into memory to write back after the npm install. We can't
	 * just copy them now as the npm install deletes the healthcenter directory
	 */
	var licenseFiles = loadLicenses(path.join(INSTALLER_DIR, 'licenses'));
	
	console.log('Launching npm for platform specific ' + TARBALL_FILE_NAME);
	var child = exec('npm install ' + path.join(ROOT_FOLDER, TARBALL_FILE_NAME) + ' -loglevel verbose', {cwd: ROOT_FOLDER}, function (error, stdout, stderr) {
		if (error !== null) {
			console.log('There seems to have been an error when trying to install : ' + error);
		}
		else if (stderr !== '') {
			console.log(stderr);
		}
		console.log('Installation complete.');
		
		/*
		 * Write the licenses to the licenses directory now that the install is complete
		 */
		var dir = path.join(INSTALL_DIR, 'licenses');
		if (!fs.existsSync(dir)){
			console.log('Creating License directory');
			fs.mkdirSync(dir);
		}
		writeLicenses(dir, licenseFiles);

		/*
		 * Copy the install log to the healthcenter directory, and clean up the temporary files
		 */
		console.log('Cleaning up and copying install log.');
		fs.writeFileSync(path.join(INSTALL_DIR, 'install.log'), fs.readFileSync(logFile, 'utf8'));
		fs.unlinkSync(logFile);
		log_file = fs.createWriteStream(path.join(INSTALL_DIR, 'install.log'), {flags : 'a'});

		fs.unlinkSync(path.join(ROOT_FOLDER, TARBALL_FILE_NAME));
		if (OS == 'win32') {
			console.log('Cleaning up healthcenter-installer directory');
			fs.unlinkSync(path.join(INSTALLER_DIR, INSTALLER_FILE));
			fs.rmdirSync(path.join(INSTALLER_DIR, 'licenses'));
			fs.rmdirSync(WINSTALLER_DIR);
		}
		console.log('Cleanup Complete');
		
		console.log('Cleaning up cache');
		var cacheClean = exec('npm cache clean healthcenter', {cwd: ROOT_FOLDER}, function (error, stdout, stderr) {
			if (error !== null) {
				console.log('Error trying to remove healthcenter module from npm cache');
			}
			else if (stderr !== '') {
				console.log(stderr);
			}
			console.log('Removal complete.');
		});	
	});	
};

var loadLicenses = function (fromDir) {
	var licenses = fs.readdirSync(fromDir);
	var licenseFiles = [];
	console.log('Reading licenses from ' + fromDir);
	licenses.forEach(function(license) {
		licenseFiles.push({filename: license, contents: fs.readFileSync(path.join(fromDir, license))});
		fs.unlinkSync(path.join(fromDir, license));
	});
	return licenseFiles;
};

var writeLicenses = function (toDir, licenseFiles) {
	console.log('Writing licences to ' + toDir);
	licenseFiles.forEach(function(license) {
		fs.writeFileSync(path.join(toDir, license.filename), license.contents);
	});
};

var copyLicenses = function (fromDir, toDir) {
	console.log('Copying licenses from ' + fromDir + ' to ' + toDir);
	var licenses = fs.readdirSync(fromDir);
	licenses.forEach(function(license) {
		fs.writeFileSync(path.join(toDir, license), fs.readFileSync(path.join(fromDir, license)));
	});
};

/*
 * Execute the install
 */
var winstall = (process.argv[2] == 'winstall') ? true : false;
if (winstall != true) {
	downloadPackage();
} else {
	installPackage();
}