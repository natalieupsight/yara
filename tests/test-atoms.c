/*
Copyright (c) 2018. The YARA Authors. All Rights Reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <yara.h>
#include "util.h"


void test_table_quality()
{
  YR_ATOM_QUALITY_TABLE_ENTRY l1[] = {
    {{0x00, 0x00, 0x00, 0x00}, 1},
    {{0x00, 0x00, 0x00, 0x01}, 2},
    {{0x00, 0x00, 0x00, 0x02}, 3},
  };

  YR_ATOM_QUALITY_TABLE_ENTRY l2[] = {
    {{0x00, 0x00, 0x00, 0x00}, 1},
    {{0x00, 0x00, 0x00, 0x01}, 2},
    {{0x00, 0x00, 0x00, 0x02}, 3},
    {{0x00, 0x00, 0x00, 0x03}, 4},
  };

  YR_ATOM a0 = {
    .length = 4,
    .bytes = {0x00, 0x00, 0x00, 0x00},
    .mask = {0xFF, 0xFF, 0xFF, 0xFF}};

  YR_ATOM a1 = {4, {0x00, 0x00, 0x00, 0x01}, {0xFF, 0xFF, 0xFF, 0xFF}};
  YR_ATOM a2 = {4, {0x00, 0x00, 0x00, 0x02}, {0xFF, 0xFF, 0xFF, 0xFF}};
  YR_ATOM a3 = {4, {0x00, 0x00, 0x00, 0x03}, {0xFF, 0xFF, 0xFF, 0xFF}};
  YR_ATOM a4 = {4, {0x00, 0x00, 0x00, 0x00}, {0xFF, 0xFF, 0xFF, 0x00}};

  YR_ATOMS_CONFIG c;

  c.get_atom_quality = yr_atoms_table_quality;
  c.quality_table = l1;
  c.quality_table_entries = 3;

  assert_true_expr(yr_atoms_table_quality(&c, &a0) == 1);
  assert_true_expr(yr_atoms_table_quality(&c, &a1) == 2);
  assert_true_expr(yr_atoms_table_quality(&c, &a2) == 3);
  assert_true_expr(yr_atoms_table_quality(&c, &a3) == YR_MAX_ATOM_QUALITY);

  c.quality_table = l2;
  c.quality_table_entries = 4;

  assert_true_expr(yr_atoms_table_quality(&c, &a0) == 1);
  assert_true_expr(yr_atoms_table_quality(&c, &a1) == 2);
  assert_true_expr(yr_atoms_table_quality(&c, &a2) == 3);
  assert_true_expr(yr_atoms_table_quality(&c, &a3) == 4);
  assert_true_expr(yr_atoms_table_quality(&c, &a4) == 1);
}


void test_heuristic_quality()
{
  YR_ATOMS_CONFIG c;

  YR_ATOM a00000000 = {
    .length = 4,
    .bytes = {0x00, 0x00, 0x00, 0x00},
    .mask = {0xFF, 0xFF, 0xFF, 0xFF}};

  YR_ATOM a00000001 = {
    .length = 4,
    .bytes = {0x00, 0x00, 0x00, 0x01},
    .mask = {0xFF, 0xFF, 0xFF, 0xFF}};

  YR_ATOM a00000102 = {
    .length = 4,
    .bytes = {0x00, 0x00, 0x01, 0x02},
    .mask = {0xFF, 0xFF, 0xFF, 0xFF}};

  YR_ATOM a00010203 = {
    .length = 4,
    .bytes = {0x00, 0x01, 0x02, 0x03},
    .mask = {0xFF, 0xFF, 0xFF, 0xFF}};

  YR_ATOM a01020304 = {
    .length = 4,
    .bytes = {0x01, 0x02, 0x03, 0x04},
    .mask = {0xFF, 0xFF, 0xFF, 0xFF}};

  YR_ATOM a01 = {
    .length = 1,
    .bytes = {0x01},
    .mask = {0xFF}};

  YR_ATOM a0001 = {
    .length = 2,
    .bytes = {0x00, 0x01},
    .mask = {0xFF, 0xFF}};

  YR_ATOM a000001 = {
    .length = 3,
    .bytes = {0x00, 0x00, 0x01},
    .mask = {0xFF, 0xFF, 0xFF}};

  YR_ATOM a000102 = {
    .length = 3,
    .bytes = {0x00, 0x01, 0x02},
    .mask = {0xFF, 0xFF, 0xFF}};

  YR_ATOM a010203 = {
    .length = 3,
    .bytes = {0x01, 0x02, 0x03},
    .mask = {0xFF, 0xFF, 0xFF}};

  YR_ATOM a0102 = {
    .length = 2,
    .bytes = {0x01, 0x02},
    .mask = {0xFF, 0xFF}};

  YR_ATOM a01020000 = {
    .length = 4,
    .bytes = {0x01, 0x02, 0x00, 0x00},
    .mask = {0xFF, 0xFF, 0xFF, 0xFF}};

  YR_ATOM a0102XX04 = {
    .length = 4,
    .bytes = {0x01, 0x02, 0x03, 0x04},
    .mask = {0xFF, 0xFF, 0x00, 0xFF}};

  c.get_atom_quality = yr_atoms_heuristic_quality;

  int q00000000 = yr_atoms_heuristic_quality(&c, &a00000000);
  int q00000001 = yr_atoms_heuristic_quality(&c, &a00000001);
  int q00000102 = yr_atoms_heuristic_quality(&c, &a00000102);
  int q00010203 = yr_atoms_heuristic_quality(&c, &a00010203);
  int q01020304 = yr_atoms_heuristic_quality(&c, &a01020304);
  int q010203   = yr_atoms_heuristic_quality(&c, &a010203);
  int q0102     = yr_atoms_heuristic_quality(&c, &a0102);
  int q01       = yr_atoms_heuristic_quality(&c, &a01);
  int q0001     = yr_atoms_heuristic_quality(&c, &a0001);
  int q000001   = yr_atoms_heuristic_quality(&c, &a000001);
  int q000102   = yr_atoms_heuristic_quality(&c, &a000102);
  int q01020000 = yr_atoms_heuristic_quality(&c, &a01020000);
  int q0102XX04 = yr_atoms_heuristic_quality(&c, &a0102XX04);


  a010203.mask[1] = 0x00;

  int q01XX03   = yr_atoms_heuristic_quality(&c, &a010203);

  a010203.mask[1] = 0x0F;

  int q01X203   = yr_atoms_heuristic_quality(&c, &a010203);

  a010203.mask[1] = 0x0F;

  int q010X03   = yr_atoms_heuristic_quality(&c, &a010203);

  a010203.mask[2] = 0x0F;

  int q010X0X   = yr_atoms_heuristic_quality(&c, &a010203);

  assert_true_expr(q00000001 > q00000000);
  assert_true_expr(q00000001 > q000001);
  assert_true_expr(q000001   > q0001);
  assert_true_expr(q00000102 > q00000001);
  assert_true_expr(q00010203 > q00000102);
  assert_true_expr(q01020304 > q00010203);
  assert_true_expr(q000102   > q000001);
  assert_true_expr(q00010203 > q010203);
  assert_true_expr(q010203   > q0102);
  assert_true_expr(q0102     > q01);
  assert_true_expr(q01X203   > q0102);
  assert_true_expr(q01X203   > q0001);
  assert_true_expr(q01X203   < q010203);
  assert_true_expr(q01X203   == q010X03);
  assert_true_expr(q01XX03   <= q0102);
  assert_true_expr(q01XX03   < q010X03);
  assert_true_expr(q01XX03   < q010203);
  assert_true_expr(q010X0X   > q01);
  assert_true_expr(q010X0X   < q010203);
  assert_true_expr(q01020000 > q0102XX04);

  assert_true_expr(q01020304 == YR_MAX_ATOM_QUALITY);
}



void test_atom_choose()
{

  struct atom atoms_61_XX_63_64[] = {
      {4, {0x61, 0x00, 0x63, 0x64}},
      {4, {0x61, 0x01, 0x63, 0x64}},
      {4, {0x61, 0x02, 0x63, 0x64}},
      {4, {0x61, 0x03, 0x63, 0x64}},
      {4, {0x61, 0x04, 0x63, 0x64}},
      {4, {0x61, 0x05, 0x63, 0x64}},
      {4, {0x61, 0x06, 0x63, 0x64}},
      {4, {0x61, 0x07, 0x63, 0x64}},
      {4, {0x61, 0x08, 0x63, 0x64}},
      {4, {0x61, 0x09, 0x63, 0x64}},
      {4, {0x61, 0x0A, 0x63, 0x64}},
      {4, {0x61, 0x0B, 0x63, 0x64}},
      {4, {0x61, 0x0C, 0x63, 0x64}},
      {4, {0x61, 0x0D, 0x63, 0x64}},
      {4, {0x61, 0x0E, 0x63, 0x64}},
      {4, {0x61, 0x0F, 0x63, 0x64}},
      {4, {0x61, 0x10, 0x63, 0x64}},
      {4, {0x61, 0x11, 0x63, 0x64}},
      {4, {0x61, 0x12, 0x63, 0x64}},
      {4, {0x61, 0x13, 0x63, 0x64}},
      {4, {0x61, 0x14, 0x63, 0x64}},
      {4, {0x61, 0x15, 0x63, 0x64}},
      {4, {0x61, 0x16, 0x63, 0x64}},
      {4, {0x61, 0x17, 0x63, 0x64}},
      {4, {0x61, 0x18, 0x63, 0x64}},
      {4, {0x61, 0x19, 0x63, 0x64}},
      {4, {0x61, 0x1A, 0x63, 0x64}},
      {4, {0x61, 0x1B, 0x63, 0x64}},
      {4, {0x61, 0x1C, 0x63, 0x64}},
      {4, {0x61, 0x1D, 0x63, 0x64}},
      {4, {0x61, 0x1E, 0x63, 0x64}},
      {4, {0x61, 0x1F, 0x63, 0x64}},
      {4, {0x61, 0x20, 0x63, 0x64}},
      {4, {0x61, 0x21, 0x63, 0x64}},
      {4, {0x61, 0x22, 0x63, 0x64}},
      {4, {0x61, 0x23, 0x63, 0x64}},
      {4, {0x61, 0x24, 0x63, 0x64}},
      {4, {0x61, 0x25, 0x63, 0x64}},
      {4, {0x61, 0x26, 0x63, 0x64}},
      {4, {0x61, 0x27, 0x63, 0x64}},
      {4, {0x61, 0x28, 0x63, 0x64}},
      {4, {0x61, 0x29, 0x63, 0x64}},
      {4, {0x61, 0x2A, 0x63, 0x64}},
      {4, {0x61, 0x2B, 0x63, 0x64}},
      {4, {0x61, 0x2C, 0x63, 0x64}},
      {4, {0x61, 0x2D, 0x63, 0x64}},
      {4, {0x61, 0x2E, 0x63, 0x64}},
      {4, {0x61, 0x2F, 0x63, 0x64}},
      {4, {0x61, 0x30, 0x63, 0x64}},
      {4, {0x61, 0x31, 0x63, 0x64}},
      {4, {0x61, 0x32, 0x63, 0x64}},
      {4, {0x61, 0x33, 0x63, 0x64}},
      {4, {0x61, 0x34, 0x63, 0x64}},
      {4, {0x61, 0x35, 0x63, 0x64}},
      {4, {0x61, 0x36, 0x63, 0x64}},
      {4, {0x61, 0x37, 0x63, 0x64}},
      {4, {0x61, 0x38, 0x63, 0x64}},
      {4, {0x61, 0x39, 0x63, 0x64}},
      {4, {0x61, 0x3A, 0x63, 0x64}},
      {4, {0x61, 0x3B, 0x63, 0x64}},
      {4, {0x61, 0x3C, 0x63, 0x64}},
      {4, {0x61, 0x3D, 0x63, 0x64}},
      {4, {0x61, 0x3E, 0x63, 0x64}},
      {4, {0x61, 0x3F, 0x63, 0x64}},
      {4, {0x61, 0x40, 0x63, 0x64}},
      {4, {0x61, 0x41, 0x63, 0x64}},
      {4, {0x61, 0x42, 0x63, 0x64}},
      {4, {0x61, 0x43, 0x63, 0x64}},
      {4, {0x61, 0x44, 0x63, 0x64}},
      {4, {0x61, 0x45, 0x63, 0x64}},
      {4, {0x61, 0x46, 0x63, 0x64}},
      {4, {0x61, 0x47, 0x63, 0x64}},
      {4, {0x61, 0x48, 0x63, 0x64}},
      {4, {0x61, 0x49, 0x63, 0x64}},
      {4, {0x61, 0x4A, 0x63, 0x64}},
      {4, {0x61, 0x4B, 0x63, 0x64}},
      {4, {0x61, 0x4C, 0x63, 0x64}},
      {4, {0x61, 0x4D, 0x63, 0x64}},
      {4, {0x61, 0x4E, 0x63, 0x64}},
      {4, {0x61, 0x4F, 0x63, 0x64}},
      {4, {0x61, 0x50, 0x63, 0x64}},
      {4, {0x61, 0x51, 0x63, 0x64}},
      {4, {0x61, 0x52, 0x63, 0x64}},
      {4, {0x61, 0x53, 0x63, 0x64}},
      {4, {0x61, 0x54, 0x63, 0x64}},
      {4, {0x61, 0x55, 0x63, 0x64}},
      {4, {0x61, 0x56, 0x63, 0x64}},
      {4, {0x61, 0x57, 0x63, 0x64}},
      {4, {0x61, 0x58, 0x63, 0x64}},
      {4, {0x61, 0x59, 0x63, 0x64}},
      {4, {0x61, 0x5A, 0x63, 0x64}},
      {4, {0x61, 0x5B, 0x63, 0x64}},
      {4, {0x61, 0x5C, 0x63, 0x64}},
      {4, {0x61, 0x5D, 0x63, 0x64}},
      {4, {0x61, 0x5E, 0x63, 0x64}},
      {4, {0x61, 0x5F, 0x63, 0x64}},
      {4, {0x61, 0x60, 0x63, 0x64}},
      {4, {0x61, 0x61, 0x63, 0x64}},
      {4, {0x61, 0x62, 0x63, 0x64}},
      {4, {0x61, 0x63, 0x63, 0x64}},
      {4, {0x61, 0x64, 0x63, 0x64}},
      {4, {0x61, 0x65, 0x63, 0x64}},
      {4, {0x61, 0x66, 0x63, 0x64}},
      {4, {0x61, 0x67, 0x63, 0x64}},
      {4, {0x61, 0x68, 0x63, 0x64}},
      {4, {0x61, 0x69, 0x63, 0x64}},
      {4, {0x61, 0x6A, 0x63, 0x64}},
      {4, {0x61, 0x6B, 0x63, 0x64}},
      {4, {0x61, 0x6C, 0x63, 0x64}},
      {4, {0x61, 0x6D, 0x63, 0x64}},
      {4, {0x61, 0x6E, 0x63, 0x64}},
      {4, {0x61, 0x6F, 0x63, 0x64}},
      {4, {0x61, 0x70, 0x63, 0x64}},
      {4, {0x61, 0x71, 0x63, 0x64}},
      {4, {0x61, 0x72, 0x63, 0x64}},
      {4, {0x61, 0x73, 0x63, 0x64}},
      {4, {0x61, 0x74, 0x63, 0x64}},
      {4, {0x61, 0x75, 0x63, 0x64}},
      {4, {0x61, 0x76, 0x63, 0x64}},
      {4, {0x61, 0x77, 0x63, 0x64}},
      {4, {0x61, 0x78, 0x63, 0x64}},
      {4, {0x61, 0x79, 0x63, 0x64}},
      {4, {0x61, 0x7A, 0x63, 0x64}},
      {4, {0x61, 0x7B, 0x63, 0x64}},
      {4, {0x61, 0x7C, 0x63, 0x64}},
      {4, {0x61, 0x7D, 0x63, 0x64}},
      {4, {0x61, 0x7E, 0x63, 0x64}},
      {4, {0x61, 0x7F, 0x63, 0x64}},
      {4, {0x61, 0x80, 0x63, 0x64}},
      {4, {0x61, 0x81, 0x63, 0x64}},
      {4, {0x61, 0x82, 0x63, 0x64}},
      {4, {0x61, 0x83, 0x63, 0x64}},
      {4, {0x61, 0x84, 0x63, 0x64}},
      {4, {0x61, 0x85, 0x63, 0x64}},
      {4, {0x61, 0x86, 0x63, 0x64}},
      {4, {0x61, 0x87, 0x63, 0x64}},
      {4, {0x61, 0x88, 0x63, 0x64}},
      {4, {0x61, 0x89, 0x63, 0x64}},
      {4, {0x61, 0x8A, 0x63, 0x64}},
      {4, {0x61, 0x8B, 0x63, 0x64}},
      {4, {0x61, 0x8C, 0x63, 0x64}},
      {4, {0x61, 0x8D, 0x63, 0x64}},
      {4, {0x61, 0x8E, 0x63, 0x64}},
      {4, {0x61, 0x8F, 0x63, 0x64}},
      {4, {0x61, 0x90, 0x63, 0x64}},
      {4, {0x61, 0x91, 0x63, 0x64}},
      {4, {0x61, 0x92, 0x63, 0x64}},
      {4, {0x61, 0x93, 0x63, 0x64}},
      {4, {0x61, 0x94, 0x63, 0x64}},
      {4, {0x61, 0x95, 0x63, 0x64}},
      {4, {0x61, 0x96, 0x63, 0x64}},
      {4, {0x61, 0x97, 0x63, 0x64}},
      {4, {0x61, 0x98, 0x63, 0x64}},
      {4, {0x61, 0x99, 0x63, 0x64}},
      {4, {0x61, 0x9A, 0x63, 0x64}},
      {4, {0x61, 0x9B, 0x63, 0x64}},
      {4, {0x61, 0x9C, 0x63, 0x64}},
      {4, {0x61, 0x9D, 0x63, 0x64}},
      {4, {0x61, 0x9E, 0x63, 0x64}},
      {4, {0x61, 0x9F, 0x63, 0x64}},
      {4, {0x61, 0xA0, 0x63, 0x64}},
      {4, {0x61, 0xA1, 0x63, 0x64}},
      {4, {0x61, 0xA2, 0x63, 0x64}},
      {4, {0x61, 0xA3, 0x63, 0x64}},
      {4, {0x61, 0xA4, 0x63, 0x64}},
      {4, {0x61, 0xA5, 0x63, 0x64}},
      {4, {0x61, 0xA6, 0x63, 0x64}},
      {4, {0x61, 0xA7, 0x63, 0x64}},
      {4, {0x61, 0xA8, 0x63, 0x64}},
      {4, {0x61, 0xA9, 0x63, 0x64}},
      {4, {0x61, 0xAA, 0x63, 0x64}},
      {4, {0x61, 0xAB, 0x63, 0x64}},
      {4, {0x61, 0xAC, 0x63, 0x64}},
      {4, {0x61, 0xAD, 0x63, 0x64}},
      {4, {0x61, 0xAE, 0x63, 0x64}},
      {4, {0x61, 0xAF, 0x63, 0x64}},
      {4, {0x61, 0xB0, 0x63, 0x64}},
      {4, {0x61, 0xB1, 0x63, 0x64}},
      {4, {0x61, 0xB2, 0x63, 0x64}},
      {4, {0x61, 0xB3, 0x63, 0x64}},
      {4, {0x61, 0xB4, 0x63, 0x64}},
      {4, {0x61, 0xB5, 0x63, 0x64}},
      {4, {0x61, 0xB6, 0x63, 0x64}},
      {4, {0x61, 0xB7, 0x63, 0x64}},
      {4, {0x61, 0xB8, 0x63, 0x64}},
      {4, {0x61, 0xB9, 0x63, 0x64}},
      {4, {0x61, 0xBA, 0x63, 0x64}},
      {4, {0x61, 0xBB, 0x63, 0x64}},
      {4, {0x61, 0xBC, 0x63, 0x64}},
      {4, {0x61, 0xBD, 0x63, 0x64}},
      {4, {0x61, 0xBE, 0x63, 0x64}},
      {4, {0x61, 0xBF, 0x63, 0x64}},
      {4, {0x61, 0xC0, 0x63, 0x64}},
      {4, {0x61, 0xC1, 0x63, 0x64}},
      {4, {0x61, 0xC2, 0x63, 0x64}},
      {4, {0x61, 0xC3, 0x63, 0x64}},
      {4, {0x61, 0xC4, 0x63, 0x64}},
      {4, {0x61, 0xC5, 0x63, 0x64}},
      {4, {0x61, 0xC6, 0x63, 0x64}},
      {4, {0x61, 0xC7, 0x63, 0x64}},
      {4, {0x61, 0xC8, 0x63, 0x64}},
      {4, {0x61, 0xC9, 0x63, 0x64}},
      {4, {0x61, 0xCA, 0x63, 0x64}},
      {4, {0x61, 0xCB, 0x63, 0x64}},
      {4, {0x61, 0xCC, 0x63, 0x64}},
      {4, {0x61, 0xCD, 0x63, 0x64}},
      {4, {0x61, 0xCE, 0x63, 0x64}},
      {4, {0x61, 0xCF, 0x63, 0x64}},
      {4, {0x61, 0xD0, 0x63, 0x64}},
      {4, {0x61, 0xD1, 0x63, 0x64}},
      {4, {0x61, 0xD2, 0x63, 0x64}},
      {4, {0x61, 0xD3, 0x63, 0x64}},
      {4, {0x61, 0xD4, 0x63, 0x64}},
      {4, {0x61, 0xD5, 0x63, 0x64}},
      {4, {0x61, 0xD6, 0x63, 0x64}},
      {4, {0x61, 0xD7, 0x63, 0x64}},
      {4, {0x61, 0xD8, 0x63, 0x64}},
      {4, {0x61, 0xD9, 0x63, 0x64}},
      {4, {0x61, 0xDA, 0x63, 0x64}},
      {4, {0x61, 0xDB, 0x63, 0x64}},
      {4, {0x61, 0xDC, 0x63, 0x64}},
      {4, {0x61, 0xDD, 0x63, 0x64}},
      {4, {0x61, 0xDE, 0x63, 0x64}},
      {4, {0x61, 0xDF, 0x63, 0x64}},
      {4, {0x61, 0xE0, 0x63, 0x64}},
      {4, {0x61, 0xE1, 0x63, 0x64}},
      {4, {0x61, 0xE2, 0x63, 0x64}},
      {4, {0x61, 0xE3, 0x63, 0x64}},
      {4, {0x61, 0xE4, 0x63, 0x64}},
      {4, {0x61, 0xE5, 0x63, 0x64}},
      {4, {0x61, 0xE6, 0x63, 0x64}},
      {4, {0x61, 0xE7, 0x63, 0x64}},
      {4, {0x61, 0xE8, 0x63, 0x64}},
      {4, {0x61, 0xE9, 0x63, 0x64}},
      {4, {0x61, 0xEA, 0x63, 0x64}},
      {4, {0x61, 0xEB, 0x63, 0x64}},
      {4, {0x61, 0xEC, 0x63, 0x64}},
      {4, {0x61, 0xED, 0x63, 0x64}},
      {4, {0x61, 0xEE, 0x63, 0x64}},
      {4, {0x61, 0xEF, 0x63, 0x64}},
      {4, {0x61, 0xF0, 0x63, 0x64}},
      {4, {0x61, 0xF1, 0x63, 0x64}},
      {4, {0x61, 0xF2, 0x63, 0x64}},
      {4, {0x61, 0xF3, 0x63, 0x64}},
      {4, {0x61, 0xF4, 0x63, 0x64}},
      {4, {0x61, 0xF5, 0x63, 0x64}},
      {4, {0x61, 0xF6, 0x63, 0x64}},
      {4, {0x61, 0xF7, 0x63, 0x64}},
      {4, {0x61, 0xF8, 0x63, 0x64}},
      {4, {0x61, 0xF9, 0x63, 0x64}},
      {4, {0x61, 0xFA, 0x63, 0x64}},
      {4, {0x61, 0xFB, 0x63, 0x64}},
      {4, {0x61, 0xFC, 0x63, 0x64}},
      {4, {0x61, 0xFD, 0x63, 0x64}},
      {4, {0x61, 0xFE, 0x63, 0x64}},
      {4, {0x61, 0xFF, 0x63, 0x64}},
    };

    struct atom atoms_61_62_0X_64[] = {
      {4, {0x61, 0x62, 0x00, 0x64}},
      {4, {0x61, 0x62, 0x01, 0x64}},
      {4, {0x61, 0x62, 0x02, 0x64}},
      {4, {0x61, 0x62, 0x03, 0x64}},
      {4, {0x61, 0x62, 0x04, 0x64}},
      {4, {0x61, 0x62, 0x05, 0x64}},
      {4, {0x61, 0x62, 0x06, 0x64}},
      {4, {0x61, 0x62, 0x07, 0x64}},
      {4, {0x61, 0x62, 0x08, 0x64}},
      {4, {0x61, 0x62, 0x09, 0x64}},
      {4, {0x61, 0x62, 0x0A, 0x64}},
      {4, {0x61, 0x62, 0x0B, 0x64}},
      {4, {0x61, 0x62, 0x0C, 0x64}},
      {4, {0x61, 0x62, 0x0D, 0x64}},
      {4, {0x61, 0x62, 0x0E, 0x64}},
      {4, {0x61, 0x62, 0x0F, 0x64}},
    };

    struct atom atoms_61_6X_63[] = {
      {3, {0x61, 0x60, 0x63}},
      {3, {0x61, 0x61, 0x63}},
      {3, {0x61, 0x62, 0x63}},
      {3, {0x61, 0x63, 0x63}},
      {3, {0x61, 0x64, 0x63}},
      {3, {0x61, 0x65, 0x63}},
      {3, {0x61, 0x66, 0x63}},
      {3, {0x61, 0x67, 0x63}},
      {3, {0x61, 0x68, 0x63}},
      {3, {0x61, 0x69, 0x63}},
      {3, {0x61, 0x6A, 0x63}},
      {3, {0x61, 0x6B, 0x63}},
      {3, {0x61, 0x6C, 0x63}},
      {3, {0x61, 0x6D, 0x63}},
      {3, {0x61, 0x6E, 0x63}},
      {3, {0x61, 0x6F, 0x63}},
    };

    assert_re_atoms("abcd", 1, (struct atom[]) {
      {4, {0x61, 0x62, 0x63, 0x64}},
    });

    assert_re_atoms("abcd1234", 1, (struct atom[]) {
      {4, {0x31, 0x32, 0x33, 0x34}},
    });

    assert_re_atoms("a..d", 1, (struct atom[]) {
      {1, {0x61}},
    });

    assert_re_atoms("a..de", 1, (struct atom[]) {
      {2, {0x64, 0x65}},
    });

    assert_re_atoms("abcd.efgh", 1, (struct atom[]) {
      {4, {0x61, 0x62, 0x63, 0x64}},
    });

    assert_re_atoms("(abcd|efgh)", 2, (struct atom[]) {
      {4, {0x65, 0x66, 0x67, 0x68}},
      {4, {0x61, 0x62, 0x63, 0x64}},
    });

    assert_re_atoms("(abcd|efgh|ij)", 3, (struct atom[]) {
      {2, {0x69, 0x6A}},
      {4, {0x65, 0x66, 0x67, 0x68}},
      {4, {0x61, 0x62, 0x63, 0x64}},
    });

    assert_re_atoms("a.cd", 256, atoms_61_XX_63_64);

    assert_hex_atoms("{61 62 63 64}", 1, (struct atom[]) {
      {4, {0x61, 0x62, 0x63, 0x64}},
    });

    assert_hex_atoms("{61 62 63 64 [1-5] 65 66 67 68}", 1, (struct atom[]) {
      {4, {0x61, 0x62, 0x63, 0x64}},
    });

    assert_hex_atoms("{61 62 63 [1-5] 65 66 67 68}", 1, (struct atom[]) {
      {4, {0x65, 0x66, 0x67, 0x68}},
    });

    assert_hex_atoms("{61 62 63 [1-5] 65 66 }", 1, (struct atom[]) {
      {3, {0x61, 0x62, 0x63}},
    });

    assert_hex_atoms("{61 6? 63 [1-5] 65 66 }", 16, atoms_61_6X_63);

    assert_hex_atoms("{(61 62 63 | 65 66 67 68)}", 2, (struct atom[]) {
      {4, {0x65, 0x66, 0x67, 0x68}},
      {3, {0x61, 0x62, 0x63}},
    });

    assert_hex_atoms("{61 62 0? 64}", 16, atoms_61_62_0X_64);

    assert_hex_atoms("{11 ?? 11 ?? 22 33 44 55 66 }", 1, (struct atom[]) {
      {4, {0x22, 0x33, 0x44, 0x55}},
    });

    // Test case for issue #1025
    assert_hex_atoms("{?? 11 22 33 ?? 55 66 }", 1, (struct atom[]) {
      {3, {0x11, 0x22, 0x33}},
    });
}


int main(int argc, char** argv)
{
  yr_initialize();

  test_table_quality();
  test_heuristic_quality();
  test_atom_choose();

  yr_finalize();
  return 0;
}
