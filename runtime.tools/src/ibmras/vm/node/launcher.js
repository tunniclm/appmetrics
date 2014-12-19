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

// Note: This launcher does not support all invocation modes
//       Not supported: use of lib/_third_party_main.js
//                      node -e <script_string>, 
//                      <some_stream> | node
//                      node < <script_file> 

// Remove argv[1] so it looks like the module is executed directly
// Note: This also causes runMain() to run the correct module 
// Note: If you are using node-hc debug, you need to skip over this
//       launcher code until you hit your application code
process.argv.splice(1, 1)
delete process.mainModule

// Check early for unsupported mode of invocation, so we don't start
// healthcenter
var tty = require('tty');
if (process.argv.length <= 1 && !(process.forceRepl || tty.isatty(0))) {
    var err = new Error("node-hc does not support reading a script from stdin") 
    throw err
}

// Start the Health Center agent
var path = require("path")
var dir = path.dirname(module.filename)
var agent_module = path.join(dir, "index.js")
if (process.argv.length > 1) {
	/* Remove any leading "./" in front of the name of the script - e.g. ./app.js => app.js  */
	if (process.argv[1].indexOf("./") === 0)
        process.argv[1] = process.argv[1].substring(2, process.argv[1].length);
	
    /* Mock the main module for the benefit of the agent (just the bit it needs) */
    process.mainModule = { 'filename': require.resolve(process.argv[1]) }
}
var agent = require(agent_module)
agent.start()
delete process.mainModule

if (process.argv.length > 1) {
    // Start module
    require('module').runMain()
} else if (process._forceRepl || tty.isatty(0)){
    // Start REPL
    require("repl").start({})
} else {
	// Should not happen, but just in case write a message
    var err = new Error("node-hc does not support this method of invocation") 
    throw err
}
