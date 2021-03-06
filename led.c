#include "led.h"
#include <stdlib.h>

/* encrypt */
void encrypt(STATE* s, KEY* k)
{
  int r;
  
  char kf = 0; /* key flag: use k_1 or k_2 */
  
  for (r = 0; r < RN; r++)
  {
    if ((r % 4) == 0)
      addKey(s, k, kf), kf ^= 1;
    
    addConstants(s, r);
    subCells(s);
    shiftRows(s);
    mixColumnsSerial(s);
  }
  addKey(s, k, 0);
}

/* decrypt */
void decrypt(STATE* s, KEY* k)
{
  int r;
  char kf = (RN / 4) % 2; /* key flag: use k_1 or k_2 */
  
  addKey(s, k, 0);
  for (r = RN - 1; r >= 0; r--)
  {
    invMixColumnsSerial(s);
    invShiftRows(s);
    invSubCells(s);
    addConstants(s, r);
    
    if ((r % 4) == 0)
      addKey(s, k, kf), kf ^= 1;
  }
}

/* addKey */
void addKey(STATE* s, KEY* k, char flag)
{
  if (flag == 1 && KEY_SIZE == 128)
    s->b[0] ^= k->w[2], s->b[1] ^= k->w[3];
  else
    s->b[0] ^= k->w[0], s->b[1] ^= k->w[1];
}

/* addConstants */
void addConstants(STATE* s, int r)
{
  s->b[0] ^= (((RCONST[r] >> 3) & 0x7) << 24) ^ (0x1 << 12) ^ (((RCONST[r] >> 0) & 0x7) << 8);
  s->b[1] ^= (0x2 << 28) ^ (((RCONST[r] >> 3) & 0x7) << 24) ^ (0x3 << 12) ^ (((RCONST[r] >> 0) & 0x7) << 8);
}

/* subCells */
void subCells(STATE* s)
{
  int i;
  uint32_t x[2] = {0, 0};
  /* apply SBox */
  for (i = 0; i < 8; i++)
  {
    x[0] ^= SBox[(s->b[0] >> (28 - 4 * i)) & 0xf] << (28 - 4 * i);
    x[1] ^= SBox[(s->b[1] >> (28 - 4 * i)) & 0xf] << (28 - 4 * i);
  }
  s->b[0] = x[0];
  s->b[1] = x[1];
}

/* invSubCells */
void invSubCells(STATE* s)
{
  int i;
  uint32_t x[2] = {0, 0};
  /* apply InvSBox */
  for (i = 0; i < 8; i++)
  {
    x[0] ^= InvSBox[(s->b[0] >> (28 - 4 * i)) & 0xf] << (28 - 4 * i);
    x[1] ^= InvSBox[(s->b[1] >> (28 - 4 * i)) & 0xf] << (28 - 4 * i);
  }
  s->b[0] = x[0];
  s->b[1] = x[1];
}

/* shiftRows */
void shiftRows(STATE* s)
{
  s->b[0] = ((ROL16((s->b[0] >> 16) & 0xffff, 0) & 0xffff) << 16) ^ (ROL16((s->b[0] >> 0) & 0xffff, 4) & 0xffff);
  s->b[1] = ((ROL16((s->b[1] >> 16) & 0xffff, 8) & 0xffff) << 16) ^ (ROL16((s->b[1] >> 0) & 0xffff, 12) & 0xffff);
}

/* invShiftRows */
void invShiftRows(STATE* s)
{
  s->b[0] = ((ROR16((s->b[0] >> 16) & 0xffff, 0) & 0xffff) << 16) ^ (ROR16((s->b[0] >> 0) & 0xffff, 4) & 0xffff);
  s->b[1] = ((ROR16((s->b[1] >> 16) & 0xffff, 8) & 0xffff) << 16) ^ (ROR16((s->b[1] >> 0) & 0xffff, 12) & 0xffff);
}

/* mixColumnsSerial */
void mixColumnsSerial(STATE* s)
{
  int i;
  uint32_t x[2] = {0, 0};
  uint8_t t[4] = {0, 0, 0, 0};
  
  /* iterate over columns */
  for (i = 0; i < 4; i++)
  {
    /* extract column */
    t[0] = (uint8_t) ((s->b[0] >> (28 - 4 * i)) & 0xf);
    t[1] = (uint8_t) ((s->b[0] >> (12 - 4 * i)) & 0xf);
    t[2] = (uint8_t) ((s->b[1] >> (28 - 4 * i)) & 0xf);
    t[3] = (uint8_t) ((s->b[1] >> (12 - 4 * i)) & 0xf);
    
    /* multiply matrix and column */
    x[0] ^= (gm(t[0], MDS[0][0]) ^ gm(t[1], MDS[0][1]) ^ gm(t[2], MDS[0][2]) ^ gm(t[3], MDS[0][3])) << (28 - 4 * i);
    x[0] ^= (gm(t[0], MDS[1][0]) ^ gm(t[1], MDS[1][1]) ^ gm(t[2], MDS[1][2]) ^ gm(t[3], MDS[1][3])) << (12 - 4 * i);
    x[1] ^= (gm(t[0], MDS[2][0]) ^ gm(t[1], MDS[2][1]) ^ gm(t[2], MDS[2][2]) ^ gm(t[3], MDS[2][3])) << (28 - 4 * i);
    x[1] ^= (gm(t[0], MDS[3][0]) ^ gm(t[1], MDS[3][1]) ^ gm(t[2], MDS[3][2]) ^ gm(t[3], MDS[3][3])) << (12 - 4 * i);
  }
  s->b[0] = x[0];
  s->b[1] = x[1];
}

/* invMixColumnsSerial */
void invMixColumnsSerial(STATE* s)
{
  int i;
  uint32_t x[2] = {0, 0};
  uint8_t t[4] = {0, 0, 0, 0};
  
  /* iterate over columns */
  for (i = 0; i < 4; i++)
  {
    /* extract column */
    t[0] = (uint8_t) ((s->b[0] >> (28 - 4 * i)) & 0xf);
    t[1] = (uint8_t) ((s->b[0] >> (12 - 4 * i)) & 0xf);
    t[2] = (uint8_t) ((s->b[1] >> (28 - 4 * i)) & 0xf);
    t[3] = (uint8_t) ((s->b[1] >> (12 - 4 * i)) & 0xf);
    
    /* multiply matrix and column */
    x[0] ^= (gm(t[0], InvMDS[0][0]) ^ gm(t[1], InvMDS[0][1]) ^ gm(t[2], InvMDS[0][2]) ^ gm(t[3], InvMDS[0][3]))
    << (28 - 4 * i);
    x[0] ^= (gm(t[0], InvMDS[1][0]) ^ gm(t[1], InvMDS[1][1]) ^ gm(t[2], InvMDS[1][2]) ^ gm(t[3], InvMDS[1][3]))
    << (12 - 4 * i);
    x[1] ^= (gm(t[0], InvMDS[2][0]) ^ gm(t[1], InvMDS[2][1]) ^ gm(t[2], InvMDS[2][2]) ^ gm(t[3], InvMDS[2][3]))
    << (28 - 4 * i);
    x[1] ^= (gm(t[0], InvMDS[3][0]) ^ gm(t[1], InvMDS[3][1]) ^ gm(t[2], InvMDS[3][2]) ^ gm(t[3], InvMDS[3][3]))
    << (12 - 4 * i);
  }
  s->b[0] = x[0];
  s->b[1] = x[1];
}

/* galois multiplication in GF(16) */
uint8_t gm(uint8_t a, uint8_t b)
{
  uint8_t g = 0;
  int i;
  for (i = 0; i < DEG_GF_POLY; i++)
  {
    if ((b & 0x1) == 1) g ^= a;
    uint8_t hbs = (uint8_t) (a & 0x8);
    a <<= 0x1;
    if (hbs == 0x8) a ^= GF_POLY;
    b >>= 0x1;
  }
  return g;
}

int main(int argc, char* argv[])
{
  STATE s;
  KEY k;
  
  enum Mode m = NONE;
  
  int i = 0;
  
  while ((i = getopt(argc, argv, "k:x:ed")) >= 0)
  {
    switch (i)
    {
      case 'k':
        if (strlen(optarg) == 17 && KEY_SIZE == 64)
        {
          sscanf(strtok(optarg, " "), "%x", &k.w[0]);
          sscanf(strtok(NULL, " "), "%x", &k.w[1]);
        }
        else if (strlen(optarg) == 35 && KEY_SIZE == 128)
        {
          sscanf(strtok(optarg, " "), "%x", &k.w[0]);
          sscanf(strtok(NULL, " "), "%x", &k.w[1]);
          sscanf(strtok(NULL, " "), "%x", &k.w[2]);
          sscanf(strtok(NULL, " "), "%x", &k.w[3]);
        }
        else
        {
          printf("Error! Wrong key length.\n");
          return EXIT_FAILURE;
        }
        break;
      case 'x':
        if (strlen(optarg) == 17)
        {
          sscanf(strtok(optarg, " "), "%x", &s.b[0]);
          sscanf(strtok(NULL, " "), "%x", &s.b[1]);
        }
        else
        {
          printf("Error! Wrong size of plaintext block.\n");
          return EXIT_FAILURE;
        }
        break;
      case 'e':
        if(m == NONE)
          m = ENCRYPT;
        else
        {
          printf("Error! More than one mode selected.\n");
          return EXIT_FAILURE;
        }
        break;
      case 'd':
        if(m == NONE)
          m = DECRYPT;
        else
        {
          printf("Error! More than one mode selected.\n");
          return EXIT_FAILURE;
        }
        break;
      default:
        return EXIT_FAILURE;
    }
  }
  
  if (m == ENCRYPT)
    encrypt(&s, &k);
  else if (m == DECRYPT)
    decrypt(&s, &k);
  else
  {
    printf("Error! Use either encryption (-e) or decryption (-d)");
    return EXIT_FAILURE;
  }
  
  printf("%08x %08x\n", s.b[0], s.b[1]);
  return EXIT_SUCCESS;
}
