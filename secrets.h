#include <pgmspace.h>
 
#define SECRET
#define THINGNAME "ESP_32"                         //Thing Name
 
const char WIFI_SSID[] = "xxx";               //WiFi Username
const char WIFI_PASSWORD[] = "xxx";           //WiFi Password
const char AWS_IOT_ENDPOINT[] = "xxx";       //End Point Name
// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)EOF";
 
// Device Certificate                                               //change this
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
 
 
)KEY";
 
// Device Private Key                                               //change this
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----

-----END RSA PRIVATE KEY-----
 
)KEY";
