// app/(tabs)/debug.tsx
import { Text, View } from "react-native";

export default function Debug() {
  return (
    <View style={{ flex: 1, padding: 20, justifyContent: "center" }}>
      <Text style={{ fontSize: 28, fontWeight: "800" }}>Debug</Text>
      <Text style={{ marginTop: 8, opacity: 0.8 }}>
        Device status, RSSI, battery, last-seen timestamps, raw payloads, etc.
      </Text>
    </View>
  );
}
