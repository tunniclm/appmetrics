/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

var util = require('util');
var EventEmitter = require('events').EventEmitter;

function API(agent) {
	this.agent = agent;
    this.environment = {};
    /*
     * We consider ourselves initialized when we have both OS and Runtime Env data
     * Decrement on when we get each, and raise 'initialized' event on 0;
     */
    this.initialized = 2;
    var that = this;

    var raiseEvent = function (topic, message) {
        if (topic === 'cpu_os' || topic === 'cpu') {
                formatCPU(message);
        } else if (topic === 'environment_os') {
                formatOSEnv(message);
        } else if (topic === 'environment_node') {
                formatRuntimeEnv(message);
        } else if (topic === 'memory_os' || topic === 'memory') {
                formatMemory(message);
        } else if (topic === 'gc_node') {
                formatGC(message);
        } else if (topic === 'profiling_node') {
		formatProfiling(message);
        } else if (topic === 'api') {
                formatApi(message);
        } else {
        /*
         * Just raise any unknown message as an event so someone can parse it themselves
         */
                that.emit(topic, message);
        }
    };

    var formatCPU = function(message) {
    	// cpu : startCPU@#1412609879696@#0.000499877@#0.137468
        var values = message.trim().split('@#'); // needs to be trimmed because of leading \n character
        var cpu = {time: parseInt(values[1]), process: parseFloat(values[2]), system: parseFloat(values[3])};
        that.emit('cpu', cpu);
    };

    var formatOSEnv = function(message) {
        /* environment_os : #EnvironmentSource
           environment.LESSOPEN=| /usr/bin/lesspipe %s
           environment.GNOME_KEYRING_PID=2668
           environment.USER=riclau
           os.arch=x86_64
           os.name=Linux
           os.version=3.5.0-54-generic#81~precise1-Ubuntu SMP Tue Jul 15 04:02:22 UTC 2014
           pid=4838
           native.library.date=Oct 20 2014 10:51:56
           number.of.processors=2
           command.line=/home/riclau/sandbox/node-v0.10.32-linux-x64/bin/node /home/riclau/sandbox/node-v0.10.32-linux-x64/lib/node_modules/healthcenter/launcher.js red.js
         */
		var values = message.split('\n');
    	var env = {};
        values.forEach(function (value) {
        	if (value[0] != '#'){
        		var terms = value.split('=');
        		env[terms[0]] = terms[1];
        	}
        });

        setEnv(env);
        that.initialized--;
        that.emit('environment', that.environment);
        if (that.initialized == 0) that.emit('initialized');
    };

    var setEnv = function(env) {
    	for (var p in env) {
            that.environment[p] = env[p];
        }
    };

    var formatRuntimeEnv = function(message) {
    	//environment_node : #EnvironmentSource
        //runtime.version=v0.10.32-IBMBuild-201410132030
        //runtime.vendor=IBM
        //runtime.name=IBM SDK for Node.js
        //command.line.arguments=
        //jar.version=3.0.0.20141020
        var values = message.trim().split('\n');
        var result = {};
        values.forEach(function (value) {
        	/* Checks for '=' sign on each line in order to ignore empty lines and "#EnvironmentSource" */
            if (value.indexOf('=') !== -1) {
            	var terms = value.split('=');
                result[terms[0]] = terms[1];
            }
        });
        setEnv(result);
        that.emit('environment', that.environment);
        that.initialized--;
        if (that.initialized == 0) that.emit('initialized');
    };

    var formatMemory = function(message) {
        /*
         * MemorySource,1415976582652,totalphysicalmemory=16725618688,physicalmemory=52428800,privatememory=374747136,virtualmemory=374747136,freephysicalmemory=1591525376
         */
        var values = message.split(/[,=]+/);
        var systemUsed = values[3] - values[11];
        var memory = {time: parseInt(values[1]), physical_total: parseInt(values[3]), physical_used: parseInt(systemUsed), private: parseInt(values[7]), virtual: parseInt(values[9]), physical_free: parseInt(values[11])};
        that.emit('memory', memory);
    };

    var formatGC = function(message) {
        /* gc_node : NodeGCData,1413903289280,S,48948480,13828320,7
         *                     , timestamp   ,M|S, size , used   , pause (ms)
         *
         * GC data can come in batches of multiple lines like the one in the example,
         * so first separate the lines, followed by the normal parsing.
         *
         */
        var lines = message.trim().split('\n');
		/* Split each line into the comma-separated values. */
	    lines.forEach(function (line) {
        	var values = line.split(/[,]+/);
            var gc = {time: parseInt(values[1]), type: values[2], size: parseInt(values[3]), used: parseInt(values[4]), duration: parseInt(values[5])};
            that.emit('gc', gc);
        });
    };

	var formatProfiling = function (message) {
		var lines = message.trim().split('\n');
		var prof = {
			date: 0,
			functions: [],
		};
		lines.forEach(function (line) {
			var values = line.split(',');
			if (values[1] == 'Node') {
				prof.functions.push({self: parseInt(values[2]), parent: parseInt(values[3]), file: values[4], name: values[5], line: parseInt(values[6]), count: parseInt(values[7])});
			} else if (values[1] == 'Start') {
				prof.time = parseInt(values[2]);
			}
		});
	 	that.emit('profiling', prof);	
	};

    var formatApi = function (message) {
    	var lines = message.trim().split('\n');
    	lines.forEach(function (line) {
    	    var parts = line.split(/:(.+)/);
            var topic = parts[0];
            var data = JSON.parse(parts[1]);
            that.emit(topic, data);	
    	});
    };

    agent.localConnect(function events(topic, data) {
       	message = data.toString();
       	raiseEvent(topic, message);
    });
//    agent.sendControlCommand("history", "");
}
module.exports.getAPI = function(agent) {
	return new API(agent);
};

util.inherits(API, EventEmitter);

API.prototype.enable = function (data) {
	var that = this;
    if (data == 'profiling') that.agent.sendControlCommand("profiling_node", "on,profiling_node_subsystem"); 
};

API.prototype.disable = function (data) {
	var that = this;
	if (data == 'profiling') that.agent.sendControlCommand("profiling_node", "off,profiling_node_subsystem");
};

API.prototype.getEnvironment = function() {
	var that = this;
	return that.environment;
};

