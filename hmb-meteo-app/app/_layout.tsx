// app/_layout.tsx
import { Stack } from "expo-router";

export default function RootLayout() {
  return (
    <Stack screenOptions={{ headerShown: false }}>
      {/* Onboarding screens live under /onboarding/... */}
      <Stack.Screen name="onboarding" />

      {/* Tabs live under /(tabs)/... */}
      <Stack.Screen name="(tabs)" />
    </Stack>
  );
}
