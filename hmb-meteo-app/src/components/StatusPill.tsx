import React from "react";
import { StyleSheet, Text, View } from "react-native";

function parseIso(ts: string): Date | null {
  const d = new Date(ts);
  return isNaN(d.getTime()) ? null : d;
}

export function computeStatus(ts_received_iso?: string) {
  if (!ts_received_iso) return { label: "Offline", sub: "no data", isOnline: false };

  const d = parseIso(ts_received_iso);
  if (!d) return { label: "Offline", sub: "bad timestamp", isOnline: false };

  const ageMs = Date.now() - d.getTime();
  const ageSec = Math.max(0, Math.floor(ageMs / 1000));

  // buckets
  let bucket = "<1m";
  if (ageSec > 2 * 3600) bucket = ">2h";
  else if (ageSec > 3600) bucket = ">1h";
  else if (ageSec > 300) bucket = ">5m";
  else if (ageSec > 60) bucket = ">1m";

  const isOnline = ageSec <= 60; // your definition; change if you want
  return { label: isOnline ? "Online" : "Offline", sub: bucket, isOnline };
}

export default function StatusPill({ ts }: { ts?: string }) {
  const s = computeStatus(ts);

  return (
    <View style={[styles.pill, s.isOnline ? styles.online : styles.offline]}>
      <Text style={styles.label}>{s.label}</Text>
      <Text style={styles.sub}>{s.sub}</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  pill: {
    paddingVertical: 8,
    paddingHorizontal: 12,
    borderRadius: 999,
    flexDirection: "row",
    alignItems: "center",
    gap: 8,
  },
  online: { backgroundColor: "#52a975" },
  offline: { backgroundColor: "#ff9b30" },
  label: { fontWeight: "700" },
  sub: { opacity: 0.7, fontWeight: "600" },
});
