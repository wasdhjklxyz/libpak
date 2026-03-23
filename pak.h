#ifndef PAK_H
#define PAK_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _PAK_SF(_, name, type) type name;
#define _PAK_SA(_, name, type, len) type name[len];

#define _PAK_TF(sname, fname, type) {offsetof(sname, fname), sizeof(type), 1},
#define _PAK_TA(sname, aname, type, len)                                       \
  {offsetof(sname, aname), sizeof(type), (len)},

typedef struct {
  size_t offset;
  size_t size;
  size_t count;
} pak_field_t;

size_t pak_sizeof(const pak_field_t *fields, size_t nfields);

ssize_t pak_serialize(const pak_field_t *fields, size_t nfields,
                      const void *src, void *buf, size_t len);

int pak_deserialize(const pak_field_t *fields, size_t nfields, const void *buf,
                    size_t len, void *dst);

#define PAK_DEFINE(name, fields)                                               \
  typedef struct name {                                                        \
    fields(_PAK_SF, _PAK_SA, _)                                                \
  } name##_t;                                                                  \
  static const pak_field_t pak_fields_##name[] = {                             \
      fields(_PAK_TF, _PAK_TA, struct name)};                                  \
  static inline size_t pak_sizeof_##name(void) {                               \
    return pak_sizeof(pak_fields_##name, sizeof(pak_fields_##name) /           \
                                             sizeof(pak_fields_##name[0]));    \
  };                                                                           \
  static inline ssize_t pak_serialize_##name(const struct name *src,           \
                                             void *buf, size_t len) {          \
    return pak_serialize(pak_fields_##name,                                    \
                         sizeof(pak_fields_##name) /                           \
                             sizeof(pak_fields_##name[0]),                     \
                         src, buf, len);                                       \
  }                                                                            \
  static inline int pak_deserialize_##name(const void *buf, size_t len,        \
                                           struct name *dst) {                 \
    return pak_deserialize(pak_fields_##name,                                  \
                           sizeof(pak_fields_##name) /                         \
                               sizeof(pak_fields_##name[0]),                   \
                           buf, len, dst);                                     \
  }

#define PAK_DEFINE_EMPTY(name)                                                 \
  typedef struct name {                                                        \
    char _empty;                                                               \
  } name##_t;                                                                  \
  static inline size_t pak_sizeof_##name(void) { return 0; };                  \
  static inline ssize_t pak_serialize_##name(const struct name *src,           \
                                             void *buf, size_t len) {          \
    (void)src;                                                                 \
    (void)buf;                                                                 \
    (void)len;                                                                 \
    return 0;                                                                  \
  };                                                                           \
  static inline int pak_deserialize_##name(const void *buf, size_t len,        \
                                           struct name *dst) {                 \
    (void)buf;                                                                 \
    (void)dst;                                                                 \
    return len == 0 ? 0 : -1;                                                  \
  }

#ifdef __cplusplus
}
#endif

#endif /* PAK_H */
