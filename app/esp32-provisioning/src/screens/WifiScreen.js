import { useState } from "react";
import { Button, Text, TextInput, View } from "react-native";
import { sendWifiCredentials } from "../services/bleService";

export default function WifiScreen() {
  const [ssid, setSsid] = useState("");
  const [password, setPassword] = useState("");

  const handleSend = async () => {
    await sendWifiCredentials(ssid, password);
    alert("Credentials Sent!");
  };

  return (
    <View style={{ padding: 30 }}>
      <Text>WiFi Setup</Text>

      <TextInput
        placeholder="SSID"
        value={ssid}
        onChangeText={setSsid}
        style={{ borderWidth: 1, marginBottom: 10 }}
      />

      <TextInput
        placeholder="Password"
        secureTextEntry
        value={password}
        onChangeText={setPassword}
        style={{ borderWidth: 1, marginBottom: 20 }}
      />

      <Button title="Send Credentials" onPress={handleSend} />
    </View>
  );
}