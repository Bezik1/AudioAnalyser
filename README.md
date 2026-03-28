# 🎵 AudioAnalyser

## 💡 Overview

Project focuses on the audio analysis using cpp. It analyzes the wav audio files and use fast fourier transformation
to distinguish the strongest frequencies from the unused ones.

## ⚙️ Building Commands:

```bash
# Initialize build directory
cmake -S . -B build

# Build and run project
cmake --build build --target main && ./build/main

# Builds and run tests
cmake --build build --target tests && ./build/tests

# Build and run documentation
doxygen && open docs/html/index.html

# Set up pre-commit local git hook.
chmod +x .githooks/pre-commit
git config core.hooksPath .githooks
```

## 🛠 Technologies

<p align="center">
    <a href="https://skillicons.dev">
        <img src="https://skillicons.dev/icons?i=cpp,cmake,vscode,github,git" />
    </a>
</p>
