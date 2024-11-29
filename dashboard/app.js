const express = require("express");
const app = express();
const mqtt = require("mqtt");

app.use(express.static("public"));

const { exec } = require("child_process");

function alertDoctor(id, name_) {
  console.log("Sending alert");
  const command = `mosquitto_pub -t alert -m "` + id + ` - ` + name_ + `"`;
  exec(command, (error, stdout, stderr) => {
    if (error) {
      console.error(`Error executing script: ${error.message}`);
      return;
    }

    if (stderr) {
      console.error(`Script error: ${stderr}`);
      return;
    }

    console.log(`Script output: ${stdout}`);
  });
}

// Connect to the MQTT broker
const mqttBroker = "mqtt://192.168.0.204";
// const mqttBroker = "mqtt://172.21.4.72";
const mqttClient = mqtt.connect(mqttBroker);

const mqttTopic = "doctorstate";

// Store the doctors' data
let hospitalData = {
  name: "IIT Ropar Hospital",
  assets: [
    { name: "", floor: 1, type: "empty" },
    { name: "", floor: 2, type: "empty" },
    { name: "", floor: 3, type: "empty" },
  ],
};

// Subscribe to the MQTT topic
mqttClient.on("connect", () => {
  console.log("Connected to MQTT broker");
  mqttClient.subscribe(mqttTopic, (err) => {
    if (!err) {
      console.log("Subscribed to topic: ", mqttTopic);
    } else {
      console.error("Failed to subscribe:", err);
    }
  });
});

// Handle incoming MQTT messages
mqttClient.on("message", (topic, message) => {
  console.log("Received message:", topic, message.toString());
  if (topic === mqttTopic) {
    try {
      // Parse the message as JSON
      received_data = message.toString();
      const parts = received_data.split(",");
      const id = parts[0];
      const name = parts[1];
      const floor = parseInt(String(parts[2]).slice(1));
      if (floor == 0) {
        alertDoctor(id, name);
        // hospitalData.assets = hospitalData.assets.filter(
        //   (asset) => String(asset.id) !== id
        // );
        // console.log("Removed asset with id:", id);
        const lostIndex = hospitalData.assets.findIndex(
          (asset) => String(asset.id) === id
        );
        hospitalData.assets[lostIndex].type = "lost";
        return;
      }
      let newType = "equipment";
      if (String(name).substring(0, 2) === "Dr") {
        newType = "doctor";
      }
      const newAsset = {
        id: String(id),
        name: String(name),
        floor: parseInt(floor),
        type: newType,
      };
      const index = hospitalData.assets.findIndex(
        (asset) => String(asset.id) === id
      );
      if (index === -1) {
        hospitalData.assets.push(newAsset);
      } else {
        hospitalData.assets[index] = newAsset;
      }
      console.log("New hospital data:", JSON.stringify(hospitalData));
    } catch (error) {
      console.error("Failed to parse message:", error);
    }
  }
});

// normal apis start here
app.get("/", (req, res) => {
  res.sendFile(__dirname + "/public/index.html");
});

function prepareHospitalData() {
  try {
    hospitalData.assets.sort((a, b) => a.floor - b.floor);
    // console.log("Prepared data:", JSON.stringify(hospitalData));
  } catch (err) {
    console.log(err);
  }
}

app.get("/api/doctors/", (req, res) => {
  // console.log("API Call");
  prepareHospitalData();
  res.json(hospitalData);
  // console.log(JSON.stringify(doctorsData));
});

app.listen(3000, () => {
  console.log("Server running on http://localhost:3000");
});
