#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mosquitto.h>
#include <sqlite3.h>
#include <signal.h>
#include <unistd.h>

#define MQTT_BROKER_HOST "192.168.0.204"
#define MQTT_BROKER_PORT 1883
#define MQTT_TOPIC_SUBSCRIBE "esp32/F1"
#define MQTT_TOPIC_PUBLISH "doctorstate"
#define DATABASE_PATH "~/Downloads/mqtt_masala/hospital.db"
#define LOG_FILE "~/Downloads/mqtt_masala/logfile.txt"

#define MAX_BEACON 10
struct mosquitto *mosq = NULL;

struct beacon {
    char beacon_name[10];
    char name[25];
    int rssi;
    time_t time_l;
    char floor[3];
};

struct beacon beacon_new[MAX_BEACON]; /

// Callback function to be called when the timer expires
void timer_callback(union sigval sv) {
    time_t now;
    time(&now);

    for (int i = 0; i < MAX_BEACON; i++) { // Use the fixed size of the array
        if(strcmp(beacon_new[i].beacon_name,"")==0)
        {
                break;
        }
        if (difftime(now, beacon_new[i].time_l) > 21) { // Compare time_t values
                strcpy(beacon_new[i].floor,"F0");            
                //beacon_new[i].floor = "Fn"; // Set to "Fn" if expired
        }

        // Prepare MQTT publish message
        char publish_msg[100];
        snprintf(publish_msg, sizeof(publish_msg), "%s,%s,%s", 
                 beacon_new[i].beacon_name, beacon_new[i].name, beacon_new[i].floor);


        printf("\npublish msg : %s\n",publish_msg);
        // Publish to MQTT topic
        int pub_rc = mosquitto_publish(mosq, NULL, MQTT_TOPIC_PUBLISH, 
                          strlen(publish_msg), publish_msg, 0, false);

        if (pub_rc != MOSQ_ERR_SUCCESS) {
            fprintf(stderr, "Failed to publish message: %s\n", 
                    mosquitto_strerror(pub_rc));
        }
        beacon_new[i].rssi = -500; // Reset RSSI
    }
    printf("Timer expired! Callback function called.\n");
}

void on_message(struct mosquitto *mosq, void *userdata, 
                const struct mosquitto_message *message) {
    // Create a copy of payload to avoid modifying original
    char payload_copy[message->payloadlen + 1];
    memcpy(payload_copy, message->payload, message->payloadlen);
    payload_copy[message->payloadlen] = '\0';

    printf("Received message: %s\n", payload_copy);

    // Parse incoming message
    char *beacon_no = strtok(payload_copy, ",");
    char *floor_no = strtok(NULL, ",");
    char *rssi_no_s = strtok(NULL, ",");
    int rssi_no = *((int*)&payload_copy[13]); // Convert to integer
    if(rssi_no<-60)
        return;

    printf("rssi Value : %d",rssi_no);

    if (beacon_no == NULL || floor_no == NULL || rssi_no == 0) {
        printf("Invalid message format\n");
        return;
    }
    // Connect to SQLite database
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc = sqlite3_open(DATABASE_PATH, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    // Prepare SQL query to get name for beacon
    const char *sql = "SELECT name FROM beacon_data WHERE beacon_no = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    // Bind beacon number to quer


    rc = sqlite3_bind_text(stmt, 1, beacon_no, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to bind beacon number: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }

     time_t now;
    time(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    // Execute query
    char *name = NULL;
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        name = strdup((const char *)sqlite3_column_text(stmt, 0));
        printf("Found name: %s\n", name);
    } else if (rc == SQLITE_DONE) {
        printf("No matching beacon found for %s\n", beacon_no);

        return;
    } else {
        fprintf(stderr, "Failed to execute query: %s\n", sqlite3_errmsg(db));
    }

    // Finalize statement and close database
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    // Update the required beacon_new data
    for (int i = 0; i < MAX_BEACON; i++) {
        if (strcmp(beacon_new[i].beacon_name, beacon_no) == 0) {
           if (beacon_new[i].rssi <= rssi_no) {
                strcpy(beacon_new[i].floor,floor_no);
              //  beacon_new[i].floor = floor_no;
                beacon_new[i].rssi = rssi_no;

                beacon_new[i].time_l = now; // Store current time
            }
            break;
        }
         else if (strcmp(beacon_new[i].beacon_name, "") == 0) {


                strcpy(beacon_new[i].beacon_name,beacon_no);
                // beacon_new[i].beacon_name = strdup(beacon_no);

                strcpy(beacon_new[i].name,name);
        //       beacon_new[i].name = name; // Assign name directly
        //      printf("name in beacon_new : %s",beacon_new[i].name);
                strcpy(beacon_new[i].floor,floor_no);
        //    beacon_new[i].floor = floor_no;
            beacon_new[i].rssi = rssi_no;
            beacon_new[i].time_l = now; // Store current time
            break;
        }
    }


    // If name found, log and publish
    if (name) {
        // Write to log file
        FILE *log_file = fopen(LOG_FILE, "a");
        if (log_file) {
            fprintf(log_file, "%s,%s,%s,%s,%d\n", 
                    beacon_no, name, floor_no, ctime(&now), rssi_no);
            fclose(log_file);
        }
        free(name); // Free allocated memory for name
    }
}

int main() {

    for (int i = 0; i < MAX_BEACON; i++) 
    {    
        beacon_new[i].beacon_name[0] = '\0';
        beacon_new[i].name[0] = '\0';
        beacon_new[i].rssi = 0;
        beacon_new[i].time_l = 0;
        beacon_new[i].floor[0] = '\0';
    }
    // Initialize Mosquitto library
    mosquitto_lib_init();

    // Create Mosquitto client
    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        fprintf(stderr, "Failed to create Mosquitto client\n");
        return 1;
    }

    // Connect to MQTT broker
    int connect_rc = mosquitto_connect(mosq, MQTT_BROKER_HOST, MQTT_BROKER_PORT, 60);
    if (connect_rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Failed to connect to MQTT broker: %s\n", 
                mosquitto_strerror(connect_rc));
        mosquitto_destroy(mosq);
        return 1;
    }

    // Subscribe to input topic
    int sub_rc = mosquitto_subscribe(mosq, NULL, MQTT_TOPIC_SUBSCRIBE, 0);
    if (sub_rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Failed to subscribe: %s\n", mosquitto_strerror(sub_rc));
        mosquitto_disconnect(mosq);
        mosquitto_destroy(mosq);
        return 1;
    }

    // Set message callback
    mosquitto_message_callback_set(mosq, on_message);

    // Start the MQTT loop
    printf("Listening for messages on topic %s\n", MQTT_TOPIC_SUBSCRIBE);

    // Create a timer
    timer_t timer_id;
    struct sigevent sev;
    struct itimerspec its;

    // Set up the signal event
    sev.sigev_notify = SIGEV_THREAD; // Notify via a thread
    sev.sigev_notify_function = timer_callback; // Callback function
    sev.sigev_value.sival_ptr = &timer_id; // Value passed to the callback
    sev.sigev_notify_attributes = NULL; // Default attributes

    // Create the timer
    if (timer_create(CLOCK_REALTIME, &sev, &timer_id) == -1) {
        perror("timer_create");
        exit(EXIT_FAILURE);
    }

    // Set the timer to expire after 1 second, and then every 1 second
    its.it_value.tv_sec = 1;  // Initial expiration after 1 second
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 10; // Repeat every 1 second
    its.it_interval.tv_nsec = 0;

    // Start the timer
    if (timer_settime(timer_id, 0, &its, NULL) == -1) {
        perror("timer_settime");
        exit(EXIT_FAILURE);
    }

    // Keep the main program running
    printf("Timer started. It will call the callback function every second.\n");
    mosquitto_loop_forever(mosq, -1, 1);

    // Cleanup
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();


    // Cleanup: delete the timer
    if (timer_delete(timer_id) == -1) {
        perror("timer_delete");
        exit(EXIT_FAILURE);
    }

    return 0;
}

