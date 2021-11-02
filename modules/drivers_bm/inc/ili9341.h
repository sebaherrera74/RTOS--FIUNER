/*! @file ili9341.h
 * @brief  TFT LCD RGB 240x320 ILI9341 driver
 *
 * This driver provide functions to configure and control a 240x320 pixels
 * TFT color display connected to the EDU-CIAA NXP.
 * It uses a SPI port and 3 GPIOs to communicate with the ILI9341 LCD driver chip.
 *
 * @author Albano Peñalva
 *
 * @note Hardware connections:
 *
 * |   	Display		|   EDU-CIAA	|
 * |:--------------:|:--------------|
 * | 	SDO/MISO 	|	SPI_MISO	|
 * | 	LED		 	| 	3V3			|
 * | 	SCK		 	| 	SPI_SCK		|
 * | 	SDI/MOSI 	| 	SPI_MOSI	|
 * | 	DC/RS	 	| 	GPIOx		|
 * | 	RESET	 	| 	GPIOx		|
 * | 	CS		 	| 	GPIOx		|
 * | 	GND		 	| 	GND			|
 * | 	VCC		 	| 	3V3			|
 *
 * @section changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 21/11/2018 | Document creation		                         |
 *
 */

#ifndef ILI9341_H_
#define ILI9341_H_

#include <stdint.h>
#include "fonts.h"

/*****************************************************************************
 * Public macros/types/enumerations/variables definitions
 ****************************************************************************/

/* LCD settings */
#define ILI9341_WIDTH       240			/*!< LCD width in pixels */
#define ILI9341_HEIGHT      320			/*!< LCD height in pixels */
#define ILI9341_PIXEL_MAX	76800

/* Colors */							/*	 R,   G,   B */
#define ILI9341_BLACK          	0x0000  /*   0,   0,   0 */
#define ILI9341_NAVY           	0x000F 	/*   0,   0, 128 */
#define ILI9341_DARKGREEN      	0x03E0  /*   0, 128,   0 */
#define ILI9341_DARKCYAN       	0x03EF  /*   0, 128, 128 */
#define ILI9341_MAROON         	0x7800  /* 128,   0,   0 */
#define ILI9341_PURPLE         	0x780F  /* 128,   0, 128 */
#define ILI9341_OLIVE         	0x7BE0  /* 128, 128,   0 */
#define ILI9341_LIGHTGREY      	0xC618  /* 192, 192, 192 */
#define ILI9341_DARKGREY       	0x7BEF  /* 128, 128, 128 */
#define ILI9341_BLUE           	0x001F  /*   0,   0, 255 */
#define ILI9341_BLUE2			0x051D
#define ILI9341_GREEN          	0x07E0  /*   0, 255,   0 */
#define ILI9341_GREEN2			0xB723
#define ILI9341_CYAN           	0x07FF  /*   0, 255, 255 */
#define ILI9341_RED            	0xF800  /* 255,   0,   0 */
#define ILI9341_MAGENTA        	0xF81F  /* 255,   0, 255 */
#define ILI9341_YELLOW         	0xFFE0  /* 255, 255,   0 */
#define ILI9341_WHITE          	0xFFFF  /* 255, 255, 255 */
#define ILI9341_ORANGE			0xFBE4
#define ILI9341_ORANGE2        	0xFD20  /* 255, 165,   0 */
#define ILI9341_GREENYELLOW    	0xAFE5  /* 173, 255,  47 */
#define ILI9341_PINK           	0xF81F
#define ILI9341_BROWN			0xBBCA

/**
 * @brief  Possible orientations for LCD
 */
typedef enum
{
	ILI9341_Portrait_1, 	/*!< Portrait orientation mode 1 */
	ILI9341_Portrait_2,  	/*!< Portrait orientation mode 2 */
	ILI9341_Landscape_1, 	/*!< Landscape orientation mode 1 */
	ILI9341_Landscape_2  	/*!< Landscape orientation mode 2 */
} ili9341_orientation_t;

/*****************************************************************************
 * Public functions definitions
 ****************************************************************************/

/**
 * @brief  		Initializes ILI9341 LCD
 * @param[in]  	spi_port: Number of SPI port connected to LCD driver
 * @param[in]  	gpio_cs: Number of GPIO pin to use as chip select
 * @param[in]  	gpio_dc: Number of GPIO pin to use as data/command
 * @param[in]  	gpio_rst: Number of GPIO pin to use as hardware reset
 * @retval 		1 when success, 0 when fails
 */
uint8_t ILI9341Init(uint8_t spi_port, uint8_t gpio_cs, uint8_t gpio_dc, uint8_t gpio_rst);

/**
 * @brief  		Draws single pixel to LCD
 * @param[in]  	x: X position for pixel
 * @param[in]  	y: Y position for pixel
 * @param[in]  	color: Color of pixel
 * @retval 		None
 */
void ILI9341DrawPixel(uint16_t x, uint16_t y, uint16_t color);

/**
 * @brief  		Fills entire LCD with color
 * @param[in]	color: Color to be used in fill
 * @retval 		None
 */
void ILI9341Fill(uint16_t color);

/**
 * @brief  		Rotates LCD to specific orientation
 * @param[in]	orientation: LCD orientation
 * @retval 		None
 */
void ILI9341Rotate(ili9341_orientation_t orientation);

/**
 * @brief  		Draw a single character on the LCD
 * @param[in]  	x: X position of top left corner
 * @param[in]  	y: Y position of top left corner
 * @param[in] 	c: Character to be displayed
 * @param[in]  	font: Pointer to used font
 * @param[in]  	foreground: Color for char
 * @param[in]  	background: Color for char background
 * @retval		None
 */
void ILI9341DrawChar(uint16_t x, uint16_t y, char data, Font_t* font, uint16_t foreground, uint16_t background);

/**
 * @brief  		Draw a string on the LCD
 * @param[in] 	x: X position of top left corner of first character in string
 * @param[in]  	y: Y position of top left corner of first character in string
 * @param[in]  	str: Pointer to first character
 * @param[in]  	font: Pointer to used font
 * @param[in]  	foreground: Color for string
 * @param[in]  	background: Color for string background
 * @retval 		None
 */
void ILI9341DrawString(uint16_t x, uint16_t y, char* str, Font_t *font, uint16_t foreground, uint16_t background);

/**
 * @brief  		Gets width and height of box with text
 * @param[in]  	str: Pointer to first character
 * @param[in] 	font: Pointer to used font
 * @param[out]	width: Pointer to variable to store width
 * @param[out]	height: Pointer to variable to store height
 * @retval 		None
 */
void ILI9341GetStringSize(char* str, Font_t* font, uint16_t* width, uint16_t* height);

/**
 * @brief  		Draws line on the LCD
 * @param[in]  	x0: X coordinate of starting point
 * @param[in]  	y0: Y coordinate of starting point
 * @param[in]  	x1: X coordinate of ending point
 * @param[in]  	y1: Y coordinate of ending point
 * @param[in]  	color: Line color
 * @retval[in] 	None
 */
void ILI9341DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

/**
 * @brief  		Draws rectangle on the LCD
 * @param[in]  	x0: X coordinate of top left point
 * @param[in]  	y0: Y coordinate of top left point
 * @param[in]  	x1: X coordinate of bottom right point
 * @param[in]  	y1: Y coordinate of bottom right point
 * @param[in]  	color: Rectangle color
 * @retval 		None
 */
void ILI9341DrawRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

/**
 * @brief  		Draws filled rectangle on the LCD
 * @param[in]  	x0: X coordinate of top left point
 * @param[in]  	y0: Y coordinate of top left point
 * @param[in]  	x1: X coordinate of bottom right point
 * @param[in]  	y1: Y coordinate of bottom right point
 * @param[in]  	color: Rectangle color
 * @retval 		None
 */
void ILI9341DrawFilledRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

/**
 * @brief  		Draws circle on the LCD
 * @param[in]  	x0: X coordinate of center circle point
 * @param[in]  	y0: Y coordinate of center circle point
 * @param[in]  	r: Circle radius
 * @param[in]  	color: Circle color
 * @retval 		None
 */
void ILI9341DrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

/**
 * @brief  		Draws filled circle on the LCD
 * @param[in]  	x0: X coordinate of center circle point
 * @param[in]  	y0: Y coordinate of center circle point
 * @param[in]  	r: Circle radius
 * @param[in]  	color: Circle color
 * @retval 		None
 */
void ILI9341DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

/**
 * @brief  		Draw a picture on the LCD
 * @param[in] 	x: X position of top left corner of picture
 * @param[in]  	y: Y position of top left corner of picture
 * @param[in] 	width: Picture width in pixels
 * @param[in]  	height: Picture height in pixels
 * @param[in]  	pic: Pointer to first byte of picture
 * @retval 		None
 */
void ILI9341DrawPicture(uint16_t x, uint16_t y, uint16_t width, uint16_t hieght, const uint8_t* pic);

/**
 * @brief  	De-initializes ILI9341 LCD
 * @param	None
 * @retval 	1 when success, 0 when fails
 */
uint8_t ILI9341DeInit(void);

#endif /* ILI9341_H_ */
