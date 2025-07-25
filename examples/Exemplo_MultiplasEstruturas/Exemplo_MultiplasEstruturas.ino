#include <val_flash_spi.h>

#define FLASH_CS PB0
#define FLASH_ADDR 0x000000


ValFlashSPI flash(FLASH_CS);

struct DadosSimples {
  uint32_t id;
  float temperatura;
  char nome[16];
};

struct DadosMatriz {
  uint16_t matriz[40][44];  // matriz 40x44 de uint16_t
};

void gravaDados() {
  flash.eraseSector(FLASH_ADDR);

  DadosSimples d1 = {4321, 25.5, "Sensor Externo"};
  flash.writeStruct(FLASH_ADDR, d1);

  DadosMatriz d2;
  // Preenche a matriz com valores de teste (exemplo: i*j)
  for (uint8_t i = 0; i < 40; i++) {
    for (uint8_t j = 0; j < 44; j++) {
      d2.matriz[i][j] = i * j;
    }
  }

  // Endereço após DadosSimples
  uint32_t addr2 = FLASH_ADDR + sizeof(DadosSimples);
  flash.writeStruct(addr2, d2);

  Serial.println("Estruturas gravadas!");
}

void leDados() {
  DadosSimples d1;
  flash.readStruct(FLASH_ADDR, d1);

  Serial.println("Dados Simples lidos:");
  Serial.print("ID: "); Serial.println(d1.id);
  Serial.print("Temperatura: "); Serial.println(d1.temperatura);
  Serial.print("Nome: "); Serial.println(d1.nome);

  DadosMatriz d2;
  uint32_t addr2 = FLASH_ADDR + sizeof(DadosSimples);
  flash.readStruct(addr2, d2);

  Serial.println("Dados Matriz lidos:");
  Serial.print("Valor [10][20]: ");
  Serial.println(d2.matriz[10][20]);
  Serial.print("Valor [39][43]: ");
  Serial.println(d2.matriz[39][43]);
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  flash.begin();

  // Comente após a primeira gravação para testar só leitura
  //gravaDados();

  leDados();
}

void loop() {
  // nada aqui
}
