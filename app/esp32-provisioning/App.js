import { NavigationContainer } from "@react-navigation/native";
import { createNativeStackNavigator } from "@react-navigation/native-stack";

import IntroScreen from "./src/screens/IntroScreen";
import ScanScreen from "./src/screens/ScanScreen";
import WifiScreen from "./src/screens/WifiScreen";

const Stack = createNativeStackNavigator();

export default function App() {
  return (
    <NavigationContainer>
      <Stack.Navigator>
        <Stack.Screen name="Intro" component={IntroScreen} />
        <Stack.Screen name="Scan" component={ScanScreen} />
        <Stack.Screen name="WiFi" component={WifiScreen} />
      </Stack.Navigator>
    </NavigationContainer>
  );
}