
# ESP32 SIM800 library
Library forked from the https://github.com/carrascoacd/ArduinoSIM800L project. 
Adaptation for using ESP32 HardwaareSerial UART. 

## Support
* Your board have to support the standard HardwareSerial library.
* The API response have to be a valid JSON.
* The library has been tested again Arduino Uno and Arduino Nano

## Instalation
Download the library and then import it.

## Quick start!

Here's some code to perform a GET request! :+1:

``` c++
unsigned int RX_PIN = 7;
unsigned int TX_PIN = 8;
unsigned int RST_PIN = 12;
HTTP http(9600, RX_PIN, TX_PIN, RST_PIN);
http.configureBearer("your.mobile.service.provider.apn");
http.connect();

char response[256];
Result result = http.get("your.api.com", response);

Serial.println(response);

http.disconnect();

```

Here's some code to perform a POST request! :+1:

``` c++
unsigned int RX_PIN = 7;
unsigned int TX_PIN = 8;
unsigned int RST_PIN = 12;
HTTP http(9600, RX_PIN, TX_PIN, RST_PIN);
http.configureBearer("your.mobile.service.provider.apn");
http.connect();

char response[256];
Result result = http.post("your.api.com", "{\"date\":\"12345678\"}", response);

Serial.println(response);

http.disconnect();
```

I suggest the [ArduinoJSON](https://github.com/bblanchon/ArduinoJson) library for parsing the JSON response, then you can play with the values easily.


## How it works?
In order to perform a request, the library follows these steps:

##### Configure Bearer:

  - AT+CREG? -> try until 0,1 (connected to the network)
  - AT+SAPBR=3,1,"Contype","GPRS" -> wait for OK
  - AT+SAPBR=3,1,"APN","movistar.es" -> wait for OK
  - AT+SAPBR=1,1 -> wait for OK

##### HTTP GET:

  - AT+HTTPINIT -> wait for OK
  - AT+HTTPPARA="CID",1 -> wait for OK
  - AT+HTTPPARA="URL","your.api.com"-> wait for OK
  - AT+HTTPSSL=0 -> wait for OK (1 when URL starts with "https://")
  - AT+HTTPACTION=0 -> wait for 200
  - AT+HTTPREAD -> read buffer and parse it
  - AT+HTTPTERM -> wait for OK
  - AT+SAPBR=0,1

##### HTTP POST:
  - AT+HTTPINIT -> wait for OK
  - AT+HTTPPARA="CID",1 -> wait for OK
  - AT+HTTPPARA="URL","your.api.com" -> wait for OK

  For example, if we have this body: {"location_id": 238, "fill_percent": 90}

  - AT+HTTPPARA="CONTENT","application/json"
  - AT+HTTPDATA=strlen(body),10000 -> wait for DOWNLOAD, then write the body and wait 10000
  - AT+HTTPSSL=0 -> wait for OK (1 when URL starts with "https://")
  - AT+HTTPACTION=1 -> wait for ,200,
  - AT+HTTPREAD -> read buffer and parse it
  - AT+HTTPTERM -> wait for OK
  - AT+SAPBR=0,1

## Future improvements

- Support of HardwareSerial.
- Support of more content types, not only JSON (application/json).
