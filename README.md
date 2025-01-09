# Asset-Tracking-in-Confined-Premises
Asset tracking systems are vital for managing resources in confined spaces like hotels and hospitals. They reduce loss, enhance efficiency, and provide real-time insights to improve operations and productivity.

## YouTube Link
https://youtu.be/zWpXuLOgyOM?si=68-Ao58v3-Z6q_vm

## BeaconAndroidApp:
- This is the Android App for the beacons. This app releases the beacon signal and the beacon signal is received by the ESP32. The ESP32 then sends the data to the server (Raspberry Pi) which then sends the summary to the dashboard.
- The app is written in Kotlin and uses the AltBeacon library to release the beacon signal.
- To run the app you have to keep the bluetooth and location on.
- Also provide the required permissions to the app. Visit the app info and provide the permissions.
- The folder for BeaconAndroidApp can be opened in the android studio and then the app can be run on the emulator or the physical device. Apk can also be generated and installed on the device.


## dashboard: 
- This is the dashboard for the project. It is written in node.js and uses the express framework. It is hosted on local server. It received the data from raspberry pi and displays the summary of the data.
- To run the dashboard, you have to install node.js and npm on your system.
- Use the command `npm install` to install the required dependencies.
- Install nodemon globally using the command `npm install -g nodemon`.
- Run the dashboard using the command `nodemon app.js`.
- The dashboard will be hosted on `localhost:3000`.
# Server Setup

## Setting Up MQTT Server on Raspberry Pi B2+

1) Run the following command to upgrade and update your system:
    ```bash 
    sudo apt update && sudo apt upgrade 
1) Press Y and Enter. It will take some time to update and upgrade.
2) To install the Mosquitto Broker enter these next commands:
   ```bash
   sudo apt install -y mosquitto mosquitto-clients
3) To make Mosquitto auto start when the Raspberry Pi boots, you need to run the following command
   ```bash
   sudo systemctl enable mosquitto.service
4) Now, test the installation by running the following command:
   ```bash
   mosquitto -v

Configure the mosquitto server

Run the following command to open the mosquitto.conf file.
    
    sudo nano /etc/mosquitto/mosquitto.conf
1) Move to the end of the file using the arrow keys and paste the following two lines:
   ```bash
    listener 1883
    allow_anonymous true
2) Then, press CTRL-X to exit and save the file. Press Y and Enter.
3) Restart Mosquitto for the changes to take effect.
   ```bash
    sudo systemctl restart mosquitto
## Setup for Sqlite3 on server

You can install SQLite on a Raspberry Pi using this command:

    sudo apt-get install sqlite3

### :::::::::::::::Instalation Completed successfully:::::::::::::::::

## Cerate database 
    sqlite3 /home/Downloads/mqtt_masala/hospital.db
    sqlite> create table beacon_data( beacon_no Text, name Text )
    sqlite> INSERT INTO beacon_data VALUES(beacon_no,name_of_device/Doctor)
    
::::::::::::::::::::::::: DATABASE created successfully::::::::::::::::::::::::

## Code for server
create a c file  name- beacon_tracker.c 
    cd Downloads/mqtt_masala

complie code

    gcc -o beacon_tracker1 beacon_tracker.c -lmosquitto -lsqlite3 -lrt

code to run the server code 

    ./beacon_tracker1
# ESP32 Setup Guide

## Setting up ESP32 with Arduino IDE

1. **Install Arduino IDE**
   - Download and install the [Arduino IDE](https://www.arduino.cc/en/software) for your operating system.

2. **Install ESP32 Library**
   - Open Arduino IDE.
   - Go to **File > Preferences**.
   - In the **Additional Board Manager URLs** field, add:
     ```
     https://dl.espressif.com/dl/package_esp32_index.json
     ```
   - Go to **Tools > Board > Board Manager**.
   - Search for "ESP32" and click **Install**.

3. **Select ESP32 Development Module**
   - Go to **Tools > Board > ESP32 Arduino > ESP32 Dev Module**.

4. **Set COM Port Permissions**
   - Update the COM port permission using the following command:
     ```bash
     sudo chmod a+rw /dev/ttyUSB0
     ```

5. **Include Necessary Libraries**
   - Ensure the following libraries are installed and included in your code:
     - **Beacon**
     - **WiFi**
     - **PubSubClient**

6. **Open Huge Partition**
   - Enable the HUGE partition to upload larger code files onto the ESP32.

---

## Field Hardware: NRF51822 Beacons
- NRF51822 beacons were used on the field to send BLE signals.

---

## ESP32 Code Features
- **BLE**: Used for detecting beacons.
- **WiFi**: Connects the ESP32 to a router.
- **MQTT**: Publishes beacon data to the server.

---

## Changes to Code Before Uploading
1. **Update Client ID**:
   - Ensure the `#CLIENT` identifier is unique for each node connected to the Raspberry Pi server.

2. **Topic Configuration**:
   - Use the same `#TOPIC` across all nodes to publish beacon data.

3. **Define Floor Number**:
   - Use `#FLOOR` to specify the floor number where the node is deployed.

4. **Router IP**:
   - Update the router IP address in the code.

5. **MQTT Server IP**:
   - Update the IP address of the MQTT server.

---

## Final Step: Upload Code to ESP32
- Burn the updated code onto the ESP32 board using Arduino IDE.
- After burning, the ESP32 is ready for deployment.


