#include "val_flash_spi.h"

constexpr uint8_t CMD_WRITE_ENABLE = 0x06;
constexpr uint8_t CMD_READ_STATUS = 0x05;
constexpr uint8_t CMD_SECTOR_ERASE = 0x20;
constexpr uint8_t CMD_CHIP_ERASE = 0xC7;
constexpr uint8_t CMD_PAGE_PROGRAM = 0x02;
constexpr uint8_t CMD_READ_DATA = 0x03;

constexpr uint32_t WAIT_WRITE_TIMEOUT_MS = 5000;

ValFlashSPI::ValFlashSPI(uint8_t csPin, uint32_t spiSpeed, SPIClass* spi)
  : _cs(csPin), _spiSpeed(spiSpeed), _spi(spi) {}

void ValFlashSPI::begin() {
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);
  _spi->begin();
}

void ValFlashSPI::writeEnable() {
  _spi->beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE0));
  digitalWrite(_cs, LOW);
  _spi->transfer(CMD_WRITE_ENABLE);
  digitalWrite(_cs, HIGH);
  _spi->endTransaction();
}

bool ValFlashSPI::waitForWrite() {
  _spi->beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE0));
  unsigned long start = millis();

  while (true) {
    digitalWrite(_cs, LOW);
    _spi->transfer(CMD_READ_STATUS);
    uint8_t status = _spi->transfer(0x00);
    digitalWrite(_cs, HIGH);

    if ((status & 0x01) == 0) {
      _spi->endTransaction();
      return true;
    }
    if (millis() - start > WAIT_WRITE_TIMEOUT_MS) {
      _spi->endTransaction();
      return false;
    }
    delay(1);
  }
}

bool ValFlashSPI::eraseSector(uint32_t addr) {
  writeEnable();

  _spi->beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE0));
  digitalWrite(_cs, LOW);
  _spi->transfer(CMD_SECTOR_ERASE);
  _spi->transfer((addr >> 16) & 0xFF);
  _spi->transfer((addr >> 8) & 0xFF);
  _spi->transfer(addr & 0xFF);
  digitalWrite(_cs, HIGH);
  _spi->endTransaction();

  return waitForWrite();
}

bool ValFlashSPI::eraseChip() {
  writeEnable();

  _spi->beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE0));
  digitalWrite(_cs, LOW);
  _spi->transfer(CMD_CHIP_ERASE);
  digitalWrite(_cs, HIGH);
  _spi->endTransaction();

  return waitForWrite();
}

bool ValFlashSPI::writeByte(uint32_t addr, uint8_t data) {
  writeEnable();

  _spi->beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE0));
  digitalWrite(_cs, LOW);
  _spi->transfer(CMD_PAGE_PROGRAM);
  _spi->transfer((addr >> 16) & 0xFF);
  _spi->transfer((addr >> 8) & 0xFF);
  _spi->transfer(addr & 0xFF);
  _spi->transfer(data);
  digitalWrite(_cs, HIGH);
  _spi->endTransaction();

  return waitForWrite();
}

uint8_t ValFlashSPI::readByte(uint32_t addr) {
  _spi->beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE0));
  digitalWrite(_cs, LOW);
  _spi->transfer(CMD_READ_DATA);
  _spi->transfer((addr >> 16) & 0xFF);
  _spi->transfer((addr >> 8) & 0xFF);
  _spi->transfer(addr & 0xFF);
  uint8_t r = _spi->transfer(0x00);
  digitalWrite(_cs, HIGH);
  _spi->endTransaction();

  return r;
}
