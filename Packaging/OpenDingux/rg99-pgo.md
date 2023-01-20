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
