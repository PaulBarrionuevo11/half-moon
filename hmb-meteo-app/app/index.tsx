import { useRouter } from "expo-router";
import { useEffect } from "react";
import { ActivityIndicator, View } from "react-native";
import { getProvisioningState } from "../src/storage/provisioning";

export default function Index() {
  const router = useRouter();

useEffect(() => {
  (async () => {
    const state = await getProvisioningState();

    if (!state.isProvisioned) {
      router.replace("/onboarding/intro");
      return;
    }

    // provisioned => ensure local user exists
    const { ensureUser } = await import("../src/storage/user");
    await ensureUser();

    router.replace("/(tabs)/home");

  })();
}, [router]);


  return (
    <View style={{ flex: 1, alignItems: "center", justifyContent: "center" }}>
      <ActivityIndicator />
    </View>
  );
}
