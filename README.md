# SteamKeysManager

A Windows desktop application for storing and managing Steam keys and Indiegala links.

## Features

- Add, edit, and delete Steam keys and Indiegala links
- Tag keys with: Trading Cards, DLC, Alpha/Beta, Already Used
- Add notes to individual entries
- Sort by date added, number of keys per game, or source
- Filter by tag
- Build a key list for trading or for redeeming in ArchiSteamFarm's BGR format

## Requirements

- Windows (Win32)
- Microsoft Access database file `SteamDB.mdb` placed in the same directory as the `.exe`
- *(Optional)* Browser shortcut files (`*.lnk`) in a `PC\` or `Notebook\` subfolder for the GaOpener tab

## Building

Open `Steam Keys Database.cbproj` in **Embarcadero RAD Studio 12 Athens** and use Project > Build, or from the command line:

```bat
msbuild "Steam Keys Database.cbproj" /p:Config=Release /p:Platform=Win32
msbuild "Steam Keys Database.cbproj" /p:Config=Debug   /p:Platform=Win32
```

Output goes to `Release\` or `Debug\` respectively.

> **Note:** `SteamDB.mdb` is not included in the repository (gitignored). You must provide or create it separately. The app will fail to connect on startup if the file is missing.

## License

MIT — Yurii Tiutiunnyk, 2018–2026
