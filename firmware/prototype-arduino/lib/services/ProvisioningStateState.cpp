#include "ProvisioningServiceState.h"

ProvisioningService::ProvisioningService(
    APService& ap,
    WifiService& wifi,
    WebProvisioningService& web,
    CredentialStorage& storage
    )
    : apService(ap),
      wifiService(wifi),
      webProvisioning(web),
      credentialStorage(storage),
      state(ProvisioningState::BOOT) {}


void ProvisioningService::begin() {
    transitionTo(ProvisioningState::CHECK_CREDENTIALS);
}

void ProvisioningService::update() {
    switch(state)
    {
        case ProvisioningState::CHECK_CREDENTIALS:
            // Check storage file. If I have the credentials, connect to wifi
            // If not, transiiton to start web privisioning
            // if (credentialStorage.hasCredentials())
            // {
            //     transitionTo(ProvisioningState::CONNECTING_WIFI);
            // }
            // else
            // {
            //     transitionTo(ProvisioningState::START_WEB_PROVISIONING);
            // }
            break;

        case ProvisioningState::START_WEB_PROVISIONING:
            apService.begin();
            webProvisioning.begin();
            transitionTo(ProvisioningState::WAITING_FOR_CREDENTIALS);
            break;

        case ProvisioningState::WAITING_FOR_CREDENTIALS:
            webProvisioning.update();

            if(webProvisioning.credentialsReceived())
            {
                transitionTo(ProvisioningState::SAVE_CREDENTIALS);
            }
            break;

        case ProvisioningState::SAVE_CREDENTIALS:
            // credentialStorage.save(
            //     webProvisioning.getSSID(),
            //     webProvisioning.getHostname(),
            //     webProvisioning.getPassword()
            // );

            webProvisioning.clearCredentialsFlag();
            apService.stop();

            transitionTo(ProvisioningState::CONNECTING_WIFI);
            break;

        case ProvisioningState::CONNECTING_WIFI:

        break;

        case ProvisioningState::CONNECTED:
        break;

        case ProvisioningState::ERROR:
        break;
    } 
}