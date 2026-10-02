/* FatalFrame1 Save Converter - Win32 GUI (Chinese / English, DPI aware) */
#define _WIN32_WINNT 0x0601
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#include <windows.h>
#include <shlobj.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "ff1_core.h"

#define APP_TITLE L"FatalFrame1 Save Converter 1.0  -  \x96f6 ~zero~ / Fatal Frame (Xbox) JP \x21c4 US"

enum { ID_SRC = 100, ID_SRC_BR, ID_HD, ID_HD2, ID_US, ID_JP, ID_OUT, ID_OUT_BR, ID_CHECK, ID_CONVERT, ID_LANG, ID_LOG,
       ID_L_SRC, ID_L_HD, ID_L_HD2, ID_L_TO, ID_L_OUT };

enum { S_L_SRC, S_L_HD, S_L_HD2, S_L_TO, S_US, S_JP, S_L_OUT, S_BROWSE, S_CHECK, S_CONVERT, S_LANG,
       S_INTRO, S_PICK_SRC, S_PICK_OUT, S_NOSAVES, S_BADKEY, S_BADKEY2, S_NOSRC, S_NOOUT,
       S_OPENFAIL, S_BADSIZE, S_FOUND, S_ITEM, S_SIGOK, S_SIGBAD, S_SIGNOKEY, S_CKBAD, S_NOSLOT,
       S_UNMAPPED, S_EXISTS, S_WRITEFAIL, S_DONE1, S_SAMEREGION, S_SUMMARY, S_NOTARGET, S_COUNT };

static const wchar_t *STR[2][S_COUNT] = {
{ /* Chinese */
  L"\x6e90\x5b58\x6863\x6587\x4ef6\x5939",                        /* 源存档文件夹 */
  L"\x6e90\x4e3b\x673a HD Key\xff08\x53ef\x9009\xff09",  /* 源主机 HD Key（可选） */
  L"\x76ee\x6807\x4e3b\x673a HD Key", /* 目标主机 HD Key */
  L"\x8f6c\x6362\x4e3a",                                            /* 转换为 */
  L"\x7f8e\x7248 (US, \x6587\x4ef6 G)",                             /* 美版 (US, 文件 G) */
  L"\x65e5\x7248 (JP, \x6587\x4ef6 N)",                             /* 日版 (JP, 文件 N) */
  L"\x8f93\x51fa\x6587\x4ef6\x5939",                                /* 输出文件夹 */
  L"\x6d4f\x89c8\x2026",                                            /* 浏览… */
  L"\x68c0\x67e5",                                                  /* 检查 */
  L"\x8f6c\x6362",                                                  /* 转换 */
  L"English",
  L"\x9009\x4e00\x4e2a\x5b58\x6863\x6587\x4ef6\x5939\xff08\x91cc\x9762\x6709 G \x6216 N\xff09\xff0c\x6216\x8005\x6574\x4e2a 54430004 \x6587\x4ef6\x5939\x3002\r\n"
  L"\x539f\x6587\x4ef6\x4e0d\x4f1a\x88ab\x4fee\x6539\xff0c\x7ed3\x679c\x5199\x5230 \x8f93\x51fa\x6587\x4ef6\x5939\\54430004\\<\x65b0\x6587\x4ef6\x5939\x540d>\x3002\r\n"
  L"\x6b27\x7248 (PAL) \x5b58\x6863\x4e0d\x652f\x6301\x3002\r\n"
  L"\x6e90 HD Key \x53ea\x7528\x6765\x6821\x9a8c\xff1b\x53ea\x586b\x4e00\x4e2a Key \x65f6\xff0c\x6e90\x548c\x76ee\x6807\x5171\x7528\x3002\r\n\r\n",
  /* 选一个存档文件夹（里面有 G 或 N），或者整个 54430004 文件夹。 原文件不会被修改，结果写到 输出文件夹\54430004\<新文件夹名>。 欧版 (PAL) 存档不支持。 */
  L"\x9009\x62e9\x6e90\x5b58\x6863\x6587\x4ef6\x5939",              /* 选择源存档文件夹 */
  L"\x9009\x62e9\x8f93\x51fa\x6587\x4ef6\x5939",                    /* 选择输出文件夹 */
  L"\x6ca1\x6709\x627e\x5230\x5b58\x6863\xff08\x9700\x8981\x6709 G \x6216 N \x6587\x4ef6\x7684\x6587\x4ef6\x5939\xff09\x3002\r\n", /* 没有找到存档（需要有 G 或 N 文件的文件夹）。 */
  L"\x6e90 HD Key \x683c\x5f0f\x4e0d\x5bf9\xff0c\x9700\x8981 32 \x4f4d\x5341\x516d\x8fdb\x5236\x3002\r\n",   /* 源 HD Key 格式不对，需要 32 位十六进制。 */
  L"\x76ee\x6807 HD Key \x683c\x5f0f\x4e0d\x5bf9\x3002\r\n",        /* 目标 HD Key 格式不对。 */
  L"\x8bf7\x5148\x9009\x6e90\x5b58\x6863\x6587\x4ef6\x5939\x3002\r\n", /* 请先选源存档文件夹。 */
  L"\x8bf7\x9009\x8f93\x51fa\x6587\x4ef6\x5939\x3002\r\n",          /* 请选输出文件夹。 */
  L"  \x6253\x4e0d\x5f00\x6587\x4ef6\x3002\r\n",                    /* 打不开文件。 */
  L"  \x6587\x4ef6\x5927\x5c0f\x4e0d\x5bf9\xff0c\x4e0d\x662f\x96f6\x521d\x4ee3\x5b58\x6863\x3002\r\n", /* 文件大小不对，不是零初代存档。 */
  L"\x627e\x5230 %d \x4e2a\x5b58\x6863\x3002\r\n",                  /* 找到 %d 个存档。 */
  L"%ls\r\n  %ls \x7248\xff0c\x540d\x79f0\x300c%ls\x300d\xff0c\x5f00\x5934\x6821\x9a8c\x548c %ls\r\n", /* %ls  %ls 版，名称「%ls」，开头校验和 %ls */
  L"  \x7b7e\x540d %d/%d \x2713\r\n",                               /* 签名 %d/%d ✓ */
  L"  \x7b7e\x540d %d/%d \x2717\xff1a\x6e90 HD Key \x4e0d\x5bf9\xff0c\x6216\x8005\x4e0d\x662f\x65e5\x7248/\x7f8e\x7248\x5b58\x6863\x3002\r\n", /* 签名 ✗：源 HD Key 不对，或者不是日版/美版存档。 */
  L"  \xff08\x672a\x586b\x6e90 HD Key\xff0c\x4e0d\x68c0\x67e5\x7b7e\x540d\xff09\r\n", /* （未填源 HD Key，不检查签名） */
  L"  \x5f00\x5934\x6821\x9a8c\x548c\x4e0d\x5bf9\xff0c\x5b58\x6863\x5df2\x635f\x574f\x3002\r\n", /* 开头校验和不对，存档已损坏。 */
  L"  \x8bfb\x4e0d\x51fa\x5b58\x6863\x7f16\x53f7\xff08SaveMeta.xbx\xff09\x3002\r\n", /* 读不出存档编号（SaveMeta.xbx）。 */
  L"  \x7b2c %d \x9879\x8d44\x6e90\x7684\x6587\x4ef6\x7f16\x53f7 0x%X \x5728\x76ee\x6807\x7248\x672c\x6ca1\x6709\x5bf9\x5e94\x6587\x4ef6\xff0c\x672a\x8f6c\x6362\x3002\r\n", /* 第 %d 项资源的文件编号 0x%X 在目标版本没有对应文件，未转换。 */
  L"  \x8f93\x51fa\x6587\x4ef6\x5939\x5df2\x5b58\x5728\x4e14\x4e0d\x4e3a\x7a7a\xff0c\x672a\x8986\x76d6\xff1a%ls\r\n", /* 输出文件夹已存在且不为空，未覆盖：%ls */
  L"  \x5199\x5165\x5931\x8d25\xff1a%ls\r\n",                        /* 写入失败：%ls */
  L"  \x2192 %ls \x7248\x300c%ls\x300d\xff0c%d \x4e2a\x6587\x4ef6\x7f16\x53f7\x5df2\x6539\xff0c\x5df2\x91cd\x7b7e\r\n     %ls\r\n", /* → %ls 版「%ls」，%d 个文件编号已改，已重签 */
  L"  \xff08\x6e90\x548c\x76ee\x6807\x662f\x540c\x4e00\x7248\x672c\xff0c\x53ea\x91cd\x7b7e\xff09\r\n", /* （源和目标是同一版本，只重签） */
  L"\x5b8c\x6210\xff1a%d \x4e2a\x6210\x529f\xff0c%d \x4e2a\x5931\x8d25\x3002\x628a\x8f93\x51fa\x91cc\x7684 54430004 \x6587\x4ef6\x5939\x6574\x4e2a\x653e\x5230\x76ee\x6807\x4e3b\x673a\x3002\r\n\r\n",
  /* 完成：%d 个成功，%d 个失败。把输出里的 54430004 文件夹整个放到目标主机。 */
  L"\x8bf7\x586b\x76ee\x6807\x4e3b\x673a\x7684 HD Key\x3002\r\n", /* 请填目标主机的 HD Key。 */
},
{ /* English */
  L"Source save folder", L"Source HD key (optional)", L"Target HD key", L"Convert to",
  L"US (file G)", L"JP (file N)", L"Output folder", L"Browse\x2026", L"Check", L"Convert", L"\x4e2d\x6587",
  L"Pick one save folder (holding G or N), or the whole 54430004 folder.\r\n"
  L"Originals are not changed; results go to <output>\\54430004\\<new folder name>.\r\n"
  L"PAL saves are not supported.\r\n"
  L"Source HD key only verifies the save; with just one key, it is used for both.\r\n\r\n",
  L"Choose the source save folder", L"Choose the output folder",
  L"No saves found (need folders that contain a G or N file).\r\n",
  L"Source HD key must be 32 hex digits.\r\n", L"Target HD key is not valid.\r\n",
  L"Choose the source save folder first.\r\n", L"Choose an output folder.\r\n",
  L"  Cannot open the file.\r\n", L"  Wrong file size; not a Fatal Frame save.\r\n",
  L"Found %d save(s).\r\n",
  L"%ls\r\n  %ls, name \"%ls\", header checksum %ls\r\n",
  L"  signature %d/%d \x2713\r\n",
  L"  signature %d/%d \x2717: wrong source HD key, or not a JP/US save.\r\n",
  L"  (no source HD key, signature not checked)\r\n",
  L"  Header checksum is wrong; the save is damaged.\r\n",
  L"  Cannot read the slot number (SaveMeta.xbx).\r\n",
  L"  Resource %d: file id 0x%X has no counterpart in the target version; not converted.\r\n",
  L"  Output folder exists and is not empty, not overwritten: %ls\r\n",
  L"  Write failed: %ls\r\n",
  L"  \x2192 %ls \"%ls\", %d file id(s) remapped, re-signed\r\n     %ls\r\n",
  L"  (same version as source: re-sign only)\r\n",
  L"Done: %d converted, %d failed. Copy the whole 54430004 folder from the output to the target console.\r\n\r\n",
  L"Enter the target HD key.\r\n",
} };

static int lang;
static HWND hMain, hLog;
static HFONT hFont;
static int dpi = 96;
#define S(x) STR[lang][x]
#define DP(x) MulDiv((x), dpi, 96)

/* ---------------- log ---------------- */
static void logw(const wchar_t *fmt, ...) {
    wchar_t buf[2048]; va_list ap; int n;
    va_start(ap, fmt); _vsnwprintf(buf, 2047, fmt, ap); va_end(ap); buf[2047] = 0;
    n = GetWindowTextLengthW(hLog);
    SendMessageW(hLog, EM_SETSEL, n, n);
    SendMessageW(hLog, EM_REPLACESEL, FALSE, (LPARAM)buf);
    UpdateWindow(hLog);
}

/* ---------------- files ---------------- */
static int has_save_file(const wchar_t *dir, wchar_t *which) {
    wchar_t p[MAX_PATH * 2]; DWORD a;
    swprintf(p, MAX_PATH * 2, L"%ls\\G", dir); a = GetFileAttributesW(p);
    if (a != INVALID_FILE_ATTRIBUTES && !(a & FILE_ATTRIBUTE_DIRECTORY)) { if (which) *which = L'G'; return 1; }
    swprintf(p, MAX_PATH * 2, L"%ls\\N", dir); a = GetFileAttributesW(p);
    if (a != INVALID_FILE_ATTRIBUTES && !(a & FILE_ATTRIBUTE_DIRECTORY)) { if (which) *which = L'N'; return 1; }
    return 0;
}
#define MAXSAVES 64
static wchar_t saves[MAXSAVES][MAX_PATH];
static int find_saves(const wchar_t *src) {
    int n = 0; WIN32_FIND_DATAW fd; HANDLE h; wchar_t pat[MAX_PATH * 2];
    if (has_save_file(src, NULL)) { wcsncpy(saves[n++], src, MAX_PATH - 1); return n; }
    swprintf(pat, MAX_PATH * 2, L"%ls\\*", src);
    h = FindFirstFileW(pat, &fd);
    if (h == INVALID_HANDLE_VALUE) return 0;
    do {
        if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && fd.cFileName[0] != L'.' && n < MAXSAVES) {
            swprintf(saves[n], MAX_PATH, L"%ls\\%ls", src, fd.cFileName);
            if (has_save_file(saves[n], NULL)) n++;
        }
    } while (FindNextFileW(h, &fd));
    FindClose(h);
    return n;
}
static uint8_t *read_all(const wchar_t *p, DWORD *n) {
    HANDLE h = CreateFileW(p, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL); uint8_t *b; DWORD sz, got;
    if (h == INVALID_HANDLE_VALUE) return NULL;
    sz = GetFileSize(h, NULL); b = (uint8_t *)malloc(sz ? sz : 1);
    if (!b || !ReadFile(h, b, sz, &got, NULL) || got != sz) { free(b); CloseHandle(h); return NULL; }
    CloseHandle(h); *n = sz; return b;
}
static int write_all(const wchar_t *p, const void *d, DWORD n) {
    HANDLE h = CreateFileW(p, GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL); DWORD put;
    if (h == INVALID_HANDLE_VALUE) return 0;
    if (!WriteFile(h, d, n, &put, NULL) || put != n) { CloseHandle(h); DeleteFileW(p); return 0; }
    CloseHandle(h); return 1;
}
static int dir_nonempty(const wchar_t *d) {
    WIN32_FIND_DATAW fd; wchar_t pat[MAX_PATH * 2]; HANDLE h; int r = 0;
    swprintf(pat, MAX_PATH * 2, L"%ls\\*", d);
    h = FindFirstFileW(pat, &fd); if (h == INVALID_HANDLE_VALUE) return 0;
    do { if (wcscmp(fd.cFileName, L".") && wcscmp(fd.cFileName, L"..")) { r = 1; break; } } while (FindNextFileW(h, &fd));
    FindClose(h); return r;
}
static void get_text(int id, wchar_t *buf, int n) { GetDlgItemTextW(hMain, id, buf, n); }
static void trim_slash(wchar_t *p) { size_t l = wcslen(p); while (l > 3 && (p[l-1] == L'\\' || p[l-1] == L'/' || p[l-1] == L' ')) p[--l] = 0; }

/* ---------------- actions ---------------- */
static int get_key(int id, uint8_t k[16], int *empty) {
    wchar_t w[128]; char a[128]; int i;
    get_text(id, w, 128);
    for (i = 0; w[i] && i < 127; i++) a[i] = w[i] < 128 ? (char)w[i] : '?';
    a[i] = 0; *empty = 1;
    for (i = 0; a[i]; i++) if (a[i] != ' ') *empty = 0;
    if (*empty) return 0;
    return ff1_parse_hdkey(a, k) == 0 ? 0 : -1;
}

static void do_work(int convert) {
    wchar_t src[MAX_PATH], out[MAX_PATH]; uint8_t k1[16], k2[16]; int e1, e2, n, i, ok = 0, bad = 0, to;
    HCURSOR old;
    get_text(ID_SRC, src, MAX_PATH); trim_slash(src);
    if (!src[0]) { logw(S(S_NOSRC)); return; }
    if (get_key(ID_HD, k1, &e1) < 0) { logw(S(S_BADKEY)); return; }
    if (get_key(ID_HD2, k2, &e2) < 0) { logw(S(S_BADKEY2)); return; }
    if (e2) { if (convert && e1) { logw(S(S_NOTARGET)); return; } memcpy(k2, k1, 16); }
    get_text(ID_OUT, out, MAX_PATH); trim_slash(out);
    if (convert && !out[0]) { logw(S(S_NOOUT)); return; }
    to = IsDlgButtonChecked(hMain, ID_JP) == BST_CHECKED ? FF1_JP : FF1_US;
    n = find_saves(src);
    if (!n) { logw(S(S_NOSAVES)); return; }
    old = SetCursor(LoadCursor(NULL, IDC_WAIT));
    logw(S(S_FOUND), n);
    for (i = 0; i < n; i++) {
        wchar_t which, p[MAX_PATH * 2], name[64] = L"?", od[MAX_PATH * 2];
        uint16_t nm[64], tn[32]; char fold[13]; DWORD sz, msz; uint8_t *b, *meta; int region, sig = -1, ck, slot = 0, len = -1, tl, bi = 0, bid = 0, ch, j;
        has_save_file(saves[i], &which);
        region = which == L'G' ? FF1_US : FF1_JP;
        swprintf(p, MAX_PATH * 2, L"%ls\\%lc", saves[i], which);
        b = read_all(p, &sz);
        swprintf(p, MAX_PATH * 2, L"%ls\\SaveMeta.xbx", saves[i]);
        meta = read_all(p, &msz);
        if (meta) { len = ff1_meta_name(meta, msz, nm); free(meta); }
        if (len >= 0) { for (j = 0; j <= len; j++) name[j] = nm[j]; slot = ff1_slot_from_name(nm, len); }
        if (!b) { logw(L"%ls\r\n", saves[i]); logw(S(S_OPENFAIL)); bad++; continue; }
        if (sz != FF1_SAVE_SIZE) { logw(L"%ls\r\n", saves[i]); logw(S(S_BADSIZE)); free(b); bad++; continue; }
        ck = ff1_checksum_ok(b);
        logw(S(S_ITEM), saves[i], region == FF1_US ? L"US" : L"JP", name, ck ? L"\x2713" : L"\x2717");
        if (!e1) { sig = ff1_sig_count(b, k1); logw(S(sig == FF1_NSIG ? S_SIGOK : S_SIGBAD), sig, FF1_NSIG); }
        else logw(S(S_SIGNOKEY));
        if (!convert) { free(b); continue; }
        if (!e1 && sig != FF1_NSIG) { free(b); bad++; continue; }
        if (!ck) { logw(S(S_CKBAD)); free(b); bad++; continue; }
        if (!slot) { logw(S(S_NOSLOT)); free(b); bad++; continue; }
        if (region == to) logw(S(S_SAMEREGION));
        ch = ff1_convert(b, region, to, k2, &bi, &bid);
        if (ch < 0) { logw(S(S_UNMAPPED), bi, bid); free(b); bad++; continue; }
        tl = ff1_save_name(to, slot, tn); ff1_folder_name(tn, tl, fold);
        swprintf(od, MAX_PATH * 2, L"%ls\\54430004", out);
        SHCreateDirectoryExW(NULL, od, NULL);
        swprintf(od, MAX_PATH * 2, L"%ls\\54430004\\%hs", out, fold);
        if (dir_nonempty(od)) { logw(S(S_EXISTS), od); free(b); bad++; continue; }
        CreateDirectoryW(od, NULL);
        {
            uint8_t mb[160]; size_t mn = ff1_meta_bytes(tn, tl, mb); wchar_t tname[40];
            swprintf(p, MAX_PATH * 2, L"%ls\\%lc", od, to == FF1_US ? L'G' : L'N');
            if (!write_all(p, b, sz)) { logw(S(S_WRITEFAIL), p); free(b); bad++; continue; }
            swprintf(p, MAX_PATH * 2, L"%ls\\SaveMeta.xbx", od);
            if (!write_all(p, mb, (DWORD)mn)) { logw(S(S_WRITEFAIL), p); free(b); bad++; continue; }
            { wchar_t si[MAX_PATH * 2]; swprintf(si, MAX_PATH * 2, L"%ls\\saveimage.xbx", saves[i]);
              swprintf(p, MAX_PATH * 2, L"%ls\\saveimage.xbx", od);
              if (GetFileAttributesW(si) != INVALID_FILE_ATTRIBUTES) CopyFileW(si, p, TRUE); }
            for (j = 0; j < tl; j++) tname[j] = tn[j];
            tname[tl] = 0;
            logw(S(S_DONE1), to == FF1_US ? L"US" : L"JP", tname, ch, od);
        }
        free(b); ok++;
    }
    if (convert) logw(S(S_SUMMARY), ok, bad); else logw(L"\r\n");
    SetCursor(old);
}

/* ---------------- folder picker ---------------- */
static int CALLBACK br_cb(HWND h, UINT m, LPARAM l, LPARAM data) {
    (void)l; if (m == BFFM_INITIALIZED && data) SendMessageW(h, BFFM_SETSELECTIONW, TRUE, data); return 0;
}
static int pick_folder(int title, wchar_t *path) {
    BROWSEINFOW bi = {0}; LPITEMIDLIST pl; wchar_t cur[MAX_PATH];
    wcsncpy(cur, path, MAX_PATH - 1); cur[MAX_PATH - 1] = 0;
    bi.hwndOwner = hMain; bi.lpszTitle = S(title);
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE; bi.lpfn = br_cb; bi.lParam = (LPARAM)(cur[0] ? cur : NULL);
    pl = SHBrowseForFolderW(&bi);
    if (!pl) return 0;
    SHGetPathFromIDListW(pl, path); CoTaskMemFree(pl); return 1;
}
static void default_out(const wchar_t *src) {
    wchar_t b[MAX_PATH], *s; wcsncpy(b, src, MAX_PATH - 1); b[MAX_PATH - 1] = 0; trim_slash(b);
    if (has_save_file(b, NULL) && (s = wcsrchr(b, L'\\'))) *s = 0;
    s = wcsrchr(b, L'\\');
    if (s && !_wcsicmp(s + 1, L"54430004")) *s = 0;
    if (wcslen(b) + 16 < MAX_PATH) { wcscat(b, L"\\FF1_converted"); SetDlgItemTextW(hMain, ID_OUT, b); }
}

/* ---------------- layout ---------------- */
static void apply_lang(void) {
    SetDlgItemTextW(hMain, ID_L_SRC, S(S_L_SRC)); SetDlgItemTextW(hMain, ID_L_HD, S(S_L_HD));
    SetDlgItemTextW(hMain, ID_L_HD2, S(S_L_HD2)); SetDlgItemTextW(hMain, ID_L_TO, S(S_L_TO));
    SetDlgItemTextW(hMain, ID_US, S(S_US)); SetDlgItemTextW(hMain, ID_JP, S(S_JP));
    SetDlgItemTextW(hMain, ID_L_OUT, S(S_L_OUT)); SetDlgItemTextW(hMain, ID_SRC_BR, S(S_BROWSE));
    SetDlgItemTextW(hMain, ID_OUT_BR, S(S_BROWSE)); SetDlgItemTextW(hMain, ID_CHECK, S(S_CHECK));
    SetDlgItemTextW(hMain, ID_CONVERT, S(S_CONVERT)); SetDlgItemTextW(hMain, ID_LANG, S(S_LANG));
}
static HWND mk(const wchar_t *cls, DWORD style, DWORD ex, int id) {
    HWND h = CreateWindowExW(ex, cls, L"", WS_CHILD | WS_VISIBLE | style, 0, 0, 10, 10, hMain, (HMENU)(INT_PTR)id, GetModuleHandleW(NULL), NULL);
    SendMessageW(h, WM_SETFONT, (WPARAM)hFont, TRUE); return h;
}
static void layout(int W, int H) {
    int m = DP(12), lw = DP(280), bh = DP(26), bw = DP(96), gap = DP(8), y = m, x2 = m + lw, ew = W - x2 - m - bw - gap;
    #define MV(id, x, yy, w, h) MoveWindow(GetDlgItem(hMain, id), x, yy, w, h, TRUE)
    MV(ID_L_SRC, m, y + DP(4), lw, bh); MV(ID_SRC, x2, y, ew, bh); MV(ID_SRC_BR, x2 + ew + gap, y, bw, bh); y += bh + gap;
    MV(ID_L_HD, m, y + DP(4), lw, bh); MV(ID_HD, x2, y, ew, bh); y += bh + gap;
    MV(ID_L_HD2, m, y + DP(4), lw, bh); MV(ID_HD2, x2, y, ew, bh); y += bh + gap;
    MV(ID_L_TO, m, y + DP(4), lw, bh); MV(ID_US, x2, y, DP(170), bh); MV(ID_JP, x2 + DP(180), y, DP(170), bh); y += bh + gap;
    MV(ID_L_OUT, m, y + DP(4), lw, bh); MV(ID_OUT, x2, y, ew, bh); MV(ID_OUT_BR, x2 + ew + gap, y, bw, bh); y += bh + gap + DP(4);
    MV(ID_CHECK, x2, y, bw, DP(30)); MV(ID_CONVERT, x2 + bw + gap, y, bw + DP(20), DP(30)); MV(ID_LANG, W - m - bw, y, bw, DP(30));
    y += DP(30) + gap + DP(4);
    MV(ID_LOG, m, y, W - 2 * m, H - y - m);
}

static LRESULT CALLBACK wndproc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: {
        NONCLIENTMETRICSW ncm; HDC dc; memset(&ncm, 0, sizeof ncm); ncm.cbSize = sizeof ncm; dc = GetDC(h);
        hMain = h; dpi = GetDeviceCaps(dc, LOGPIXELSY); ReleaseDC(h, dc);
        SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof ncm, &ncm, 0);
        ncm.lfMessageFont.lfHeight = -DP(14); hFont = CreateFontIndirectW(&ncm.lfMessageFont);
        mk(L"STATIC", 0, 0, ID_L_SRC); mk(L"EDIT", ES_AUTOHSCROLL | WS_TABSTOP, WS_EX_CLIENTEDGE, ID_SRC); mk(L"BUTTON", WS_TABSTOP, 0, ID_SRC_BR);
        mk(L"STATIC", 0, 0, ID_L_HD); mk(L"EDIT", ES_AUTOHSCROLL | WS_TABSTOP, WS_EX_CLIENTEDGE, ID_HD);
        mk(L"STATIC", 0, 0, ID_L_HD2); mk(L"EDIT", ES_AUTOHSCROLL | WS_TABSTOP, WS_EX_CLIENTEDGE, ID_HD2);
        mk(L"STATIC", 0, 0, ID_L_TO); mk(L"BUTTON", BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP, 0, ID_US); mk(L"BUTTON", BS_AUTORADIOBUTTON, 0, ID_JP);
        mk(L"STATIC", 0, 0, ID_L_OUT); mk(L"EDIT", ES_AUTOHSCROLL | WS_TABSTOP, WS_EX_CLIENTEDGE, ID_OUT); mk(L"BUTTON", WS_TABSTOP, 0, ID_OUT_BR);
        mk(L"BUTTON", WS_TABSTOP, 0, ID_CHECK); mk(L"BUTTON", BS_DEFPUSHBUTTON | WS_TABSTOP, 0, ID_CONVERT); mk(L"BUTTON", WS_TABSTOP, 0, ID_LANG);
        hLog = mk(L"EDIT", ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL | WS_VSCROLL, WS_EX_CLIENTEDGE, ID_LOG);
        SendMessageW(hLog, EM_SETLIMITTEXT, 0, 0);
        CheckRadioButton(h, ID_US, ID_JP, ID_US);
        apply_lang(); logw(S(S_INTRO));
        return 0; }
    case WM_SIZE: layout(LOWORD(lp), HIWORD(lp)); return 0;
    case WM_GETMINMAXINFO: ((MINMAXINFO *)lp)->ptMinTrackSize.x = DP(700); ((MINMAXINFO *)lp)->ptMinTrackSize.y = DP(480); return 0;
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case ID_SRC_BR: { wchar_t p[MAX_PATH]; get_text(ID_SRC, p, MAX_PATH);
            if (pick_folder(S_PICK_SRC, p)) { wchar_t w; SetDlgItemTextW(h, ID_SRC, p); default_out(p);
                if (has_save_file(p, &w)) CheckRadioButton(h, ID_US, ID_JP, w == L'G' ? ID_JP : ID_US);
                else if (find_saves(p) > 0 && has_save_file(saves[0], &w)) CheckRadioButton(h, ID_US, ID_JP, w == L'G' ? ID_JP : ID_US); } break; }
        case ID_OUT_BR: { wchar_t p[MAX_PATH]; get_text(ID_OUT, p, MAX_PATH); if (pick_folder(S_PICK_OUT, p)) SetDlgItemTextW(h, ID_OUT, p); break; }
        case ID_CHECK: do_work(0); break;
        case ID_CONVERT: do_work(1); break;
        case ID_LANG: lang ^= 1; apply_lang(); break;
        }
        return 0;
    case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(h, msg, wp, lp);
}

int WINAPI wWinMain(HINSTANCE hi, HINSTANCE hp, LPWSTR cmd, int show) {
    WNDCLASSW wc = {0}; MSG m; HWND h; HDC dc; int d;
    (void)hp;
    lang = (PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_CHINESE) ? 0 : 1;
    if (cmd && wcsstr(cmd, L"--en")) lang = 1;
    if (cmd && wcsstr(cmd, L"--zh")) lang = 0;
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    wc.lpfnWndProc = wndproc; wc.hInstance = hi; wc.lpszClassName = L"FatalFrame1SaveConverter";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassW(&wc);
    dc = GetDC(NULL); d = GetDeviceCaps(dc, LOGPIXELSY); ReleaseDC(NULL, dc);
    h = CreateWindowExW(0, wc.lpszClassName, APP_TITLE, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                        MulDiv(820, d, 96), MulDiv(600, d, 96), NULL, NULL, hi, NULL);
    ShowWindow(h, show);
    while (GetMessageW(&m, NULL, 0, 0) > 0) {
        if (!IsDialogMessageW(h, &m)) { TranslateMessage(&m); DispatchMessageW(&m); }
    }
    CoUninitialize();
    return 0;
}
