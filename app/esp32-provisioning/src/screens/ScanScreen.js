import { useEffect } from "react";
import { Text, View } from "react-native";
import { connectToDevice, scanForESP32 } from "../services/bleService";

export default function ScanScreen({ navigation }) {
  useEffect(() => {
    scanForESP32(async (device) => {
      await connectToDevice(device);
      navigation.replace("WiFi");
    });
  }, []);

  return (
    <View style={{ padding: 30 }}>
      <Text>Scanning for ESP32...</Text>
    </View>
  );
}