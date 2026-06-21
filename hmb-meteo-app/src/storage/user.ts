import AsyncStorage from "@react-native-async-storage/async-storage";

const KEY_USER = "hmb.user";

export type UserProfile = {
  userId: string;       // random UUID
  name?: string;        // optional
  createdAtIso: string; // ISO date
};

function uuidv4() {
  // good enough for local ids; if you prefer, install `expo-crypto` and use randomUUID
  return "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx".replace(/[xy]/g, (c) => {
    const r = (Math.random() * 16) | 0;
    const v = c === "x" ? r : (r & 0x3) | 0x8;
    return v.toString(16);
  });
}

export async function getUser(): Promise<UserProfile | null> {
  const raw = await AsyncStorage.getItem(KEY_USER);
  if (!raw) return null;
  try {
    return JSON.parse(raw) as UserProfile;
  } catch {
    return null;
  }
}

export async function ensureUser(): Promise<UserProfile> {
  const existing = await getUser();
  if (existing) return existing;

  const created: UserProfile = {
    userId: uuidv4(),
    createdAtIso: new Date().toISOString(),
  };

  await AsyncStorage.setItem(KEY_USER, JSON.stringify(created));
  return created;
}

export async function setUserName(name: string) {
  const user = (await getUser()) ?? (await ensureUser());
  const updated = { ...user, name };
  await AsyncStorage.setItem(KEY_USER, JSON.stringify(updated));
  return updated;
}

export async function clearUser() {
  await AsyncStorage.removeItem(KEY_USER);
}
