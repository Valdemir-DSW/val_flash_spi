# val_flash_spi
On both stm32 and Arduino you can easily use and record common variables with your Ci BIOS W25Q16

# 📦 val_flash_spi — Biblioteca de memória Flash SPI para Arduino e STM32

**val_flash_spi** é uma biblioteca simples e direta para controlar memórias Flash SPI como a **W25Q16JV** utilizando qualquer microcontrolador compatível com **Arduino IDE**, incluindo placas **STM32 (STM32F1, STM32F4, etc)** via STM32duino.

Ela oferece leitura e gravação de bytes e structs diretamente na flash, como se fosse uma EEPROM de alta capacidade (2MB ou mais). Ideal para armazenar configurações, logs, calibrações, etc.

---

## ✅ Compatibilidade

- ✅ Memórias Flash SPI padrão JEDEC (testado com W25Q16JV)
- ✅ STM32 (STM32F103, STM32F407, Blue Pill, Black Pill...)
- ✅ ESP32 / ESP8266
- ✅ Arduino Mega, Uno, Nano, Leonardo...
- ✅ Compatível com SPI padrão ou SPI personalizada (ex: SPI1, SPI2...)

---

## 🚀 Instalação

1. Baixe o arquivo `val_flash_spi.zip`
2. No Arduino IDE, vá em: `Sketch > Incluir Biblioteca > Adicionar Biblioteca .ZIP...`
3. Selecione o arquivo `.zip` baixado
4. Pronto! Você verá os exemplos em `Arquivo > Exemplos > val_flash_spi`

---

## 🧰 Pinos e Conexões com W25Q16JV

| Flash | STM32 / Arduino | Descrição |
|-------|------------------|-----------|
| CS    | Qualquer pino    | Chip Select (ex: PA4) |
| MISO  | SPI MISO         | Ex: PA6 (STM32), D12 (Uno) |
| MOSI  | SPI MOSI         | Ex: PA7 (STM32), D11 (Uno) |
| SCK   | SPI SCK          | Ex: PA5 (STM32), D13 (Uno) |
| VCC   | 3.3V             | Não use 5V! |
| GND   | GND              | Terra comum |

---

## 🧾 Exemplo Básico

```cpp
#include <val_flash_spi.h>

#define FLASH_CS PA4
#define FLASH_ADDR 0x000000

ValFlashSPI flash(FLASH_CS); // 4 MHz padrão

struct Dados {
  uint32_t id;
  float temperatura;
  char nome[16];
};

void setup() {
  Serial.begin(115200);
  flash.begin();

  Dados dados = {1234, 25.5, "Teste"};
  flash.eraseSector(FLASH_ADDR);
  flash.writeStruct(FLASH_ADDR, dados);

  Dados lidos;
  flash.readStruct(FLASH_ADDR, lidos);

  Serial.println(lidos.nome);
}
```

---

## 🧠 Recursos

- `writeByte(addr, valor)` → grava 1 byte
- `readByte(addr)` → lê 1 byte
- `writeStruct(addr, struct)` → grava struct inteira
- `readStruct(addr, struct)` → lê struct inteira
- `eraseSector(addr)` → apaga 4KB
- `eraseChip()` → apaga tudo (⚠️ lento!)
- SPI customizável: `ValFlashSPI(cs, velocidade, &SPIx)`

---

## 🧪 Exemplo com SPI personalizada

```cpp
#include <val_flash_spi.h>

SPIClass SPI_ALT(PB15, PB14, PB13); // MISO, MOSI, SCK

ValFlashSPI flash(PB12, 2000000, &SPI_ALT);

void setup() {
  Serial.begin(115200);
  flash.begin();
  flash.eraseChip();
}
```

---

## 📂 Estrutura da Biblioteca

```
val_flash_spi/
├── src/
│   ├── val_flash_spi.h
│   ├── val_flash_spi.cpp
│   └── val_flash_spi.tpp
├── examples/
│   ├── Exemplo_Basico/
│   └── Exemplo_Completo/
└── library.properties
```

---

## 🛠️ Sugestões futuras (em desenvolvimento)

- Buffer circular de structs
- Registro com índice automático
- Verificação CRC
- Backup automático

---

## 👨‍💻 Autor

**Valdemir Junior** – Uso livre e educacional .
I will be happy if you can leave the credits!
