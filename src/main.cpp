#include <Arduino.h>
// #include "hal/gpio_ll.h"

#include <ld2410.h>
ld2410 radar;

#include <HardwareSerial.h>
HardwareSerial SerialPort(2); // if using UART2

/*
  Hint: OneWire project didnt work for me
  It was necessary to update file
    >  OneWire_direct_gpio.h
  @ line 160
  with
    > #include <soc/gpio_struct.h>
*/
#include <OneWire.h>
#include <DallasTemperature.h>

// GPIO where the DS18B20 is connected to
const int oneWireBus = 25;
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature TempSensor(&oneWire);

#include "FS.h"
#include <LITTLEFS.h>

#ifndef CONFIG_LITTLEFS_FOR_IDF_3_2
#include <time.h>
#endif

#define FORMAT_LITTLEFS_IF_FAILED true

#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
#include "define_esp32.h"

// needed for OTA & MQTT
#include <ArduinoOTA.h>
#include <PubSubClient.h>

#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson

// define your default values here, if there are different values in config.json, they are overwritten.
char mqtt_server[40] = MQTT_SERVER;
char mqtt_port[6] = MQTT_PORT;

// Project name
char WifiApName[40] = WIFI_AP_NAME;
char MQTTClientName[40] = MQTT_CLIENT_NAME;
char OtaClientName[40] = OTA_CLIENT_NAME;

// MQTT
const char *pub_alive = MQTT_ALIVE;

const char *pub_status = MQTT_STATUS;

const char *pub_failure = MQTT_E_FAILURE;
const char *pub_debug = MQTT_E_DEBUG;

const char *sub_value1 = MQTT_subControl;
const char *sub_value2 = MQTT_subControlOn;
const char *sub_value3 = MQTT_subControlTime;

const char *pub_light = MQTT_LIGHT;
const char *pub_temperature = MQTT_TEMPERATURE;
const char *pub_motion = MQTT_MOTION;

const char *pub_stationaryTargetDistance = MQTT_STATIONARYTARGETDISTANCE;
const char *pub_stationaryTargetEnergy = MQTT_STATIONARYTARGETENERGY;

const char *pub_movingTargetDistanceValue = MQTT_MOVINGTARGETDISTANCE;
const char *pub_movingTargetEnergyValue = MQTT_MOVINGTARGETENERGY;

// WIFI
WiFiClient espClient;
PubSubClient client(espClient);

WiFiServer server(80);

///////////////////////////////////////////////

bool motion = false;
bool config = false;
int light = 0;
int count = 0;
float temperature = 0;

void getTemp(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  // Serial.print("Requesting temperatures...");
  TempSensor.requestTemperatures(); // Send the command to get temperatures
  // Serial.println("DONE");

  temperature = TempSensor.getTempCByIndex(0);
  Serial.print("Temperature: ");
  Serial.println(temperature);
  client.publish(pub_temperature, String(temperature).c_str());

  count++;
  Serial.print("counter: ");
  Serial.println(count);
}

// flag for saving data
bool shouldSaveConfig = false;

// callback notifying us of the need to save config
void saveConfigCallback()
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

// OTA setup function:
void OTA_setup(void)
{
  // Ergänzung OTA
  // Port defaults to 3232
  ArduinoOTA.setPort(3232);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("WEMOS_ESP32");

  // No authentication by default
  ArduinoOTA.setPassword((const char *)"admin2Motion");

  ArduinoOTA.onStart([]()
                     {
                       Serial.println("Start");
                       // analogWrite(14, 0);
                       // digitalWrite(2, LOW);
                     });
  ArduinoOTA.onEnd([]()
                   { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error)
                     {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed"); });
  ArduinoOTA.begin();
}

// MQTT callback function:
bool DoorRequestUpdate = false;
bool DoorRequestENABLE = false;

void MQTTcallback(char *data, byte *payload, unsigned int length)
{
  Serial.print("Message arrived @ PUB [");
  Serial.print(data);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (strcmp(data, sub_value1) == 0)
  {
  }

  if (strcmp(data, sub_value2) == 0)
  {
  }

  if (strcmp(data, sub_value3) == 0)
  {
    payload[length] = '\0';
    String s = String((char *)payload);
  }
}

// RECONNECT MQTT Server
void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTTClientName))
    {

      // client.subscribe(sub_value1);
      // client.loop();
      // client.subscribe(sub_value2);
      // client.loop();
      // client.subscribe(sub_value3);
      // client.loop();

      Serial.println("connected ...");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

String ip = "";

String ip2Str(IPAddress ip)
{
  String s = "";
  for (int i = 0; i < 4; i++)
  {
    s += i ? "." + String(ip[i]) : String(ip[i]);
  }
  return s;
}

bool startup = false;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();

  SerialPort.begin(256000, SERIAL_8N1, 16, 17);

  // OUTPUT Definition !
  pinMode(ONBOARD_LED, OUTPUT);
  pinMode(EXTERNAL_LED, OUTPUT);
  pinMode(CONFIG_PIN, INPUT);
  int WifiReset = digitalRead(CONFIG_PIN);

  pinMode(SENSOR_PIN, INPUT);
  TempSensor.begin();

  Serial.print(F("\nLD2410 radar sensor initialising: "));
  if (radar.begin(SerialPort))
  {
    Serial.println(F("OK"));
  }
  else
  {
    Serial.println(F("not connected"));
  }

  // clean FS, for testing
  // LittleFS.format();

  // read configuration from FS json
  Serial.println("mounting FS...");

  if (LittleFS.begin())
  {
    Serial.println("mounted file system");
    if (LittleFS.exists("/config.json"))
    {
      // file exists, reading and loading
      Serial.println("reading config file");
      File configFile = LittleFS.open("/config.json", "r");
      if (configFile)
      {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject &json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success())
        {
          Serial.println("\nparsed json");

          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(mqtt_port, json["mqtt_port"]);
        }
        else
        {
          Serial.println("failed to load json config");
        }
        configFile.close();
      }
    }
    else
    {
      Serial.println("failed to open json config");
    }
  }
  else
  {
    Serial.println("failed to mount FS");
  }
  // end read

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // set static ip
  // wifiManager.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  // add all your parameters here
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);

  // reset settings - for testing
  if (WifiReset == 1)
    wifiManager.resetSettings();

  // set minimu quality of signal so it ignores AP's under that quality
  // defaults to 8%
  // wifiManager.setMinimumSignalQuality();

  // sets timeout until configuration portal gets turned off
  // useful to make it all retry or go to sleep
  // in seconds
  wifiManager.setTimeout(120);

  // fetches ssid and pass and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  // if (!wifiManager.autoConnect("WEMOS D1", "password")) {
  if (!wifiManager.autoConnect(WifiApName))
  {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    // reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(5000);
  }

  // if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  // read updated parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());

  // save the custom parameters to FS
  if (shouldSaveConfig)
  {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    json["mqtt_server"] = mqtt_server;
    json["mqtt_port"] = mqtt_port;

    File configFile = LittleFS.open("/config.json", "w");
    if (!configFile)
    {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    // end save
  }

  // IP Adr.
  ip = ip2Str(WiFi.localIP());

  // OTA starts here!
  OTA_setup();

  // MQTT - Connection:
  client.setServer(mqtt_server, atoi(mqtt_port));
  client.setCallback(MQTTcallback);

  // Webserver!
  server.begin();

  // MQTT connect
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  startup = true;
}

// prepare a web page to be send to a client (web browser)
String prepareHtmlPage(void)
{
  String htmlPage;
  htmlPage.reserve(1024); // prevent ram fragmentation
  htmlPage = F("HTTP/1.1 200 OK\r\n"
               "Content-Type: text/html\r\n"
               "Connection: close\r\n" // the connection will be closed after completion of the response
               "Refresh: 10\r\n"       // refresh the page automatically every 10 sec
               "\r\n"
               "<!DOCTYPE HTML>"
               "<html>");
  htmlPage += "<body><h1>ESP32 Web Server</h1>";
  htmlPage += "<br>";

  htmlPage += "Device Name      : ";
  htmlPage += DEVICE_NAME; //
  htmlPage += "<br> <br>";

  htmlPage += "SW-Version       : ";
  htmlPage += SW_BASE_VERSION; //
  htmlPage += "<br> <br>";

  htmlPage += "IP Adresse       : ";
  htmlPage += ip; //
  htmlPage += "<br> <br>";

  htmlPage += "MQTT Server      : ";
  htmlPage += mqtt_server; //
  htmlPage += "<br>";      // "<br>" erschafft eine Leerzeile (bzw. definiert das Ende einer Zeile)

  htmlPage += "MQTT Port        : ";
  htmlPage += mqtt_port; //
  htmlPage += "<br>";    // "<br>" erschafft eine Leerzeile (bzw. definiert das Ende einer Zeile)

  htmlPage += "MQTT Client Name : ";
  htmlPage += MQTT_CLIENT_NAME; //
  htmlPage += "<br>";

  htmlPage += "MQTT Client : ";
  if (!client.connected())
  {
    htmlPage += "NOT ";
  }
  htmlPage += "connected ";
  htmlPage += "<br> <br>";

  htmlPage += "Helligkeit LDR : ";
  htmlPage += light; //
  htmlPage += "<br>";

  htmlPage += "ConfigInput : ";
  htmlPage += config; //
  htmlPage += "<br>";

  htmlPage += "Temperatur : ";
  htmlPage += temperature; //
  htmlPage += "C <br>";

  if (motion)
  {
    htmlPage += "RadarSensor: >>> MOTION";
    htmlPage += "<br> <br>";
  }
  else
  {
    htmlPage += "RadarSensor: >>> Keiner da ! :-)";
    htmlPage += "<br> <br>";
  }
  htmlPage += "timer            : ";
  htmlPage += millis(); //
  htmlPage += "<br>";

  htmlPage += F("</html>\r\n");
  return htmlPage;
}

void webServer(void)
{
  WiFiClient client = server.accept();
  // wait for a client (web browser) to connect
  if (client)
  {
    // Serial.println("\n[Client connected]");
    while (client.connected())
    {
      // read line by line what the client (web browser) is requesting
      if (client.available())
      {
        String line = client.readStringUntil('\r');
        // Serial.print(line);
        //  wait for end of client's request, that is marked with an empty line
        if (line.length() == 1 && line[0] == '\n')
        {
          client.println(prepareHtmlPage());
          break;
        }
      }
    }

    while (client.available())
    {
      // but first, let client finish its request
      // that's diplomatic compliance to protocols
      // (and otherwise some clients may complain, like curl)
      // (that is an example, prefer using a proper webserver library)
      client.read();
    }

    // close the connection:
    client.stop();
    // Serial.println("[Client disconnected]");
  }
}

/*
MOTION
*/
bool get_motion(void)
{
  if (digitalRead(SENSOR_PIN))
    return true;
  else
    return false;
}

void get_data(void)
{
  config = bool(digitalRead(CONFIG_PIN));
  Serial.print("Config:");
  Serial.println(config);

  light = analogRead(LDR_PIN);
  Serial.print("Light:");
  Serial.println(light);
  client.publish(pub_light, String(light).c_str());

  getTemp();
  Serial.println();
}

void loop()
{
  static long lastTransferTime = 0;

  static long lastReading = 0;
  static long lastMotion = 0;

  static long lastLEDTime = 0;
  static long lastCheckTime = 0;

  static bool ledState = false;
  static bool last_motion = false;
  bool change = false;

  // MQTT connect
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  // OTA handler !
  ArduinoOTA.handle();

  // WebServer
  webServer();

  radar.read(); // Always read frames from the sensor

  ////////////////////////////////////////////////////////////////////////
  // Hello MQTT World
  if ((millis() - lastTransferTime > (INTERVAL1)) || startup)
  { // tbd sec
    lastTransferTime = millis();
    startup = false;
    client.publish(pub_alive, "Hello World!");
    get_data();
  }

  ////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////
  // LED blinken
  if (millis() - lastLEDTime > (INTERVAL3))
  { // tbd sec
    lastLEDTime = millis();
    if (ledState)
    {
      ledState = false;
      digitalWrite(ONBOARD_LED, HIGH);
    }
    else
    {
      ledState = true;
      digitalWrite(ONBOARD_LED, LOW);
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Control state machine :-)
  if (millis() - lastCheckTime > (INTERVAL2))
  {
    lastCheckTime = millis();
    get_data();
  }

  ////////////////////////////////////////////////////////////////////////
  // FAST reation based on

  // fast Check :-) MOTION! == TRUE?
  motion = get_motion();
  delay(10);
  if ((motion != last_motion) || (millis() - lastMotion > INTERVAL4))
  {
    lastMotion = millis();
    Serial.print("Motion  : ");
    Serial.println(motion);
    // Serial.println(millis());
    if (motion)
    {
      digitalWrite(EXTERNAL_LED, HIGH);
      client.publish(pub_motion, String("1").c_str());
    }
    else
    {
      client.publish(pub_motion, String("0").c_str());
      digitalWrite(EXTERNAL_LED, LOW);
    }
    last_motion = motion;
  }

  ////////////////////////////////////////////////////////////////////////
  //
  if (radar.isConnected() && millis() - lastReading > INTERVAL4)
  {
    lastReading = millis();
    if (radar.presenceDetected())
    {
      if (radar.stationaryTargetDetected())
      {
        Serial.print(F("Stationary target: "));
        int stationaryTargetDistanceValue = radar.stationaryTargetDistance();
        Serial.print(stationaryTargetDistanceValue);
        Serial.print(F("cm energy:"));
        client.publish(pub_stationaryTargetDistance, String(stationaryTargetDistanceValue).c_str());

        int stationaryTargetEnergyValue = radar.stationaryTargetEnergy();
        Serial.print(stationaryTargetEnergyValue);
        Serial.print(' ');
        client.publish(pub_stationaryTargetEnergy, String(stationaryTargetEnergyValue).c_str());
      }
      else
      {
        client.publish(pub_stationaryTargetEnergy, String("0").c_str());
      }

      if (radar.movingTargetDetected())
      {
        Serial.print(F("Moving target: "));
        int movingTargetDistanceValue = radar.movingTargetDistance();
        Serial.print(movingTargetDistanceValue);
        Serial.print(F("cm energy:"));
        client.publish(pub_movingTargetDistanceValue, String(movingTargetDistanceValue).c_str());

        int movingTargetEnergyValue = radar.movingTargetEnergy();
        Serial.print(movingTargetEnergyValue);
        client.publish(pub_movingTargetEnergyValue, String(movingTargetEnergyValue).c_str());
      }
      else
      {
        client.publish(pub_movingTargetEnergyValue, String("0").c_str());
      }
      Serial.println();
    }
    else
    {
      client.publish(pub_stationaryTargetEnergy, String("0").c_str());
      client.publish(pub_movingTargetEnergyValue, String("0").c_str());
      Serial.println(F("No target"));
    }
  }
}
