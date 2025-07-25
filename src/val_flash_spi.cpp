#include "val_flash_spi.h"

ValFlashSPI::ValFlashSPI(uint8_t csPin, uint32_t spiSpeed, SPIClass* spi)
  : _cs(csPin), _spiSpeed(spiSpeed), _spi(spi) {}

void ValFlashSPI::begin() {
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);
  _spi->begin();
  _spi->beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE0));
}

void ValFlashSPI::writeEnable() {
  digitalWrite(_cs, LOW);
  _spi->transfer(0x06);
  digitalWrite(_cs, HIGH);
}

void ValFlashSPI::waitForWrite() {
  digitalWrite(_cs, LOW);
  _spi->transfer(0x05);
  while (_spi->transfer(0x00) & 0x01);
  digitalWrite(_cs, HIGH);
}

void ValFlashSPI::eraseSector(uint32_t addr) {
  writeEnable();
  digitalWrite(_cs, LOW);
  _spi->transfer(0x20);
  _spi->transfer((addr >> 16) & 0xFF);
  _spi->transfer((addr >> 8) & 0xFF);
  _spi->transfer(addr & 0xFF);
  digitalWrite(_cs, HIGH);
  waitForWrite();
}

void ValFlashSPI::eraseChip() {
  writeEnable();
  digitalWrite(_cs, LOW);
  _spi->transfer(0xC7);
  digitalWrite(_cs, HIGH);
  waitForWrite();
}

void ValFlashSPI::writeByte(uint32_t addr, uint8_t data) {
  writeEnable();
  digitalWrite(_cs, LOW);
  _spi->transfer(0x02);
  _spi->transfer((addr >> 16) & 0xFF);
  _spi->transfer((addr >> 8) & 0xFF);
  _spi->transfer(addr & 0xFF);
  _spi->transfer(data);
  digitalWrite(_cs, HIGH);
  waitForWrite();
}

uint8_t ValFlashSPI::readByte(uint32_t addr) {
  digitalWrite(_cs, LOW);
  _spi->transfer(0x03);
  _spi->transfer((addr >> 16) & 0xFF);
  _spi->transfer((addr >> 8) & 0xFF);
  _spi->transfer(addr & 0xFF);
  uint8_t r = _spi->transfer(0x00);
  digitalWrite(_cs, HIGH);
  return r;
}
