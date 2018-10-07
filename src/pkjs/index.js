var messageKeys = require('message_keys');
var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig, null, { autoHandleEvents: false });

Pebble.addEventListener('showConfiguration', function(e) {
	Pebble.openURL(clay.generateUrl());
});

function sendData(dict, keys, i){
	if(typeof(keys[i]) === "undefined")
		return;
	var part = {};
	part[keys[i]] = dict[keys[i]];

	Pebble.sendAppMessage(part, function(e) {
		//console.log('Sent config data to Pebble');
		sendData(dict, keys, i+1);
	}, function(e) {
		console.log('Failed to send config data!');
		console.log(keys[i]);
		console.log(part[keys[i]].length);
		console.log("\r\n");
		//console.log(JSON.stringify(e));
		sendData(dict, keys, i+1);
	});
}
  
Pebble.addEventListener('webviewclosed', function(e) {
	if (e && !e.response) {
		return;
	}
  
	// Get the keys and values from each config item
	var dict = clay.getSettings(e.response);
	console.log(JSON.stringify(messageKeys));
	console.log(JSON.stringify(dict));
	var keys = [];
	for(var key in dict){
		keys.push(key);
	}

	sendData(dict, keys, 0);
});