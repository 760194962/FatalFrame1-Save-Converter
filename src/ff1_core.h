/* FatalFrame1 Save Converter - core (portable C, no I/O) */
#ifndef FF1_CORE_H
#define FF1_CORE_H
#include <stddef.h>
#include <stdint.h>

#define FF1_SAVE_SIZE 0x2945CC0u
#define FF1_HEADER    0x5414u
#define FF1_BLOCK     0x60000u
#define FF1_NBLOCK    0x6E
#define FF1_NSIG      (FF1_NBLOCK + 1)

enum { FF1_US = 0, FF1_JP = 1 };

/* blocks whose signature matches hdkey (0..FF1_NSIG) */
int  ff1_sig_count(const uint8_t *save, const uint8_t hdkey[16]);
int  ff1_checksum_ok(const uint8_t *save);
/* remap file ids (if from != to), fix checksum, re-sign with hdkey.
   returns number of remapped ids, or -1 with bad_index and bad_id set. */
int  ff1_convert(uint8_t *save, int from, int to, const uint8_t hdkey[16], int *bad_index, int *bad_id);
/* list loaded file ids; returns count */
int  ff1_resources(const uint8_t *save, int ids[40]);

/* save name for region/slot as UTF-16; returns length (no terminator written past len) */
int  ff1_save_name(int region, int slot, uint16_t out[32]);
/* save folder name (12 hex chars + NUL) from UTF-16 save name */
void ff1_folder_name(const uint16_t *name, int len, char out[13]);
/* SaveMeta.xbx -> name (UTF-16, NUL-terminated); returns length or -1 */
int  ff1_meta_name(const uint8_t *meta, size_t n, uint16_t out[64]);
/* SaveMeta.xbx bytes for a name; returns size */
size_t ff1_meta_bytes(const uint16_t *name, int len, uint8_t out[160]);
/* first decimal number in name, or 0 */
int  ff1_slot_from_name(const uint16_t *name, int len);
/* parse 32 hex digits (spaces/colons/dashes ignored); 0 on success */
int  ff1_parse_hdkey(const char *s, uint8_t out[16]);
#endif
