# HealthCenter
Health Center monitoring and profiling agent

**NOTE: This project is not currently released under an open source licence, however raising issues and enhancement requests is encouraged.**

The Health Center agent instruments the Node.js runtime for performance monitoring using either the provided API, or by connecting using the "[IBM Monitoring and Diagnostics Tools - Health Center][1]" client in an Eclipse IDE.

See https://www.ibm.com/developerworks/java/jdk/tools/healthcenter/ for more details.

## Getting Started
### Prerequisites
The Health Center Agent supports either the Node.js 0.10 or 0.12 runtime environments on the following platform architectures:

* 64-bit or 32-bit runtime on Windows (x64 or x86)
* 64-bit or 32-bit runtime on Linux (x64, x86, PPC32, PPC64, PPC64LE, z31, z64)
* 64-bit or 32-but runtime on AIX (PPC32, PPC64)

### Installation
Health Center can be installed globally using npm to be able to use the node-hc command:
```sh
$ npm install -g healthcenter
```
or installed locally in order to be able to use the API:
```sh
$ npm install healthcenter
```
### Configuring Health Center


### Running Health Center
If installed globally, Health Center can be run using the `node-hc` command:
```sh
$ node-hc app.js
```
If installed locally, the use of the Health Center agent needs to be added as the first line of your application:
```sh
var healthcenter = require('healthcenter');
```
and your application launched using the `node` command as usual.
### Connecting the Health Center Eclipse IDE client

##The Health Center API
In addition to being able to connect the Eclipse IDE client, you can also access the Health Center monitoring data using the provided event API.

###Getting started with the API
To access the Health Center monitoring data, first load the Health Center module, then request access to the monitoring data:
```sh
var healthcenter = require('healthcenter');
var monitoring = healthcenter.monitor();
```
The monitoring instance can then be used to register callbacks and request information about the application:
```sh
monitoring.on('initialized', function (env) {
	env = monitoring.getEnvironment());
    for (var entry in env) {
		console.log(entry + ':' + env[entry]);
	};
});
monitoring.on('cpu', function (cpu) {
	console.log('[' + new Date(data.time) + ] CPU: ' + cpu.process');
});
```

### API Documentation

#### healthcenter.monitor()
Creates a Health Center agent client instance. This can subsequently be used to control data collection, request data, and subscribe to data events.

#### healthcenter.emit(`type`, `data`)
Allows custom monitoring events to be added into the Health Center agent.
* `type` (String) the name you wish to use for the data. A subsequent event of that type will be raised, allowing callbacks to be registered for it.
* `data` (Object) the data to be made available with the event. The object must not contain circular references, and by convention should contain a `time` value representing the milliseconds when the event occurred.

#### healthcenter.monitor.getEnvironment()
Requests an object containing all of the available environment information for the running application.

#### healthcenter.monitor.enable(`type`)
Enable data generation of the specified data type.
* `type` (String) the type of event to start generating data for. Only 'profiling' is currently supported.

#### healthcenter.monitor.disable(`type`)
Disable data generation of the specified data type.
* `type` (String) the type of event to stop generating data for. Only 'profiling' is currently supported.

#### Event: 'cpu'
Emitted when a CPU monitoring sample is taken.
* `data` (Object) the data from the CPU sample:
    * `time` (Number) the milliseconds when the sample was taken. This can be converted to a Date using `new Date(cpu.time)`.
    * `process` (Number) the percentage of CPU used by the Node.js application itself. This is a value between 0.0 and 1.0.
    * `system` (Number) the percentage of CPU used by the system as a whole. This is a value between 0.0 and 1.0.

#### Event: 'memory'
Emitted when a memory monitoring sample is taken.
* `data` (Object) the data from the memory sample:
    * `time` (Number) the milliseconds when the sample was taken. This can be converted to a Date using `new Date(cpu.time)`.
    * `physical_total` (Number) the total amount of RAM available on the system in bytes.
    * `physical_used` (Number) the total amount of RAM in use on the system in bytes.
    * `physical_free` (Number) the total amount of free RAM available on the system in bytes.
    * `private` (Number) the amount of RAM in use by the Node.js application in bytes.
    * `virtual` (Number) the memory address space used by Node.js application in bytes.

#### Event: 'gc'
Emitted when a garbage collection (GC) cycle occurs in the underlying V8 runtime.
* `data` (Object) the data from the GC sample:
    * `time` (Number) the milliseconds when the sample was taken. This can be converted to a Date using `new Date(cpu.time)`.
    * `type` (String) the type of GC cycle, either 'M' or 'S'.
    * `size` (Number) the size of the JavaScript heap in bytes.
    * `used` (Number) the amount of memory used on the JavaScript heap in bytes.
    * `duration` (Number) the duration of the GC cycle in milliseconds.

#### Event: 'profiling'
Emitted when a profiling sample is available from the underlying V8 runtime.
* `data` (Object) the data from the profiling sample:
    * `time` (Number) the milliseconds when the sample was taken. This can be converted to a Date using `new Date(cpu.time)`.
    * `functions` (Array) an array of functions that ran during the sample. Each array entry consists of:
        * `self` (Number) the ID for this function.
        * `parent` (Number) the ID for this function's caller.
        * `name` (String) the name of this function.
        * `file` (String) the file in which this function is defined.
        * `line` (Number) the line number in the file.
        * `count` (Number) the number of samples for this function.

### Version
3.0.3

[1]:https://marketplace.eclipse.org/content/ibm-monitoring-and-diagnostic-tools-health-center