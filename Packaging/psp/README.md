
# Building for PSP

Install psptoolchain from https://github.com/pspdev/psptoolchain

It should have set the ENV var "PSPDEV" to "/usr/local/pspdev" and added "$PSPDEV/bin" to PATH automatically. If not, just do it as stated on the github page.

After that you should run build.sh from this folder ("Packaging/psp)" and it will put EBOOT.PBP under "build/".