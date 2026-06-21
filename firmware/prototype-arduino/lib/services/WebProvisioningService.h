#pragma once

#include <Arduino.h>
#include <WebServer.h>

class WebProvisioningService {
public:
    WebProvisioningService();

    void begin();
    void update();

    bool credentialsReceived() const;

    String getSSID() const;
    String getPassword() const;
    String getHostname() const;

    void clearCredentialsFlag();

private:
    WebServer server;

    bool receivedCredentials;

    String ssid;
    String password;
    String hostname;

    void handleRoot();
    void handleSubmit();
    void handleNotFound();

    String buildSetupPage();
};