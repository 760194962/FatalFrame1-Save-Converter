/* FatalFrame1 Save Converter - core: SHA-1/HMAC, XCalculateSignature, file-id remap */
#include <string.h>
#include "ff1_core.h"
#include "idmap.h"

/* ---------------- SHA-1 ---------------- */
typedef struct { uint32_t h[5]; uint64_t n; uint8_t buf[64]; size_t len; } sha1_t;
#define ROL(x,c) (((x)<<(c))|((x)>>(32-(c))))
static void sha1_block(sha1_t *s, const uint8_t *p) {
    uint32_t w[80], a, b, c, d, e, t; int i;
    for (i = 0; i < 16; i++) w[i] = (uint32_t)p[4*i]<<24 | (uint32_t)p[4*i+1]<<16 | (uint32_t)p[4*i+2]<<8 | p[4*i+3];
    for (; i < 80; i++) w[i] = ROL(w[i-3]^w[i-8]^w[i-14]^w[i-16], 1);
    a = s->h[0]; b = s->h[1]; c = s->h[2]; d = s->h[3]; e = s->h[4];
    for (i = 0; i < 80; i++) {
        uint32_t f, k;
        if (i < 20)      { f = (b & c) | (~b & d);          k = 0x5A827999; }
        else if (i < 40) { f = b ^ c ^ d;                   k = 0x6ED9EBA1; }
        else if (i < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8F1BBCDC; }
        else             { f = b ^ c ^ d;                   k = 0xCA62C1D6; }
        t = ROL(a,5) + f + e + k + w[i]; e = d; d = c; c = ROL(b,30); b = a; a = t;
    }
    s->h[0] += a; s->h[1] += b; s->h[2] += c; s->h[3] += d; s->h[4] += e;
}
static void sha1_init(sha1_t *s) {
    s->h[0]=0x67452301; s->h[1]=0xEFCDAB89; s->h[2]=0x98BADCFE; s->h[3]=0x10325476; s->h[4]=0xC3D2E1F0;
    s->n = 0; s->len = 0;
}
static void sha1_update(sha1_t *s, const uint8_t *p, size_t n) {
    s->n += n;
    if (s->len) { size_t k = 64 - s->len; if (k > n) k = n; memcpy(s->buf + s->len, p, k); s->len += k; p += k; n -= k;
                  if (s->len == 64) { sha1_block(s, s->buf); s->len = 0; } }
    while (n >= 64) { sha1_block(s, p); p += 64; n -= 64; }
    if (n) { memcpy(s->buf, p, n); s->len = n; }
}
static void sha1_final(sha1_t *s, uint8_t out[20]) {
    uint64_t bits = s->n * 8; uint8_t pad = 0x80, z = 0, l[8]; int i;
    sha1_update(s, &pad, 1);
    while (s->len != 56) sha1_update(s, &z, 1);
    for (i = 0; i < 8; i++) l[i] = (uint8_t)(bits >> (56 - 8*i));
    sha1_update(s, l, 8);
    for (i = 0; i < 20; i++) out[i] = (uint8_t)(s->h[i/4] >> (24 - 8*(i%4)));
}
static void hmac_sha1(const uint8_t *key, size_t kn, const uint8_t *d, size_t n, uint8_t out[20]) {
    uint8_t k[64] = {0}, ip[64], op[64], ih[20]; sha1_t s; int i;
    memcpy(k, key, kn);                       /* keys here are <= 20 bytes */
    for (i = 0; i < 64; i++) { ip[i] = k[i] ^ 0x36; op[i] = k[i] ^ 0x5C; }
    sha1_init(&s); sha1_update(&s, ip, 64); sha1_update(&s, d, n); sha1_final(&s, ih);
    sha1_init(&s); sha1_update(&s, op, 64); sha1_update(&s, ih, 20); sha1_final(&s, out);
}

/* ---------------- signature ---------------- */
static const uint8_t CERT_KEY[16]  = {0x5C,0x07,0x33,0xAE,0x04,0x01,0xF7,0xE8,0xBA,0x79,0x93,0xFD,0xCD,0x2F,0x1F,0xE0};
static const uint8_t TITLE_KEY[16] = {0xBD,0x1E,0x1C,0x7B,0x4D,0xB4,0xBA,0x8D,0x49,0xE3,0x7E,0xA2,0x4F,0x80,0xF1,0x4E};
static void sign(const uint8_t *d, size_t n, const uint8_t hd[16], uint8_t out[20]) {
    uint8_t sk[20], t[20];
    hmac_sha1(CERT_KEY, 16, TITLE_KEY, 16, sk);
    hmac_sha1(sk, 16, d, n, t);
    hmac_sha1(hd, 16, t, 20, out);
}
static size_t blk_off(int i) { return i == 0 ? 0 : FF1_HEADER + 20 + (size_t)(i - 1) * (FF1_BLOCK + 20); }
static size_t blk_len(int i) { return i == 0 ? FF1_HEADER : FF1_BLOCK; }

int ff1_sig_count(const uint8_t *save, const uint8_t hd[16]) {
    int i, ok = 0; uint8_t s[20];
    for (i = 0; i < FF1_NSIG; i++) {
        size_t o = blk_off(i), n = blk_len(i);
        sign(save + o, n, hd, s);
        ok += memcmp(s, save + o + n, 20) == 0;
    }
    return ok;
}
static uint32_t hsum(const uint8_t *save) { uint32_t s = 0; size_t i; for (i = 4; i < FF1_HEADER; i++) s += save[i]; return s; }
static uint32_t rd32(const uint8_t *p) { return p[0] | p[1]<<8 | p[2]<<16 | (uint32_t)p[3]<<24; }
int ff1_checksum_ok(const uint8_t *save) { return rd32(save) == hsum(save); }

#define RES_TABLE 0x3C
#define RES_COUNT 40
/* model (8), animation (9, 10) and sound bank (2) entries are not carried across versions:
   a JP clear save hangs the US game on load when both kinds are kept, and the game
   reloads them by itself when they are missing (tested on Xbox 360 and xemu). */
static int res_dropped(int type) { return type == 2 || type == 8 || type == 9 || type == 10; }
int ff1_resources(const uint8_t *save, int ids[40]) {
    int i, n = 0;
    for (i = 0; i < RES_COUNT; i++) { int id = save[RES_TABLE+8*i] | save[RES_TABLE+8*i+1] << 8; if (id != 0xFFFF) ids[n++] = id; }
    return n;
}
static int map_id(int from, int id) {
    const unsigned short (*r)[3] = from == FF1_JP ? FF_J2U : FF_U2J;
    size_t n = from == FF1_JP ? sizeof FF_J2U / sizeof *FF_J2U : sizeof FF_U2J / sizeof *FF_U2J, i;
    for (i = 0; i < n; i++) if (id >= r[i][0] && id < r[i][0] + r[i][2]) return r[i][1] + (id - r[i][0]);
    return -1;
}
int ff1_convert(uint8_t *save, int from, int to, const uint8_t hd[16], int *bad_index, int *bad_id) {
    int i, changed = 0; uint32_t s;
    if (from != to) {
        int nfiles = from == FF1_JP ? FF_JP_FILES : FF_US_FILES;
        for (i = 0; i < RES_COUNT; i++) {          /* check all first, so nothing is half-done */
            int id = save[RES_TABLE+8*i] | save[RES_TABLE+8*i+1] << 8;
            if (id == 0xFFFF || res_dropped(save[RES_TABLE+8*i+2])) continue;
            if (id >= nfiles || map_id(from, id) < 0) { if (bad_index) *bad_index = i; if (bad_id) *bad_id = id; return -1; }
        }
        for (i = 0; i < RES_COUNT; i++) {
            uint8_t *p = save + RES_TABLE + 8*i; int id = p[0] | p[1] << 8, m;
            if (id == 0xFFFF) continue;
            if (res_dropped(p[2])) { p[0] = p[1] = 0xFF; continue; }
            m = map_id(from, id);
            if (m != id) { p[0] = (uint8_t)m; p[1] = (uint8_t)(m >> 8); changed++; }
        }
        s = hsum(save);
        save[0] = (uint8_t)s; save[1] = (uint8_t)(s>>8); save[2] = (uint8_t)(s>>16); save[3] = (uint8_t)(s>>24);
    }
    for (i = 0; i < FF1_NSIG; i++) { size_t o = blk_off(i), n = blk_len(i); sign(save + o, n, hd, save + o + n); }
    return changed;
}

/* ---------------- names ---------------- */
int ff1_save_name(int region, int slot, uint16_t out[32]) {
    char num[12]; int n = 0, i;
    static const uint16_t JP_TAIL[] = {' ', 0x30B2, 0x30FC, 0x30E0, 0x30C7, 0x30FC, 0x30BF};   /* " ゲームデータ" */
    const char *pre = region == FF1_US ? "Game No." : "No.";
    for (i = 0; pre[i]; i++) out[n++] = (uint8_t)pre[i];
    i = 0; do { num[i++] = (char)('0' + slot % 10); slot /= 10; } while (slot && i < 11);
    while (i) out[n++] = (uint8_t)num[--i];
    if (region == FF1_JP) for (i = 0; i < 7; i++) out[n++] = JP_TAIL[i];
    return n;
}
void ff1_folder_name(const uint16_t *name, int len, char out[13]) {
    const uint64_t P = (1ull << 48) - 59; uint64_t h = 0; int i;
    for (i = 0; i < len; i++) {                 /* (h * 0x10000 + c) mod P without overflow */
        uint64_t x = h; int k;
        for (k = 0; k < 16; k++) { x <<= 1; if (x >= P) x -= P; }
        h = (x + name[i]) % P;
    }
    for (i = 11; i >= 0; i--) { out[i] = "0123456789ABCDEF"[h & 15]; h >>= 4; }
    out[12] = 0;
}
int ff1_meta_name(const uint8_t *m, size_t n, uint16_t out[64]) {
    size_t i = 0; int k = 0, j;
    static const char key[] = "Name=";
    if (n >= 2 && m[0] == 0xFF && m[1] == 0xFE) i = 2;
    for (; i + 1 < n; i += 2) {
        uint16_t c = m[i] | m[i+1] << 8;
        if (k < 5) { k = (c == (uint8_t)key[k]) ? k + 1 : (c == 'N'); if (k == 5) { j = 0;
            for (i += 2; i + 1 < n && j < 63; i += 2) { c = m[i] | m[i+1] << 8; if (c == '\r' || c == '\n' || c == 0) break; out[j++] = c; }
            out[j] = 0; return j; } }
    }
    return -1;
}
size_t ff1_meta_bytes(const uint16_t *name, int len, uint8_t out[160]) {
    size_t n = 0; int i; static const char key[] = "Name=";
    out[n++] = 0xFF; out[n++] = 0xFE;
    for (i = 0; key[i]; i++) { out[n++] = (uint8_t)key[i]; out[n++] = 0; }
    for (i = 0; i < len && i < 70; i++) { out[n++] = (uint8_t)name[i]; out[n++] = (uint8_t)(name[i] >> 8); }
    out[n++] = '\r'; out[n++] = 0; out[n++] = '\n'; out[n++] = 0;
    return n;
}
int ff1_slot_from_name(const uint16_t *name, int len) {
    int i = 0, v = 0;
    while (i < len && !(name[i] >= '0' && name[i] <= '9')) i++;
    while (i < len && name[i] >= '0' && name[i] <= '9' && v < 100000) v = v * 10 + (name[i++] - '0');
    return v;
}
int ff1_parse_hdkey(const char *s, uint8_t out[16]) {
    int n = 0, hi = -1;
    for (; *s; s++) {
        int v = *s >= '0' && *s <= '9' ? *s - '0' : *s >= 'a' && *s <= 'f' ? *s - 'a' + 10 : *s >= 'A' && *s <= 'F' ? *s - 'A' + 10 : -2;
        if (v == -2) { if (*s == ' ' || *s == ':' || *s == '-' || *s == '\t') continue; return -1; }
        if (hi < 0) hi = v; else { if (n == 16) return -1; out[n++] = (uint8_t)(hi << 4 | v); hi = -1; }
    }
    return n == 16 && hi < 0 ? 0 : -1;
}
