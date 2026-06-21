import { LatestReading } from "../types/models";
import { makeApi } from "./client";

export async function getLatest(baseUrl: string, deviceId: string) {
  const api = makeApi(baseUrl);
  const q = encodeURIComponent(deviceId);
  return api.get<LatestReading>(`/latest?device_id=${q}`);
}
