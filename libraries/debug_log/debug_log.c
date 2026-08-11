#include "debug_log.h"

#include <string.h>
#include "dnp3_utils.h"
#include "usbd_cdc_if.h"


extern USBD_HandleTypeDef hUsbDeviceFS;
extern volatile uint8_t usb_cdc_port_open;


/*
	Implement this function.
	Backend to send data by the means you choose.
*/
static void send(const char *cstr);


void debug_log_format(const char *fmt, ...)
{
	char buffer[DEBUG_LOG_MAX_LEN_CSTR_BUFFER];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	send(buffer);
}

static void send(const char *cstr)
{
	if ( (cstr == NULL) || (strlen(cstr) == 0) )
	{
		return;
	}

	// Checks whether the USB device is enumerated and configured.
	if (hUsbDeviceFS.dev_state != USBD_STATE_CONFIGURED)
	{	
		return;
	}

	/*
		This variable is set in CDC_Control_FS() in USB_DEVICE\App\usbd_cdc_if.c
		and cleared in HAL_PCD_DisconnectCallback() in USB_DEVICE\Target\usbd_conf.c
	*/
	if (usb_cdc_port_open == 0)
	{
		return;
	}

	uint32_t ti = dnp3_get_ms();
	while (CDC_Transmit_FS((uint8_t*)cstr, strlen(cstr)) == USBD_BUSY) 
	{
		if ( (dnp3_get_ms() - ti) > DEBUG_LOG_SEND_TIMEOUT_MS )
		{
			break;
		}
	}
}
