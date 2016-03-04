function HTTPGET(url) {
	var req = new XMLHttpRequest();
	req.open("GET", url, false);
	req.send(null);
	return req.responseText;
}

var getWeather = function() {
	//Get weather info
	var response = HTTPGET("http://api.openweathermap.org/data/2.5/weather?q=Newcastle,au&appid=1690ddd0f595a2bcbdbaa6c1f2009296");
  //var response = HTTPGET("http://api.openweathermap.org/data/2.5/weather?lat=-32&lon=151&appid=1690ddd0f595a2bcbdbaa6c1f2009296");
		
	//Convert to JSON
	var json = JSON.parse(response);
	
	//Extract the data
	var temperature = Math.round(json.main.temp - 273.15);
	var location = json.name;
	
	//Console output to check all is working.
	console.log("It is " + temperature + " degrees in " + location + " today!");
	
	//Construct a key-value dictionary
  var dict = {"KEY_TEMPERATURE": temperature};
	
	//Send data to watch for display
	Pebble.sendAppMessage(dict);
};

Pebble.addEventListener("ready",
  function(e) {
	//App is ready to receive JS messages
	getWeather();
  }
);

Pebble.addEventListener("appmessage",
  function(e) {
	//Watch wants new data!
	getWeather();
  }
);