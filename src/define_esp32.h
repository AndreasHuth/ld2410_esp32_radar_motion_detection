
#define MQTT_SERVER         "192.168.2.100"
#define MQTT_SERVER_TEST    "192.168.1.0"
#define MQTT_PORT           "1883"

#define WIFI_AP_NAME        "Wifi_EspAP_MOTION_ESP32"
#define MQTT_CLIENT_NAME    "MQTT_MOTION_ESP32"
#define OTA_CLIENT_NAME     "Wifi_OTA_MOTION_ESP32"

#define DEVICE_NAME         "WEMOS ESP32 - MOTION"

#define SW_BASE_VERSION     "V1.1"

#define INTERVAL1           600000          // Hello World 10min
#define INTERVAL2           60000           // Temperature / Light 1 min
#define INTERVAL3           1000            // toggle LED  1 sec
#define INTERVAL4           1000            // Radar addional infomation ...  1sec

// MQTT
#define MQTT_ALIVE          "/ESP32Radar1/active"
#define MQTT_STATUS         "/ESP32Radar1/StatusESP32Radar1"

#define MQTT_E_FAILURE      "/ESP32Radar1/failure"
#define MQTT_E_DEBUG        "/ESP32Radar1/debug"

#define MQTT_subControl     "/ESP32Radar1/ESP32Radar1DoorControl"
#define MQTT_subControlOn   "/ESP32Radar1/ESP32Radar1DoorControlEnable"
#define MQTT_subControlTime "/ESP32Radar1/ESP32Radar1DoorTimevalue"

#define MQTT_MOTION          "/ESP32Radar1/motion"
#define MQTT_LIGHT           "/ESP32Radar1/light"
#define MQTT_TEMPERATURE     "/ESP32Radar1/temperature"

#define MQTT_MOVINGTARGETDISTANCE   "/ESP32Radar1/movingTargetDistanceValue"
#define MQTT_MOVINGTARGETENERGY     "/ESP32Radar1/movingTargetEnergyValue"

#define MQTT_STATIONARYTARGETDISTANCE   "/ESP32Radar1/stationaryTargetDistanceValue"
#define MQTT_STATIONARYTARGETENERGY     "/ESP32Radar1/stationaryTargetEnergyValue"


#define ONBOARD_LED 2
#define EXTERNAL_LED 14
#define LDR_PIN 34
#define CONFIG_PIN 26
#define SENSOR_PIN 33

// Request
#define     REQUEST2CLOSE       0
#define     REQUEST2OPEN        1
#define     REQUEST2CHANGE      2

