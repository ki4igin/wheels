#!/usr/bin/env python3
import sys

COLORS = {
    "red": "\033[0;31m",
    "green": "\033[0;32m",
    "yellow": "\033[0;33m",
    "reset": "\033[0m",
}

def print_mixed(color, colored_part, normal_part):
    color_code = COLORS.get(color.lower(), "")
    reset = COLORS["reset"]
    print(f"{color_code}{colored_part}{reset}{normal_part}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: print_color.py <color> <colored_text> [normal_text]")
        sys.exit(1)

    color = sys.argv[1]
    colored_text = sys.argv[2]
    normal_text = " ".join(sys.argv[3:]) if len(sys.argv) > 3 else ""
    print_mixed(color, colored_text, normal_text)
