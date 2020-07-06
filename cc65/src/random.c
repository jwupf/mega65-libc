/* 
   Simple random number generator.

   All MEGA65 models have a thermal noise register from the FPGA that
   we can use to get some entropy from.

   We can then use the hardware multiplier to get random values within
   arbitrary range without needing to resort to a modulus operator.

*/

#include <stdint.h>
#include <memory.h>

uint32_t random_temp;
uint8_t random_step;
uint8_t random_byte;

void generate_random_byte(void)
{
  random_byte=0;
  random_step=31;

  while(random_step)
    {
      random_byte=(random_byte<<1)|(random_byte>>7)|(PEEK(0xD6DE)&0x01);
    }
}

uint8_t *zero=0;

uint32_t random32(uint32_t range)
{
#if 1
  generate_random_byte();
  POKE(0xD770,random_byte);
  generate_random_byte();
  POKE(0xD771,random_byte);
  generate_random_byte();
  POKE(0xD772,random_byte);
  generate_random_byte();
  POKE(0xD773,random_byte);

  if (!range) return *(uint32_t *)0xD770;

  *(uint32_t *)0xD774 = range;

  return *(uint32_t *)0xD77C;

}

uint16_t random16(uint16_t range)
{
  generate_random_byte();
  POKE(0xD770,random_byte);
  generate_random_byte();
  POKE(0xD771,random_byte);

  POKE(0xD772,0);
  POKE(0xD773,0);
  POKE(0xD776,0);
  POKE(0xD777,0);
  
  if (!range) return *(uint16_t *)0xD770;

  *(uint16_t *)0xD774 = range;

  return *(uint16_t *)0xD77A;  
}

uint8_t random8(uint8_t range)
{
  // We don't really trust that the low enough bit has enough entropy
  // on a single reading, so we calculate the XOR of several samples
  generate_random_byte();

  if (range) {
    POKE(0xD770,random_byte);
    POKE(0xD771,0);
    POKE(0xD774,range);
    POKE(0xD775,0);
    return PEEK(0xD779);
  } else return random_byte;
}