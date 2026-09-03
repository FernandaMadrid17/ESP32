#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Configuración del teclado
const byte FILAS = 4;
const byte COLUMNAS = 4;

// Distribución de teclas
char teclas[FILAS][COLUMNAS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// Pines del teclado
byte pinesFilas[FILAS] = {19, 18, 5, 17};
byte pinesColumnas[COLUMNAS] = {16, 4, 12, 14};

Keypad teclado = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, FILAS, COLUMNAS);

void setup() {
  Serial.begin(115200);

  // LCD I2C en ESP32
  Wire.begin(21, 22);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Tecla:");
}

void loop() {
  char tecla_presionada = teclado.getKey();

  if (tecla_presionada) {
    Serial.print("Tecla presionada: ");
    Serial.println(tecla_presionada);

    lcd.setCursor(0, 1);
    lcd.print("                "); // limpia la línea
    lcd.setCursor(0, 1);
    lcd.print(tecla_presionada);
  }
}
