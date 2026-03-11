#include "debug_log.h"

#include <string.h>
#include "utils.h"
#include "usbd_cdc_if.h"


/*
	Implement this function.
	Backend to send data by the means you choose.
*/
static void send(const char *cstr)
{
	uint8_t count = 0;
	while (CDC_Transmit_FS((uint8_t*)cstr, strlen(cstr)) == USBD_BUSY) 
	{
		delay_ms(1);

		count++;
		if (count > 100)
		{
			break;
		}
	}
}

void debug_log_format(const char *fmt, ...)
{
	char buffer[MAX_LEN_CSTR_BUFFER];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	send(buffer);
}
