#ifndef API_HANDLER_H
#define API_HANDLER_H

struct ApiConfig {
  String serverIP;   // e.g. "192.168.178.59"
  String sensorId;  // default tag like "livingroom"
};

void setApiConfig(const ApiConfig& config);
IPAddress mDNSResolveIP(const String& domain_name);

// Optional extra tag (e.g. pot = "basil"), pass emtyString if unused
bool postSensorData(
    float value,
    const String& measurement,
    const String& extraTagKey = "",
    const String& extraTagValue = ""
);

#endif // API_HANDLER_H
