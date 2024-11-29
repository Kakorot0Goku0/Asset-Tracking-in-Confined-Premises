# Asset-Tracking-in-Confined-Premises
Asset tracking systems are vital for managing resources in confined spaces like hotels and hospitals. They reduce loss, enhance efficiency, and provide real-time insights to improve operations and productivity.


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



