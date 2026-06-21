import { useRouter } from "expo-router";
import { useState } from "react";
import { Alert, Button, Text, TextInput, View } from "react-native";
// import { getDeviceStatus, provisionDevice } from "../../src/api/deviceProvisioning";
import { markProvisioned, setWifiCredentials } from "../../src/storage/provisioning";
import { ensureUser } from "../../src/storage/user";


export default function ProvisionScreen() {
  const router = useRouter();
  const [ssid, setSsid] = useState("");
  const [password, setPassword] = useState("");
  const [busy, setBusy] = useState(false);
  const [statusText, setStatusText] = useState("");

  const onSend = async () => {
    if (!ssid.trim()) return Alert.alert("SSID required", "Enter your home Wi-Fi name.");
    if (!password.trim()) return Alert.alert("Password required", "Enter your home Wi-Fi password.");

    setBusy(true);
    setStatusText("Checking station connection...");
    try {
      // await getDeviceStatus(); // confirms still connected to SoftAP

      setStatusText("Sending home Wi-Fi to station...");
      // await provisionDevice({
      //   ssid: ssid.trim(),
      //   password,
      //   deviceName: "hmb-01", // optional label; you can change later
      // });

      // Store locally (optional, but matches your earlier design)
      await setWifiCredentials(ssid.trim(), password);
      await ensureUser();          // creates user if none exists
      await markProvisioned(true);

      setStatusText("Done ✅ Going to Home...");
      router.replace("/(tabs)/home");
    } catch (e: any) {
      setStatusText("");
      Alert.alert(
        "Provisioning failed",
        "Make sure you are connected to the station Wi-Fi (hmb-01) before sending.\n\n" +
          (e?.message ?? "")
      );
    } finally {
      setBusy(false);
    }
  };

  return (
    <View style={{ flex: 1, padding: 20, gap: 12, justifyContent: "center" }}>
      <Text style={{ fontSize: 26, fontWeight: "700" }}>Connect station to home Wi-Fi</Text>
      <Text style={{ opacity: 0.7 }}>
        Enter the Wi-Fi network the station should use for Internet (your home Wi-Fi).
      </Text>

      <TextInput
        placeholder="Home Wi-Fi SSID"
        value={ssid}
        onChangeText={setSsid}
        autoCapitalize="none"
        style={{ borderWidth: 1, borderRadius: 12, padding: 12 }}
      />

      <TextInput
        placeholder="Home Wi-Fi password"
        value={password}
        onChangeText={setPassword}
        secureTextEntry
        style={{ borderWidth: 1, borderRadius: 12, padding: 12 }}
      />

      <Button title={busy ? "Working..." : "Send to Meteo Station"} onPress={onSend} disabled={busy} />

      {!!statusText && <Text style={{ marginTop: 8, opacity: 0.8 }}>{statusText}</Text>}
    </View>
  );
}
