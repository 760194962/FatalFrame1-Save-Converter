/* checks the C core against files written by the Python version:
   core_test SRC_SAVE SRC_META EXPECTED_SAVE from(us|jp) to(us|jp) HDKEY */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/ff1_core.h"
static uint8_t *rd(const char *p, size_t *n) { FILE *f = fopen(p, "rb"); uint8_t *b; if (!f) { perror(p); exit(2); }
    fseek(f, 0, SEEK_END); *n = ftell(f); fseek(f, 0, SEEK_SET); b = malloc(*n); if (fread(b, 1, *n, f) != *n) exit(2); fclose(f); return b; }
int main(int c, char **v) {
    if (c < 7) { fprintf(stderr, "usage: core_test SRC META EXPECTED us|jp us|jp HDKEY\n"); return 2; }
    size_t n, mn, en; uint8_t *s = rd(v[1], &n), *m = rd(v[2], &mn), *e = rd(v[3], &en), hd[16];
    int from = !strcmp(v[4], "jp"), to = !strcmp(v[5], "jp"), bi, bid, ch; uint16_t name[64], nn[32]; char fold[13];
    if (ff1_parse_hdkey(v[6], hd)) return 3;
    int len = ff1_meta_name(m, mn, name);
    printf("sig %d/%d checksum %d slot %d\n", ff1_sig_count(s, hd), FF1_NSIG, ff1_checksum_ok(s), ff1_slot_from_name(name, len));
    ch = ff1_convert(s, from, to, hd, &bi, &bid);
    int l2 = ff1_save_name(to, ff1_slot_from_name(name, len), nn); ff1_folder_name(nn, l2, fold);
    printf("changed %d folder %s same_as_expected %d\n", ch, fold, n == en && !memcmp(s, e, n));
    return !(n == en && !memcmp(s, e, n));
}
