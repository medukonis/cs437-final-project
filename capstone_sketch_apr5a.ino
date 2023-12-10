//##################################################
//# Name:         Michael Edukonis
//# UIN:          677141300
//# email:        meduk2@illinois.edu
//# class:        CS437
//# assignment:   Capstone Project
//# date:         4/22/2023
//##################################################
//capstone_sketch_apr5a.ino - Arduino code to run
//the ESP32 WROOM board with oled screen.  Sensor
//is a WCS-1800 current sensor.  When power is applied
//the board will go in to AP mode to allow user to connect
//using any wireless device.  User can then configure
//wireless and MQTT settings.

#include <ArduinoMqttClient.h>
#include "wcs.h"              //https://www.winson.com.tw/     Current Sensor
#include <WiFiManager.h>      //https://github.com/tzapu/WiFiManager
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
char mqtt_server[40] = "";
char mqtt_topic[40] = "";
char mqtt_port[6] = "1883";

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

time_t tnow;
char dateTime[20];
static char ap_name[10];
double data = 0;
// Setting Analog Pin & Sensitivity(mV/A)
WCS  WCS1 = WCS( 36, _WCS1800);
int loopCount = 0;


void setup() 
{
  Serial.begin(115200);
  
  // Start I2C Communication SDA = 5 and SCL = 4 on ESP32 with built-in SSD1306 OLED
  Wire.begin(5, 4);  

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000); // Pause for 2 seconds
  
  // WCS1800 sensor - Power-on Reset
  Serial.println("Reset");
  WCS1.Reset(); 

 //display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Welcome");

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 17);

  //Setup Wireless
  //Open up AP mode to allow users to connect via wireless
  //Get random string of length 10 and this will be the access point name for WiFi setup
  //Show the AP name on the oled screen
  get_random_string(ap_name, 10);
  display.println("Waiting to");
  display.println("connect to WIFI");
  display.print("AP: ");
  display.println(ap_name);
  display.display();

  //Setup some additional needed parameters for this implementation
  //3 settings needed for MQTT
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
  WiFiManagerParameter custom_mqtt_topic("topic", "mqtt topic", mqtt_topic, 40);

  //Setup instance of wifi manager.  
  WiFiManager wifiManager;
  
  //reset saved settings
  //Normally, this would be commented out however for demonstration purposes,
  //wifi settings will not be saved when power is lost.
  wifiManager.resetSettings();
   
  //set custom ip for portal 10.0.1.1
  wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
  
  //add MQTT parameters here
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_topic);
  
  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the
  //name sent back by the random string generator
  //and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect(ap_name);
  
  //or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();
  
  //if you get here you have connected to the WiFi
  Serial.println("connected...");
  
  //read updated MQTT parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());
  strcpy(mqtt_topic, custom_mqtt_topic.getValue());
  Serial.println("The values in the file are: ");
  Serial.println("\tmqtt_server : " + String(mqtt_server));
  Serial.println("\tmqtt_port : " + String(mqtt_port));
  Serial.println("\tmqtt_topic : " + String(mqtt_topic));
  
  Serial.println(WiFi.localIP());
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 17);
  display.print("IP Address: ");
  display.print(WiFi.localIP().toString());

  if (!mqttClient.connect(mqtt_server, atoi(mqtt_port))) 
  {
    Serial.print("MQTT connection failed! Error code = ");
    display.println("MQTT connection failed! Error code = ");
    display.println(mqttClient.connectError());
    Serial.println(mqttClient.connectError());
    while (1);
  }
  
  
//Setup Timekeeping
//#########################################################################################################################
  configTime(0, 0, "0.north-america.pool.ntp.org", "1.north-america.pool.ntp.org", "2.north-america.pool.ntp.org");
  Serial.println("\nWaiting for time");
  while (time(nullptr)<= 100000)
  {
    Serial.print(".");
    delay(100);
  }
    
  Serial.println("");
  tnow = time(nullptr);
  tm *ptm = localtime(&tnow);
  strftime(dateTime, 20, "%Y-%m-%d,%H:%M:%S,", ptm);
  Serial.println(dateTime);
//#########################################################################################################################
  
  delay(5000);
}

void loop() 
{
  //######################################
  // Update Time
  //######################################
  tnow = time(nullptr);
  tm *ptm = localtime(&tnow);
  strftime(dateTime, 20, "%Y-%m-%d,%H:%M:%S", ptm);
  //Serial.println(dateTime);
  
  //######################################
  // Measure AC Current
  //######################################
  data = WCS1.A_AC();
  
  Serial.print("Current(A) : ");
  Serial.println(String(data,3) + " A" +  "\t MQTT Topic: " + String(mqtt_topic) + "\t Server: " + String(mqtt_server) + "\t Port: " + String(mqtt_port));
  
  
  //######################################
  // Display information on OLED screen
  // loop has a 1 second delay so we use
  // that to keep count.  Everyone 10 
  // seconds, the OLED screen will alternate
  // it's information being displayed
  // because it could not all fit on one
  //######################################
  if (loopCount >= 10 && loopCount <= 20)
  {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Server:"); 
    display.println(String(mqtt_server));
    display.println("Port: " + String(mqtt_port));
    display.println("Topic:");
    display.println(String(mqtt_topic));
    display.setTextSize(2);
    display.print(data);
    display.println("  A");
    display.display();
    if (loopCount == 20)
    {
      loopCount = 0;
    }
  }
 
  else
  {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println(dateTime);
    display.print("IP: ");
    display.println(WiFi.localIP().toString()); 
    display.println("Current(A):\n");
    display.setTextSize(3);
    display.print(data);
    display.println("  A");
    display.display();
  }

  //######################################
  // Send Data to MQTT Broker
  //######################################
  mqttClient.beginMessage(mqtt_topic);
  mqttClient.print(String(mqtt_topic) + "," + String(dateTime) + "," + String(data));
  mqttClient.endMessage();

  loopCount += 1;
  delay(1000);
  
}

//#######################################################
//Function: get_random_string
//Input: char *, unsigned int
//Output: None
//Generates random string in "str" of length "len" chars
//writes directly to the passed variable using a pointer
//#######################################################

static void get_random_string(char *str, unsigned int len)
{
    unsigned int i;
    const char possible_values[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789$&*!@";

    // reseed the random number generator
    srand(time(NULL));
    
    for (i = 0; i < len; i++)
    {
        // Add random printable ASCII char
        int r = random(0, strlen(possible_values));
        str[i] = possible_values[r];
    }
    str[i] = '\0';
}


