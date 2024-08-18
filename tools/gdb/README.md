# gdb debugging enhancements

Requires gdb v14.1+.

The code in this directory is imported via `.gdbinit`.

Working directory `.gdb` is not loaded by default.

You can run gdb with `-iex 'add-auto-load-safe-path .'` to load it.

For example:

```bash
gdb -iex 'add-auto-load-safe-path .' build/devilutionx
```

If you're using VS Code with CMake, you can instead add the following to your `.vscode/settings.json`:

```json
"cmake.debugConfig": {
  "setupCommands": [
    {
      "description": "Enable pretty-printing for gdb",
      "text": "-enable-pretty-printing",
      "ignoreFailures": true
    },
    {
      "description": "Load gdb enhancements",
      "text": "source ${workspaceFolder}/tools/gdb/devilution_gdb/__init__.py",
      "ignoreFailures": false
    }
  ]
}
```
