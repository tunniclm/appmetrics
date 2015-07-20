var agent = require('appmetrics');
agent.start();

var duration_secs = process.argv[2];

var t = null;
var ih = setInterval(function() { 
	var dummy = new Buffer(1024*1024);
	dummy.write("hello");
	t = dummy.toString()[0];
}, 100);

if (duration_secs != null) {
	setTimeout(function() {
		clearInterval(ih);
	}, duration_secs*1000);
}
