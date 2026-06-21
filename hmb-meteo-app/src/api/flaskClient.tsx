// src/api/flaskClient.ts
import Constants from "expo-constants";

/**
 * Where your Flask server lives.
 * Priority:
 * 1) EXPO_PUBLIC_FLASK_BASE_URL env var
 * 2) app config extra.flaskBaseUrl (optional)
 * 3) fallback to your LAN IP
 *
 * Example:
 * EXPO_PUBLIC_FLASK_BASE_URL="http://10.0.0.110:5000"
 */
const FALLBACK_BASE_URL = "http://10.0.0.110:5000";

function getBaseUrl(): string {
  const env = process.env.EXPO_PUBLIC_FLASK_BASE_URL;
  if (env && env.trim()) return env.trim();

  // Optional: if you set it in app.json/app.config.js under extra
  const extraBase =
    (Constants.expoConfig as any)?.extra?.flaskBaseUrl ??
    (Constants.manifest as any)?.extra?.flaskBaseUrl;

  if (typeof extraBase === "string" && extraBase.trim()) return extraBase.trim();

  return FALLBACK_BASE_URL;
}

const BASE_URL = getBaseUrl();

/** Common shape you log in CSV. You can extend this anytime. */
export type TelemetryRow = {
  ts_received_iso?: string;
  device_id?: string;

  temp_c?: number | string | null;
  humidity?: number | string | null;

  // Your backend uses "luminosity" in a bunch of places; keep it here.
  // If you rename to `lux`, support both (see below).
  luminosity?: number | string | null;

  battery_v?: number | string | null;
  rssi?: number | string | null;

  // allow extra fields without TypeScript exploding
  [key: string]: any;
};

export class ApiError extends Error {
  status: number;
  url: string;
  body?: string;

  constructor(message: string, status: number, url: string, body?: string) {
    super(message);
    this.name = "ApiError";
    this.status = status;
    this.url = url;
    this.body = body;
  }
}

type FetchJsonOptions = {
  method?: "GET" | "POST" | "PUT" | "PATCH" | "DELETE";
  path: string; // e.g. "/latest"
  body?: any;
  timeoutMs?: number;
  headers?: Record<string, string>;
};

async function fetchJson<T>(opts: FetchJsonOptions): Promise<T> {
  const method = opts.method ?? "GET";
  const url = `${BASE_URL}${opts.path.startsWith("/") ? "" : "/"}${opts.path}`;
  const timeoutMs = opts.timeoutMs ?? 8_000;

  const controller = new AbortController();
  const t = setTimeout(() => controller.abort(), timeoutMs);

  try {
    const res = await fetch(url, {
      method,
      headers: {
        Accept: "application/json",
        ...(opts.body ? { "Content-Type": "application/json" } : {}),
        ...(opts.headers ?? {}),
      },
      body: opts.body ? JSON.stringify(opts.body) : undefined,
      signal: controller.signal,
    });

    const text = await res.text();
    const contentType = res.headers.get("content-type") ?? "";

    if (!res.ok) {
      throw new ApiError(
        `Request failed (${res.status})`,
        res.status,
        url,
        text
      );
    }

    // Some Flask endpoints might return empty body
    if (!text) return {} as T;

    if (contentType.includes("application/json")) {
      try {
        return JSON.parse(text) as T;
      } catch {
        throw new ApiError("Invalid JSON response", res.status, url, text);
      }
    }

    // If it returned non-JSON but was ok, still surface it
    return text as unknown as T;
  } catch (e: any) {
    if (e?.name === "AbortError") {
      throw new Error(`Timeout talking to Flask at ${BASE_URL}`);
    }
    // If it's already ApiError, let it bubble
    throw e;
  } finally {
    clearTimeout(t);
  }
}

/**
 * GET /latest
 * Expected: a single latest row (object).
 */
export async function fetchLatest(params?: {
  deviceId?: string;
}): Promise<TelemetryRow> {
  const q = params?.deviceId ? `?device_id=${encodeURIComponent(params.deviceId)}` : "";
  const row = await fetchJson<TelemetryRow>({ path: `/latest${q}` });

  // Compatibility: if backend switches to `lux`, still work:
  if (row.lux == null && row.luminosity != null) row.lux = row.luminosity;
  if (row.luminosity == null && row.lux != null) row.luminosity = row.lux;

  return row;
}

/**
 * GET /history?limit=...
 * Only add this if you have the endpoint.
 */
export async function fetchHistory(params?: {
  deviceId?: string;
  limit?: number;
}): Promise<TelemetryRow[]> {
  const qs = new URLSearchParams();
  if (params?.deviceId) qs.set("device_id", params.deviceId);
  if (typeof params?.limit === "number") qs.set("limit", String(params.limit));

  const suffix = qs.toString() ? `?${qs.toString()}` : "";
  return fetchJson<TelemetryRow[]>({ path: `/history${suffix}` });
}

/**
 * GET /status - good for "offline if last_seen > 1 min"
 * Add it if your server supports it.
 */
export type DeviceStatus = {
  device_id: string;
  last_seen_iso?: string;
  age_seconds?: number;
  is_online?: boolean;
};

export async function fetchStatus(params?: {
  deviceId?: string;
}): Promise<DeviceStatus | DeviceStatus[]> {
  const q = params?.deviceId ? `?device_id=${encodeURIComponent(params.deviceId)}` : "";
  return fetchJson<DeviceStatus | DeviceStatus[]>({ path: `/status${q}` });
}

/**
 * Export base URL in case you want to display it in Debug.
 */
export const FLASK_BASE_URL = BASE_URL;
