import { Buffer } from "buffer";
import { BleManager } from "react-native-ble-plx";
import { CHARACTERISTIC_UUID, SERVICE_UUID } from "../constants/bleUUIDs";

const manager = new BleManager();

let connectedDevice = null;

export const scanForESP32 = (onDeviceFound) => {
  manager.startDeviceScan(null, null, (error, device) => {
    if (error) {
      console.log(error);
      return;
    }

    if (device.name?.includes("ESP32")) {
      manager.stopDeviceScan();
      onDeviceFound(device);
    }
  });
};

export const connectToDevice = async (device) => {
  connectedDevice = await device.connect();
  await connectedDevice.discoverAllServicesAndCharacteristics();
  return connectedDevice;
};

export const sendWifiCredentials = async (ssid, password) => {
  if (!connectedDevice) return;

  const payload = JSON.stringify({ ssid, password });
  const base64Data = Buffer.from(payload).toString("base64");

  await connectedDevice.writeCharacteristicWithResponseForService(
    SERVICE_UUID,
    CHARACTERISTIC_UUID,
    base64Data
  );
};