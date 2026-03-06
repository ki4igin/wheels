#!/usr/bin/env python3
import subprocess
import sys

def get_last_tag():
    try:
        # Получаем последний аннотированный или лёгкий тег
        tag = subprocess.check_output(
            ["git", "describe", "--tags", "--abbrev=0"],
            stderr=subprocess.DEVNULL
        ).strip().decode("utf-8")
    except subprocess.CalledProcessError:
        tag = "v0.0"
    return tag

def get_short_commit():
    try:
        # Короткий хэш текущего коммита
        commit = subprocess.check_output(
            ["git", "rev-parse", "--short", "HEAD"]
        ).strip().decode("utf-8")
    except subprocess.CalledProcessError:
        commit = "no-commit"
    return commit

def main():
    tag = get_last_tag()
    commit = get_short_commit()
    version_str = f"{tag}-{commit}"
    print(version_str)
    sys.exit(0)

if __name__ == "__main__":
    main()
