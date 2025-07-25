#include <val_flash_spi.h>

#define FLASH_CS PA4
#define FLASH_ADDR 0x001000

// Define com velocidade customizada (ex: 2MHz)
ValFlashSPI flash(FLASH_CS, 2000000); 

struct Registro {
  uint16_t codigo;
  float tensao;
  char descricao[20];
};

void setup() {
  Serial.begin(115200);
  while (!Serial);

  flash.begin();

  // Apaga o chip todo (cuidado!)
  flash.eraseChip();

  Registro reg = { 77, 3.33, "Completo" };
  flash.writeStruct(FLASH_ADDR, reg);

  Registro regLido;
  flash.readStruct(FLASH_ADDR, regLido);

  Serial.println("Registro lido:");
  Serial.print("Código: "); Serial.println(regLido.codigo);
  Serial.print("Tensão: "); Serial.println(regLido.tensao);
  Serial.print("Texto: "); Serial.println(regLido.descricao);
}

void loop() {}
