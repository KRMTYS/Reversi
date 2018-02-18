# Reversi

## 概要

1P用リバーシゲーム

![screenshot](./screenshot.png)

## 詳細

- コンソール上に盤が表示されます
    - 黒石：`@` 白石：`O`
    - 有効手：`*`
- プレイヤーは先攻、黒石で固定
- 石を置く座標は
    `（列アルファベット）（行番号）`
    の形式で入力します
    ```
    [1] Black >> d3
    ```
- 探索：MinMax法
- 探索の深さ：5手先まで
- 評価関数：石数の差

## ビルド

`make`、`mingw32-make`

## ライセンス

[![WTFPL](http://www.wtfpl.net/wp-content/uploads/2012/12/wtfpl-badge-1.png)](http://www.wtfpl.net/)

## 参考

- [オセロ・リバーシプログラミング講座 ～勝ち方・考え方～](http://uguisu.skr.jp/othello/)

- [オセロプログラムの作り方](http://hp.vector.co.jp/authors/VA015468/platina/algo/)

- [リバーシプログラムの作り方](http://www.es-cube.net/es-cube/reversi/sample/index.html)