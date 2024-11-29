import React, { useEffect, useState } from 'react';
import { View, Text } from 'react-native';
import mqtt from 'mqtt';

const MqttComponent = () => {
  const [client, setClient] = useState<mqtt.MqttClient | null>(null);
  const [connected, setConnected] = useState(false);
  const [message, setMessage] = useState<string | null>(null);

  useEffect(() => {
    // Configure the MQTT client to connect via TCP
    const mqttClient = mqtt.connect("ws://192.168.0.106:9001", {
      protocol: 'tcp',
      connectTimeout: 5000, // Timeout in milliseconds
    });

    mqttClient.on("connect", () => {
      console.log("Connected to MQTT broker");
      setConnected(true);
      mqttClient.subscribe("alert", (err) => {
        if (err) {
          console.error("Subscription error:", err);
        } else {
          console.log("Subscribed to topic: alert");
        }
      });
    });

    mqttClient.on("message", (topic, payload) => {
      console.log(`Received message on ${topic}: ${payload.toString()}`);
      setMessage(payload.toString());
    });

    mqttClient.on("error", (error) => {
      console.error("Connection error:", error);
    });

    mqttClient.on("offline", () => {
      console.log("MQTT broker is offline");
    });

    mqttClient.on("reconnect", () => {
      console.log("Reconnecting to MQTT broker...");
    });

    mqttClient.on("close", () => {
      console.log("Connection to MQTT broker closed");
    });

    setClient(mqttClient);

    // Cleanup on unmount
    return () => {
      mqttClient.end();
    };
  }, []);

  return (
    <View>
      <Text>Connected: {connected ? "Yes" : "No"}</Text>
      <Text>Message: {message ? message : "No messages yet"}</Text>
    </View>
  );
};

export default MqttComponent;