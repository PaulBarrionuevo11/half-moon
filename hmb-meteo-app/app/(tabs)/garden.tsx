// app/(tabs)/garden.tsx
import { MaterialCommunityIcons } from "@expo/vector-icons";
import { StatusBar } from "expo-status-bar";
import { useEffect, useMemo, useRef, useState } from "react";
import { Pressable, ScrollView, View } from "react-native";
import { fetchLatest, type TelemetryRow } from "../../src/api/flaskClient";
import { AppText } from "../../src/components/AppText";

type PlantProfile = {
  id: string;
  name: string;
  ranges: {
    tempC: [number, number];
    humidity: [number, number];
    lux: [number, number];
  };
  tips: {
    lowLux: string;
    highLux: string;
    lowHum: string;
    highHum: string;
    lowTemp: string;
    highTemp: string;
  };
};

const PLANTS: PlantProfile[] = [
  {
    id: "pothos",
    name: "Pothos (easy)",
    ranges: { tempC: [18, 28], humidity: [35, 70], lux: [300, 2500] },
    tips: {
      lowLux: "Light is low for Pothos. Move closer to a bright window (indirect) or add a small grow light.",
      highLux: "Light is very strong. Keep it out of direct sun to avoid leaf burn.",
      lowHum: "Humidity is low. Consider grouping plants, a pebble tray, or a humidifier.",
      highHum: "Humidity is high. Improve airflow to prevent mold/fungus issues.",
      lowTemp: "Temperature is low. Keep it away from cold drafts or windows at night.",
      highTemp: "Temperature is high. Avoid hot direct sun and ensure airflow.",
    },
  },
  {
    id: "snake",
    name: "Snake Plant (low light tolerant)",
    ranges: { tempC: [16, 30], humidity: [20, 60], lux: [50, 2000] },
    tips: {
      lowLux: "Light is very low, but Snake plants tolerate it. Growth will be slower—consider a brighter spot.",
      highLux: "Very bright light is okay if indirect. Avoid harsh direct sun for long periods.",
      lowHum: "Humidity is fine (Snake plants do well in lower humidity).",
      highHum: "Humidity is high. Ensure soil dries between watering and keep airflow.",
      lowTemp: "Temperature is a bit low. Keep above ~16°C to avoid stress.",
      highTemp: "Temperature is high. Avoid hot direct sun and reduce watering frequency.",
    },
  },
  {
    id: "succulent",
    name: "Succulent (bright light)",
    ranges: { tempC: [18, 32], humidity: [15, 50], lux: [2500, 20000] },
    tips: {
      lowLux: "Light is too low for succulents. Move to a much brighter window or use a grow light.",
      highLux: "Light is strong—great for succulents. Watch for heat buildup behind glass.",
      lowHum: "Low humidity is fine for succulents.",
      highHum: "Humidity is high for succulents. Increase airflow and reduce watering.",
      lowTemp: "Temperature is low. Keep warm and avoid cold nights near windows.",
      highTemp: "Temperature is high. Ensure airflow and avoid extreme midday heat behind glass.",
    },
  },
  {
    id: "fern",
    name: "Fern (humidity lover)",
    ranges: { tempC: [18, 26], humidity: [50, 85], lux: [200, 2000] },
    tips: {
      lowLux: "Light is low; ferns can tolerate it. Ensure it’s not in deep darkness.",
      highLux: "Light is too strong for many ferns. Prefer bright indirect light, not direct sun.",
      lowHum: "Humidity is low for a fern. This is the #1 issue—use a humidifier or place in a bathroom with light.",
      highHum: "Humidity is great for ferns. Keep airflow to avoid fungal issues.",
      lowTemp: "Temperature is low. Keep it warmer and away from drafts.",
      highTemp: "Temperature is high. Increase humidity and airflow.",
    },
  },
];

const GOALS = [
  { id: "general", name: "General check" },
  { id: "growth", name: "Grow faster" },
  { id: "stable", name: "Keep stable" },
  { id: "avoid_mold", name: "Avoid mold" },
] as const;

type GoalId = (typeof GOALS)[number]["id"];

type Sample = {
  ts: number; // local time when sampled
  serverTs?: string; // row.ts_received_iso
  tempC: number | null;
  humidity: number | null;
  lux: number | null;
};

const DURATION_MS = 5 * 60_000; // 5 minutes
const SAMPLE_MS = 5_000; // sample every 5 seconds

function toNumberOrNull(v: unknown): number | null {
  if (typeof v === "number") return Number.isFinite(v) ? v : null;
  if (typeof v === "string") {
    const cleaned = v.replace(",", ".");
    const n = parseFloat(cleaned);
    return Number.isFinite(n) ? n : null;
  }
  return null;
}

function avg(nums: (number | null)[]) {
  const ok = nums.filter((n): n is number => typeof n === "number" && Number.isFinite(n));
  if (!ok.length) return null;
  return ok.reduce((a, b) => a + b, 0) / ok.length;
}

function fmt(v: number | null, decimals: number) {
  if (v == null) return "—";
  return v.toFixed(decimals);
}

function clamp01(x: number) {
  return Math.max(0, Math.min(1, x));
}

function scoreMetric(value: number | null, range: [number, number]) {
  if (value == null) return 0;
  const [lo, hi] = range;
  if (value >= lo && value <= hi) return 1;

  // soft scoring: within 20% outside range => 0.5, else 0
  const span = Math.max(1e-6, hi - lo);
  const dist = value < lo ? (lo - value) : (value - hi);
  const ratio = dist / span;
  if (ratio <= 0.2) return 0.5;
  return 0;
}

function verdictFromScore(s: number) {
  if (s >= 0.85) return { emoji: "✅", label: "Good" };
  if (s >= 0.5) return { emoji: "⚠️", label: "Okay" };
  return { emoji: "❌", label: "Not ideal" };
}

function makeFeedback(profile: PlantProfile, t: number | null, h: number | null, lx: number | null) {
  const tips: string[] = [];

  if (t != null) {
    const [lo, hi] = profile.ranges.tempC;
    if (t < lo) tips.push(profile.tips.lowTemp);
    else if (t > hi) tips.push(profile.tips.highTemp);
  }

  if (h != null) {
    const [lo, hi] = profile.ranges.humidity;
    if (h < lo) tips.push(profile.tips.lowHum);
    else if (h > hi) tips.push(profile.tips.highHum);
  }

  if (lx != null) {
    const [lo, hi] = profile.ranges.lux;
    if (lx < lo) tips.push(profile.tips.lowLux);
    else if (lx > hi) tips.push(profile.tips.highLux);
  }

  // if everything is in range, give a positive
  if (tips.length === 0) {
    tips.push("This spot looks within the ideal range for this plant. Keep it consistent and monitor weekly.");
  }

  // return top 3 max
  return tips.slice(0, 3);
}

export default function GardenTab() {
  const [plantId, setPlantId] = useState<string>(PLANTS[0].id);
  const [goalId, setGoalId] = useState<GoalId>("general");

  const profile = useMemo(
    () => PLANTS.find((p) => p.id === plantId) ?? PLANTS[0],
    [plantId]
  );

  const [running, setRunning] = useState(false);
  const [busy, setBusy] = useState(false);
  const [err, setErr] = useState<string>("");

  const [samples, setSamples] = useState<Sample[]>([]);
  const startMsRef = useRef<number | null>(null);
  const lastServerTsRef = useRef<string | null>(null);

  const [nowMs, setNowMs] = useState(() => Date.now());
  useEffect(() => {
    const t = setInterval(() => setNowMs(Date.now()), 1_000);
    return () => clearInterval(t);
  }, []);

  const elapsedMs = useMemo(() => {
    if (!startMsRef.current) return 0;
    return Math.max(0, nowMs - startMsRef.current);
  }, [nowMs]);

  const remainingMs = Math.max(0, DURATION_MS - elapsedMs);
  const progress = clamp01(elapsedMs / DURATION_MS);

  const averages = useMemo(() => {
    const t = avg(samples.map((s) => s.tempC));
    const h = avg(samples.map((s) => s.humidity));
    const lx = avg(samples.map((s) => s.lux));
    return { t, h, lx };
  }, [samples]);

  const score = useMemo(() => {
    const sT = scoreMetric(averages.t, profile.ranges.tempC);
    const sH = scoreMetric(averages.h, profile.ranges.humidity);
    const sL = scoreMetric(averages.lx, profile.ranges.lux);
    return (sT + sH + sL) / 3;
  }, [averages.t, averages.h, averages.lx, profile]);

  const verdict = verdictFromScore(score);
  const feedback = useMemo(
    () => makeFeedback(profile, averages.t, averages.h, averages.lx),
    [profile, averages.t, averages.h, averages.lx]
  );

  const sampleOnce = async () => {
    try {
      const row: TelemetryRow = await fetchLatest();

      const tempC = toNumberOrNull((row as any)?.temp_c);
      const humidity = toNumberOrNull((row as any)?.humidity);

      // ✅ If your field is `lux` not `luminosity`, change this line:
      const lux = toNumberOrNull((row as any)?.luminosity);

      const serverTs = (row as any)?.ts_received_iso as string | undefined;

      // If server timestamp didn't change, still allow sampling (sensor values might change),
      // but you can also skip duplicates by uncommenting below.
      // if (serverTs && lastServerTsRef.current === serverTs) return;

      if (serverTs) lastServerTsRef.current = serverTs;

      const s: Sample = {
        ts: Date.now(),
        serverTs,
        tempC,
        humidity,
        lux,
      };

      setSamples((prev) => [...prev, s]);
      setErr("");
    } catch (e: any) {
      setErr(e?.message ?? "Failed to sample");
    }
  };

  const startCheck = async () => {
    setErr("");
    setSamples([]);
    startMsRef.current = Date.now();
    lastServerTsRef.current = null;
    setRunning(true);

    // take one sample immediately
    await sampleOnce();
  };

  const stopCheck = () => {
    setRunning(false);
    startMsRef.current = null;
  };

  // sampling loop
  useEffect(() => {
    if (!running) return;

    const id = setInterval(() => {
      sampleOnce();
    }, SAMPLE_MS);

    return () => clearInterval(id);
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [running]);

  // auto-stop at 5 minutes
  useEffect(() => {
    if (!running) return;
    if (elapsedMs >= DURATION_MS) {
      setRunning(false);
      startMsRef.current = null;
    }
  }, [running, elapsedMs]);

  const mm = Math.floor(remainingMs / 60_000);
  const ss = Math.floor((remainingMs % 60_000) / 1000);
  const timeLeft = `${mm}:${String(ss).padStart(2, "0")}`;

  return (
    <ScrollView
      style={{ flex: 1, backgroundColor: "#FFFFFF" }}
      contentContainerStyle={{
        paddingHorizontal: 22,
        paddingTop: 22,
        paddingBottom: 28,
        gap: 16,
      }}
    >
      <StatusBar style="dark" />

      <AppText variant="title" style={{ fontSize: 24 }}>
        My Garden
      </AppText>

      {/* Plant selector */}
      <View
        style={{
          borderRadius: 18,
          padding: 14,
          borderWidth: 1,
          borderColor: "rgba(0,0,0,0.06)",
          backgroundColor: "rgba(0,0,0,0.035)",
          gap: 10,
        }}
      >
        <AppText style={{ fontSize: 12, opacity: 0.6 }}>Plant</AppText>

        <View style={{ flexDirection: "row", flexWrap: "wrap", gap: 10 }}>
          {PLANTS.map((p) => {
            const selected = p.id === plantId;
            return (
              <Pressable
                key={p.id}
                onPress={() => setPlantId(p.id)}
                style={{
                  paddingVertical: 10,
                  paddingHorizontal: 12,
                  borderRadius: 999,
                  borderWidth: 1,
                  borderColor: selected ? "rgba(0,0,0,0.14)" : "rgba(0,0,0,0.06)",
                  backgroundColor: selected ? "rgba(0,0,0,0.08)" : "rgba(255,255,255,0.6)",
                }}
              >
                <AppText style={{ fontWeight: "800", color: "#2C2C2E" }}>
                  {p.name}
                </AppText>
              </Pressable>
            );
          })}
        </View>
      </View>

      {/* Goal selector */}
      <View
        style={{
          borderRadius: 18,
          padding: 14,
          borderWidth: 1,
          borderColor: "rgba(0,0,0,0.06)",
          backgroundColor: "rgba(0,0,0,0.035)",
          gap: 10,
        }}
      >
        <AppText style={{ fontSize: 12, opacity: 0.6 }}>Goal</AppText>

        <View style={{ flexDirection: "row", flexWrap: "wrap", gap: 10 }}>
          {GOALS.map((g) => {
            const selected = g.id === goalId;
            return (
              <Pressable
                key={g.id}
                onPress={() => setGoalId(g.id)}
                style={{
                  paddingVertical: 10,
                  paddingHorizontal: 12,
                  borderRadius: 999,
                  borderWidth: 1,
                  borderColor: selected ? "rgba(0,0,0,0.14)" : "rgba(0,0,0,0.06)",
                  backgroundColor: selected ? "rgba(0,0,0,0.08)" : "rgba(255,255,255,0.6)",
                }}
              >
                <AppText style={{ fontWeight: "800", color: "#2C2C2E" }}>
                  {g.name}
                </AppText>
              </Pressable>
            );
          })}
        </View>
      </View>

      {/* Spot check control */}
      <View
        style={{
          borderRadius: 18,
          padding: 14,
          borderWidth: 1,
          borderColor: "rgba(0,0,0,0.06)",
          backgroundColor: "rgba(0,0,0,0.035)",
          gap: 12,
        }}
      >
        <View style={{ flexDirection: "row", alignItems: "center", justifyContent: "space-between" }}>
          <View style={{ gap: 4 }}>
            <AppText style={{ fontSize: 14, fontWeight: "900", color: "#2C2C2E" }}>
              5-minute Spot Check
            </AppText>
            <AppText style={{ fontSize: 12, opacity: 0.6 }}>
              Samples every 5 seconds • compares to {profile.name}
            </AppText>
          </View>

          {running ? (
            <View style={{ flexDirection: "row", alignItems: "center", gap: 10 }}>
              <AppText style={{ fontSize: 14, fontWeight: "900", color: "#2C2C2E" }}>
                {timeLeft}
              </AppText>
              <Pressable
                onPress={stopCheck}
                style={{
                  width: 42,
                  height: 42,
                  borderRadius: 999,
                  alignItems: "center",
                  justifyContent: "center",
                  borderWidth: 1,
                  borderColor: "rgba(0,0,0,0.08)",
                  backgroundColor: "rgba(255,59,48,0.10)",
                }}
              >
                <MaterialCommunityIcons name="stop" size={20} color="rgba(255,59,48,0.9)" />
              </Pressable>
            </View>
          ) : (
            <Pressable
              onPress={async () => {
                setBusy(true);
                try {
                  await startCheck();
                } finally {
                  setBusy(false);
                }
              }}
              disabled={busy}
              style={{
                paddingVertical: 10,
                paddingHorizontal: 14,
                borderRadius: 999,
                backgroundColor: "rgba(0,0,0,0.08)",
                borderWidth: 1,
                borderColor: "rgba(0,0,0,0.10)",
                opacity: busy ? 0.6 : 1,
              }}
            >
              <AppText style={{ fontWeight: "900", color: "#2C2C2E" }}>
                {busy ? "Starting…" : "Start"}
              </AppText>
            </Pressable>
          )}
        </View>

        {/* progress bar */}
        <View
          style={{
            height: 8,
            borderRadius: 99,
            backgroundColor: "rgba(0,0,0,0.06)",
            overflow: "hidden",
          }}
        >
          <View
            style={{
              height: "100%",
              width: `${Math.round(progress * 100)}%`,
              backgroundColor: "rgba(0,0,0,0.25)",
            }}
          />
        </View>

        {err ? (
          <AppText style={{ fontSize: 12, opacity: 0.7 }}>
            Error: {err}
          </AppText>
        ) : null}
      </View>

      {/* Results card */}
      <View
        style={{
          borderRadius: 22,
          padding: 18,
          borderWidth: 1,
          borderColor: "rgba(0,0,0,0.06)",
          backgroundColor: "rgba(0,0,0,0.035)",
          gap: 12,
        }}
      >
        <View style={{ flexDirection: "row", alignItems: "baseline", justifyContent: "space-between" }}>
          <AppText style={{ fontSize: 16, fontWeight: "900", color: "#2C2C2E" }}>
            Result
          </AppText>

          <AppText style={{ fontSize: 16, fontWeight: "900", color: "#2C2C2E" }}>
            {verdict.emoji} {verdict.label}
          </AppText>
        </View>

        <AppText style={{ fontSize: 12, opacity: 0.6 }}>
          Based on {samples.length} samples • Goal: {GOALS.find((g) => g.id === goalId)?.name}
        </AppText>

        {/* Averages */}
        <View style={{ gap: 10, marginTop: 6 }}>
          <View style={{ flexDirection: "row", justifyContent: "space-between" }}>
            <AppText style={{ opacity: 0.6 }}>Avg Temperature</AppText>
            <AppText style={{ fontWeight: "900", color: "#2C2C2E" }}>
              {fmt(averages.t, 1)} °C
            </AppText>
          </View>

          <View style={{ flexDirection: "row", justifyContent: "space-between" }}>
            <AppText style={{ opacity: 0.6 }}>Avg Humidity</AppText>
            <AppText style={{ fontWeight: "900", color: "#2C2C2E" }}>
              {fmt(averages.h, 0)} %
            </AppText>
          </View>

          <View style={{ flexDirection: "row", justifyContent: "space-between" }}>
            <AppText style={{ opacity: 0.6 }}>Avg Luminosity</AppText>
            <AppText style={{ fontWeight: "900", color: "#2C2C2E" }}>
              {averages.lx == null ? "—" : `${Math.round(averages.lx)} lx`}
            </AppText>
          </View>
        </View>

        {/* Feedback */}
        <View style={{ gap: 8, marginTop: 10 }}>
          <AppText style={{ fontSize: 12, opacity: 0.6 }}>Feedback</AppText>
          {feedback.map((t, idx) => (
            <AppText key={idx} style={{ color: "#2C2C2E", fontWeight: "700" }}>
              • {t}
            </AppText>
          ))}
        </View>

        {/* show target ranges (nice for transparency) */}
        <View style={{ gap: 6, marginTop: 10 }}>
          <AppText style={{ fontSize: 12, opacity: 0.6 }}>Target ranges</AppText>
          <AppText style={{ fontSize: 12, opacity: 0.75 }}>
            Temp: {profile.ranges.tempC[0]}–{profile.ranges.tempC[1]} °C •
            Hum: {profile.ranges.humidity[0]}–{profile.ranges.humidity[1]}% •
            Light: {profile.ranges.lux[0]}–{profile.ranges.lux[1]} lx
          </AppText>
        </View>
      </View>
    </ScrollView>
  );
}
