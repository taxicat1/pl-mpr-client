# Building

0. Clone the repository. Requires:
	- Make
	- Cygwin/MSYS2/WSL/etc. if using Windows
	- Wine if not using Windows

1. Download a copy of the [Metrowerks C compiler](https://github.com/pret/pmd-sky/raw/workflows/assets/mwccarm.zip) and extract its contents to `tools/`, such that for example `tools/mwccarm/2.0/sp2p2/mwccarm.exe` exists.

2. Download a copy of [Nitro SDK 4.2](https://github.com/pret/pmd-sky/raw/workflows/assets/NitroSDK-4_2-071210-jp.7z), and copy its `tools/bin/` folder into `tools/`, such that for example `tools/bin/makerom.exe` exists.

3. Run `make` in the root of repository, optionally with `-j` to accelerate the process.

4. The output file will be `build/Rom-client.JPN.srl`.
