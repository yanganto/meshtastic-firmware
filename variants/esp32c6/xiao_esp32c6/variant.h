/*
  XIAO ESP32C6 + Wio-SX1262 for XIAO (standalone SKU 113010003)
  Schematic: https://files.seeedstudio.com/products/SenseCAP/Wio_SX1262/Wio-SX1262%20for%20XIAO%20V1.0_SCH.pdf

  XIAO ESP32C6 D-pin to GPIO mapping (from pins_arduino.h):
    D0=GPIO0,  D1=GPIO1,  D2=GPIO2,  D3=GPIO21
    D4=GPIO22 (SDA),      D5=GPIO23 (SCL)
    D8=GPIO19 (SCK), D9=GPIO20 (MISO), D10=GPIO18 (MOSI)

  NOTE: SX1262 CS (D4/GPIO22) and RXEN (D5/GPIO23) share the XIAO I2C pins.
  No I2C devices on this board. BBTxRXHal calls gpio_reset_pin(22) in its
  constructor to reclaim GPIO22 from the I2C peripheral before SPI init.
*/

#define I2C_SDA 22
#define I2C_SCL 23

#define LED_POWER 15
#define LED_STATE_ON 1

// LoRa SX1262 (Wio-SX1262 for XIAO standalone)
#undef LORA_SCK
#undef LORA_MISO
#undef LORA_MOSI
#undef LORA_CS

#define USE_SX1262
#define LORA_SCK  19    // D8
#define LORA_MISO 20    // D9
#define LORA_MOSI 18    // D10
#define LORA_CS   22    // D4 = GPIO22
#define SX126X_CS     LORA_CS
#define SX126X_DIO1   1    // D1 = GPIO1
#define SX126X_BUSY   21   // D3 = GPIO21
#define SX126X_RESET  2    // D2 = GPIO2
#define LORA_RESET    SX126X_RESET

// RF switch: DIO2 (D0/GPIO0) driven by SX1262 for TX; RXEN (D5/GPIO23) driven by MCU for RX
#define SX126X_DIO2_AS_RF_SWITCH
#define SX126X_RXEN   23   // D5 = GPIO23
#define SX126X_TXEN   RADIOLIB_NC

// Wio-SX1262 for XIAO uses a TCXO powered via DIO3 (same as other Seeed SX126x boards)
#define SX126X_DIO3_TCXO_VOLTAGE 1.8

#define SERIAL_PRINT_PORT 1
