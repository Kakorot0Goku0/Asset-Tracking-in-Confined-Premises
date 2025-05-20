## 🌐 Dashboard

This dashboard is built using **Node.js** and the **Express** framework. It runs on a local server and receives data from a **Raspberry Pi**, displaying a summarized overview of the collected data.

### 📦 Requirements

- [Node.js](https://nodejs.org/) and npm installed on your system
- Use the command `npm install` to install the required dependencies.
- Install nodemon globally using the command `npm install -g nodemon`.
- Run the dashboard using the command `nodemon app.js`.
- The dashboard will be hosted on `localhost:3000`.


## Publishing stats for different doctors on different floors
mosquitto_pub -t doctorstats -h MQTT_BROKER_IP -m 'R24030307,nitin,F2'

mosquitto_pub -t doctorstats -h MQTT_BROKER_IP -m 'R24030308,npar,F2'

mosquitto_pub -t doctorstats -h MQTT_BROKER_IP -m 'R24030393,prashnt,F3'

mosquitto_pub -t doctorstats -h MQTT_BROKER_IP -m 'R1ba9ffa1,adw,F2'

mosquitto_pub -t doctorstats -h MQTT_BROKER_IP -m 'R1ba9ffa2,eve,F1'

mosquitto_pub -t doctorstats -h MQTT_BROKER_IP -m 'R1ba9ffa3,esv,F2'

mosquitto_pub -t doctorstats -h MQTT_BROKER_IP -m 'R1ba9ffa4,loj,F2'
