/*******************************************************************************
 * Copyright 2015 IBM Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************/

var fs = require('fs');
var util = require('util');
var http = require('http');
var url = require('url');
var path = require('path');

var OS = process.platform; // e.g. linux
var ARCH = process.arch; // e.g. ia32
var ENDIANNESS = process.config.variables.node_byteorder; // e.g. 'little'
var INSTALL_DIR = process.cwd();
var PLUGINS_DIR = path.join(INSTALL_DIR, 'plugins');
var BASE_DOWNLOAD_URL = 'http://public.dhe.ibm.com/ibmdl/export/pub/software/websphere/runtimes/tools/healthcenter/agents/nodejs/binaries';
var HC_VERSION = "1.0.0";
var BINARY_FILE = 'appmetrics.node';
var PLUGIN_NAMES = [ 'apiplugin',
                     'cpuplugin',
                     'envplugin',
                     'hcmqtt',
                     'memoryplugin' ];

var LOG_FILE = path.join(INSTALL_DIR, 'install.log');
var logFileStream = fs.createWriteStream(LOG_FILE, {flags : 'a'});

console.log = function(info) { //
	logFileStream.write(util.format(info) + '\n');
	process.stdout.write(util.format(info) + '\n');
};

var showLegalWarning = function() {
	/* Legal warning */
	console.log(new Date().toUTCString());
	console.log('********************************************************************************');
	console.log('You are installing the Node Application Metrics monitoring and profiling module.');
	console.log('This package includes the IBM Monitoring and Diagnostic Tools - Health Center ');
	console.log('monitoring agent for Node.js, which is automatically downloaded as the module is');
	console.log('installed on your system/device. This is released under a proprietary IBM');
	console.log('licence. The license agreement for IBM Monitoring and Diagnostic Tools - Health');
	console.log('Center is available in the following location:');
	console.log('node_modules/appmetrics/licenses');
	console.log('Your use of the components of the package and dependencies constitutes your ');
	console.log('acceptance of this license agreement. If you do not accept the terms of the ');
	console.log('license agreement(s), delete the relevant component(s) immediately from your ');
	console.log('device.');
	console.log('********************************************************************************');
};

var ensureSupportedOSOrExit = function() {
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
};

var getLibraryFileName = function(name) {
	if (OS == 'win32') {
		return name + '.dll';
	}
	if (OS == 'aix') {
		return 'lib' + name + '.a';
	}
	return 'lib' + name + '.so';
};

var getPlatformDir = function() {
	var platformDir;
	if (ARCH === 'ppc64' && ENDIANNESS === 'little') {
		platformDir = 'linux-ppc64le';
	} else {
		platformDir = OS + '-' + ARCH;
	}
	return platformDir;
};

var getSupportedNodeVersionOrExit = function() {
	if (process.version.indexOf('v0.10') === 0) {
		return '0.10';
	}
	if (process.version.indexOf('v0.12') === 0) {
		return '0.12';
	}
	console.log('Unsupported version ' + process.version + '. Exiting.');
	process.exit(1);
};

var getPlatformVersionDownloadURL = function() {
	return [BASE_DOWNLOAD_URL, getSupportedNodeVersionOrExit(), getPlatformDir(), HC_VERSION].join('/');
};

var downloadBinary = function(filename, sourcePathURL, destDir) {
	var downloadURL = [sourcePathURL, filename].join('/');

	/* Downloading the binaries */
	var file = fs.createWriteStream(path.join(destDir, filename));

	var req = http.get(downloadURL, function(response) {
		console.log('Downloading binary from ' + downloadURL + ' to ' + path.join(destDir, filename));

		if (response.statusCode != 200) {
			console.log('ERROR: Unable to download ' + filename + ' from ' + downloadURL);
			process.exit(1);
		}

		response.pipe(file);

		file.on('finish', function() {
			console.log('Download of ' + filename + ' finished.');
			file.close();
		});
	}).on('error', function(e) {
		console.log('Got an error: ' + e.message);
		process.exit(1);
	});	
};


/*
 * Start the download
 */
showLegalWarning();
ensureSupportedOSOrExit();
fs.mkdir(PLUGINS_DIR, function(err) { 
	// ignore err creating directory (eg if it already exists)
	downloadBinary(BINARY_FILE, getPlatformVersionDownloadURL(), INSTALL_DIR);
	for (var i=0; i < PLUGIN_NAMES.length; i++) {
		downloadBinary(getLibraryFileName(PLUGIN_NAMES[i]), 
		               getPlatformVersionDownloadURL() + '/plugins',
		               PLUGINS_DIR);
	}
});
