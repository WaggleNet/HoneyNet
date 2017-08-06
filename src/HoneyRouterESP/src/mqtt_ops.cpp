#include "mqtt_ops.h"

PubSubClient mqclient;

uint8_t mqtt_on = 0;
String esp_chip_id(ESP.getChipId(), HEX);
String base_topic = String("esp-mesh/")+esp_chip_id+"/";
String node_name = String("esp-") + esp_chip_id;

void mqtt_refresh_state() {
    if (mqtt_broker_enable && !mqtt_on) {
        mqtt_on = 1;
        mqclient = PubSubClient(mqtt_broker_address.c_str(), 1883, mqtt_callback, wclient);
        if (mqclient.connect(node_name.c_str())) {
            Serial.print(F("[MQTT] Connected to "));
            Serial.println(mqtt_broker_address);
            Serial.print("Base topic: ");
            Serial.println(base_topic);
            String topic = base_topic + "router/mqtt/status";
            String message = "{\"msg\": \"connected\"}";
            mqclient.publish(topic.c_str(), message.c_str());
            mqclient.subscribe((base_topic + "#").c_str());
        } else {
            Serial.println(F("[MQTT] Connection failed"));
            mqtt_on = 0;
        }
    }
}

void mqtt_loop() {
    if (mqtt_on)
        if (!mqclient.loop()) mqtt_on = 0;
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    Serial.print(F("[MQTT] Received message on ch#"));
    String t(topic);
    // Parse the topic String
    int endslash = t.lastIndexOf('/');
    int startslash = t.lastIndexOf('/', endslash - 1);
    int ch = t.substring(startslash + 1, endslash).toInt();
    endslash = startslash;
    startslash = t.lastIndexOf('/', endslash - 1);
    int nodeID = t.substring(startslash + 1, endslash).toInt();
    Serial.print(ch);
    Serial.print(F(", node#"));
    Serial.println(nodeID);
    mesh.write(payload, ch, length, nodeID);
}
