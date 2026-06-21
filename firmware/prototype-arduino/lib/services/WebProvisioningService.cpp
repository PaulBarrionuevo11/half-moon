#include "WebProvisioningService.h"

WebProvisioningService::WebProvisioningService()
    : server(80),
      receivedCredentials(false),
      ssid(""),
      password(""),
      hostname("") {}

void WebProvisioningService::begin() {
    server.on("/", HTTP_GET, [this]() {
        handleRoot();
    });

    server.on("/submit", HTTP_POST, [this]() {
        handleSubmit();
    });

    server.onNotFound([this]() {
        handleNotFound();
    });

    server.begin();
    Serial.println("[WebProvisioning] Web server started on port 80");
}

void WebProvisioningService::update() {
    server.handleClient();
}

bool WebProvisioningService::credentialsReceived() const {
    return receivedCredentials;
}

String WebProvisioningService::getSSID() const {
    return ssid;
}

String WebProvisioningService::getPassword() const {
    return password;
}

String WebProvisioningService::getHostname() const {
    return hostname;
}

void WebProvisioningService::clearCredentialsFlag() {
    receivedCredentials = false;
}

void WebProvisioningService::handleRoot() {
    server.send(200, "text/html", buildSetupPage());
}

void WebProvisioningService::handleSubmit() {
    if (!server.hasArg("ssid") || !server.hasArg("password")) {
        server.send(400, "text/plain", "Missing SSID or password");
        return;
    }

    ssid = server.arg("ssid");
    password = server.arg("password");
    hostname = server.arg("hostname");

    ssid.trim();
    password.trim();
    hostname.trim();

    if (ssid.length() == 0) {
        server.send(400, "text/plain", "SSID cannot be empty");
        return;
    }

    if (hostname.length() == 0) {
        hostname = "halfmoon-station";
    }

    receivedCredentials = true;

    String response =
        "<!DOCTYPE html><html><head>"
        "<meta name='viewport' content='width=device-width, initial-scale=1'>"
        "<title>Half Moon Setup</title>"
        "</head><body>"
        "<h2>Credentials received</h2>"
        "<p>Half Moon will now try to connect to your WiFi network.</p>"
        "<p>You may close this page.</p>"
        "</body></html>";

    server.send(200, "text/html", response);

    Serial.println("[WebProvisioning] Credentials received:");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Hostname: ");
    Serial.println(hostname);
}

void WebProvisioningService::handleNotFound() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
}

String WebProvisioningService::buildSetupPage() {
    return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Half Moon WiFi Setup</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 24px;
            background: #f5f5f5;
        }
        .card {
            max-width: 420px;
            margin: auto;
            background: white;
            padding: 24px;
            border-radius: 14px;
            box-shadow: 0 4px 18px rgba(0,0,0,0.12);
        }
        h2 {
            margin-top: 0;
        }
        label {
            display: block;
            margin-top: 16px;
            font-weight: bold;
        }
        input {
            width: 100%;
            padding: 10px;
            margin-top: 6px;
            box-sizing: border-box;
            font-size: 16px;
        }
        button {
            width: 100%;
            margin-top: 22px;
            padding: 12px;
            font-size: 16px;
            border: none;
            border-radius: 8px;
            background: #222;
            color: white;
        }
        .note {
            font-size: 13px;
            color: #666;
            margin-top: 12px;
        }
    </style>
</head>
<body>
    <div class="card">
        <h2>Half Moon Setup</h2>
        <p>Connect your station to your home WiFi network.</p>

        <form action="/submit" method="POST">
            <label for="ssid">WiFi Network Name</label>
            <input type="text" id="ssid" name="ssid" placeholder="Home WiFi SSID" required>

            <label for="password">WiFi Password</label>
            <input type="password" id="password" name="password" placeholder="WiFi password">

            <label for="hostname">Station Hostname</label>
            <input type="text" id="hostname" name="hostname" placeholder="halfmoon-station">

            <button type="submit">Save and Connect</button>
        </form>

        <p class="note">
            After saving, the station will leave setup mode and try to join your home WiFi.
        </p>
    </div>
</body>
</html>
)rawliteral";
}