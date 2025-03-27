#define I2C_ADDR 0x21
#include <Wire.h>
#include <stdint.h>

bool write_I2C_byte(uint8_t address, uint8_t byte);

uint8_t registers[31][2] = {
    {0x12, 0x14}, /* "COMP7 QVGA, RGB" */
    {0x8c, 0x00}, /* "Disable RGB444" */
    {0x40, 0x90}, /* "COM15 output range, RGB565" */
    {0x14, 0x6a}, /* "COM9 AGC ceiling" */
    
    {0x4f, 0x7d}, /* "matrix coefficient 1" */ // b3
    {0x50, 0x84}, /* "matrix coefficient 2" */ // b3
    {0x51, 0x1a},  /* "matrix coefficient 3, vb" */ // 0
    {0x52, 0x16}, /* "matrix coefficient 4" */ // 3d
    {0x53, 0x3a}, /* "matrix coefficient 5" */ // a7
    {0x54, 0x30}, /* "matrix coefficient 6" */ // e4
    {0x58, 0x1e}, /* "MTXS " */ // e4
    // {0x3d, 0x58}, /* "COM13 enable gamma" */

    {0xbe, 0x4f},      /* "AD-CHB Blue Channel Black Level Compensation" */
    {0xbf, 0x4f},      /* "AD-CHR Red Channel Black Level Compensation" */
    {0xc0, 0x3f},      /* "AD-CHGb Gb Channel Black Level Compensation" */
    {0xc1, 0x3f},      /* "AD-CHGr Gr Channel Black Level Compensation" */

    {0x19, 0 >> 1},                                           /* "VSTRT" */
    {0x1a, 245 >> 1},                                         /* "VSTOP" */
    {0x03, ((0 & 0b1) << 1) | ((245 & 0b1) << 3)},            /* "VREF" */
    {0x17, 174 >> 3},                                         /* "HSTART" */
    {0x18, 34 >> 3},                                          /* "HSTOP" */
    {0x32, 0b00000000 | (174 & 0b111) | ((34 & 0b111) << 3)}, /* "HREF" */

    {0x0c, 0x04}, /* "COM3 enable downsamp/crop/window" */
    {0x3e, 0x19}, /* "COM14 divide by 2" */
    {0x72, 0x11}, /* "SCALING_DCWCTR downsample by 2" */
    {0x73, 0xf1}, /* "SCALING_PCLK_DIV divide by 2" */

    {0x15, 0x20},      /* "COM10 disable PCLK when blank" */
    {0x0f, 0x80},      /* "COM6 disable HREF at optical black" */
    {0x11, 0x3f},      /* "CLKRC internal clock pre-scalar" */
    {0x6b, 0x0a},      /* "DBLV pll" */

    {0x13, 0xcf},      /* "COM8 AEC fast, step size" */
    {0xaa, 0x94},      /* "HAECC7 Histogram-based AEC" */

    // {0x71, 0xb5},      /* "colour bar" */
    
};

void setup() {
  // put your setup code here, to run once:

  Wire.begin();
  Serial.begin(9600);

  write_I2C_byte(0x12, 0x80); // Reset

  delay(200);

  for (int i = 0; i < 31; ++i) {
    write_I2C_byte(registers[i][0], registers[i][1]);
    Serial.print("i2c write: ");
    Serial.print(registers[i][0]);
    Serial.print(", ");
    Serial.print(registers[i][1]);
    Serial.print("\n");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  // for (int i = 0; i < 14; ++i) {
  //   write_I2C_byte(registers[i][0], registers[i][1]);
  //   Serial.print("i2c write: ");
  //   Serial.print(registers[i][0]);
  //   Serial.print(", ");
  //   Serial.print(registers[i][1]);
  //   Serial.print("\n");
  // }
  // delay(100);
}

bool write_I2C_byte(uint8_t address, uint8_t byte) {
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(address);
  Wire.write(byte);
  return Wire.endTransmission() == 0;
}
