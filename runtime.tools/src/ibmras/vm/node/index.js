/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

var path = require("path")
var module_dir = path.dirname(module.filename)
var os = require("os")

var agent = require("./healthcenter")
// Set the plugin search path
agent.spath(path.join(module_dir, "plugins"))
agent.start();

var hcAPI = require("./healthcenter-api.js");

// Export any functions exported by the agent
for (var prop in agent) {
    if (typeof agent[prop] == "function") {
        module.exports[prop] = agent[prop]
    }
}

// Export emit() API for JS data providers
module.exports.emit = function (topic, data) {
	agent.nativeEmit(topic, JSON.stringify(data));
};

// Export monitor() API for consuming data in-process
module.exports.monitor = function() {
	api = hcAPI.getAPI(agent);
	return api;
}
