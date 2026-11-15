# ft_shield アーキテクチャ

このリポジトリを読むための地図。何がどこにあり、起動するとどの順で何が動くかをまとめる。
課題仕様は `en.subject.pdf`（簡易トロイの木馬を C で自作する 42 の課題）。

---

## 全体像

`ft_shield` は2つの顔を持つ単一バイナリ。

1. **初回実行**: ログイン名を表示し、裏で自分を `/bin/ft_shield` に複製 → 自動起動を仕込む → デーモンを起動。
2. **デーモン**: 4242 番で待受け、パスワード認証後に root シェルを提供する。

同じバイナリが「今 `/bin/ft_shield` として動いているか」で振る舞いを切り替える（`is_installed()`）。

---

## ディレクトリ構成

```
inc/ft_shield.h    全宣言・定数・構造体（最初に読む）
src/               本体バイナリのソース
tools/gen_hash.c   ビルド時ツール。パスワードのソルト+ハッシュを生成
Makefile           ビルド。make 時に credentials.h を自動生成
```

`src/credentials.h` はビルド時生成物（`.gitignore` 済み）。平文パスワードはリポジトリに残さない。

---

## ビルドの流れ（Makefile）

```
make
 └─ gen_hash をコンパイル (tools/gen_hash.c + src/sha256.c)
     └─ ./gen_hash "4242" > src/credentials.h   # ソルトとハッシュを C 配列で出力
         └─ src/*.c を -Iinc でコンパイル → ft_shield
```

- パスワードは `make PASSWORD=xxx` で変更可能（デフォルト `4242`）。
- `-Wl,-x` でシンボルを落としている。
- `credentials.h` には `g_salt[]` と `g_hash[]`（ソルト付き SHA-256）だけが入り、平文は入らない。

---

## 実行時の流れ

### フェーズ A: 初回実行（`main.c`）

```
main()
 ├─ geteuid() != 0 なら即終了        … 要件: root 専用
 ├─ print_login()                    … ログイン名を stdout へ
 └─ is_installed()?
     ├─ Yes → run_daemon()           … 既に /bin/ft_shield → デーモンとして動く
     └─ No  → install_self()         … /proc/self/exe を /bin/ft_shield へ複製(rename でアトミック)
              install_service()       … /etc/init.d/ft_shield を書いて update-rc.d 登録
              spawn_daemon()          … fork して /bin/ft_shield を起動
```

### フェーズ B: デーモン（`main.c: run_daemon()`）

```
run_daemon()
 ├─ daemonize()        … 二重 fork + setsid + / へ chdir + 標準入出力を /dev/null へ
 ├─ acquire_lock()     … flock で多重起動を防止（1インスタンスのみ）
 ├─ setup_signals()    … SIGCHLD で子を刈り取り、SIGPIPE/SIGHUP を無視
 ├─ write_pidfile()    … /var/run/ft_shield.pid
 └─ run_server()       … poll ループ本体
```

### フェーズ C: 接続とシェル提供（`server.c` → `auth.c` → `session.c` → `shell.c`）

```
run_server() の poll ループ
 ├─ 新規接続 → "Keycode: " を送り、認証待ちスロットに登録（最大 3）
 ├─ 認証中(auth_step)   … read_line → verify_password（定数時間比較）
 │                        失敗3回で切断、成功で authed=1 → "$> "
 └─ セッション中(session_step)
      ├─ "?"     … ヘルプ
      ├─ "shell" … g_pending_shell=1 を立てて切断
      └─ "exit"  … 切断
```

`shell` コマンドでは直接シェルに入れない（要件）。代わりにフラグを立て、
**次の新規接続**で `accept_new()` が `spawn_shell()` を呼び、`/bin/sh -i` を接続 fd に繋ぐ。

---

## モジュール早見表

| ファイル | 役割 |
|----------|------|
| `main.c` | エントリ。初回処理とデーモン起動の分岐 |
| `identity.c` | ログイン名の取得（SUDO_USER → tty 所有者 → getuid の順） |
| `install.c` | 自己複製（`install_self`）・init 登録（`install_service`）・多重起動用の `is_installed` |
| `init_script.c` | `/etc/init.d/ft_shield` の中身（LSB init スクリプト文字列） |
| `daemonize.c` | 二重 fork によるデーモン化 |
| `lock.c` | `flock` による単一インスタンス保証 + PID ファイル書き込み |
| `server.c` | poll ループ、クライアント管理（`MAX_CLI=3`）、接続受理 |
| `auth.c` | パスワード認証。`credentials.h` のソルト+ハッシュと定数時間比較 |
| `session.c` | 認証後のコマンド処理（`?` / `shell` / `exit`） |
| `shell.c` | fork + `execve("/bin/sh")` で接続 fd に root シェルを接続 |
| `signal.c` | SIGCHLD/SIGPIPE/SIGHUP ハンドラ |
| `sha256.c` | SHA-256 実装（外部ライブラリ禁止のため自前） |
| `utils.c` | `send_str`（部分送信対応）/ `read_line`（1行受信） |
| `tools/gen_hash.c` | ビルド時にソルト生成+ハッシュ化し `credentials.h` を出力 |

---

## 主要な定数・構造体（`inc/ft_shield.h`）

```c
#define PORT     4242          // 待受ポート
#define MAX_CLI  3             // 同時接続数
#define MAX_TRY  3             // 認証試行回数
#define SALT_LEN 16            // ソルト長(128bit)
#define HASH_LEN 32            // SHA-256 出力長

#define INSTALL_PATH "/bin/ft_shield"
#define INIT_PATH    "/etc/init.d/ft_shield"
#define LOCK_PATH    "/var/lock/ft_shield.lock"
#define PID_PATH     "/var/run/ft_shield.pid"

typedef struct s_client { int fd; int tries; int authed; } t_client;
```

グローバル: `g_pending_shell`（shell 要求フラグ）、`g_init_script`（init スクリプト文字列）。

---

## 読む順番のおすすめ

1. `inc/ft_shield.h` — 定数・プロトタイプで全体を把握
2. `main.c` — 分岐の起点
3. `server.c` — デーモンの中心（poll ループ）
4. `auth.c` → `session.c` → `shell.c` — 接続後の流れ
5. 残り（install / daemonize / lock / signal）は `main.c` から辿れば十分

---

## パスワードの扱い（セキュリティ要点）

- 平文はコードにもリポジトリにも残さない。`gen_hash` が `/dev/urandom` のソルト付き SHA-256 を `credentials.h` に出力。
- 認証はハッシュ同士の**定数時間比較**（`auth.c: verify_password`）でタイミング攻撃を避ける。
- `make fclean` で `credentials.h` を削除。
