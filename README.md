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

Pode baixar diretamente pelo gerenciador de bibliotecas.

<img width="258" height="609" alt="image" src="https://github.com/user-attachments/assets/de19242c-dfc6-4b62-b497-3bd6c757d34c" />

ou manual

1. Baixe o arquivo `val_flash_spi.zip`
2. No Arduino IDE, vá em: `Sketch > Incluir Biblioteca > Adicionar Biblioteca .ZIP...`
3. Selecione o arquivo `.zip` baixado
4. Pronto! Você verá os exemplos em `Arquivo > Exemplos > val_flash_spi`

---

## 🧰 Pinos e Conexões com W25Q16JV

| Flash | STM32 / Arduino | Descrição |
|-------|------------------|-----------|
| CS    | Qualquer pino    | Chip Select (ex: PB0) |
| MISO  | SPI MISO         | Ex: PB4 (STM32 F4), D12 (Uno) |
| MOSI  | SPI MOSI         | Ex: PB5 (STM32 F4), D11 (Uno) |
| SCK   | SPI SCK          | Ex: pb3 (STM32 F4), D13 (Uno) |
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
⚠️ Atenção ao uso de endereços na Flash SPI
Quando você grava dados na memória Flash usando esta biblioteca, os dados são salvos byte a byte a partir do endereço que você especificar. Isso significa que não basta escolher endereços aleatórios ou sequenciais manualmente sem considerar o tamanho real dos dados gravados.

📌 Exemplo prático:
Se você gravar uma estrutura no endereço 0x000000, o próximo endereço disponível para gravação depende do tamanho total da estrutura anterior:
```cpp
struct __attribute__((packed)) Config {
  uint8_t modo;        // 1 byte
  uint16_t tempo;      // 2 bytes
  float valor;         // 4 bytes
}; // Total = 7 bytes
```
Neste caso, a próxima gravação não deve começar em 0x000001, 0x000002, etc., pois isso sobrescreveria parte dos dados anteriores.

O endereço correto para o próximo bloco seria:
```
0x000000 + sizeof(Config) = 0x000007
```
❌ O que pode dar errado
Se você começar a gravar outra estrutura em um endereço mal calculado, você pode:

Corromper dados anteriores

Ler dados truncados ou inválidos

Ter comportamento imprevisível no seu código

✅ Como fazer certo
Sempre calcule os endereços usando sizeof():
```cpp
#define ADDR_CONFIG   0x000000
#define ADDR_LOG1     (ADDR_CONFIG + sizeof(Config))
#define ADDR_LOG2     (ADDR_LOG1 + sizeof(Log))
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
