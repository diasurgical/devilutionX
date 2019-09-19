# DevilutionX OpenDingus Port

To build DevilutionX for OpenDingux, you first need to build the
cross-compilation toolchain. This can take about an hour or longer and requires
~9 GiB of space:

~~~bash
Packaging/OpenDingux/build-toolchain.sh
~~~

Then, run this from the root DevilutionX directory to compile and package the binary:

~~~bash
nice make -j`nproc` -f Packaging/OpenDingux/Makefile
~~~
