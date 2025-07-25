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

private:
  uint8_t _cs;
  uint32_t _spiSpeed;
  SPIClass* _spi;

  void writeEnable();

  // Espera escrita terminar; retorna true se sucesso
  bool waitForWrite();
};

#include "val_flash_spi.tpp"

#endif
