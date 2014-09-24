var agent = require('./healthcenter').start();

var t=null;
setInterval(function() { 
	var dummy = new Buffer(1024*1024);
	dummy.write("hello");
	t = dummy.toString()[0];
}, 100);
