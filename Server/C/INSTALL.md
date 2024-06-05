# Installation
* `Note`: This library currently only supports Linux. i have no plans on working for support for windows for this library.
## Prequesites
### Libraries
* **uuid-dev**
### Binaries 
* **Make** (Porobably comes pre-isntalled in yout linux installation)
* **Clang** (preferrably with Clang >= 14.0.6)

### Installing the prequesites
`Note`: Installing prequisites require root previlages
* **Ubuntu/Debian**:
```bash
#Libraries
apt install uuid-dev
#Binaries
apt install build-essential clang make
```

## Installing
Download code source from the [Github page](https://github.com/AbdelhadiSeddar/SNF) or From the [Releases page](https://github.com/AbdelhadiSeddar/SNF/releases)

* unzip it
```bash 
#In case you don't have unzip
apt install unzip
#Unzip the code source
unzip <PATH_TO_DOWNLOADED_CODESOURCE>
``` 
* go to diractory of SNF C Library (Server)
```bash
cd <PATH_TO_UNZIPPED_CODESOURCE>
cd Server/C
```
* `Note` To make sure it compiles correctly run this first
```bash
make
```

* Install it: (Requires Root previlages)
```bash
make install
```
* `Note` You can install this framework as a static library using
```bash
make install_static
```
Done! Enjoy SNF C Library! (Server)
Next Go to [Getting Started](https://docs.abdelhadiseddar.com/snf/c/latest/d9/d5c/md_GET__START.html)
