# multiuser-xv6

大学の講義の最終課題の供養に公開します．

## 1. 外部仕様書 

### 目的

今回の課題では，xv6 のマルチユーザ化を目指した．具体的に実装した機能を以下に示す．

- ユーザ管理機能
  - `/users` ファイルへのユーザ情報の保存
  - ユーザ名とパスワードを入力したログイン
  - システムコールを通じたユーザの登録・取得
- プロセスの所有者機能
  - ログイン時にプロセスの所有者を変更
  - ファイルアクセス管理に所有者情報を利用
- ファイルシステムのマルチユーザ対応
  - ファイル所有者情報の追加
  - ファイル権限情報の追加とアクセス制御
    - 今回はユーザとそれ以外の二つに区分した制御を行った
    - グループは実装しなかった
  - read/write/execute の 3-bit * 2 で権限管理
    - 16-bit の構造を持つ
    - ユーザとそれ以外で分けた

### 全体の説明

#### ユーザ管理機能
ここでは，OSブート後からユーザプロセスが実行されるまでの今回のマルチユーザ対応の実装流れを説明する．

OS が起動すると，`kernel/main.c` に記述された `main` 関数が実行される
main 関数内で，`userinit` 関数が呼び出されたタイミングで，ユーザプロセスが生成されるが，この時にプロセスIDを `0` に設定した．

その後，OS によってプロセス `user/init.c` が実行される．通常はこのプロセスはすぐにフォークした後 `user/sh.c` を実行するが，今回はログイン処理を行うため，`user/login.c` を実行するよう変更した．

`user/login.c` では，ユーザ名とパスワードを入力してログイン処理を行う．
初めにシステムコール `sys_reloadusrs` を呼び出し，カーネル空間上にユーザ情報をロードさせる．
この時，`/users` ファイルが存在しない場合は新規作成し，login プロセス上でも初期ユーザ（すなわち root ユーザ）を入力させるフローに分岐する．
ログイン処理はシステムコール `sys_login` で行われ，ユーザ名とパスワードが一致するかどうかを確認し，一致した場合にプロセスの pid を変更する．

ログイン処理が成功すると，`user/sh.c` が実行される．
失敗した場合は再度ログイン処理を行う．
`user/sh.c` 以降のフローは既存の xv6 と同様である．

ユーザの登録は，システムコール `sys_addusr` を通じて行う．
SYS_addusr は新しいユーザ名とパスワードを入力させ，ユーザ名が重複していないかを確認し，重複していない場合はユーザ情報を `/users` ファイルに書き込む．

ユーザ情報の取得は，システムコール `sys_getusr` を通じて行う．
sys_getusr はユーザ名を入力させ，ユーザ名が存在するかどうかを確認する．
存在する場合は `kernel/uman.h` に定義された構造体 `user` を返す．
存在しない場合はその旨を返す．
この時パスワードは含まれない．

#### プロセスの所有者機能

`kernel/proc.h` の構造体 `proc` に `uid` というフィールドを追加し，プロセスの所有者情報を持つようにした．
`sys_login` が実行され，かつ正しいユーザ名とパスワードが入力された場合，この `uid` にログインしたユーザの ID を設定している．

#### ファイルシステムのマルチユーザ対応

`kernel/fs.h` の構造体 `dinode` と `kernel/file.h` の構造体 `inode` に `uid` と `mode` というフィールドを追加し，ファイルの所有者情報と権限情報を持つようにした．
`uid` はファイルの所有者の ID を，`mode` はファイルの権限情報を持つ．
`mode` は 16-bit (ushort) の構造を持ち，下位 9-bit がファイルの権限情報を表す．
この 9-bit は 1 = read, 2 = write, 4 = exec の 3-bit * 3 で構成されており，最初の 3-bit がファイル所有者の権限，次の 3-bit がそれ以外のユーザの権限を表している．
最後の 3-bit は今後の実装のために予約している．

`kernel/sysfile.c` の `create` でファイル作成時のデフォルトの所有者情報を呼び出し元プロセスの uid に設定し，権限は 733 に設定している．
また，ファイルのアクセス権限を確認するための関数として `kernel/fs.c` に `checkfperm` 関数を追加した．
この関数はファイル操作に関連するシステムコール（`sys_open`, `sys_read`, `sys_write`, `sys_exec`）が呼ばれた際に呼び出され，ファイルの権限情報を確認する．

### 利用ガイド

今回新たに実装したシステムコールの一覧とその使い方を以下に示す．

- `int login(char* username, char* password);` 
  - 引数
    - `username` : ユーザ名
    -  `password` : パスワード
  - 戻り値
    - 成功時 : 0
    - 失敗時 : -1
  - 説明
    - ユーザ名とパスワードを入力してログインする
    - ログイン成功時にプロセスの pid を変更し 0 を返す
    - ログイン失敗時に -1 を返す
- `int addusr(char* username, char* password);`
  - 引数
    - `username` : ユーザ名
    - `password` : パスワード
  - 戻り値
    - 成功時 : 0
    - 失敗時 : -1
  - 説明
    - ユーザを追加する
    - ユーザ名が重複していた場合は -1 を返す
- `int reloadusrs(void);`
  - 引数：なし
  - 戻り値
    - 成功時 : 0
    - 失敗時 : -1
    - users ファイル新規作成時 : 1
  - 説明
    - ユーザ情報を kernel 空間にロードする
    - users ファイルが存在しない場合は新規作成し，1 を返す
- `int getusr(struct user* u);`
  - 引数
    - `u` : ユーザ情報を格納するユーザ空間上の構造体のポインタ
  - 戻り値
    - 成功時 : 0
    - 失敗時 : -1
  - 説明
    - ユーザ情報を取得し，渡されたポインタにコピーする
    - ユーザが存在しないなど，エラーが発生した場合は -1 を返す
    - ユーザ情報のパスワードは削除されて渡される
- `int chmod(char* path, int mode);`
  - 引数
    - `path` : ファイルパス
    - `mode` : 権限情報
  - 戻り値
    - 成功時 : 0
    - 失敗時 : -1
  - 説明
    - ファイルの権限情報を変更する
    - mode は 16-bit の構造を持ち，下位 9-bit がファイルの権限情報を表す
    - 9-bit は 1 = read, 2 = write, 4 = exec の 3-bit * 3 で構成されている
    - 最初の 3-bit がファイル所有者の権限，次の 3-bit がそれ以外のユーザの権限を表す
    - 最後の 3-bit は今後の実装のために予約している
- `int chown(char* path, char* username);`
  - 引数
    - `path` : ファイルパス
    - `username` : ユーザ名
  - 戻り値
    - 成功時 : 0
    - 失敗時 : -1
  - 説明
    - ファイルの所有者情報を変更する
    - ユーザ名が存在しないなど，エラーが発生した場合は -1 を返す

## 2. 内部仕様書

ここでは，追加変更したプログラムとその内容について説明する．

### カーネルの変更点

#### syscall.c / syscall.h
新しいシステムコール `login`, `addusr`, `reloadusrs`, `getusr`, `chmod`, `chown` の追加．

#### sysuman.c
新しいシステムコール `login`, `addusr`, `reloadusrs`, `getusr` の実装を記述した．
詳細は利用ガイドを参照．


#### uman.c / uman.h
- （新）構造体 `user`：ユーザ情報を管理するための構造体
- （新）関数 `create_savefile`：ユーザ情報を保存するファイル `/users` が存在するか確認し，存在しない場合は新規作成するための関数
  - ファイルの存在と新規作成で別のフラグを返す
- （新）関数 `saveusrs`：ユーザ情報を保存するための関数
- （新）関数 `loadusrs`：ユーザ情報をロードするための関数
  - グローバル変数 `users` にユーザ情報を格納する
  - ユーザ情報が存在しない場合は新規作成し，1 を返す
  - ロードに成功した場合は 0 を返し，ロードに失敗した場合は -1 を返す
- （新）関数 `getusr`：ロードされた中からユーザ情報を取得するための関数
  - グローバル変数 `users` にロードされたユーザ情報から uid が一致するものを探し，その構造体を与えられたポインタに宛ててコピーする
- （新）関数 `usrauthenticate`：ユーザ認証を行うための関数
  - ユーザ名とパスワードが一致するかどうかを確認する
  - 一致した場合はログインに成功したユーザの uid を返す
  - 一致しない場合は -1 を返す
- （新）関数 `addusr`：ユーザを追加するための関数
  - ユーザ名が重複していないかを確認し，重複していない場合はユーザ情報を保存する
  - 保存に成功した場合は 0 を返し，失敗した場合は -1 を返す
- （新）関数 `getusername`：uid からユーザ名を取得するための関数
  - uid が一致するユーザ名を渡されたポインタにコピーする
  - uid が存在しない場合は -1 を返す
- （新）関数 `getuid`：ユーザ名から uid を取得するための関数
  - ユーザ名が一致する uid を返す
  - ユーザ名が存在しない場合は -1 を返す

#### proc.c / proc.h
- （更）構造体 `proc` に `uid` フィールドを追加
- （更）関数 `userinit` に `uid` の初期化（0 に設定）を追加
- （更）関数 `fork` に `uid` のコピー操作を追加

#### fs.c / fs.h
- （更）構造体 `dinode` に `uid`，`mode` フィールドを追加
- （新）関数 `checkfperm`：ファイルの権限情報を確認するための関数
  - ファイルの権限情報を確認し，アクセス権限があるかどうかを確認する
  - 引数に inode 構造体のポインタと要求する権限（1 = read, 2 = write, 4 = exec）をとる
  - アクセス権限がある場合は 0 を返し，ない場合は -1 を返す
- （更）関数 `stati`：ファイルの情報を取得する際に uid と mode もコピーするように変更
- （更）関数 `iupdate`：inode 情報更新に uid と mode もサポート
- （更）関数 `ilock`： uid と mode もロックするように変更

## stat.h
- （更）構造体 `stat` に `uid`，`mode` フィールドを追加

#### file.c / file.h
- （更）構造体 `inode` に `uid`，`mode` フィールドを追加

#### sysfile.c
- （更）関数 `sys_read`：ファイルの読み込み時に `checkfperm` を呼び出すように変更
- （更）関数 `sys_write`：ファイルの書き込み時に `checkfperm` を呼び出すように変更
- （更）関数 `sys_open`：ファイルのオープン時に `checkfperm` を呼び出すように変更
  - 渡された O_MODE に応じて読み込み権限，書き込み権限，実行権限を確認する
  - O_CREATE が指定された場合はファイルの所有者情報を呼び出し元プロセスの uid に設定し，権限は 733 に設定する
- （更）関数 `sys_exec`：ファイルの実行時に `checkfperm` を呼び出すように変更
- （更）関数 `create`：ファイル作成時にファイルの所有者情報を呼び出し元プロセスの uid に設定し，権限は 733 に設定するように変更
- （新）関数 `sys_chmod`：ファイルの権限情報を変更するためのシステムコール
  - 引数にファイルパスと権限情報を取る
  - 呼び出し元プロセスの uid とファイルの所有者が一致するか確認した上で，ファイルの権限情報を変更する
- （新）関数 `sys_chown`：ファイルの所有者情報を変更するためのシステムコール
  - 引数にファイルパスとユーザ名を取る
  - 呼びだし元プロセスの uid とファイルの所有者が一致するか確認した上で，ファイルの所有者情報を変更する

#### fcntl.h
- 権限チェック用の定数 S_IRUSR, S_IWUSR, S_IXUSR を追加

#### exec.c
- （更）関数 `exec`：ファイル実行時にパーミッションを確認するように変更
  - 実行不可能な場合はエラーを返す

#### printf.c
- （更）関数 `printf`：octal 表現と binary 表現を追加
  - `%o` で 8進数表現を行うように変更（デバッグがしやすいように）
  - `%b` で 2進数表現を行うように変更（デバッグがしやすいように）

#### defs.h
- 新しく定義された関数・構造体の宣言を追加

### ユーザ空間の変更点
#### init.c
- （更）関数 `main`：`login` を `sh` プロセスの代わりに実行するように変更

#### login.c
- （新）関数 `main`：ユーザ名とパスワードを入力してログイン処理を行う
  - ログインに成功したら `sh` プロセスを実行
- （新）関数 `cutnl`：入力文字列から改行文字を削除するための関数

#### ls.c
- （更）関数 `ls`：ファイルの権限情報と所有者情報を表示するように変更

#### adduser.c
- （新）関数 `main`：ユーザを追加するためのプログラム
  - addusr システムコールを呼び出す

#### whoami.c
- （新）関数 `main`：現在のユーザ名を表示するプログラム
  - getusr システムコールを呼び出す

#### chmod.c
- （新）関数 `main`：ファイルの権限情報を変更するプログラム
  - chmod システムコールを呼び出す

#### chown.c
- （新）関数 `main`：ファイルの所有者情報を変更するプログラム
  - chown システムコールを呼び出す

#### usys.pl
- 新しいシステムコールの登録

### ビルド関連
- `mkfs.c` にて初期バイナリの権限を 755 に設定
- `Makefile` に新しいファイルの追加


following is original README file
---

xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix
Version 6 (v6).  xv6 loosely follows the structure and style of v6,
but is implemented for a modern RISC-V multiprocessor using ANSI C.

ACKNOWLEDGMENTS

xv6 is inspired by John Lions's Commentary on UNIX 6th Edition (Peer
to Peer Communications; ISBN: 1-57398-013-7; 1st edition (June 14,
2000)).  See also https://pdos.csail.mit.edu/6.1810/, which provides
pointers to on-line resources for v6.

The following people have made contributions: Russ Cox (context switching,
locking), Cliff Frey (MP), Xiao Yu (MP), Nickolai Zeldovich, and Austin
Clements.

We are also grateful for the bug reports and patches contributed by
Takahiro Aoyagi, Silas Boyd-Wickizer, Anton Burtsev, carlclone, Ian
Chen, Dan Cross, Cody Cutler, Mike CAT, Tej Chajed, Asami Doi,
eyalz800, Nelson Elhage, Saar Ettinger, Alice Ferrazzi, Nathaniel
Filardo, flespark, Peter Froehlich, Yakir Goaron, Shivam Handa, Matt
Harvey, Bryan Henry, jaichenhengjie, Jim Huang, Matúš Jókay, John
Jolly, Alexander Kapshuk, Anders Kaseorg, kehao95, Wolfgang Keller,
Jungwoo Kim, Jonathan Kimmitt, Eddie Kohler, Vadim Kolontsov, Austin
Liew, l0stman, Pavan Maddamsetti, Imbar Marinescu, Yandong Mao, Matan
Shabtay, Hitoshi Mitake, Carmi Merimovich, Mark Morrissey, mtasm, Joel
Nider, Hayato Ohhashi, OptimisticSide, Harry Porter, Greg Price, Jude
Rich, segfault, Ayan Shafqat, Eldar Sehayek, Yongming Shen, Fumiya
Shigemitsu, Cam Tenny, tyfkda, Warren Toomey, Stephen Tu, Rafael Ubal,
Amane Uehara, Pablo Ventura, Xi Wang, WaheedHafez, Keiichi Watanabe,
Nicolas Wolovick, wxdao, Grant Wu, Jindong Zhang, Icenowy Zheng,
ZhUyU1997, and Zou Chang Wei.


The code in the files that constitute xv6 is
Copyright 2006-2022 Frans Kaashoek, Robert Morris, and Russ Cox.

ERROR REPORTS

Please send errors and suggestions to Frans Kaashoek and Robert Morris
(kaashoek,rtm@mit.edu).  The main purpose of xv6 is as a teaching
operating system for MIT's 6.1810, so we are more interested in
simplifications and clarifications than new features.

BUILDING AND RUNNING XV6

You will need a RISC-V "newlib" tool chain from
https://github.com/riscv/riscv-gnu-toolchain, and qemu compiled for
riscv64-softmmu.  Once they are installed, and in your shell
search path, you can run "make qemu".
