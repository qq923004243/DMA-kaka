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

this is 2025.10.16

```c++
DWORD Decrypt::CIndex(DWORD encrypted_id)
{
    // 1. 从您的偏移量数据中获取所有需要的密钥和配置
    const DWORD key1 = GameData.Offset["DecryptNameIndexXorKey1"];
    const DWORD key2 = GameData.Offset["DecryptNameIndexXorKey2"];
    const DWORD rval = GameData.Offset["DecryptNameIndexRval"]; // 旋转位数
    const DWORD sval = GameData.Offset["DecryptNameIndexSval"]; // 左移位数
    const bool  use_ror = (GameData.Offset["DecryptNameIndexRor"] != 0); // 旋转方向标志
    // 2. 初始XOR，这步和之前类似
    DWORD xor_result = encrypted_id ^ key1;

    // 3. 新增的条件旋转 (Conditional Rotation)
    DWORD rotated_part;
    if (use_ror) {
        // 如果标志为真，执行右旋转
        rotated_part = _rotr(xor_result, rval);
    }
    else {
        // 否则，执行左旋转
        rotated_part = _rotl(xor_result, rval);
    }
    // 4. 新增的复杂位运算部分
    DWORD left_shifted_part = xor_result << sval;
    DWORD right_shifted_masked_part = (xor_result >> 7) & 0x1FF0000; // 使用硬编码的 7 和掩码
    DWORD complex_part = left_shifted_part | right_shifted_masked_part;

    // 5. 最终组合
    return rotated_part ^ complex_part ^ key2;
}
```

