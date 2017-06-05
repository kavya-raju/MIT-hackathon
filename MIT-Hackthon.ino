#include <WiFi.h>
#include <WiFiClient.h>
#include <Temboo.h>
#include "TembooAccount.h" // Contains Temboo account information
#include <Wire.h>
#include <BMA222.h>
#include <math.h>

BMA222 mySensor;

WiFiClient client;

int calls = 1;   // Execution count, so this doesn't run forever
int maxCalls = 10;   // Maximum number of times the Choreo should be executed
//int smsCounter = 2;   // Limit the number of messages to be sent

void setup() {
  Serial.begin(9600);
  
  int wifiStatus = WL_IDLE_STATUS;

  mySensor.begin();
  uint8_t chipID = mySensor.chipID();
  Serial.print("chipID: ");
  Serial.println(chipID);
  
  // Determine if the WiFi Shield is present
  Serial.print("\n\nShield:");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("FAIL");

    // If there's no WiFi shield, stop here
    while(true);
  }

  Serial.println("OK");

  // Try to connect to the local WiFi network
  while(wifiStatus != WL_CONNECTED) {
    Serial.print("WiFi:");
    wifiStatus = WiFi.begin(WIFI_SSID);
    if (wifiStatus == WL_CONNECTED) {
      Serial.println("OK");
    } else {
      Serial.println("FAIL");
    }
    delay(5000);
  }

  Serial.println("Setup complete.\n");
}

void loop() {
  int8_t Xdata = mySensor.readXData();
  Serial.print("X: ");
  Serial.print(Xdata);

  int8_t Ydata = mySensor.readYData();
  Serial.print(" Y: ");
  Serial.print(Ydata);

  int8_t Zdata = mySensor.readZData();
  Serial.print(" Z: ");
  Serial.println(Zdata);
  
  if (calls <= maxCalls) {
    Serial.println("Running SendSMS - Run #" + String(calls++));

    TembooChoreo SendSMSChoreo(client);

    // Invoke the Temboo client
    SendSMSChoreo.begin();

    // Set Temboo account credentials
    SendSMSChoreo.setAccountName(TEMBOO_ACCOUNT);
    SendSMSChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    SendSMSChoreo.setAppKey(TEMBOO_APP_KEY);

    // Set Choreo inputs
    String AuthTokenValue = *authentication token from temboo*;
    SendSMSChoreo.addInput("AuthToken", AuthTokenValue);
    String FromValue = *phone number from which message is generated - Obtained from Twilio*;
    SendSMSChoreo.addInput("From", FromValue);
    String ToValue = *phone number to which message is sent - Obtained from Twilio"*;
    SendSMSChoreo.addInput("To", ToValue);
    //String BodyValue = "Temp data received ,X data: " + String(Xdata) + " ,Y data: " + String(Ydata) + " ,Z data: " + String(Zdata);
    String BodyValue = "Baby's breathing is normal";
    SendSMSChoreo.addInput("Body", BodyValue);
    String AccountSIDValue = *account SID from temboo*;
    SendSMSChoreo.addInput("AccountSID", AccountSIDValue);

    // Identify the Choreo to run
    SendSMSChoreo.setChoreo("/Library/Twilio/SMSMessages/SendSMS");

    // Run the Choreo; when results are available, print them to serial
    // 901 time to wait for a Choreo response. Can be edited as needed
    // USE_SSL input to tell library to use HTTPS
    SendSMSChoreo.run(901, USE_SSL);
    
    while(SendSMSChoreo.available()) {
    // if (smsCounter < 2) {
     char c = SendSMSChoreo.read();
     Serial.print(c);
     //}
     SendSMSChoreo.close();
     //smsCounter++;
    }
    
  }

  Serial.println("\nWaiting...\n");
  //delay(30000); // wait 30 seconds between SendSMS calls
}
