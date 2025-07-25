#ifndef VAL_FLASH_SPI_TPP
#define VAL_FLASH_SPI_TPP

template<typename T>
bool ValFlashSPI::writeStruct(uint32_t addr, const T& data) {
  const uint8_t* ptr = (const uint8_t*)&data;
  for (size_t i = 0; i < sizeof(T); ++i) {
    if (!writeByte(addr + i, ptr[i])) {
      return false;
    }
  }
  return true;
}

template<typename T>
void ValFlashSPI::readStruct(uint32_t addr, T& data) {
  uint8_t* ptr = (uint8_t*)&data;
  for (size_t i = 0; i < sizeof(T); ++i) {
    ptr[i] = readByte(addr + i);
  }
}

inline bool ValFlashSPI::isErased(const void* data, size_t size) {
  const uint8_t* ptr = (const uint8_t*)data;
  for (size_t i = 0; i < size; ++i) {
    if (ptr[i] != 0xFF) return false;
  }
  return true;
}

template<typename T>
bool ValFlashSPI::writeStructOffset(uint32_t baseAddr, size_t index, const T& data) {
  uint32_t addr = baseAddr + index * sizeof(T);
  return writeStruct(addr, data);
}

template<typename T>
void ValFlashSPI::readStructOffset(uint32_t baseAddr, size_t index, T& data) {
  uint32_t addr = baseAddr + index * sizeof(T);
  readStruct(addr, data);
}

template<typename T1, typename T2, typename T3, typename T4>
bool ValFlashSPI::writeAll(uint32_t base, const T1& a, const T2& b, const T3& c, const T4& d) {
  uint32_t addr = base;
  if (!writeStruct(addr, a)) return false;
  addr += sizeof(T1);
  if (!writeStruct(addr, b)) return false;
  addr += sizeof(T2);
  if (!writeStruct(addr, c)) return false;
  addr += sizeof(T3);
  if (!writeStruct(addr, d)) return false;
  return true;
}

#endif
