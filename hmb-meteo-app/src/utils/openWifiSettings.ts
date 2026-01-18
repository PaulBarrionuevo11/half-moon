import { Linking, Platform } from "react-native";

export async function openWifiSettings() {
  try {
    if (Platform.OS === "android") {
      const url = "android.settings.WIFI_SETTINGS";
      const can = await Linking.canOpenURL(url);
      if (can) return await Linking.openURL(url);
      return await Linking.openSettings();
    }

    if (Platform.OS === "ios") {
      const url = "App-Prefs:WIFI";
      const can = await Linking.canOpenURL(url);
      if (can) return await Linking.openURL(url);
      return await Linking.openSettings();
    }

    return await Linking.openSettings();
  } catch {
    return await Linking.openSettings();
  }
}
