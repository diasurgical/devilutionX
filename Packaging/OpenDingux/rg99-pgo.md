# RG99 profile-guided optimization

The RG99 build must be PGO'd for reasonable performance.

Here are the instructions for producing a PGO'd build.

1.  Install <https://github.com/diasurgical/devilutionx-mpq-tools>

2.  Build the OPK for profiling data collection:

    ```sh
    TOOLCHAIN=/opt/rs90-toolchain Packaging/OpenDingux/build.sh rg99 --profile-generate
    ```

3.  Copy the OPK to RG99 (`rg99` is 10.1.1.3):

    ```sh
    scp -O build-rg99/devilutionx-rg99.opk rg99:/media/sdcard/apps
    ```

4.  Now, run the OPK. It will run the timedemo instead of the actual game and will take about 1 hour (due to heavy swapping).


5.  Copy the profiling data from RG99:

    ```sh
    rm -rf /tmp/devilutionx-profile
    scp -r -O rg99:/media/data/local/home/devilutionx-profile /tmp/devilutionx-profile
    ```

6.  Build the OPK use the collected profiling data:

    ```sh
    TOOLCHAIN=/opt/rs90-toolchain Packaging/OpenDingux/build.sh rg99 --profile-use --profile-dir /tmp/devilutionx-profile
    ```

7.  The final package is at `build-rg99/devilutionx-rg99.opk`.

## Remote Debugging with VS Code

If the demo crashes and you cannot reproduce this on PC, you can
use a remote debugger to diagnose the issue.

Unpack the package and copy it to the RG99:

```bash
cd build-rg99
rm -rf squashfs-root
unsquashfs devilutionx-rg99.opk
ssh rg99 'rm -rf /media/data/local/home/squashfs-root'
scp -r -O squashfs-root/ rg99:/media/data/local/home/squashfs-root
```

Then, on RG99, prepare the demo files and run `gdbserver`:

```bash
mkdir -p demo
cp -r squashfs-root/demo_0* demo
cp -r squashfs-root/spawn_0_sv demo
cd squashfs-root
gdbserver 10.1.1.1:8001 devilutionx --diablo --spawn --demo 0 --timedemo \
  --save-dir ~/demo --data-dir ~/.local/share/diasurgical/devilution
```

Then, on the PC, add the following VS Code configuration to `.vscode/launch.json`:

```json
{
	"name": "rg99 remote debug",
	"type": "cppdbg",
	"request": "launch",
	"program": "build-rg99/devilutionx",
	"stopAtEntry": true,
	"miDebuggerPath": "/opt/rs90-toolchain/bin/mipsel-linux-gdb",
	"miDebuggerArgs": "-ix /opt/rs90-toolchain/mipsel-rs90-linux-musl/sysroot/usr/share/buildroot/gdbinit",
	"MIMode": "gdb",
	"miDebuggerServerAddress": "10.1.1.3:8001",
	"targetArchitecture": "mips",
	"additionalSOLibSearchPath": "/opt/rs90-toolchain/mipsel-rs90-linux-musl/sysroot",
	"setupCommands": [
		{
			"description": "Enable pretty-printing for gdb",
			"text": "-enable-pretty-printing",
			"ignoreFailures": true
		}
	],
	"externalConsole": false,
	"cwd": "${workspaceFolder}"
}
```

Finally, run the configuration from the "Run and Debug" VS Code tab.
