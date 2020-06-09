#include "Pushover.h"
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>


Pushover::Pushover(String token, String user, bool unsafe = false) {
  _token = token;
  _user = user;
  _unsafe = unsafe;
}
void Pushover::setMessage(String message) {
  _message = message;
}
void Pushover::setDevice(String device) {
  _device = device;
}
void Pushover::setTitle(String title) {
  _title = title;
}
void Pushover::setUrl(String url) {
  _url = url;
}
void Pushover::setUrlTitle(String url_title) {
  _url_title = url_title;
}
void Pushover::setPriority(int8_t priority) {
  _priority = priority;
}
void Pushover::setRetry(uint16_t retry) {
  _retry = retry;
}
void Pushover::setExpire(uint16_t expire) {
  _expire = expire;
}
void Pushover::setTimestamp(uint32_t timestamp) {
  _timestamp = timestamp;
}
void Pushover::setSound(String sound) {
  _sound = sound;
}
void Pushover::setTimeout(uint16_t timeout) {
  _timeout = timeout;
}
void Pushover::setHTML(boolean html) {
  _html = html;
}
boolean Pushover::send(void) {
  WiFiClientSecure clientAPI;
  HTTPClient https;
  String host = "api.pushover.net";
  String path = "/1/messages.json";

  clientAPI.setTimeout(10000);
  clientAPI.setBufferSizes(1024, 256);
  
  if (_unsafe) {
    clientAPI.setInsecure();
  }

  Serial.print("connecting to ");
  Serial.println(host);
  if (!clientAPI.connect(host, 443)) {
    Serial.println("connection failed");
    return false;
  } else {
    Serial.println("connection host");
  }

  if (https.begin(clientAPI, host, 443, path)) {

    String post = String("token=") + _token + "&user=" + _user + "&title=" + _title + "&message=" + _message + "&device=" + _device + "&url=" + _url + "&url_title=" + _url_title + "&priority=" + _priority + "&retry=" + _retry + "&expire=" + _expire + "&sound=" + _sound;
    https.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpsCode = https.POST(post);
    Serial.println(httpsCode);
    if (httpsCode > 0) {
      if (httpsCode == HTTP_CODE_OK) {
        Serial.println(https.getString());
      }
    } else {
      Serial.println(https.getString());
      Serial.println("failed to GET");
    }
    https.end();
    clientAPI.stop();
    return true;
  } else {
    return false;
  }
}
