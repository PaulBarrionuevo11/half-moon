import AsyncStorage from "@react-native-async-storage/async-storage";

const KEY = "hm_device_store_v1";

export type DeviceStoreState = {
  baseUrl: string;
  deviceId: string;
};

export const DEFAULTS: DeviceStoreState = {
  baseUrl: "http://10.0.0.110:5000",
  deviceId: "HM-0001",
};

export async function loadDeviceStore(): Promise<DeviceStoreState> {
  const raw = await AsyncStorage.getItem(KEY);
  if (!raw) return DEFAULTS;
  try {
    const parsed = JSON.parse(raw);
    return {
      baseUrl: typeof parsed.baseUrl === "string" ? parsed.baseUrl : DEFAULTS.baseUrl,
      deviceId: typeof parsed.deviceId === "string" ? parsed.deviceId : DEFAULTS.deviceId,
    };
  } catch {
    return DEFAULTS;
  }
}

export async function saveDeviceStore(next: DeviceStoreState) {
  await AsyncStorage.setItem(KEY, JSON.stringify(next));
}
