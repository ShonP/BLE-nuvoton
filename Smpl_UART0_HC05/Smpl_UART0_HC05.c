// haim ozer & shon pazarker
// 316063569 & 208426890

// Smpl_UART0_HC05 : bluetooth module receive 8 bytes and display on LCD
//
// HC05 Bluetooth module
// pin1 : KEY   N.C
// pin2 : VCC   to Vcc +5V
// pin3 : GND   to GND
// pin4 : TXD   to NUC140 UART0-RX (GPB0)
// pin5 : RXD   to NUC140 UART0-TX (GPB1)
// pin6 : STATE N.C.

#include <stdio.h>
#include <string.h>
#include "NUC1xx.h"
#include "Driver\DrvUART.h"
#include "Driver\DrvGPIO.h"
#include "Driver\DrvSYS.h"
#include "NUC1xx-LB_002\LCD_Driver.h"
#include "LCD_Driver.h"
#include "2D_Graphic_Driver.h"
#include "DrvADC.h"
#define X0 64 // Circle initial X
#define Y0 28 // Circle initial Y

unsigned char DisplayBuf[128 * 8];
static char prevDirection[10] = "";
#define DATASIZE 8

char TEXT[16];
volatile uint8_t comRbuf[9];
volatile uint8_t comRbytes = 0;

const char *getDirection(int vx, int vy)
{
	static char direction[10];				 // Static buffer to hold the direction string
	memset(direction, 0, sizeof(direction)); // Clear the buffer

	if (vy > 4000)
	{
		strcpy(direction, "UP");
	}
	else if (vy < 10)
	{
		strcpy(direction, "DOWN");
	}
	else if (vx > 4000)
	{
		strcpy(direction, "LEFT");
	}
	else if (vx < 10)
	{
		strcpy(direction, "RIGHT");
	}
	else
	{
		strcpy(direction, "STATIC");
	}

	return direction;
}

void Init_LED() // Initialize GPIO pins
{
	DrvGPIO_Open(E_GPC, 12, E_IO_OUTPUT); // GPC12 pin set to output mode
	DrvGPIO_SetBit(E_GPC, 12);			  // Goutput Hi to turn off LED
	DrvGPIO_Open(E_GPC, 13, E_IO_OUTPUT); // GPC12 pin set to output mode
	DrvGPIO_SetBit(E_GPC, 13);			  // Goutput Hi to turn off LED
	DrvGPIO_Open(E_GPC, 14, E_IO_OUTPUT); // GPC12 pin set to output mode
	DrvGPIO_SetBit(E_GPC, 14);			  // Goutput Hi to turn off LED
	DrvGPIO_Open(E_GPC, 15, E_IO_OUTPUT); // GPC12 pin set to output mode
	DrvGPIO_SetBit(E_GPC, 15);			  // Goutput Hi to turn off LED
	DrvGPIO_Open(E_GPA, 12, E_IO_OUTPUT); // GPA12 pin set to output mode
	DrvGPIO_Open(E_GPA, 13, E_IO_OUTPUT); // GPA13 pin set to output mode
	DrvGPIO_Open(E_GPA, 14, E_IO_OUTPUT); // GPA14 pin set to output mode
	DrvGPIO_SetBit(E_GPA, 12);			  // GPA12 pin output Hi to turn off Blue  LED
	DrvGPIO_SetBit(E_GPA, 13);			  // GPA13 pin output Hi to turn off Green LED
	DrvGPIO_SetBit(E_GPA, 14);			  // GPA14 pin output Hi to turn off Red   LED
}

void UART_INT_HANDLE(void)
{
	uint8_t x_ball, y_ball, r; // circle variables
	// add variables to create a smile face
	int32_t eyes, mouth;
	x_ball = X0; // circle center x
	y_ball = Y0; // circle center y
	r = 20;		 // circle radius
	eyes = 1;	 // eyes
	mouth = 3;	 // mouth
	while (UART0->ISR.RDA_IF == 1)
	{
		comRbuf[comRbytes] = UART0->DATA;
		comRbytes++;
		if (comRbytes == 3)
		{
			sprintf(TEXT, "%s", comRbuf);
			print_lcd(1, TEXT);
			// compare the comRbytes to text "led"
			if (strcmp(TEXT, "led") == 0)
			{
				DrvGPIO_SetBit(E_GPA, 12); // GPA12 pin output Low to turn on Blue  LED
				DrvGPIO_SetBit(E_GPA, 13); // GPA13 pin output Low to turn on Green LED
				DrvGPIO_SetBit(E_GPA, 14); // GPA14 pin output Low to turn on Red   LED
				DrvGPIO_ClrBit(E_GPC, 12);
				DrvGPIO_ClrBit(E_GPC, 13);
				DrvGPIO_ClrBit(E_GPC, 14);
				DrvGPIO_ClrBit(E_GPC, 15);
				clr_all_panel();
				sprintf(TEXT, "%s", comRbuf);
				print_lcd(1, TEXT);
				DrvUART_Write(UART_PORT0, "LED", 3);
			};
			// if the handler send "rgb" it will turn on the RGB LED
			if (strcmp(TEXT, "rgb") == 0)
			{
				DrvGPIO_SetBit(E_GPC, 12);
				DrvGPIO_SetBit(E_GPC, 13);
				DrvGPIO_SetBit(E_GPC, 14);
				DrvGPIO_SetBit(E_GPC, 15);
				// add delay betwwen the rgb leds and turn on and off the leds
				DrvSYS_Delay(1000000);
				DrvGPIO_ClrBit(E_GPA, 12); // GPA12 pin output Low to turn on Blue  LED
				DrvSYS_Delay(1000000);
				DrvGPIO_SetBit(E_GPA, 12); // GPA12 pin output Low to turn on Blue  LED
				DrvGPIO_ClrBit(E_GPA, 13); // GPA13 pin output Low to turn on Green LED
				DrvSYS_Delay(1000000);
				DrvGPIO_SetBit(E_GPA, 13); // GPA12 pin output Low to turn on Blue  LED
				DrvGPIO_ClrBit(E_GPA, 14); // GPA14 pin output Low to turn on Red   LED
				DrvSYS_Delay(1000000);
				clr_all_panel();
				sprintf(TEXT, "%s", comRbuf);
				print_lcd(1, TEXT);
				DrvUART_Write(UART_PORT0, "RGB", 3);
			};
			// if the handler send "prn" it will print the message "Haim Ozer" on the LCD
			if (strcmp(TEXT, "prn") == 0)
			{
				DrvGPIO_SetBit(E_GPC, 12);
				DrvGPIO_SetBit(E_GPC, 13);
				DrvGPIO_SetBit(E_GPC, 14);
				DrvGPIO_SetBit(E_GPC, 15);
				DrvGPIO_SetBit(E_GPA, 12); // GPA12 pin output Low to turn on Blue  LED
				DrvGPIO_SetBit(E_GPA, 13); // GPA13 pin output Low to turn on Green LED
				DrvGPIO_SetBit(E_GPA, 14); // GPA14 pin output Low to turn on Red   LED
				clr_all_panel();
				sprintf(TEXT, "%s", comRbuf);
				print_lcd(0, TEXT);
				print_lcd(1, "Names:");
				print_lcd(2, "Haim Ozer");
				print_lcd(3, "Shon Pazarker");
				DrvUART_Write(UART_PORT0, "PRN", 3);
			};
			// if the handler send "sml" it will print on the LCD a smile face
			if (strcmp(TEXT, "sml") == 0)
			{
				DrvGPIO_SetBit(E_GPC, 12);
				DrvGPIO_SetBit(E_GPC, 13);
				DrvGPIO_SetBit(E_GPC, 14);
				DrvGPIO_SetBit(E_GPC, 15);
				DrvGPIO_SetBit(E_GPA, 12);						  // GPA12 pin output Low to turn on Blue  LED
				DrvGPIO_SetBit(E_GPA, 13);						  // GPA13 pin output Low to turn on Green LED
				DrvGPIO_SetBit(E_GPA, 14);						  // GPA14 pin output Low to turn on Red   LED
				CircleBresenham(x_ball, y_ball, r, 1);			  // draw a circle into display buffer
				CircleBresenham(x_ball, y_ball + 5, mouth, 1);	  // draw a circle into display buffer
				CircleBresenham(x_ball - 5, y_ball - 5, eyes, 1); // draw a circle into display buffer
				CircleBresenham(x_ball + 5, y_ball - 5, eyes, 1); // draw a circle into display buffer
				draw_LCD(DisplayBuf);							  // from display buffer to LCD
				DrvUART_Write(UART_PORT0, "SML", 3);			  // adjustable delay for vision
				print_lcd(0, TEXT);
				print_lcd(3, "Its a smile...");
			};
			if (strcmp(TEXT, "lec") == 0)
			{
				DrvGPIO_SetBit(E_GPC, 12);
				DrvGPIO_SetBit(E_GPC, 13);
				DrvGPIO_SetBit(E_GPC, 14);
				DrvGPIO_SetBit(E_GPC, 15);
				DrvGPIO_SetBit(E_GPA, 12); // GPA12 pin output Low to turn on Blue  LED
				DrvGPIO_SetBit(E_GPA, 13); // GPA13 pin output Low to turn on Green LED
				DrvGPIO_SetBit(E_GPA, 14); // GPA14 pin output Low to turn on Red   LED
				clr_all_panel();
				sprintf(TEXT, "%s", comRbuf);
				print_lcd(0, TEXT);
				print_lcd(2, "Lecturer: Dr.");
				print_lcd(3, "Avihai Aharon");
				DrvUART_Write(UART_PORT0, "LEC", 3); // adjustable delay for vision
			};
			comRbytes = 0;
		}
	}
}

int32_t main()
{
	uint16_t Vx, Vy;
	uint8_t SW;
	char TEXT[16];
	const char *direction;
	STR_UART_T sParam;

	UNLOCKREG();
	DrvSYS_Open(48000000);
	// Joystick Init
	SYSCLK->PWRCON.XTL12M_EN = 1; // enable external clock (12MHz)
	SYSCLK->CLKSEL0.HCLK_S = 0;	  // select external clock (12MHz)
	LOCKREG();

	DrvGPIO_InitFunction(E_FUNC_UART0); // Set UART pins

	/* UART Setting */
	sParam.u32BaudRate = 9600;
	sParam.u8cDataBits = DRVUART_DATABITS_8;
	sParam.u8cStopBits = DRVUART_STOPBITS_1;
	sParam.u8cParity = DRVUART_PARITY_NONE;
	sParam.u8cRxTriggerLevel = DRVUART_FIFO_1BYTES;

	/* Set UART Configuration */
	if (DrvUART_Open(UART_PORT0, &sParam) != E_SUCCESS)
		;
	DrvUART_EnableInt(UART_PORT0, DRVUART_RDAINT, UART_INT_HANDLE);
	// Joystick init
	DrvADC_Open(ADC_SINGLE_END, ADC_SINGLE_CYCLE_OP, 0x03, INTERNAL_HCLK, 1); // ADC1 & ADC0
	DrvGPIO_Open(E_GPB, 0, E_IO_INPUT);

	Initial_panel(); // initialize LCD panel
	clr_all_panel(); // clear LCD panel
	print_lcd(1, "Shon");

	while (1)
	{
		DrvADC_StartConvert(); // start A/D conversion
		while (DrvADC_IsConversionDone() == FALSE)
			; // wait till conversion is done

		SW = DrvGPIO_GetBit(E_GPB, 0);
		Vx = ADC->ADDR[0].RSLT & 0xFFF;
		Vy = ADC->ADDR[1].RSLT & 0xFFF;
		direction = getDirection(Vx, Vy);
		// Compare with previous direction
		sprintf(TEXT, " %s", direction); // print ADC0 value into text
		if (strcmp(direction, prevDirection) != 0)
		{
			DrvUART_Write(UART_PORT0, TEXT, 7); // adjustable delay for vision
			strcpy(prevDirection, direction);	// Update previous direction
		}

		print_lcd(0, TEXT);
		sprintf(TEXT, "Vx: %4d", Vx); // print ADC0 value into text
		print_lcd(1, TEXT);
		sprintf(TEXT, "Vy: %4d", Vy); // print ADC1 value into text
		print_lcd(2, TEXT);
		sprintf(TEXT, "SW: %4d", SW); // print switch input
		print_lcd(3, TEXT);
		DrvSYS_Delay(100000);
	}
}
