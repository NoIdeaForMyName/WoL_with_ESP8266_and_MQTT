// CHANGE NAME TO ca_cert.h

#ifndef CA_CERT_H
#define CA_CERT_H

// SSL certificate for MQTT broker
// Load DigiCert Global Root CA ca_cert, which is used by EMQX Serverless Deployment
static const char ca_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----

your certificate here...

-----END CERTIFICATE-----
)EOF";

#endif
