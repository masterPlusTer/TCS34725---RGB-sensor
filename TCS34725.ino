#include <Wire.h>

// Direcciones y registros del TCS34725
#define TCS34725_ADDRESS 0x29 // Dirección I2C del TCS34725
#define COMMAND_BIT 0x80      // Comando para acceder a registros
#define ENABLE_REGISTER 0x00  // Registro de habilitación
#define ATIME_REGISTER 0x01   // Tiempo de integración
#define CONTROL_REGISTER 0x0F // Ganancia
#define CLEAR_LOW 0x14        // Datos claros (bajos)
#define RED_LOW 0x16          // Datos rojos (bajos)
#define GREEN_LOW 0x18        // Datos verdes (bajos)
#define BLUE_LOW 0x1A         // Datos azules (bajos)

// Valores para configuración del sensor
#define ENABLE_PON 0x01       // Power ON
#define ENABLE_AEN 0x02       // Habilitar ADC
#define GAIN_1X 0x00          // Ganancia 1X
#define INTEGRATION_TIME 0xD5 // Tiempo de integración (~101ms)

// Pines del LED RGB
#define LED_RED 9
#define LED_GREEN 10
#define LED_BLUE 11

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Configura los pines del LED RGB
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  // Inicializa el sensor
  initializeTCS34725();

  Serial.println("TCS34725 inicializado.");
}

void loop() {
  uint16_t clear = readColor(CLEAR_LOW);
  uint16_t red = readColor(RED_LOW);
  uint16_t green = readColor(GREEN_LOW);
  uint16_t blue = readColor(BLUE_LOW);

  // Normalizar los valores a rango PWM (0-255)
  uint8_t redPWM = map(red, 0, 65535, 0, 255);
  uint8_t greenPWM = map(green, 0, 65535, 0, 255);
  uint8_t bluePWM = map(blue, 0, 65535, 0, 255);

  // Mostrar los datos en consola
  Serial.print("Clear: "); Serial.print(clear);
  Serial.print(", Red: "); Serial.print(red);
  Serial.print(", Green: "); Serial.print(green);
  Serial.print(", Blue: "); Serial.println(blue);

  // Controlar el LED RGB
  analogWrite(LED_RED, redPWM);
  analogWrite(LED_GREEN, greenPWM);
  analogWrite(LED_BLUE, bluePWM);

  delay(1000); // Leer datos cada segundo
}

// Función para inicializar el TCS34725
void initializeTCS34725() {
  // Habilita el sensor
  writeRegister(ENABLE_REGISTER, ENABLE_PON);
  delay(3); // Tiempo mínimo para encender el sensor
  writeRegister(ENABLE_REGISTER, ENABLE_PON | ENABLE_AEN);

  // Configura el tiempo de integración
  writeRegister(ATIME_REGISTER, INTEGRATION_TIME);

  // Configura la ganancia
  writeRegister(CONTROL_REGISTER, GAIN_1X);
}

// Función para leer un color del sensor
uint16_t readColor(uint8_t colorRegister) {
  uint8_t low = readRegister(colorRegister);
  uint8_t high = readRegister(colorRegister + 1);
  return (high << 8) | low; // Combina los bytes alto y bajo
}

// Función para escribir en un registro del TCS34725
void writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(TCS34725_ADDRESS);
  Wire.write(COMMAND_BIT | reg);
  Wire.write(value);
  Wire.endTransmission();
}

// Función para leer un registro del TCS34725
uint8_t readRegister(uint8_t reg) {
  Wire.beginTransmission(TCS34725_ADDRESS);
  Wire.write(COMMAND_BIT | reg);
  Wire.endTransmission();

  Wire.requestFrom(TCS34725_ADDRESS, 1);
  return Wire.available() ? Wire.read() : 0;
}
