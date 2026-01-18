import AsyncStorage from "@react-native-async-storage/async-storage";
import * as SecureStore from "expo-secure-store";

const KEY_IS_PROVISIONED = "hmb.isProvisioned";
const KEY_WIFI_SSID = "hmb.wifiSsid";
const KEY_WIFI_PASS = "hmb.wifiPass";

export async function getProvisioningState() {
  const [prov, ssid] = await Promise.all([
    AsyncStorage.getItem(KEY_IS_PROVISIONED),
    AsyncStorage.getItem(KEY_WIFI_SSID),
  ]);

  return {
    isProvisioned: prov === "true",
    wifiSsid: ssid,
  };
}

export async function setWifiCredentials(ssid: string, password: string) {
  await AsyncStorage.setItem(KEY_WIFI_SSID, ssid);
  await SecureStore.setItemAsync(KEY_WIFI_PASS, password);
}

export async function markProvisioned(value: boolean) {
  await AsyncStorage.setItem(KEY_IS_PROVISIONED, value ? "true" : "false");
}

export async function resetProvisioning() {
  await AsyncStorage.multiRemove([KEY_IS_PROVISIONED, KEY_WIFI_SSID]);
  await SecureStore.deleteItemAsync(KEY_WIFI_PASS);
}
