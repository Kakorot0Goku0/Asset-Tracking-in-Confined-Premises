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



