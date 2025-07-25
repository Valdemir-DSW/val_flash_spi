#ifndef VAL_FLASH_SPI_H
#define VAL_FLASH_SPI_H

#include <Arduino.h>
#include <SPI.h>

class ValFlashSPI {
public:
  // csPin: pino de chip select
  // spiSpeed: velocidade SPI (opcional)
  // spi: ponteiro para SPI customizada (opcional)
  ValFlashSPI(uint8_t csPin, uint32_t spiSpeed = 4000000, SPIClass* spi = &SPI);

  void begin();
  void eraseSector(uint32_t addr);
  void eraseChip();
  void writeByte(uint32_t addr, uint8_t data);
  uint8_t readByte(uint32_t addr);

  template<typename T>
  void writeStruct(uint32_t addr, const T& data);

  template<typename T>
  void readStruct(uint32_t addr, T& data);

private:
  uint8_t _cs;
  uint32_t _spiSpeed;
  SPIClass* _spi;  // ponteiro para SPI customizada

  void writeEnable();
  void waitForWrite();
};

#include "val_flash_spi.tpp"

#endif
