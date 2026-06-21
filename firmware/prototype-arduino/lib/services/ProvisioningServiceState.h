#pragma once

#include <Arduino.h>
#include "APService.h"
#include "WifiService.h"
#include "WebProvisioningService.h"
#include "CredentialStorage.h"

enum class ProvisioningState {
    BOOT,
    CHECK_CREDENTIALS,
    CONNECTING_WIFI,
    START_WEB_PROVISIONING,
    WAITING_FOR_CREDENTIALS,
    SAVE_CREDENTIALS,
    CONNECTED,
    ERROR
};

class ProvisioningService {
public:
    ProvisioningService(
        APService& ap,
        WifiService& wifi,
        WebProvisioningService& web,
        CredentialStorage& storage
    );

    void begin();
    void update();

    bool isConnected() const;
    ProvisioningState getState() const;

private:
    APService& apService;
    WifiService& wifiService;
    WebProvisioningService& webProvisioning;
    CredentialStorage& credentialStorage;

    ProvisioningState state;

    void transitionTo(ProvisioningState newState);
};