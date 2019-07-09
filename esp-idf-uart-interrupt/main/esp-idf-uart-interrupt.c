/*
UART Interrupt Example
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_intr_alloc.h"



#define BLINK_GPIO1 GPIO_NUM_23
#define BLINK_GPIO2 GPIO_NUM_22
#define BLINK_GPIO3 GPIO_NUM_21
#define BLINK_GPIO4 GPIO_NUM_19

static const char *TAG = "uart_events";

/**
 * This example shows how to use the UART driver to handle UART interrupt.
 *
 * - Port: UART0
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: on
 * - Pin assignment: TxD (default), RxD (default)
 */

#define EX_UART_NUM UART_NUM_0
#define PATTERN_CHR_NUM    (3)         /*!< Set the number of consecutive and identical characters received by receiver which defines a UART pattern*/

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)
static QueueHandle_t uart0_queue;

// Both definition are same and valid
//static uart_isr_handle_t *handle_console;
static intr_handle_t handle_console;

// Receive buffer to collect incoming data
uint8_t rxbuf[256];
// Register to collect data length
uint16_t urxlen;

#define NOTASK 0
static bool i = 0;
volatile bool rx_complate = false;
const char find1 = ':';
const char find2 = '!';

void blink_task(void *pvParameter)
{
    gpio_pad_select_gpio(BLINK_GPIO1);
    gpio_pad_select_gpio(BLINK_GPIO2);
    gpio_pad_select_gpio(BLINK_GPIO3);
    gpio_pad_select_gpio(BLINK_GPIO4);

    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO1, GPIO_MODE_OUTPUT);
    gpio_set_direction(BLINK_GPIO2, GPIO_MODE_OUTPUT);
    gpio_set_direction(BLINK_GPIO3, GPIO_MODE_OUTPUT);
    gpio_set_direction(BLINK_GPIO4, GPIO_MODE_OUTPUT);
	static bool a = false, b = false, c = false, d = false;

    while(1) {
        /* Blink off (output low) */
//        gpio_set_level(BLINK_GPIO, 0);
//        vTaskDelay(1000 / portTICK_PERIOD_MS);
//        /* Blink on (output high) */
//        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    	if(rx_complate)
    	{
    		switch((int)(rxbuf[0])-'0')
    		{
				case 1:
					a = !a;
					gpio_set_level(BLINK_GPIO1,a );
					rxbuf[0] = '5';
					break;
				case 2:
					b = !b;
					gpio_set_level(BLINK_GPIO2,b );
					rxbuf[0] = '5';
					break;
				case 3:
					c = !c;
					gpio_set_level(BLINK_GPIO3,c );
					rxbuf[0] = '5';
					break;
				case 4:
					d = !d;
					gpio_set_level(BLINK_GPIO4,d );
					rxbuf[0] = '5';
					break;

    		}

    	}
//    	if(i)
//    	{
//    		 gpio_set_level(BLINK_GPIO1, 0);
//    		 gpio_set_level(BLINK_GPIO2, 0);
//    		 gpio_set_level(BLINK_GPIO3, 0);
//    		 gpio_set_level(BLINK_GPIO4, 0);
//    	}
//    	else
//    	{
//    		gpio_set_level(BLINK_GPIO1, 1);
//    		gpio_set_level(BLINK_GPIO2, 1);
//    		gpio_set_level(BLINK_GPIO3, 1);
//    		gpio_set_level(BLINK_GPIO4, 1);
//    	}
    }
}
/*
 * Define UART interrupt subroutine to ackowledge interrupt
 */
static void IRAM_ATTR uart_intr_handle(void *arg)
{
  uint16_t rx_fifo_len, status;
  uint16_t is = 0;

  status = UART0.int_st.val; // read UART interrupt Status
  rx_fifo_len = UART0.status.rxfifo_cnt; // read number of bytes in UART buffer
  rx_complate = false;
  while(rx_fifo_len)
  {
   rxbuf[is++] = UART0.fifo.rw_byte; // read all bytes
   rx_fifo_len--;
  }
  rx_complate = true;
 // after reading bytes from buffer clear UART interrupt status
 uart_clear_intr_status(EX_UART_NUM, UART_RXFIFO_FULL_INT_CLR|UART_RXFIFO_TOUT_INT_CLR);

// a test code or debug code to indicate UART receives successfully,
// you can redirect received byte as echo also
 uart_write_bytes(EX_UART_NUM, (const char*) rxbuf, 1);

 i = !i;
}
/*
 * main
 */
void app_main()
{
	int ret;
	esp_log_level_set(TAG, ESP_LOG_INFO);

	/* Configure parameters of an UART driver,
	* communication pins and install the driver */
	uart_config_t uart_config = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE
	};

	ESP_ERROR_CHECK(uart_param_config(EX_UART_NUM, &uart_config));

	//Set UART log level
	esp_log_level_set(TAG, ESP_LOG_INFO);

	//Set UART pins (using UART0 default pins ie no changes.)
	ESP_ERROR_CHECK(uart_set_pin(EX_UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

	//Install UART driver, and get the queue.
	ESP_ERROR_CHECK(uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0));

	// release the pre registered UART handler/subroutine
	ESP_ERROR_CHECK(uart_isr_free(EX_UART_NUM));

	// register new UART subroutine
	ESP_ERROR_CHECK(uart_isr_register(EX_UART_NUM,uart_intr_handle, NULL, ESP_INTR_FLAG_IRAM, &handle_console));

	// enable RX interrupt
	ESP_ERROR_CHECK(uart_enable_rx_intr(EX_UART_NUM));
	#if (NOTASK == 1)
	while(1)
	{
		vTaskDelay(1000);
	}
	#else
		xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
	#endif
}
