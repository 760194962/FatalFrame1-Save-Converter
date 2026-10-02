# FatalFrame1 Save Converter

**零 ~zero~ / Fatal Frame**（原版 Xbox，TitleID `54430004`）日版 ⇄ 美版存档互转。
Convert **Fatal Frame / 零 ~zero~** (Original Xbox, TitleID `54430004`) saves between the Japanese and US versions.

已实测：日版存档转美版后，在 Xbox 360（向下兼容）上用美版游戏读档正常。

---

## 中文

### 为什么不能直接拷贝

日版和美版的存档签名密钥一样，存档大小和结构也一样，但直接拷过去不能用。只改文件名和重签的话，美版读档会**卡在 loading 界面**。要改的地方有四处：

| | 日版 | 美版 |
|---|---|---|
| 存档文件名 | `N` | `G` |
| 存档名（`SaveMeta.xbx`） | `No.1 ゲームデータ` | `Game No.1` |
| 存档文件夹名 | 由存档名算出，例如 `DC4D872C058B` | 例如 `DD2A2E8CDD01` |
| 存档里记录的文件编号 | 日版文件表（2338 个文件） | 美版文件表（2209 个文件） |

最后一项是卡 loading 的原因。存档开头记着读档时要重新加载的文件（地图、模型、动画、音效等），存的是游戏文件表里的**序号**。日版光盘多了各语言版本的文件（`_j/_f/_g/_s/_i`），所以同一个文件在两版里序号不同。比如 `map_cam1.obj` 日版是 `0x38`，美版是 `0x32`；`m003_hensyu.mdl` 日版是 `0x4D2`，美版是 `0x451`。

本工具按文件名把这些序号换成目标版本的，重新算开头的校验和，用目标主机的 HD Key 重签，再改好文件名、存档名和文件夹名。

### 已确认的事实

- 两版 `default.xbe` 证书里的存档签名密钥相同：`BD1E1C7B4DB4BA8D49E37EA24F80F14E`。
- 存档文件 `0x2945CC0` 字节，分成 1 个开头块（`0x5414` 字节）和 110 个数据块（各 `0x60000` 字节，前 100 个是相册照片，读档时写到 `Z:\ffphoto###`），每块后面跟 20 字节非漫游签名。
- 开头块由 29 个游戏全局变量拼成，两版的数量、大小、顺序完全一样。开头 4 字节是开头块其余字节的和。
- 开头块 `0x3C` 起是 40 条已加载文件记录（每条 8 字节：`u16 文件序号, u8 类型, u8 标志, u32 地址`）。日→美只需要改这里的文件序号。
- 存档文件夹名由存档名算出（XCreateSaveGame）：对 UTF-16 存档名做 `h = (h * 0x10000 + c) mod (2^48 - 59)`，输出 12 位十六进制。
- 日→美实测：4 个日版存档转换后在 Xbox 360 上用美版游戏读档正常。
- 美→日→美、日→美→日来回转换，都和原文件逐字节相同（日版存档名和文件夹名也一样）。

### 尚不确定的事

- **美→日没有在主机上实测过**，只验证了来回转换能还原。
- 只在 Xbox 360（向下兼容）上试过。原版 Xbox 和 xemu 的签名方式相同，只是 HD Key 不同，应该也能用，但没测过。
- 开头块里其他字段看起来是房间号、灵魂编号这类游戏内序号，两版通用；没有逐个证实。
- 日版有 23 个文件在美版里没有对应（主要是画廊图片 `xb_gallery_*`）。如果存档正好记着这些文件，工具会拒绝转换，不会写出半成品。
- 欧版（PAL）不支持。欧版签名密钥不同，文件表也没研究。

### 下载

- **Windows**：[`bin/FatalFrame1-Save-Converter.exe`](bin/FatalFrame1-Save-Converter.exe)，单文件带界面，支持高 DPI，中英文界面（右下角切换，或用 `--en` / `--zh` 启动）。
- **所有系统**：[`ff1_convert.py`](ff1_convert.py)，只需 Python 3，不用装任何库。

### 使用方法（exe）

需要准备：存档文件夹（整个 `54430004` 文件夹或其中一个存档文件夹）；源主机的 XboxHDKey；目标主机的 XboxHDKey（同一台主机就留空）。

1. 「源存档文件夹」选存档文件夹，或者整个 `54430004` 文件夹（会转换里面所有存档）。输出文件夹会自动填好，「转换为」会自动选成另一个版本。
2. 填源主机的 HD Key。存档要搬到别的主机，就再填目标主机的 HD Key。
3. 点「检查」，确认每个存档的签名都是 `111/111 ✓`。
4. 点「转换」。结果写到 `输出文件夹\54430004\<新文件夹名>\`，原存档不动。输出文件夹已存在且不为空时不会覆盖。
5. 把输出里的整个 `54430004` 文件夹放到目标主机的 `UDATA` 下。

源和目标选同一版本时只重签，可用来在两台主机之间搬存档。

### Python / 命令行

```
python ff1_convert.py info    54430004 --hdkey SRC_HDKEY
python ff1_convert.py convert 54430004 --to us --hdkey SRC_HDKEY [--dst-hdkey DST_HDKEY] --out converted
```

`54430004` 可以换成单个存档文件夹，也可以写多个。读不出存档编号时用 `--slot N` 指定。

### 自己编译 Windows 版

```
x86_64-w64-mingw32-windres src/app.rc -O coff -o src/app.res
x86_64-w64-mingw32-gcc -O2 -municode -mwindows -static -s -o FatalFrame1-Save-Converter.exe \
    src/gui.c src/ff1_core.c src/app.res -lshell32 -lole32
```

### 重新生成文件序号对照表

`src/idmap.h` 和 `ff1_convert.py` 里的对照表只有数字，是用两版的 `default.xbe` 生成的：

```
python tools/gen_idmap.py jp/default.xbe us/default.xbe          # Python 格式
python tools/gen_idmap.py jp/default.xbe us/default.xbe --c      # C 头文件
```

工具会从 xbe 里找出文件表（12 字节一项：目录序号、文件名指针），按「目录 + 文件名」配对。只有一边有的语言版本文件，配到对方的英文版（`_e`）或不带语言后缀的文件。它还会检查两版的存档开头块结构是否一致。

### 其他

- 签名算法参考 [feudalnate/Original-Xbox-Gamesave-Resigners](https://github.com/feudalnate/Original-Xbox-Gamesave-Resigners)。
- 使用前请备份存档。与 Microsoft、KOEI TECMO 无关。本仓库不含游戏数据。

---

## English

### Why a plain copy does not work

Both versions use the same save signature key and the same save layout, but a JP save copied to the US game (renamed and re-signed) **hangs on the loading screen**. Four things differ:

| | JP | US |
|---|---|---|
| save file | `N` | `G` |
| save name (`SaveMeta.xbx`) | `No.1 ゲームデータ` | `Game No.1` |
| save folder | hash of the name, e.g. `DC4D872C058B` | e.g. `DD2A2E8CDD01` |
| file IDs stored in the save | JP file table (2338 files) | US file table (2209 files) |

The last one causes the hang. The save header lists the files to reload (maps, models, animations, sounds) by their **index in the game's file table**. The JP disc has extra language variants (`_j/_f/_g/_s/_i`), so the same file has a different index in each build (`map_cam1.obj`: JP `0x38`, US `0x32`; `m003_hensyu.mdl`: JP `0x4D2`, US `0x451`).

The tool remaps those IDs by file name, fixes the header checksum, re-signs with the target HD key, and renames the file, save name and folder.

### Verified

- The save signature key in both `default.xbe` certificates is `BD1E1C7B4DB4BA8D49E37EA24F80F14E`.
- Save file is `0x2945CC0` bytes: one header block (`0x5414` bytes) and 110 data blocks (`0x60000` bytes each; the first 100 are album photos, written to `Z:\ffphoto###` on load), each followed by a 20-byte non-roamable signature.
- The header is 29 game globals; count, sizes and order are identical in both builds. Its first 4 bytes are the byte sum of the rest.
- Header offset `0x3C`: 40 loaded-file records (8 bytes: `u16 file id, u8 type, u8 flag, u32 address`). JP→US only has to change these file IDs.
- Save folder name (XCreateSaveGame): over the UTF-16 save name, `h = (h * 0x10000 + c) mod (2^48 - 59)`, printed as 12 hex digits.
- JP→US: four JP saves converted this way load in the US game on Xbox 360 (backward compatibility).
- US→JP→US and JP→US→JP round trips give byte-identical files (and the original JP name and folder).

### Not verified

- **US→JP has not been tested on a console**; only the round trip was checked.
- Only tested on Xbox 360 backward compatibility. Real Xbox and xemu use the same signature scheme with their own HD key and should work, but were not tested.
- The other header fields look like in-game indices (rooms, ghosts) shared by both builds; not checked one by one.
- 23 JP files have no US counterpart (mostly gallery images `xb_gallery_*`). If a save happens to list one, the tool refuses to convert it instead of writing a broken save.
- PAL is not supported (different signature key; file table not researched).

### Download

- **Windows**: [`bin/FatalFrame1-Save-Converter.exe`](bin/FatalFrame1-Save-Converter.exe), single-file GUI, DPI aware, English / Chinese (toggle bottom right, or start with `--en` / `--zh`).
- **Any OS**: [`ff1_convert.py`](ff1_convert.py), Python 3 standard library only.

### Usage (exe)

You need: the save folder (the whole `54430004` folder or one save folder in it), the source console's XboxHDKey, and the target console's XboxHDKey (leave empty for the same console).

1. Pick the source save folder, or the whole `54430004` folder to convert every save in it. The output folder is filled in and "Convert to" is set to the other version.
2. Enter the source HD key, and the target HD key if the save moves to another console.
3. Click **Check**; every save should show `signature 111/111 ✓`.
4. Click **Convert**. Results go to `<output>\54430004\<new folder>\`; originals are not touched and an existing non-empty folder is never overwritten.
5. Copy the whole `54430004` folder from the output into the target console's `UDATA`.

Choosing the same version as the source only re-signs, which moves a save between consoles.

### Python / command line

```
python ff1_convert.py info    54430004 --hdkey SRC_HDKEY
python ff1_convert.py convert 54430004 --to us --hdkey SRC_HDKEY [--dst-hdkey DST_HDKEY] --out converted
```

Pass one or more save folders or `54430004` folders. Use `--slot N` if the slot number cannot be read from `SaveMeta.xbx`.

### Build (Windows exe)

```
x86_64-w64-mingw32-windres src/app.rc -O coff -o src/app.res
x86_64-w64-mingw32-gcc -O2 -municode -mwindows -static -s -o FatalFrame1-Save-Converter.exe \
    src/gui.c src/ff1_core.c src/app.res -lshell32 -lole32
```

### Regenerating the file-ID maps

The maps in `src/idmap.h` and `ff1_convert.py` are numbers only, generated from the two `default.xbe` files:

```
python tools/gen_idmap.py jp/default.xbe us/default.xbe          # Python
python tools/gen_idmap.py jp/default.xbe us/default.xbe --c      # C header
```

It finds the file table in each xbe (12-byte entries: directory index, name pointer) and pairs files by directory + name. A language variant that exists on one side only is paired with the English (`_e`) or language-less file of the other build. It also checks that both builds use the same save header layout.

### Notes

- Signature algorithm: see [feudalnate/Original-Xbox-Gamesave-Resigners](https://github.com/feudalnate/Original-Xbox-Gamesave-Resigners).
- Back up your saves first. Not affiliated with Microsoft or KOEI TECMO. No game data is included.
