#include "val_flash_spi.h"

constexpr uint8_t CMD_WRITE_ENABLE = 0x06;
constexpr uint8_t CMD_READ_STATUS = 0x05;
constexpr uint8_t CMD_SECTOR_ERASE = 0x20;
constexpr uint8_t CMD_CHIP_ERASE = 0xC7;
constexpr uint8_t CMD_PAGE_PROGRAM = 0x02;
constexpr uint8_t CMD_READ_DATA = 0x03;
constexpr uint8_t CMD_READ_JEDEC_ID = 0x9F;  // Detecta tamanho

constexpr uint32_t WAIT_WRITE_TIMEOUT_MS = 5000;
constexpr uint32_t SECTOR_SIZE_DEFAULT = 4096;

ValFlashSPI::ValFlashSPI(uint8_t csPin, uint32_t spiSpeed, SPIClass* spi)
  : _cs(csPin), _spiSpeed(spiSpeed), _spi(spi), _cachedCapacity(0) {}

void ValFlashSPI::begin() {
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);
  _spi->begin();
  // Detectar capacidade na inicialização
  _cachedCapacity = _detectCapacity();
  if (_cachedCapacity == 0) {
    Serial.println(F("Aviso: Capacidade da Flash não detectada. Usando valor padrão."));
    _cachedCapacity = 2097152;  // 2MB padrão
  }
}

// Detecta capacidade lendo JEDEC ID
uint32_t ValFlashSPI::_detectCapacity() {
  _spi->beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE0));
  digitalWrite(_cs, LOW);
  _spi->transfer(CMD_READ_JEDEC_ID);
  
  uint8_t manufacturer = _spi->transfer(0x00);  // Byte 0: Manufacturer
  uint8_t memType = _spi->transfer(0x00);       // Byte 1: Memory Type
  uint8_t capacity = _spi->transfer(0x00);      // Byte 2: Capacity
  
  digitalWrite(_cs, HIGH);
  _spi->endTransaction();
  
  // W25Q series: manufacturer=0xEF, memType=0x40/0x60/0x70
  if (manufacturer != 0xEF) {
    return 0;  // Flash desconhecida
  }
  
  // Decodificar capacidade pelo terceiro byte
  // Padrão JEDEC: capacity = log2(bytes) - 20
  // Ex: 0x15 = 21 - 20 = 1 => 2^(20+1) = 2MB
  //     0x16 = 22 - 20 = 2 => 2^(20+2) = 4MB
  //     0x17 = 23 - 20 = 3 => 2^(20+3) = 8MB
  //     0x18 = 24 - 20 = 4 => 2^(20+4) = 16MB
  
  if (capacity >= 0x15 && capacity <= 0x20) {
    uint32_t megabytes = (1 << (capacity - 0x15));  // 1, 2, 4, 8, 16 MB...
    return megabytes * 1024 * 1024;  // Converter para bytes
  }
  
  return 0;
}

uint32_t ValFlashSPI::getFlashCapacity() {
  return _cachedCapacity;
}

uint32_t ValFlashSPI::getMaxAddress() {
  return _cachedCapacity - 1;
}

uint32_t ValFlashSPI::getSectorSize() {
  return SECTOR_SIZE_DEFAULT;
}

// Distribui slots de backup pelo espaço disponível
// Exemplo: 4 slots em 2MB Flash => cada slot fica em ~512KB
// Os slots são alinhados ao tamanho de setor para máxima segurança
uint32_t ValFlashSPI::calculateBackupSlotAddr(uint8_t slotIndex, uint8_t numSlots) {
  if (numSlots == 0 || slotIndex >= numSlots) return 0;
  // totalCapacity: capacidade total detectada da flash
  uint32_t totalCapacity = getFlashCapacity();
  // slotSize: cada slot recebe uma fração igual do espaço
  uint32_t slotSize = totalCapacity / numSlots;
  // Alinhar ao setor para evitar corrupção
  slotSize = (slotSize / SECTOR_SIZE_DEFAULT) * SECTOR_SIZE_DEFAULT;
  // Retorna o endereço inicial do slot
  return slotIndex * slotSize;
}

// Retorna o tamanho disponível para cada slot de backup
// Exemplo: getBackupSlotSize(4) retorna o tamanho de cada slot se dividir a flash em 4 partes
uint32_t ValFlashSPI::getBackupSlotSize(uint8_t numSlots) {
  if (numSlots == 0) return 0;
  uint32_t totalCapacity = getFlashCapacity();
  uint32_t slotSize = totalCapacity / numSlots;
  // Alinhar ao setor para máxima segurança
  slotSize = (slotSize / SECTOR_SIZE_DEFAULT) * SECTOR_SIZE_DEFAULT;
  return slotSize;
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
