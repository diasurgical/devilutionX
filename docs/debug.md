## Introduction

If you compile the game in debug, you have multiple debug features available.

## Debug commands

In-game you have the possibility to use the chat to trigger debug commands.
This is currently a replacement for a console.

| Command | Description |
| ------- | ----------- |
| `help` | Shows a list of all debug commands with descriptions. |
| `god` | Toggles godmode. |
| ... | For the other commands see `help` ingame and there are a lot. |

Tip: Debug commands are also supported in quick messages. If you need a debug command frequently, put it in a quick message. :wink:

## Command-line parameters

| Command | Description |
| ------- | ----------- |
| `+` | Executes a debug command when loading the first game. For exampel `+god` or `+changelevel 1 +spawn 4 skeleton`. |
| `-f` | Display frames per second. |
| `-i` | Disable network timeout. |
| `-n` | Disable startup video. |

## In-game hotkeys

| Hotkey | Description |
| ------ | ----------- |
| `Shift` | While holding, you can use the mouse to scroll screen. |
| `m` | Print debug monster info. |
| `M` | Switch current debug monster. |
| `x` | Toggles `DebugToggle` variable. `DebugToggle` is a generic solution for temporary toggles needed for debugging. |
