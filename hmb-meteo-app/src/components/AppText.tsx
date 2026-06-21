import { StyleSheet, Text, TextProps } from "react-native";

type Variant =
  | "title"
  | "subtitle"
  | "body"
  | "muted"
  | "caption";

type AppTextProps = TextProps & {
  variant?: Variant;
};

export function AppText(props: AppTextProps) {
  const { variant = "body", style, children, ...rest } = props;

  return (
    <Text
      {...rest}
      style={[
        styles.base,
        styles[variant],
        style, // allow overrides per usage
      ]}
    >
      {children}
    </Text>
  );
}

const styles = StyleSheet.create({
  base: {
    color: "#2C2C2E", // iOS-style dark gray (good on white)
  },

  title: {
    paddingTop: 50,
    fontSize: 28,
    fontWeight: "900",
    letterSpacing: -0.5,
  },

  subtitle: {
    fontSize: 18,
    fontWeight: "700",
  },

  body: {
    fontSize: 14,
    fontWeight: "500",
  },

  muted: {
    fontSize: 12,
    fontWeight: "500",
    color: "#8E8E93", // iOS secondary gray
  },

  caption: {
    fontSize: 11,
    fontWeight: "500",
    color: "#8E8E93",
  },
});
