#!/usr/bin/env python

import datetime
import glob
import os
import sys
import pathlib
import subprocess

from packaging.version import Version


# Version 0.22 is the first version to support c++-format
_MIN_XGETTEXT_VERSION = Version("0.22")

_HEADER = """
#, fuzzy
msgid ""
msgstr ""
"Project-Id-Version: DevilutionX\\n"
"POT-Creation-Date: {timestamp}\\n"
"PO-Revision-Date: \\n"
"Last-Translator: \\n"
"Language-Team: \\n"
"MIME-Version: 1.0\\n"
"Content-Type: text/plain; charset=UTF-8\\n"
"Content-Transfer-Encoding: 8bit\\n"
"X-Generator: xgettext {xgettext_version}\\n"
"X-Poedit-SourceCharset: UTF-8\\n"
"X-Poedit-KeywordsList: ;_;N_;ngettext:1,2;pgettext:1c,2;P_:1c,2\\n"
"X-Poedit-Basepath: ..\\n"
"X-Poedit-SearchPath-0: Source\\n"

"""


class Error(Exception):
    def __init__(self, message):
        self.message = message

    def __str__(self):
        return self.message


def get_xgettext() -> tuple[str, str]:
    xgettext_bin = os.environ.get("XGETTEXT_BIN", "xgettext")
    result: subprocess.CompletedProcess = subprocess.run(
        [xgettext_bin, "--version"],
        stdout=subprocess.PIPE,
        stderr=sys.stderr,
        check=True,
    )
    version_str = result.stdout.split(b"\n")[0].split(b" ")[-1].decode()
    version = Version(version_str.split("-")[0])
    if version < _MIN_XGETTEXT_VERSION:
        raise Error(
            f"{xgettext_bin} version {version} is too low,"
            + f" minimum required is {_MIN_XGETTEXT_VERSION}"
        )
    return xgettext_bin, version_str


def run_xgettext():
    xgettext_binary, xgettext_version = get_xgettext()
    files = []
    for ext in ["h", "c", "hpp", "cpp"]:
        files.extend(glob.glob(f"Source/**/*.{ext}", recursive=True))
    files.sort()

    result: subprocess.CompletedProcess = subprocess.run(
        [
            xgettext_binary,
            "--output=-",
            "--keyword",
            "--keyword=_",
            "--flag=_:1:pass-c++-format",
            "--keyword=N_",
            "--flag=N_:1:pass-c++-format",
            "--keyword=ngettext:1,2",
            "--flag=ngettext:1:pass-c++-format",
            "--flag=ngettext:2:pass-c++-format",
            "--keyword=pgettext:1c,2",
            "--flag=pgettext:2:pass-c++-format",
            "--keyword=P_:1c,2",
            "--flag=P_:2:pass-c++-format",
            "--flag=runtime:1:c++-format",
            "--flag=Log:1:c++-format",
            "--flag=LogError:1:c++-format",
            "--flag=LogWarn:1:c++-format",
            "--flag=LogInfo:1:c++-format",
            "--flag=LogDebug:1:c++-format",
            "--flag=LogVerbose:1:c++-format",
            "--language=C++",
            "--from-code=UTF-8",
            "--add-comments=TRANSLATORS: ",
            # Omitting the header causes spurious warnings:
            # "msgid contains non-ASCII characters".
            #
            # To avoid the warnings, we generate the header
            # but skip writing it later.
            # "--omit-header",
            "--debug",
            *files,
        ],
        stdout=subprocess.PIPE,
        stderr=sys.stderr,
        check=True,
    )

    timestamp = datetime.datetime.now(datetime.UTC).strftime("%Y-%m-%d %H:%M:%S%z")
    with open("Translations/devilutionx.pot", "wb") as f:
        f.write(
            _HEADER.format(
                timestamp=timestamp,
                xgettext_version=xgettext_version,
            ).encode()
        )
        f.write(b"\n\n".join(result.stdout.split(b"\n\n")[1:]))


def main():
    os.chdir(pathlib.Path(__file__).resolve().parent.parent)
    try:
        run_xgettext()
    except Error as e:
        print("Error:", e, file=sys.stderr)
        return 1
    except subprocess.CalledProcessError as e:
        print("Error:", e.cmd[0], "failed", file=sys.stderr)
        return e.returncode


main()
