# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

SteamKeysManager is a Windows desktop application for storing and managing Steam keys and Indiegala links. Built with Embarcadero RAD Studio / C++Builder using the VCL framework and a local Microsoft Access (`.mdb`) database via ADO.

## Build

Open `Steam Keys Database.cbproj` in RAD Studio (version 12 Athens recommended) and use Project > Build, or via command line:

```bat
msbuild "Steam Keys Database.cbproj" /p:Config=Release /p:Platform=Win32
msbuild "Steam Keys Database.cbproj" /p:Config=Debug   /p:Platform=Win32
```

Output goes to `Release\` or `Debug\` respectively. No test suite exists.

## Runtime Requirements

- `SteamDB.mdb` (Microsoft Access database) must be present alongside the `.exe` — it is gitignored and not created automatically by the app.
- [Microsoft Access Database Engine](https://www.microsoft.com/en-us/download/details.aspx?id=54920) must be installed (provides the ACE OLEDB 12.0 provider). Any machine with Microsoft Office/Access already has this.
- Optional: `PC\*.lnk` and `Notebook\*.lnk` shortcut files in the same directory as the `.exe` for the browser launcher tab.

## Architecture

Single-form VCL application. All business logic lives in `Steam_Keys_DB.cpp`; the UI layout is in `Steam_Keys_DB.dfm`.

**Database:** One table — `Keys` — with columns: `Add_date`, `Key_link` (used as the de-facto record identifier in queries), `Game_name`, `Source`, `Trading_cards`, `DLC`, `Other`, `Already_used`, `Notes`.

**Data flow:**
1. `FormShow` connects to `SteamDB.mdb` via ACE OLEDB 12.0 and populates the game list and source combobox.
2. Selecting a game filters the `DBGridKeys_list` grid.
3. Clicking a grid row loads the record into the edit fields.
4. CRUD operations use dedicated query components: `ADOQuerySelect` (SELECT + INSERT), `ADOQueryUpdate` (UPDATE), `ADOQueryDelete` (DELETE).

**Key subsystems:**
- **Sorting/filtering:** `SortBoxChange` (date/count/source) + `SortGroupClick` radio group (tag filters: Trading Cards, DLC, Other, Already Used).
- **Multiselection/export:** Accumulates keys into `Key_buffer` TMemo, formatted for ArchiSteamFarm BGR (`!redeem KEY`) or trade lists.
- **GaOpener tab:** Discovers `.lnk` browser shortcuts and opens them with `ShellExecute`.
- **UI feedback:** `TTimer` resets button captions/colors after operations (timer is disabled at the end of `TimerTimer`).
- **System tray:** Minimize-to-tray and stay-on-top support via `TTrayIcon`.

## Coding Conventions

### SQL — always use parameterized queries

All SQL in this codebase uses ADO named parameters. Never concatenate user input or field values into SQL strings.

```cpp
// Correct — parameterized
ADOQueryDBGrid->Active = false;
ADOQueryDBGrid->SQL->Clear();
ADOQueryDBGrid->SQL->Add("SELECT ... FROM Keys WHERE Game_name = :Game_name");
ADOQueryDBGrid->Parameters->ParamByName("Game_name")->Value = Game_name->Text;
ADOQueryDBGrid->Active = true;

// Wrong — string concatenation (SQL injection risk)
ADOQueryDBGrid->SQL->Text = "SELECT ... WHERE Game_name='" + Game_name->Text + "'";
```

For DML (INSERT/UPDATE/DELETE), use `ExecSQL()` instead of setting `Active = true`. Always call `Active = false` and `SQL->Clear()` before rebuilding a query.

For UPDATE with many fields, use positional `Parameters->Items[N]` in left-to-right order matching the SQL placeholders:

```cpp
ADOQueryUpdate->SQL->Add("UPDATE Keys SET Add_date=:Add_date, Key_link=:Key_link, ... WHERE Key_link=:Key_cache");
ADOQueryUpdate->Parameters->Items[0]->Value = Add_date->DateTime;   // TDateTime, not .DateTimeString()
ADOQueryUpdate->Parameters->Items[1]->Value = Key_link->Text.Trim();
// ...boolean fields as int 0/1, not string "0"/"1"
ADOQueryUpdate->Parameters->Items[5]->Value = Trading_cards->Checked ? 1 : 0;
```

### Guard before dereferencing VCL pointers

`DBGridKeys_list->SelectedField` returns `nullptr` when no column is focused. Always guard before use:

```cpp
if (DBGridKeys_list->SelectedField == nullptr) return;
```

### Known design limitation

`Key_link` is used as the de-facto primary key in all queries — there is no integer primary key and no unique constraint enforced by the application. Duplicate key strings in the database will cause UPDATE and DELETE to affect multiple rows silently. This is a schema-level issue that requires a database migration to fully resolve.
