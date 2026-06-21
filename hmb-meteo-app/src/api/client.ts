export const DEFAULT_BASE_URL = "http://10.0.0.110:5000";

export function makeApi(baseUrl: string) {
  async function get<T>(path: string): Promise<T> {
    const res = await fetch(`${baseUrl}${path}`, { method: "GET" });
    if (!res.ok) throw new Error(`GET ${path} -> ${res.status}`);
    return (await res.json()) as T;
  }
  return { get };
}
