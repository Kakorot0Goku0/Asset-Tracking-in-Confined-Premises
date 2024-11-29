#include <WiFi.h>
#include <BLEDevice.h>
#include <PubSubClient.h>
#include <BLEEddystoneTLM.h>
#include <BLEBeacon.h>

#define FLOOR "F3"
#define TOPIC "esp32/F1"
#define CLIENT "ESP32_CLIENT_F3"

const char* ssid = "TP-Link_7070";
const char* password = "56428780";

const char* mqtt_server = "192.168.0.204";
WiFiClient espClient;
PubSubClient client(espClient);


unsigned long previousMillis = 0;
unsigned long interval = 30000;

int scanTime = 5;  //In seconds
BLEScan *pBLEScan;

TaskHandle_t Task1;
TaskHandle_t Task2;
int BLE_RSSI = 0;

// LED pins
const int led1 = 2;


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(CLIENT)) {
      Serial.println("connected");
      // Subscribe
      client.subscribe(TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(115200); 
  pinMode(led1, OUTPUT);

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    20000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500); 

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
                    Task2code,   /* Task function. */
                    "Task2",     /* name of task. */
                    20000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
    delay(500); 
}

//Task1code: blinks an LED every 1000 ms
void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();  //create new scan
  //pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);  //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value

  delay(8000);

  for(;;){
    Serial.print("Task1 running on core ");
    Serial.println(xPortGetCoreID());

    BLEScanResults *foundDevices = pBLEScan->start(scanTime, false);
    Serial.print("Devices found: ");
    Serial.println(foundDevices->getCount());

    for(int i=0; i< foundDevices->getCount(); i++)
    {
    BLEAdvertisedDevice Device = foundDevices->getDevice(i);
  
    /*changes added 19 oct*/
    if (Device.haveManufacturerData() == true) {
      String strManufacturerData = Device.getManufacturerData();

      uint8_t cManufacturerData[100];
      memcpy(cManufacturerData, strManufacturerData.c_str(), strManufacturerData.length());

      if (strManufacturerData.length() == 25 && cManufacturerData[0] == 0x4C && cManufacturerData[1] == 0x00) {
        Serial.println("Found an iBeacon!");
        BLEBeacon oBeacon = BLEBeacon();
        oBeacon.setData(strManufacturerData);
        Serial.printf("iBeacon Frame\n");
        Serial.printf(
          "ID: %04X Major: %d Minor: %d UUID: %s Power: %d\n", oBeacon.getManufacturerId(), ENDIAN_CHANGE_U16(oBeacon.getMajor()),
          ENDIAN_CHANGE_U16(oBeacon.getMinor()), oBeacon.getProximityUUID().toString().c_str(), oBeacon.getSignalPower()
        );

        if (Device.haveName()) 
        {
          Serial.print("Beacon Device name: ");
          Serial.println(Device.getName().c_str());
          Serial.println("");
          
          BLE_RSSI = Device.getRSSI();
          Serial.print("BLE_RSSI ");
          Serial.print("-> ");
          Serial.println(BLE_RSSI);

          char BDATA[25];
          memset(BDATA, ',', 25);
          memcpy(BDATA, Device.getName().c_str(), 10);
          memcpy(&BDATA[10], FLOOR, 2);
          memcpy(&BDATA[13], &BLE_RSSI, 4);
          BDATA[9] = ',';
          BDATA[12] = ',';
          BDATA[17] = '\0';

          Serial.printf("BDATA: %s", BDATA);
          Serial.printf("BDATA RSSI: %d\n", *((int*)&BDATA[13]));
          client.publish(TOPIC, BDATA);
        }
        else
        { //its a doc

          BLE_RSSI = Device.getRSSI();
          Serial.print("BLE_RSSI ");
          Serial.print("-> ");
          Serial.println(BLE_RSSI);

          char BDATA[36]; 
          memcpy(BDATA, oBeacon.getProximityUUID().toString().c_str(), 36);
          memcpy(&BDATA[0], "R", 1);
          memcpy(&BDATA[1], &BDATA[28], 9);
          memcpy(&BDATA[10], FLOOR, 2);
          memcpy(&BDATA[13], &BLE_RSSI, 4);
          //Serial.printf("BDATA: %s", BDATA);
          BDATA[9] = ',';
          BDATA[12] = ',';
          BDATA[17] = '\0';

          Serial.printf("BDATA DOC: %s", BDATA);
          Serial.printf("BDATA RSSI: %d\n", *((int*)&BDATA[13]));
          client.publish(TOPIC, BDATA);
        }
      }
      
    }
    /*changes added end*/
    }
    
    Serial.println("Scan done!");
    pBLEScan->clearResults();  // delete results fromBLEScan buffer to release memory
    delay(1000);
   
  } 
}

//Task2code: blinks an LED every 700 ms
void Task2code( void * pvParameters ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  delay(500);

  client.setServer(mqtt_server, 1883);

  for(;;){
  Serial.print("Task2 running on core ");
  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
    
  Serial.println(xPortGetCoreID());

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  if (WiFi.status() == WL_CONNECTED)
  Serial.println("WIFI CONNECTED STATE!!!");
  //delay(1000);
  //digitalWrite(led1, LOW);
  delay(500);
  digitalWrite(led1, HIGH);
    
  }
}

void loop() {
  //delay(1000);
}