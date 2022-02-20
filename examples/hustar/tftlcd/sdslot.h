#include <stdint.h>
#include <string.h>

#include "app_uart.h"
#include "nrf_delay.h"
#include "nrf_drv_gpiote.h"
#include "boards.h"

#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

#include "app_util_platform.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


bool fatfs_init(void);
void fatfs_uninit(void);
void fatfs_ls(void);
void fatfs_write(char * fileName, char * text, int bytes);
int fatfs_read(char * fileName, char * text, int bytes);
void fatfs_delete_all_file(void);


