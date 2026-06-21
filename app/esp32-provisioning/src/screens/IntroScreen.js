import { Button, Text, View } from "react-native";

export default function IntroScreen({ navigation }) {
  return (
    <View style={{ padding: 30 }}>
      <Text style={{ fontSize: 24 }}>ESP32 Setup</Text>

      <Text>1. Power your ESP32</Text>
      <Text>2. Press Connect Device</Text>
      <Text>3. Enter WiFi credentials</Text>

      <Button
        title="Connect Device"
        onPress={() => navigation.navigate("Scan")}
      />
    </View>
  );
}