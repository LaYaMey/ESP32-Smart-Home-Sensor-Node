#ifndef API_HANDLER_H
#define API_HANDLER_H

struct ApiConfig {
  const char* baseUrl;   // e.g. "http://192.168.178.59:8000"
  const char* sensorId;  // default tag like "livingroom"
};

void setApiConfig(const ApiConfig& config);
IPAddress mDNSResolveIP(const char* domain_name);

// Optional extra tag (e.g. pot = "basil"), pass nullptr if unused
bool postSensorData(float value, const char* measurement, const char* extraTagKey = nullptr, const char* extraTagValue = nullptr);

#endif // API_HANDLER_H
