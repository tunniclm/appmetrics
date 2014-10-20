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

// Export any functions exported by the agent
for (var prop in agent) {
    if (typeof agent[prop] == "function") {
        module.exports[prop] = agent[prop]
    }
}

// Set the plugin search path
agent.spath(path.join(module_dir, "plugins"))

