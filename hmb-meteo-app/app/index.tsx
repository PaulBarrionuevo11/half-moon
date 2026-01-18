import { useRouter } from "expo-router";
import { useEffect } from "react";
import { ActivityIndicator, View } from "react-native";
import { getProvisioningState } from "../src/storage/provisioning";

export default function Index() {
  const router = useRouter();

  useEffect(() => {
    (async () => {
      const state = await getProvisioningState();
      router.replace(state.isProvisioned ? "/home" : "/onboarding/intro");
    })();
  }, [router]);

  return (
    <View style={{ flex: 1, alignItems: "center", justifyContent: "center" }}>
      <ActivityIndicator />
    </View>
  );
}
