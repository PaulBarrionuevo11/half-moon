export type LatestReading = {
  ts_received_iso: string; // server timestamp when it received data
  device_id: string;

  temp_c?: number;
  humidity?: number;
  luminosity?: number;
  battery_v?: number;
  rssi?: number;

  // optional extra fields if you send them
  systemState?: string;
  batteryState?: string;
};
