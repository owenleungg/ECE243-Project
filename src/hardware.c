#include "globals.h"

void delay_us(uint32_t c)
{
  timer->control = 0x8; // stop the timer
  timer->status = 0;    // reset TO
  timer->periodlo = ((TIMERVAL * c) & 0x0000FFFF);
  timer->periodhi = ((TIMERVAL * c) & 0xFFFF0000) >> 16;
  timer->control = 0x4;
  while ((timer->status & 0x1) == 0)
    ;
  timer->status = 0; // reset TO
  return;
}

uint32_t time_stamp()
{
  return (timer->snaphi << 16) & timer->snaplo;
}

void GPIO_setup(GPIO_t *port)
{
  uint32_t dir = 0x0;
  // dir |= (1 << FRAME_PIN);
  // dir |= (1 << PIX_PIN);
  // dir |= (1 << SDA_PIN);
  // dir |= (1 << SCL_PIN);
  port->direction = dir;
  port->data = 0x0;

  write_GPIO_bit(port, SDA_PIN, 1); // Ensure SDA is high
  write_GPIO_bit(port, SCL_PIN, 1); // Ensure SCL is high
  return;
}

void write_I2C_byte(GPIO_t *port, uint8_t address, uint8_t byte) {
  i2c_start(port); // Generate start condition
  
  // Send I2C address with the write flag (LSB = 0)
  i2c_send_byte(port, I2C_ADDR << 1); // Shift address left and send
  
  // Send the register address
  i2c_send_byte(port, address);

  // Send the data byte
  i2c_send_byte(port, byte);
  
  i2c_stop(port); // Generate stop condition
  
  printf("in i2c: %u, %u\n", address, byte);
  return;
}

////////

// Function to send a start condition
void i2c_start(GPIO_t *port) {
  write_GPIO_bit(port, SDA_PIN, 1); // Ensure SDA is high
  write_GPIO_bit(port, SCL_PIN, 1); // Ensure SCL is high
  delay_us(I2C_DELAY);
  write_GPIO_bit(port, SDA_PIN, 0);  // Pull SDA low
  delay_us(I2C_DELAY);
  write_GPIO_bit(port, SCL_PIN, 0);  // Pull SCL low
  delay_us(I2C_DELAY);
  return;
}

// Function to send a stop condition
void i2c_stop(GPIO_t *port) {
  write_GPIO_bit(port, SDA_PIN, 0);  // Ensure SDA is low
  write_GPIO_bit(port, SCL_PIN, 1); // Pull SCL high
  delay_us(I2C_DELAY);
  write_GPIO_bit(port, SDA_PIN, 1); // Pull SDA high
  delay_us(I2C_DELAY);
  return;
}

// Function to send a single bit
void i2c_send_bit(GPIO_t *port, bool bit) {
  write_GPIO_bit(port, SDA_PIN, bit);  // Set SDA to the bit value
  delay_us(I2C_DELAY);
  write_GPIO_bit(port, SCL_PIN, 1); // Pulse SCL high
  delay_us(I2C_DELAY);
  write_GPIO_bit(port, SCL_PIN, 0);  // Pulse SCL low
  delay_us(I2C_DELAY);
  return;
}

// Function to send a byte of data
void i2c_send_byte(GPIO_t *port, uint8_t byte) {
  for (int i = 7; i >= 0; i--) {
    bool bit = (byte >> i) & 1;
    i2c_send_bit(port, bit); // Send each bit starting from MSB
  }
  // Now wait for the acknowledgment bit
  // pinMode(SDA_PIN, INPUT); // Release SDA for ACK
  write_GPIO_bit(port, SCL_PIN, 1); // Pulse SCL high to read ACK
  delay_us(I2C_DELAY);
  // bool ack = digitalRead(SDA_PIN); // Read the ACK bit
  write_GPIO_bit(port, SCL_PIN, 0); // Pulse SCL low
  // pinMode(SDA_PIN, OUTPUT); // Restore SDA to output mode
  
  // if (ack) {
  //   // If the ACK is high, there was no acknowledgment (error)
  //   Serial.println("I2C write error: No ACK received");
  // }
  return;
}

////////

uint32_t read_GPIO_word(GPIO_t *port)
{
  return (port->data);
}

void write_GPIO_word(GPIO_t *port, uint32_t word)
{
  (port->data) = word;
  return;
}

bool read_GPIO_bit(GPIO_t *port, uint8_t pin)
{
  uint32_t mask = (port->data) & (1 << pin);
  return mask > 0;
}

void write_GPIO_bit(GPIO_t *port, uint8_t pin, bool value)
{
  if (value != read_GPIO_bit(port, pin))
  {
    uint32_t word = read_GPIO_word(port);
    word ^= (1 << pin);
    (port->data) = word;
  }
  return;
}

void wait_for_rising_edge_GPIO(GPIO_t *port, uint8_t pin)
{
  while (read_GPIO_bit(port, pin) == 1)
  {
    ;
  }
  while (read_GPIO_bit(port, pin) == 0)
  {
    ;
  }
  return;
}

void wait_for_falling_edge_GPIO(GPIO_t *port, uint8_t pin)
{
  while (read_GPIO_bit(port, pin) == 0)
  {
    ;
  }
  while (read_GPIO_bit(port, pin) == 1)
  {
    ;
  }
  return;
}

uint16_t read_pixel(GPIO_t* port, uint8_t plk_pin) 
{
  wait_for_rising_edge_GPIO(port, plk_pin);
  uint32_t p = read_GPIO_word(port);
  uint8_t byte_1 = ((p & 0x80000000) >> 24) | ((p & 0x3c000000) >> 23) | ((p & 0x01c00000) >> 22);
  wait_for_rising_edge_GPIO(port, plk_pin);
  p = read_GPIO_word(port);
  uint8_t byte_2 = ((p & 0x80000000) >> 24) | ((p & 0x3c000000) >> 23) | ((p & 0x01c00000) >> 22);
  return ((byte_1 << 8) | (byte_2));
}

uint16_t read_pixel2(GPIO_t *port)
{
  uint16_t result = 0x0;
  for (int i = 0; i < 8; ++i) {
    write_GPIO_bit(GPIO_1, PIX_PIN, 1);
    delay_us(1);
    uint32_t current = read_GPIO_word(port) & (0x03);
    result |= (current << (7-i)*2);
    write_GPIO_bit(GPIO_1, PIX_PIN, 0);
    delay_us(1);
  }
  return result;
}

void plot_pixel(int x, int y, short int line_color)
{
  volatile short int *one_pixel_address =
      (volatile short int *)(pixel_buffer_start + (y << 10) + (x << 1));

  *one_pixel_address = line_color;
}

void wait_for_vsync()
{
  volatile int *pixel_ctrl_ptr = (int *)PIXEL_BUF_CTRL_BASE;
  *pixel_ctrl_ptr = 1;
  int status = *(pixel_ctrl_ptr + 3);
  while ((status & 0x01) != 0)
  {
    status = *(pixel_ctrl_ptr + 3);
  }
}

void clear_screen()
{
  for (int x = 0; x < SCREEN_WIDTH; x++)
  {
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
      plot_pixel(x, y, 0x0000);
    }
  }
}
