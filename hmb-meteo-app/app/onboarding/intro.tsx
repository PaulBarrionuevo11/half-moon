import { useRouter } from "expo-router";
import { useState } from "react";
import { Alert, Button, Text, View } from "react-native";
// import { getDeviceStatus } from "../../src/api/deviceProvisioning";
import { AppText } from "@/src/components/AppText";
import { openWifiSettings } from "../../src/utils/openWifiSettings";

export default function IntroScreen() {
  const router = useRouter();
  const [busy, setBusy] = useState(false);

  const onImConnected = async () => {
    setBusy(true);
    try {
      // This confirms the phone is connected to the station SoftAP,
      // because only then should 192.168.4.1 be reachable.
      // await getDeviceStatus();
      router.push("/onboarding/provision");
    } catch (e: any) {
      Alert.alert(
        "Not connected to hmb-01 yet",
        "Connect your phone to the station Wi-Fi:\n\nWi-Fi name: hmb-01\nPassword: XXXXX\n\nThen come back and try again.\n\n" +
          (e?.message ?? "")
      );
    } finally {
      setBusy(false);
    }
  };

  return (
    <View style={{ flex: 1, padding: 20, gap: 14, justifyContent: "center" }}>
      <AppText variant="title"> 
        First-time setup 
      </AppText>

      <View style={{ borderWidth: 1, borderRadius: 14, padding: 14, gap: 10 }}>
        <AppText variant="subtitle"> 
          Steps
        </AppText>

        <AppText variant="body"> 
          1️⃣ Power on your Meteo Station.
        </AppText>
        <AppText variant="body"> 
          2️⃣ Wait for a few seconds.
        </AppText>
        <AppText variant="body"> 
          3️⃣ On your phone, open Wi-Fi settings and connect to:
          {"\n"}• Wi-Fi: <Text style={{ fontWeight: "700" }}>hmb-01</Text>
          {"\n"}• Password: <Text style={{ fontWeight: "700" }}>123456789</Text>
        </AppText>
        <AppText variant="body"> 
          4️⃣ Return here and tap “I’m connected”.
        </AppText>
      </View>

      <Button title="Open Wi-Fi Settings" onPress={openWifiSettings} disabled={busy} />
      <Button title={busy ? "Checking..." : "I’m connected to hmb-01"} onPress={onImConnected} disabled={busy} />

      <Text style={{ fontSize: 12, opacity: 0.65, marginTop: 8 }}>
        Tip: If your phone warns “No Internet”, that’s normal during setup.
      </Text>
    </View>
  );
}
