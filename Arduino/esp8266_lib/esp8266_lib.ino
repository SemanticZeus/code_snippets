#include "Arduino.h"
#include "ESP8266.h"
// If SOFTWARSERIAL or Serial is applied,
// requires declaration of SortwareSerial.h

#define SSID	"Maxwell"
#define PWD		"dennisritchie"

void setup() {
	char	*ipAddress, ap[31];

	Serial.begin(115200);
	Serial.println(F("Setup"));
	if (WiFi.reset(WIFI_RESET_HARD))
		Serial.println(F("reset Complete"));
	else {
		Serial.println(F("reset fail"));
		while (1);
	}
	if (WiFi.begin(115200)) {
		if (WiFi.setup(WIFI_CONN_CLIENT, WIFI_PRO_TCP, WIFI_MUX_SINGLE) != WIFI_ERR_OK) {
			Serial.println(F("setup fail"));
			while (1);
		}
	} else {
		Serial.println(F("begin fail"));
		while (1);
	}


	if (WiFi.join(SSID, PWD) == WIFI_ERR_OK) {
		ipAddress = WiFi.ip(WIFI_MODE_STA);
		Serial.print(F("\n\rIP address:"));
		Serial.print(ipAddress);
		Serial.print(':');
		if (WiFi.isConnect(ap))
			Serial.println(ap);
		else
			Serial.println(F("not found"));
	} else {
		Serial.println(F("connect fail"));
		while (1);
	}
}

void loop() {
	if (WiFi.connect((char *)"www.google.co.jp", 80) == WIFI_ERR_CONNECT) {
		Serial.println(F("Send Start"));
		if (WiFi.send((const uint8_t *)"GET / HTTP/1.1\r\n\r\n") == WIFI_ERR_OK) {
			int16_t	c;
			uint16_t len = WiFi.listen(10000UL);
			Serial.print(F("\nRCV:"));
			Serial.print(len);
			Serial.print(F("-->"));
			while (len)
				if ((c = WiFi.read()) > 0) {
					Serial.write((char)c);
					len--;
				}
			Serial.println(F("<--RCV"));
		} else {
			Serial.println(F("Send fail"));
		}
		WiFi.close();
	} else {
		Serial.println(F("TCP connect fail"));
	}
	WiFi.disconnect();
	while (1);
}
