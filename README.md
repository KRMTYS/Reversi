# Reversi

## Overview

A reversi game for 2 players

![screenshot](./screenshot.png)

## Description

- Disks are indicated by characters `@` (Black) and `O` (White)
- Coordinate: row character (`a` to `h`) + column number (`1` to `8`)
- A player can put the disk in turn by input coordinate

## Testing Environment

- Windows10 64bit + MinGW
    - gcc version 6.3.0
    - GNU Make 3.82.90

- Windows Subsystem for Linux: Ubuntu 14.04.5 LTS
    - gcc version 4.8.4
    - GNU Make 3.81

## Build and Run

Windows + MinGW
```
$ cd ./src
$ mingw32-make
$ ./reversi.exe
```

Linux
```
$ cd ./src
$ make
$ ./reversi
```

## License

[![WTFPL](http://www.wtfpl.net/wp-content/uploads/2012/12/wtfpl-badge-1.png)](http://www.wtfpl.net/)

## References (Japanese)

- [オセロ・リバーシプログラミング講座 ～勝ち方・考え方～](http://uguisu.skr.jp/othello/)
- [オセロプログラムの作り方](http://hp.vector.co.jp/authors/VA015468/platina/algo/)