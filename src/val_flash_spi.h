#ifndef VAL_FLASH_SPI_H
#define VAL_FLASH_SPI_H

#include <Arduino.h>
#include <SPI.h>

class ValFlashSPI {
public:
  ValFlashSPI(uint8_t csPin, uint32_t spiSpeed = 4000000, SPIClass* spi = &SPI);

  void begin();

  // Apaga setor que contém o endereço; retorna true se sucesso
  bool eraseSector(uint32_t addr);

  // Apaga chip inteiro; retorna true se sucesso
  bool eraseChip();

  // Grava 1 byte na flash; retorna true se sucesso
  bool writeByte(uint32_t addr, uint8_t data);

  // Lê 1 byte da flash
  uint8_t readByte(uint32_t addr);

  // Grava estrutura genérica; retorna true se sucesso
  template<typename T>
  bool writeStruct(uint32_t addr, const T& data);

  // Lê estrutura genérica
  template<typename T>
  void readStruct(uint32_t addr, T& data);

  // === EXTENSÕES ===

  // Verifica se estrutura está apagada (0xFF)
  static bool isErased(const void* data, size_t size);

  // Grava estrutura com índice (offset automático); retorna true se sucesso
  template<typename T>
  bool writeStructOffset(uint32_t baseAddr, size_t index, const T& data);

  // Lê estrutura com índice (offset automático)
  template<typename T>
  void readStructOffset(uint32_t baseAddr, size_t index, T& data);

  // Grava 4 estruturas sequenciais; retorna true se sucesso
  template<typename T1, typename T2, typename T3, typename T4>
  bool writeAll(uint32_t base, const T1& a, const T2& b, const T3& c, const T4& d);

  // === DETECÇÃO DE CAPACIDADE ===
  
  // Detecta capacidade total da Flash SPI (retorna em bytes)
  // Detecta automaticamente: W25Q16 (2MB), W25Q32 (4MB), W25Q64 (8MB), W25Q128 (16MB), etc.
  uint32_t getFlashCapacity();
  
  // Retorna endereço máximo válido (capacidade - 1)
  uint32_t getMaxAddress();
  
  // Retorna tamanho de um setor em bytes (normalmente 4096)
  uint32_t getSectorSize();

  // === GERENCIAMENTO DE SLOTS DE BACKUP DINÂMICO ===
  
  // Calcula endereço de um slot de backup dinamicamente
  // Distribui automaticamente N slots pelo espaço disponível
  // slotIndex: 0 a (numSlots-1)
  // numSlots: quantos slots você quer criar (padrão recomendado: 4)
  // Exemplo: 4 slots em 2MB Flash => cada slot fica em ~512KB
  // Os slots são alinhados ao tamanho de setor para máxima segurança
  uint32_t calculateBackupSlotAddr(uint8_t slotIndex, uint8_t numSlots);

  // Calcula tamanho disponível para cada slot (espaço total / numSlots)
  // Exemplo: getBackupSlotSize(4) retorna o tamanho de cada slot se dividir a flash em 4 partes
  uint32_t getBackupSlotSize(uint8_t numSlots);

private:
  uint8_t _cs;
  uint32_t _spiSpeed;
  SPIClass* _spi;
  uint32_t _cachedCapacity;  // Cache da capacidade detectada

  void writeEnable();

  // Espera escrita terminar; retorna true se sucesso
  bool waitForWrite();

  // Detecta tamanho da Flash internamente (lê JEDEC ID)
  uint32_t _detectCapacity();
};

#include "val_flash_spi.tpp"

#endif
