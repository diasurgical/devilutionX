# lldb debugging enhancements

The code in this directory is imported via `.lldbinit`.

Working directory `.lldbinit` is not loaded by default.

You can add the following to `~/.lldbinit` to load it when launching `lldb` from the command line:

```
settings set target.load-cwd-lldbinit true
```

If you're using VS Code, you can instead add the following to your configuration:

```json
"lldb.launch.initCommands": ["command source ${workspaceFolder}/.lldbinit"]
```
