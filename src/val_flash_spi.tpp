#ifndef VAL_FLASH_SPI_TPP
#define VAL_FLASH_SPI_TPP

template<typename T>
void ValFlashSPI::writeStruct(uint32_t addr, const T& data) {
  const uint8_t* ptr = (const uint8_t*)&data;
  for (size_t i = 0; i < sizeof(T); ++i) {
    writeByte(addr + i, ptr[i]);
  }
}

template<typename T>
void ValFlashSPI::readStruct(uint32_t addr, T& data) {
  uint8_t* ptr = (uint8_t*)&data;
  for (size_t i = 0; i < sizeof(T); ++i) {
    ptr[i] = readByte(addr + i);
  }
}

#endif
