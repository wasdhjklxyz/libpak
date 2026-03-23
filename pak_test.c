#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "pak.h"

#define TEST_ARRAY "hello world"

#define TEST_FIELDS(F, A, S)                                                   \
  F(S, i8, int8_t)                                                             \
  F(S, u8, uint8_t)                                                            \
  F(S, i16, int16_t)                                                           \
  F(S, u16, uint16_t)                                                          \
  F(S, i32, int32_t)                                                           \
  F(S, u32, uint32_t)                                                          \
  F(S, i64, int64_t)                                                           \
  F(S, u64, uint64_t)                                                          \
  A(S, arr, char, 29)
PAK_DEFINE(test, TEST_FIELDS)

int main(void) {
  test_t test = {
      .i8 = 0x12,
      .u8 = 0xAB,
      .i16 = 0x1234,
      .u16 = 0xABCD,
      .i32 = 0x12345678,
      .u32 = 0xDEADBEEF,
      .i64 = 0x12345678DEADBEEF,
      .u64 = 0xDEADBEEFABCD1234,
      .arr = "hello world",
  };

  uint8_t buf[1024];
  ssize_t n = pak_serialize_test(&test, buf, sizeof(buf));
  assert(n > 0);
  printf("wire size: %zd bytes\n", n);
  printf("struct size: %zu bytes\n", sizeof(test_t));

  test_t got = {0};
  int ret = pak_deserialize_test(buf, (size_t)n, &got);
  assert(ret == 0);

  assert(got.i8 == 0x12);
  assert(got.u8 == 0xAB);
  assert(got.i16 == 0x1234);
  assert(got.u16 == 0xABCD);
  assert(got.i32 == 0x12345678);
  assert(got.u32 == 0xDEADBEEF);
  assert(got.i64 == 0x12345678DEADBEEF);
  assert(got.u64 == 0xDEADBEEFABCD1234);
  assert(memcmp(got.arr, TEST_ARRAY, sizeof(TEST_ARRAY)) == 0);

  return 0;
}
