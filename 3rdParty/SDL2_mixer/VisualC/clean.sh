#!/bin/sh
find . -type f \( -name '*.user' -o -name '*.sdf' -o -name '*.ncb' -o -name '*.suo' \) -print -delete
rm -rvf Win32 */Win32 x64 */x64
