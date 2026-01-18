import { useRouter } from "expo-router";
import { useEffect, useState } from "react";
import { Alert, Button, Text, View } from "react-native";
import { getProvisioningState, resetProvisioning } from "../src/storage/provisioning";

export default function Home() {
  const router = useRouter();
  const [ssid, setSsid] = useState<string | null>(null);

  useEffect(() => {
    (async () => {
      const s = await getProvisioningState();
      setSsid(s.wifiSsid);
    })();
  }, []);

  const onReset = async () => {
    Alert.alert("Reset setup?", "Forget saved setup and start again.", [
      { text: "Cancel", style: "cancel" },
      {
        text: "Reset",
        style: "destructive",
        onPress: async () => {
          await resetProvisioning();
          router.replace("/onboarding/intro");
        },
      },
    ]);
  };

  return (
    <View style={{ flex: 1, padding: 20, gap: 12, justifyContent: "center" }}>
      <Text style={{ fontSize: 28, fontWeight: "800" }}>Home</Text>
      <Text style={{ opacity: 0.8 }}>Provisioned ✅</Text>
      <Text style={{ opacity: 0.8 }}>Home Wi-Fi SSID: {ssid ?? "—"}</Text>
      <Button title="Reset provisioning" onPress={onReset} />
    </View>
  );
}
