#define NO_GROUP       0

#ifdef SYS_1802MINI
  #define BRMK         bn2
  #define BRSP         b2
  #define SEMK         seq
  #define SESP         req
  #define EXP_PORT     1
  #define EXP_MEMORY
  #define IDE_GROUP    0
  #define IDE_SELECT   2
  #define IDE_DATA     3
  #define RTC_GROUP    0
  #define RTC_PORT     5
  #define UART_GROUP   0
  #define UART_DATA    6
  #define UART_STATUS  7
  #define SET_BAUD     19200
  #define FREQ_KHZ     4000
#endif

#ifdef SYS_SUPERELF
  #define BRMK         bn2
  #define BRSP         b2
  #define SEMK         seq
  #define SESP         req
  #define EXP_PORT     5
  #define EXP_MEMORY
  #define IDE_GROUP    0
  #define IDE_SELECT   2
  #define IDE_DATA     3
  #define UART_GROUP   0
  #define UART_DATA    6
  #define UART_STATUS  7
  #define RTC_GROUP    1
  #define RTC_PORT     3
  #define SET_BAUD     9600
  #define FREQ_KHZ     1790
#endif

#ifdef SYS_RC1802
  #define BRMK         bn3
  #define BRSP         b3
  #define SEMK         seq
  #define SESP         req
  #define EXP_PORT     1
  #define IDE_GROUP    0
  #define IDE_SELECT   2
  #define IDE_DATA     3
  #define UART_GROUP   1
  #define UART_DATA    2
  #define UART_STATUS  3
  #define RTC_GROUP    2
  #define RTC_PORT     3
  #define SET_BAUD     9600
  #define FREQ_KHZ     2000
#endif

#ifdef SYS_TEST
  #define BRMK         bn2
  #define BRSP         b2
  #define SEMK         seq
  #define SESP         req
  #define EXP_PORT     5
  #define IDE_GROUP    1
  #define IDE_SELECT   2
  #define IDE_DATA     3
  #define UART_GROUP   4
  #define UART_DATA    6
  #define UART_STATUS  7
  #define RTC_GROUP    2
  #define RTC_PORT     3
  #define FREQ_KHZ     4000
#endif

#ifdef SYS_1802MAX
  #define BRMK         bn2
  #define BRSP         b2
  #define SEMK         req
  #define SESP         seq
  #define EXP_PORT     5
  #define EXP_MEMORY
  #define MICRO_SD
  #define SPI_GROUP    0
  #define SPI_CTL      2
  #define SPI_DATA     3
  #define RTC_GROUP    0
  #define RTC_PORT     1
  #define UART_GROUP   0
  #define UART_DATA    6
  #define UART_STATUS  7
  #define FAST_UART
  #define HI_BAUD
  #define SET_BAUD     38400
  #define FREQ_KHZ     4000
  #define I2C_GROUP    1
  #define I2C_PORT     3
  #define PIO1_GROUP   1
  #define PIO1_OUTA    2
  #define PIO1_OUTB    3
#endif
