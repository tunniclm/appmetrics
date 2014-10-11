/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

// argv[0] should be 'node',
// argv[1] the name of this file.
// argv[2] the module to run

// Remove argv[1] so it looks like the module is executed directly
process.argv.splice(1, 1)

// Start the Health Center agent
var path = require("path")
var dir = path.dirname(module.filename)
var agent_module = path.join(dir, "index.js")
var agent = require(agent_module)
agent.start()

if (process.argv.length > 1) {
    // Start module
    require(process.argv[1])
} else {
    // Start REPL
    require("repl").start({})
}
