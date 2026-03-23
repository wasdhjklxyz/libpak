#include <endian.h>
#include <stddef.h>
#include <string.h>

#include "pak.h"

#define _PAK_HTOBE(n, src, offset, buf, pos)                                   \
  do {                                                                         \
    uint##n##_t tmp;                                                           \
    memcpy(&tmp, (const char *)src + offset, n / 8);                           \
    tmp = htobe##n(tmp);                                                       \
    memcpy((char *)buf + pos, &tmp, n / 8);                                    \
    pos += n / 8;                                                              \
  } while (0);

#define _PAK_BETOH(n, dst, offset, buf, pos)                                   \
  do {                                                                         \
    uint##n##_t tmp;                                                           \
    memcpy(&tmp, (const char *)buf + pos, n / 8);                              \
    tmp = be##n##toh(tmp);                                                     \
    memcpy((char *)dst + offset, &tmp, n / 8);                                 \
    pos += n / 8;                                                              \
  } while (0);

size_t pak_sizeof(const pak_field_t *fields, size_t nfields) {
  size_t n = 0;
  for (size_t i = 0; i < nfields; i++)
    n += fields[i].size * fields[i].count;
  return n;
};

ssize_t pak_serialize(const pak_field_t *fields, size_t nfields,
                      const void *src, void *buf, size_t len) {
  if (!fields || !src || !buf)
    return -1;

  size_t fields_size = pak_sizeof(fields, nfields);
  if (fields_size > len)
    return -1;

  size_t pos = 0;
  for (size_t i = 0; i < nfields; i++) {
    if (fields[i].count == 1 && fields[i].size > 1) {
      if (fields[i].size == 2) {
        _PAK_HTOBE(16, src, fields[i].offset, buf, pos);
      } else if (fields[i].size == 4) {
        _PAK_HTOBE(32, src, fields[i].offset, buf, pos);
      } else if (fields[i].size == 8) {
        _PAK_HTOBE(64, src, fields[i].offset, buf, pos);
      }
    } else { /* NOTE: Arrays are just blobs of bytes */
      size_t field_bytes = fields[i].size * fields[i].count;
      memcpy((char *)buf + pos, (const char *)src + fields[i].offset,
             field_bytes);
      pos += field_bytes;
    }
  }

  return (ssize_t)fields_size;
}

int pak_deserialize(const pak_field_t *fields, size_t nfields, const void *buf,
                    size_t len, void *dst) {
  if (!fields || !buf || !dst)
    return -1;

  size_t fields_size = pak_sizeof(fields, nfields);
  if (fields_size != len)
    return -1;

  size_t pos = 0;
  for (size_t i = 0; i < nfields; i++) {
    if (fields[i].count == 1 && fields[i].size > 1) {
      if (fields[i].size == 2) {
        _PAK_BETOH(16, dst, fields[i].offset, buf, pos);
      } else if (fields[i].size == 4) {
        _PAK_BETOH(32, dst, fields[i].offset, buf, pos);
      } else if (fields[i].size == 8) {
        _PAK_BETOH(64, dst, fields[i].offset, buf, pos);
      }
    } else { /* NOTE: Arrays are just blobs of bytes */
      size_t field_bytes = fields[i].size * fields[i].count;
      memcpy((char *)dst + fields[i].offset, (const char *)buf + pos,
             field_bytes);
      pos += field_bytes;
    }
  }

  return 0;
}
