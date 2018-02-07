# Reversi

## 概要

1P用リバーシゲーム

![screenshot](./screenshot.png)

## 詳細

- コンソール上に盤面が表示される
    - 黒石：`@` 白石：`O`
- プレイヤーは先攻、黒石で固定
- 石を置く座標は
    `列アルファベット[a~h]+行番号[1~8]`
    と指定して入力できる
    ```
    [1] Black: d3
    ```

## 開発･テスト環境

- Windows10 64bit + MinGW
    - gcc version 6.3.0
    - GNU Make 3.82.90

- Windows Subsystem for Linux: Ubuntu 14.04.5 LTS
    - gcc version 4.8.4
    - GNU Make 3.81

## ライセンス

[![WTFPL](http://www.wtfpl.net/wp-content/uploads/2012/12/wtfpl-badge-1.png)](http://www.wtfpl.net/)

## 参考

- [オセロ・リバーシプログラミング講座 ～勝ち方・考え方～](http://uguisu.skr.jp/othello/)
- [オセロプログラムの作り方](http://hp.vector.co.jp/authors/VA015468/platina/algo/)