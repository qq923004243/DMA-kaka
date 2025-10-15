# DMA-kaka
 pubg dma source

  

Bad news: The decryption changes with each game update, so you'll need to update the Cindex yourself. I don't know the specific method yet—waiting for someone smarter. Here's what someone previously leaked：

```c++
DWORD Decrypt::CIndex(DWORD value)
{
  /*uint32_t eax = value;
  eax <<= 16;
  eax ^= 0x49469D07;
  eax ^= value;
  return eax;*/
  uint32_t v7 = value ^ 0x6BAF7D4F;
  return ((v7 << 25) | (v7 >> 7) & 0x1FF0000) ^ _rotr(v7, 23) ^ 0xCFA04625;
}


```

