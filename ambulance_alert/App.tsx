import React, { useEffect, useState } from 'react';
import { View, Text, StyleSheet, FlatList } from 'react-native';
import mqtt from 'mqtt';
import Sound from 'react-native-sound';
import Icon from 'react-native-vector-icons/MaterialIcons';

// Load the sound file
Sound.setCategory('Playback');
const notificationSound = new Sound('notification.mp3', Sound.MAIN_BUNDLE, (error) => {
  if (error) {
    console.log('Failed to load the sound', error);
  } else {
    console.log('Sound loaded successfully');
  }
});

const MqttComponent = () => {
  const [client, setClient] = useState<mqtt.MqttClient | null>(null);
  const [connected, setConnected] = useState(false);
  const [messages, setMessages] = useState<{ text: string; timestamp: string }[]>([]);

  useEffect(() => {
    const mqttClient = mqtt.connect("ws://192.168.0.106:9001");

    mqttClient.on("connect", () => {
      console.log("Connected to Mosquitto broker");
      setConnected(true);
      mqttClient.subscribe("alert", (err) => {
        if (err) {
          console.error("Subscription error:", err);
        } else {
          console.log("Subscribed to topic: test/topic");
        }
      });
    });

    mqttClient.on("error", (err) => {
      console.error("Connection error:", err);
    });

    mqttClient.on("message", (topic, payload) => {
  const messageText = payload.toString();
  const timestamp = new Date().toLocaleTimeString(); // Get the current time
  console.log( {messageText}, {topic});
  
  setMessages((prevMessages) => [
    ...prevMessages,
    { text: messageText, timestamp }, // Store both message and timestamp
  ]);

  // Play the notification sound
  notificationSound.play((success) => {
    if (!success) {
      console.log('Sound playback failed');
    }
  });
});

    setClient(mqttClient);

    return () => {
      mqttClient.end();
      notificationSound.release(); // Release the sound when done
    };
  }, []);

  return (
    <View style={styles.container}>
      <View style={styles.header}>
        <Text style={styles.headerText}>Emergency notifications</Text>
      </View>
      
      <View style={styles.body}>
        <Text style={[styles.status, connected ? styles.connected : styles.disconnected]}>
          {connected ? "Connected" : "Disconnected"}
        </Text>
        <FlatList
  data={messages}
  keyExtractor={(item, index) => index.toString()}
  renderItem={({ item }) => (
    <View style={styles.messageContainer}>
      <Icon name="alarm" size={24} color="red" style={styles.icon} />
      <View style={styles.messageContent}>
        <Text style={styles.message}>{item.text}</Text>
        <Text style={styles.timestamp}>{item.timestamp}</Text>
      </View>
    </View>
  )}
/>
    </View>
      <View style={styles.footer}>
        <Text style={styles.footerText}>Home</Text>
      </View>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#f5f5f5',
  },
  header: {
    backgroundColor: '#6200ee',
    width: '100%',
    padding: 20,
  },
  headerText: {
    color: '#ffffff',
    fontSize: 20,
    textAlign: 'center',
  },
  body: {
    flex: 1,
    width: '100%',
    padding: 10,
  },
  status: {
    fontSize: 16,
    textAlign: 'center',
    marginBottom: 10,
  },
  connected: {
    color: 'green',
  },
  disconnected: {
    color: 'red',
  },
  message: {
    fontSize: 16,
    padding: 5,
    borderBottomWidth: 1,
    borderBottomColor: '#cccccc',
  },
  footer: {
    padding: 10,
    backgroundColor: '#6200ee',
    width: '100%',
  },
  footerText: {
    color: '#ffffff',
    textAlign: 'center',
  },
  messageContainer: {
    flexDirection: 'row',
    alignItems: 'center',
    padding: 5,
    borderBottomWidth: 1,
    borderBottomColor: '#cccccc',
  },
  icon: {
    marginRight: 10,
  },
  messageContent: {
    flexDirection: 'column',
  },
  timestamp: {
    fontSize: 12,
    color: '#888888',
    marginTop: 2,
  },
});

export default MqttComponent;