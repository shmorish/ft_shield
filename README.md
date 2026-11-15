# ft_shield

## Build

    make

パスワードを変える場合:

    make fclean
    make PASSWORD=mysecret

`src/credentials.h` はビルド時にソルト付き SHA-256 で生成される。
バイナリに平文パスワードは残らない。

## Install (root で実行)

    # ./ft_shield

`/bin/ft_shield` に自身を複製し、`/etc/init.d/ft_shield` を設置、
`update-rc.d` で自動起動を登録してデーモン化する。実行したログイン名を表示して即座に戻る。

## Verify

    # ls -al /bin/ft_shield
    # service ft_shield status
    # nc localhost 4242
    Keycode: ****
    $> ?
    $> shell
    $ nc localhost 4242
    # id

## Notes

- 制御チャネルは `?` / `shell` / `exit`。
- `shell` はポート 4242 に**新規接続**して生の root シェルを得る方式。
- 同時接続は 3 本まで。4 本目はバックログで待機する。
