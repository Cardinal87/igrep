# igrep
## Description
Igrep CLI tool is an indexed version of base Unix grep that allow index file before searching and then run fast searching by index
## Features
* Index file or whole directory
* Remove files from index
* Fast searching query by indexed files
* Supported extensions:
  * txt
  * json
  * ini
  * csv
  * log
## Using igrep
Usage: igrep [OPTION] [PARAM]


**`index`** — Add files to index
  * `-f, --file <PATH>` — Index single file.
    * Example: `igrep index -f ./data.txt`
  * `-d, --dir <PATH>` — Recursively index all files in directory.
    * Example: `igrep index -d ./logs`

**`remove`** — Remove file from index
  * `-f, --file <PATH>` — Remove file from index.
    * Example: `igrep remove -f ./old-file.log`

**`find`** — Find using index
  * `-q, --query <QUERY>` — Find query by indexed files.
    * Example: `igrep find -q "ERROR"`

**`help`** — Manual
  * `-h, --help` — Show manual.

**`version`** — Version
  * `-v, --version` — Show version.

## Quick start
Download .deb package from [latest release](https://github.com/cardinal87/igrep/releases/latest) manualy \
or using next command:
``` bash
curl -O https://github.com/Cardinal87/igrep/releases/latest/download/igrep-linux.deb
```
and then install:
``` bash
sudo dpkg -i igrep-linux.deb
```
Run `igrep` in terminal to ensure that all installed correctly
To remove igrep run:
``` bash
sudo dpkg -r igrep
```
## Build from source code
Clone repository:
``` bash
git clone git@github.com:Cardinal87/igrep.git
```
Install requirement packages
``` bash
sudo apt update
sudo apt install -y build-essential cmake
```
and build source code
``` bash
cd igrep
mkdir build
cd build
cmake ../igrep
cmake --build .
```
After this you can install it with
``` bash
cmake --install . --prefix /specify/prefered/path
```
