#!/usr/bin/env node

/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

var node_args = process.execArgv

// First argv[0] should be 'node' and argv[1] the name of this file.
var args = process.argv.splice(0)
var node_exe = args.shift()
var this_filename = args.shift()

var arg = args.shift()
while (typeof arg != "undefined") {
    if (arg.length > 0 && arg.charAt(0) == '-') {
        node_args.push(arg)
        arg = args.shift()
    } else {
        args.unshift(arg)
        break;
    }
}

var path = require("path")
var launcher_filename = path.join(path.dirname(module.filename), "..", "launcher.js")
var child_process = require("child_process")
var p = child_process.fork(launcher_filename, args, {"execArgv": node_args})
