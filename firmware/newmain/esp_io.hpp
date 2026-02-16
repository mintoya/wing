#pragma once
#include "Arduino.h"
#include "hal/usb_serial_jtag_ll.h"
#include "my-lib/mytypes.h"
#include <HWCDC.h>
#include <stdint.h>
static inline int usb_serial_jtag_ll_txfifo_writable(void);
extern inline int usb_serial_jtag_ll_rxfifo_data_available(void);
extern inline __attribute__((always_inline)) void usb_serial_jtag_ll_enable_bus_clock(bool clk_en);
extern inline void usb_serial_jtag_ll_txfifo_flush(void);
extern inline uint32_t usb_serial_jtag_ll_read_rxfifo(uint8_t *buf, uint32_t rd_len);
static inline uint32_t usb_serial_jtag_ll_write_txfifo(const uint8_t *buf, uint32_t wr_len);

// namespace ESP_IO {
// static usize available(void) {
//   return usb_serial_jtag_ll_rxfifo_data_available();
// }
// static u8 read(void) {
//   u8 r = -1;
//   usb_serial_jtag_ll_read_rxfifo(&r, 1);
//   return r;
// }
// static usize read(u8 *buffer, usize len) {
//   usb_serial_jtag_ll_read_rxfifo(buffer, len);
// }
// static void write(u8 in) {
//   while (!usb_serial_jtag_ll_txfifo_writable())
//     ;
//   usb_serial_jtag_ll_write_txfifo(&in, 1);
// }
// static void write(u8 *buffer, usize len) {
//   while (!usb_serial_jtag_ll_txfifo_writable())
//     ;
//   usb_serial_jtag_ll_write_txfifo(buffer, len);
// }
// static void flush(void) {
//   usb_serial_jtag_ll_txfifo_flush();
// }
// static void begin(usize _) {
//   (void)_;
//   usb_serial_jtag_ll_enable_bus_clock(true);
// }
// }; // namespace ESP_IO

namespace ESP_IO {
static usize available(void) {
  return Serial.available();
}
static u8 read(void) {
  return Serial.read();
}
static usize read(u8 *buffer, usize len) {
  return Serial.readBytes((char *)buffer, len);
}
static void write(u8 in) {
  Serial.write(in);
}
static void write(u8 *buffer, usize len) {
  Serial.write(buffer, len);
}
static void flush(void) {
  Serial.flush();
}
static void begin(usize baud) {
  Serial.begin(baud);
}
}; // namespace ESP_IO
