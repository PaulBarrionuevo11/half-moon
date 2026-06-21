import { AppText } from "@/src/components/AppText";
import React, { useEffect, useMemo, useRef, useState } from "react";
import { AppState, ScrollView, StyleSheet, Text, TextInput, View } from "react-native";
import { getLatest } from "../../src/api/readings";
import SensorCard from "../../src/components/sensorCard";
import StatusPill from "../../src/components/StatusPill";
import { loadDeviceStore, saveDeviceStore } from "../../src/storage/deviceStore";
import { LatestReading } from "../../src/types/models";

function fmtNum(x?: number, digits = 1) {
  if (typeof x !== "number" || !isFinite(x)) return "—";
  return x.toFixed(digits);
}

export default function Home() {
  const [baseUrl, setBaseUrl] = useState<string>("http://10.0.0.110:5000");
  const [deviceId, setDeviceId] = useState<string>("HM-0001");

  const [latest, setLatest] = useState<LatestReading | null>(null);
  const [error, setError] = useState<string | null>(null);
  const [pollMs, setPollMs] = useState<number>(1500); // 1–2s default

  const appState = useRef(AppState.currentState);
  const pollTimer = useRef<ReturnType<typeof setInterval> | null>(null);
  const inFlight = useRef(false);

  // Load saved config once
  useEffect(() => {
    (async () => {
      const s = await loadDeviceStore();
      setBaseUrl(s.baseUrl);
      setDeviceId(s.deviceId);
    })();
  }, []);

  // Save config (debounced-ish)
  useEffect(() => {
    const t = setTimeout(() => {
      saveDeviceStore({ baseUrl, deviceId }).catch(() => {});
    }, 500);
    return () => clearTimeout(t);
  }, [baseUrl, deviceId]);

  const canPoll = useMemo(() => {
    return baseUrl.startsWith("http") && deviceId.trim().length > 0;
  }, [baseUrl, deviceId]);

  async function tick() {
    if (!canPoll) return;
    if (inFlight.current) return;
    inFlight.current = true;
    try {
      const data = await getLatest(baseUrl, deviceId.trim());
      setLatest(data);
      setError(null);
    } catch (e: any) {
      setError(e?.message ?? "Fetch failed");
    } finally {
      inFlight.current = false;
    }
  }

  function startPolling() {
    if (pollTimer.current) return;
    // immediate tick so the UI updates instantly
    tick();
    pollTimer.current = setInterval(() => tick(), pollMs);
  }

  function stopPolling() {
    if (!pollTimer.current) return;
    clearInterval(pollTimer.current);
    pollTimer.current = null;
  }

  // Polling lifecycle: start/stop based on app foreground
  useEffect(() => {
    if (!canPoll) {
      stopPolling();
      return;
    }

    startPolling();

    const sub = AppState.addEventListener("change", (next) => {
      const prev = appState.current;
      appState.current = next;

      // If app goes background/inactive -> stop.
      if (prev === "active" && next !== "active") stopPolling();
      // If comes back -> start.
      if (prev !== "active" && next === "active") startPolling();
    });

    return () => {
      sub.remove();
      stopPolling();
    };
    // IMPORTANT: restart timer if pollMs changes
  }, [canPoll, pollMs]);

  return (
    <ScrollView contentContainerStyle={styles.container}>
      <View style={styles.header}>
        <AppText variant="title">Half Moon • V1</AppText>
      </View>

      <View style={styles.section}>
        <Text style={styles.sectionTitle}>Connection</Text>
        <StatusPill ts={latest?.ts_received_iso} />

        <Text style={styles.label}>Flask base URL</Text>
        <TextInput
          value={baseUrl}
          onChangeText={setBaseUrl}
          autoCapitalize="none"
          autoCorrect={false}
          placeholder="http://10.0.0.110:5000"
          style={styles.input}
        />

        <Text style={styles.label}>Device ID</Text>
        <TextInput
          value={deviceId}
          onChangeText={setDeviceId}
          autoCapitalize="characters"
          autoCorrect={false}
          placeholder="HM-0001"
          style={styles.input}
        />

        <Text style={styles.small}>
          Polling every {Math.round(pollMs / 100) / 10}s • Endpoint: /latest?device_id={encodeURIComponent(deviceId)}
        </Text>

        {error ? <Text style={styles.error}>⚠ {error}</Text> : null}
      </View>

      <View style={styles.grid}>
        <SensorCard title="Temperature" value={fmtNum(latest?.temp_c, 1)} unit="°C" />
        <SensorCard title="Humidity" value={fmtNum(latest?.humidity, 1)} unit="%" />
        <SensorCard title="Luminosity" value={fmtNum(latest?.luminosity, 0)} unit="lux" />
        <SensorCard title="Battery" value={fmtNum(latest?.battery_v, 2)} unit="V" />
        <SensorCard title="RSSI" value={typeof latest?.rssi === "number" ? `${latest?.rssi}` : "—"} unit="dBm" />
        <SensorCard title="Device" value={latest?.device_id ?? "—"} />
      </View>

      <View style={styles.section}>
        <Text style={styles.sectionTitle}>Raw</Text>
        <Text style={styles.mono}>{latest ? JSON.stringify(latest, null, 2) : "No data yet."}</Text>
      </View>
    </ScrollView>
  );
}

const styles = StyleSheet.create({
  container: { padding: 16, paddingBottom: 40, backgroundColor: "#fafafa" },
  header: { flexDirection: "row", justifyContent: "space-between", alignItems: "center", marginBottom: 14 },
  h1: { fontSize: 20, fontWeight: "900" },

  section: {
    backgroundColor: "#fff",
    borderRadius: 16,
    padding: 14,
    borderWidth: 1,
    borderColor: "#eee",
    marginBottom: 14,
  },
  sectionTitle: { fontWeight: "800", marginBottom: 10 },
  label: { fontWeight: "700", opacity: 0.7, marginTop: 8, marginBottom: 6 },
  input: {
    backgroundColor: "#f6f6f6",
    borderRadius: 12,
    paddingHorizontal: 12,
    paddingVertical: 10,
    borderWidth: 1,
    borderColor: "#eee",
  },
  small: { marginTop: 10, opacity: 0.65, fontWeight: "600" },
  error: { marginTop: 10, color: "#B00020", fontWeight: "700" },

  grid: {
    display: "grid" as any, // RN web only; ignored on native
    gap: 12,
  },

  mono: {
    fontFamily: "Courier",
    fontSize: 12,
    backgroundColor: "#f6f6f6",
    padding: 12,
    borderRadius: 12,
    borderWidth: 1,
    borderColor: "#eee",
  },
});
