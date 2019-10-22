# Reversi

## 概要

1P用リバーシゲーム

![screenshot](./screenshot.png)

- 3種のモード
    1. 自手黒（先攻）
    2. 自手白（後攻）
    3. COM vs COM

- 黒石は`@`、白石は`O`で表示
- 有効手の位置は`*`で表示
- 打つ位置は`[列アルファベット][行番号]`の形式で入力 (e.g. `d3`, `a2`)

## 開発環境

- Ubuntu 18.04.2 LTS (WSL)
- gcc 7.4.0
- GNU Make 4.1

## ビルドと実行

```sh
# GNU Make
$ make

# Out-of-source build
# 実行ファイル：./build/reversi
$ ls ./build
reversi src

# 実行
$ ./build/reversi
Select your turn
1: Black 2: White Other: None (COM vs COM)
>> 1
    a b c d e f g h 
  +-----------------+
1 | - - - - - - - - |
2 | - - - - - - - - |
3 | - - - * - - - - |
4 | - - * O @ - - - |
5 | - - - @ O * - - |
6 | - - - - * - - - |
7 | - - - - - - - - |
8 | - - - - - - - - |
  +-----------------+
@: 2 O: 2
[1] Black(@) >> 
...
```

## ライセンス

[![WTFPL](http://www.wtfpl.net/wp-content/uploads/2012/12/wtfpl-badge-1.png)](http://www.wtfpl.net/)

## 参考情報

- [オセロ・リバーシプログラミング講座 ～勝ち方・考え方～](http://uguisu.skr.jp/othello/)

- [オセロプログラムの作り方](http://hp.vector.co.jp/authors/VA015468/platina/algo/)

- [リバーシプログラムの作り方](http://www.es-cube.net/es-cube/reversi/sample/index.html)