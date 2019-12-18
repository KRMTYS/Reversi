# creversi

## 概要

1P用リバーシゲーム

```
$ ./reversi 
    A B C D E F G H 
  +-----------------+
1 |                 |
2 |                 |
3 |       *         |
4 |     * O X       |
5 |       X O *     |
6 |         *       |
7 |                 |
8 |                 |
  +-----------------+
X: 2 O: 2
>> 
```

## 機能

- 思考ルーチンとの対戦機能
  - 盤面上のパターンにより局面を評価
  - NegaAlpha法による探索

- 自己対局による学習機能
  - ファイルを経由した評価パラメータの入出力

### 操作

- 画面表示
  - 黒石：`X`
  - 白石：`O`
  - 着手できる座標：`*`

- `（列アルファベット）（行数）`の入力で指定した座標に着手する

  ```sh
  # プレイヤー黒手番
      A B C D E F G H 
    +-----------------+
  1 |                 |
  2 |                 |
  3 |       *         |
  4 |     * O X       |
  5 |       X O *     |
  6 |         *       |
  7 |                 |
  8 |                 |
    +-----------------+
  X: 2 O: 2
  >> D3 # プレイヤー着手
      A B C D E F G H 
    +-----------------+
  1 |                 |
  2 |                 |
  3 |     * X *       |
  4 |       X X       |
  5 |     * X O       |
  6 |                 |
  7 |                 |
  8 |                 |
    +-----------------+
  X: 4 O: 1
  >> C5 # 思考ルーチン着手
      A B C D E F G H 
    +-----------------+
  1 |                 |
  2 |                 |
  3 |       X         |
  4 |       X X       |
  5 |     O O O       |
  6 |   * * * * *     |
  7 |                 |
  8 |                 |
    +-----------------+
  X: 3 O: 3
  >> 
  ...
  ```

### オプション

```
$ ./reversi -h
options
     -b  play with BLACK (by default)
     -w  play with WHITE
     -c  COM vs COM
     -l iterations
        self-playing learning by specified iterations
     -h  show this help
```

- `-b`: プレイヤー黒手番（先攻）
- `-w`: プレイヤー白手番（後攻）
- `-c`: COM戦
- `-l iterations`: 自己対局による学習（要回数指定）
- `-h`: ヘルプ表示

## 開発環境

- Ubuntu 18.04.2 LTS (WSL)
- gcc 7.4.0
- GNU Make 4.1

## ビルド・実行

```sh
$ make

# Out-of-source build
# 実行ファイル：./build/release/reversi
$ ls ./build/release
reversi src

# 既定ではプレイヤー黒手番で開始
$ ./build/release/reversi
    A B C D E F G H 
  +-----------------+
1 |                 |
2 |                 |
3 |       *         |
4 |     * O X       |
5 |       X O *     |
6 |         *       |
7 |                 |
8 |                 |
  +-----------------+
X: 2 O: 2
>> 
```

## 未実装の機能

- COMの強さ設定
- アンドゥ・リドゥ
- 定石
- 棋譜の入出力
- 高速化のための仕組み
  - 枝刈り (Multi Prob Cut?)
  - 置換表・ハッシュ法

## ライセンス

[![WTFPL](http://www.wtfpl.net/wp-content/uploads/2012/12/wtfpl-badge-1.png)](http://www.wtfpl.net/)

## 参考情報

- [オセロ・リバーシプログラミング講座 ～勝ち方・考え方～](http://uguisu.skr.jp/othello/)

- [オセロプログラムの作り方](http://hp.vector.co.jp/authors/VA015468/platina/algo/)

- [リバーシプログラムの作り方](http://www.es-cube.net/es-cube/reversi/sample/index.html)