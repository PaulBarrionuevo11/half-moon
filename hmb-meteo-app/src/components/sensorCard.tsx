import React from "react";
import { StyleSheet, Text, View } from "react-native";

export default function SensorCard({
  title,
  value,
  unit,
}: {
  title: string;
  value: string;
  unit?: string;
}) {
  return (
    <View style={styles.card}>
      <Text style={styles.title}>{title}</Text>
      <View style={styles.row}>
        <Text style={styles.value}>{value}</Text>
        {unit ? <Text style={styles.unit}>{unit}</Text> : null}
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  card: {
    backgroundColor: "#fff",
    borderRadius: 16,
    padding: 14,
    borderWidth: 1,
    borderColor: "#eee",
  },
  title: { opacity: 0.7, fontWeight: "600", marginBottom: 8 },
  row: { flexDirection: "row", alignItems: "baseline", gap: 6 },
  value: { fontSize: 22, fontWeight: "800" },
  unit: { fontSize: 14, fontWeight: "700", opacity: 0.6 },
});
