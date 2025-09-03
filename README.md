# igrep
## Description
Igrep CLI tool is an indexed version of base Unix grep that allows index file before searching and then run fast searching using full-inverted index
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
Usage: igrep [OPTION] [PARAM]\
Possible options and flags:\
**`create`** — Create new index file
  * `-d, --destination <PATH>` — Specify path for index file. **Optional** (default: ~/.config/igrep/index.bin)
    * Example: `igrep create -d ~/myindex.bin`

**`index`** — Add files to index
  * `-f, --file <PATH>` — Index single file.
    * Example: `igrep index -f ./data.txt`
  * `-d, --dir <PATH>` — Recursively index all files in directory.
    * Example: `igrep index -d ./logs`
  * `-s, --source-index <PATH>` — Specify path to source index file. **Optional** (default: ~/.config/igrep/index.bin)
    * Example: `igrep index -d ./logs -s ~/myindex.bin`

**`remove`** — Remove file from index
  * `-f, --file <PATH>` — Remove file from index.
    * Example: `igrep remove -f ./old-file.log`
  * `-s, --source-index <PATH>` — Specify path to source index file. **Optional** (default: ~/.config/igrep/index.bin)
    * Example: `igrep remove -f ./old-log.txt -s ~/myindex.bin`

**`find`** — Find using index
  * `-q, --query <QUERY>` — Find query by indexed files.
    * Example: `igrep find -q "ERROR"`
  * `-s, --source-index <PATH>` — Specify path to source index file. **Optional** (default: ~/.config/igrep/index.bin)
    * Example: `igrep find -q "ERROR" -s ~/myindex.bin`


`-h, --help` — Show manual.\
`-v, --version` — Show version.

## Quick start
Download .deb package from [latest release](https://github.com/cardinal87/igrep/releases/latest) manualy \
or using next command:
``` bash
curl -O -L https://github.com/Cardinal87/igrep/releases/latest/download/igrep-linux.deb
```
and then install:
``` bash
sudo dpkg -i igrep-linux.deb
```
Run `igrep` in terminal to ensure that all installed correctly\
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
