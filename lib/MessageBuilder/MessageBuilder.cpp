#include <MessageBuilder.h>

MessageBuilder::MessageBuilder() {
}

void MessageBuilder::buildConfigrationPayload(const char* node, 
  const char* device_class,
  const char* state_class,
  const char* name, 
  const char* unit_of_measurement, 
  const char* value_template,
  char* payload,
  uint16_t buf_size,
  uint8_t tNSize) {

  char stateTopicName[tNSize];   
  getStateTopicName(node, stateTopicName, tNSize);

  JSONVar jpayload;
  jpayload["device_class"] = device_class;
  jpayload["state_class"] = state_class;
  jpayload["name"] = name;
  jpayload["state_topic"] = stateTopicName;
  jpayload["json_attributes_topic"] = stateTopicName;
  jpayload["unique_id"] = String(name + String("-ESP"));
  jpayload["unit_of_measurement"] = unit_of_measurement;
  jpayload["value_template"] = value_template;
  
  strcpy(payload, JSONVar::stringify(jpayload).c_str());
}

void MessageBuilder::getStatePayload(const char* node, float powerInWatts, 
      float today_energy_consumed_kwh, float today_energy_inserted_kwh, float today_energy_produced_kwh, char* buf, uint8_t buf_size) {
  
  JSONVar statePayload;
  statePayload["node"] = node;
  statePayload["today_energy_consumed_kwh"] = today_energy_consumed_kwh;
  statePayload["today_energy_inserted_kwh"] = today_energy_inserted_kwh;
  statePayload["today_energy_produced_kwh"] = today_energy_produced_kwh;
  statePayload["power"] = powerInWatts;
  
  strcpy(buf, JSONVar::stringify(statePayload).c_str());
} 

void MessageBuilder::getConfigTopicName(const char* node, const char* device_class, char* tName, uint8_t buff_size) {
  strcpy(tName,"homeassistant/sensor/");
  strcat(tName, node);
  strcat(tName, "/");
  strcat(tName, device_class);
  strcat(tName, "/config");
  
}

void MessageBuilder::getStateTopicName(const char* node, char* tName, uint8_t buff_size) {
  strcpy(tName,"homeassistant/sensor/");
  strcat(tName, node);
  strcat(tName, "/state");
}

void MessageBuilder::getPowerConfigurationPayload(const char* node, char* payload, uint16_t buff_size, uint8_t tNSize) {
  char name[64];
  strcpy(name, node);
  strcat(name, "-Power");

  buildConfigrationPayload(node, "power", "measurement", name, "W", "{{value_json.power}}", payload, buff_size, tNSize);
}

void MessageBuilder::getConsumptionConfigurationPayload(const char* node, char* payload, uint16_t buff_size, uint8_t tNSize) {
  char name[64];
  strcpy(name, node);
  strcat(name, "-Consumption");

  buildConfigrationPayload(node, "energy", "total", name, "kWh", "{{value_json.today_energy_consumed_kwh}}", payload, buff_size, tNSize);
}

void MessageBuilder::getInsertionConfigurationPayload(const char* node, char* payload, uint16_t buff_size, uint8_t tNSize) {
  char name[64];
  strcpy(name, node);
  strcat(name, "-Insertion");

  buildConfigrationPayload(node, "energy", "total", name, "kWh", "{{value_json.today_energy_inserted_kwh}}", payload, buff_size, tNSize);
}

void MessageBuilder::getProductionConfigurationPayload(const char* node, char* payload, uint16_t buff_size, uint8_t tNSize) {
  char name[64];
  strcpy(name, node);
  strcat(name, "-Production");

  buildConfigrationPayload(node, "energy", "total", name, "kWh", "{{value_json.today_energy_produced_kwh}}", payload, buff_size, tNSize);
}