# Changelog

## [Unreleased] — Bug fixes (2026-04-30)

All changes are in `Steam_Keys_DB.cpp`. No `.dfm` or `.h` changes.

### Fixed — Critical

- **Null pointer crash on empty grid** (`DBGridKeys_listCellClick`, `DBGridKeys_listMouseWheel`, `Delete_keyClick`): `SelectedField` returns `nullptr` when no column is focused. Added an early-return guard at the top of all three handlers.

- **SQL injection in DELETE** (`Delete_keyClick`): The `DELETE FROM Keys WHERE Key_link = '...'` statement was built by string concatenation from the selected grid cell value. Replaced with a parameterized query using `ADOQueryDelete`.

- **SQL injection in UPDATE** (`Update_keyClick`): All 9 editable fields (`Key_link`, `Game_name`, `Source`, `Notes`, boolean tags, and the `WHERE` key cache) were concatenated directly into the UPDATE statement. Replaced with a fully parameterized query. Also fixed: boolean fields are now passed as `int` 0/1 instead of the string `"0"`/`"1"`, and `Add_date` is passed as `TDateTime` instead of `.DateTimeString()` (avoids locale-dependent string parsing).

### Fixed — High

- **Stale SQL in sort modes 6 and 7** (`SortBoxChange`): Cases 6 and 7 set `TextSQL` but not `TextSQLList`. The list-box query unconditionally used `TextSQLList` after the switch, which would be empty or stale from the previous sort selection. Added the missing `TextSQLList` assignments to both cases.

- **SQL injection in all SELECT queries**: Every grid-refresh SELECT that filtered by `Game_name` or `Key_link` was built by string concatenation. Replaced all 10+ instances across `FormShow`, `DBGridKeys_listCellClick`, `DBGridKeys_listMouseWheel`, `GamesListBoxClick`, `Add_Key_ButtonClick`, `Update_keyClick`, and `Delete_keyClick` with parameterized queries.

- **Trailing space bug in DELETE refresh** (`Delete_keyClick`, `else` branch): The SELECT after a delete used `" ' ORDER BY Add_date"` (space before the closing quote), producing a zero-row result and an unpositioned cursor. Fixed automatically by the parameterization above.

### Fixed — Medium

- **Out-of-bounds crash when no link is selected** (`Delete_linkClick`): `Items->Delete(Links_list->ItemIndex)` was called without checking for `ItemIndex == -1`. Wrapped in a guard.

- **Out-of-bounds crash when no browser is selected** (`Open_linkClick`, `Open_linksClick`): `Browser->Items->Strings[Browser->ItemIndex]` was accessed without checking for `ItemIndex == -1`. Added an early-return guard in both handlers.

### Fixed — Low

- **`FindClose` called on uninitialized `TSearchRec`** (`FormShow`, `DeviceClick`): `FindClose(Rec)` was called unconditionally even when `FindFirst` returned non-zero, leaving `Rec` uninitialized. Moved `FindClose` inside the `if (FindFirst(...) == 0)` block in both locations.

- **Date picker not updated on empty DB** (`Delete_keyClick` empty-DB branch): `Add_date->DateTime.CurrentDateTime()` was a no-op — the return value was discarded. Replaced with `Add_date->DateTime = TDateTime::CurrentDateTime()`.

- **Date picker not updated on invalid key entry** (`Add_Key_ButtonClick`): `Add_date->Date.CurrentDate()` and `Add_date->Time.CurrentTime()` were both no-ops for the same reason. Replaced with a single `Add_date->DateTime = TDateTime::CurrentDateTime()`.

- **Timer fired indefinitely** (`TimerTimer`): The timer reset button captions and colors but never stopped itself, firing on every interval forever. Added `Timer->Enabled = false` at the end of the handler.

- **Post-INSERT grid refresh ran after failed INSERT** (`Add_Key_ButtonClick`): The `ADOQueryDBGrid` refresh block was placed after the closing brace of the `try/catch`, so it ran unconditionally — even when the INSERT threw and the catch block ran. Moved it inside the `try` block.
