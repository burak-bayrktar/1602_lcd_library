#include <stdio.h>
/*
    Pin Connection of PCF8574A to LCD1602

    | D7 |D6 |D5 |D4 |BL |EN |R/W|RS |
    | P7 |P6 |P5 |P4 |P3 |P2 |P1 |P0 |

	In order to send data correctly, the data needs to be flipped due to system sending data lsb first.
	example of setting rw pin high 0b00000010 needs to be send, which results:

	|0 |0 |0 |0 |0 |0|1 |0 |
	|d7|d6|d5|d4|b'|e|rw|rs|
	
	however, address definitions written in 8bit length considering the datasheet definitions.
	correct sending sequence is handled in functions respectively.

	addresses will be treated as below bit sequence
	|d7|d6|d5|d4|d3|d2|d1|d0
*/

#define LCD_ADDR 0x27
//general defines
#define LCD_CLEAR_DISPLAY 		0x01
#define LCD_RETURN_HOME			0x02
#define LCD_POS_IN_FIRST_ROW    0x80	//used while setting cursor position in first row
#define LCD_POS_IN_SECOND_ROW   0xC0	//used while setting cursor position in second row

#define LCD_BACKLIGHT           0x08
#define LCD_ENABLE              0x04
#define LCD_WRITE          		0x02
#define LCD_REGISTER_SELECT     0x01
/*
	Entry Mode Set 
	|0 |0 |0 |0 |0 |1 |i/d|S |  
	|d7|d6|d5|d4|d3|d2|d1 |d0|
	(shift operations are for shifting entire display)
*/
#define LCD_ENTRY_MODE_SET_INC  	0x06  // S=0, ID=1 cursor moves right
#define LCD_ENTRY_MODE_SET_DEC  	0x04  // S=0, ID=0 cursor moves left
#define LCD_ENTRY_MODE_SET_SHIFT_R  0x05  // S=1, ID=0 shifts right
#define LCD_ENTRY_MODE_SET_SHIFT_L  0x07  // S=1, ID=1 shifts left

/*
	Display ON/OFF
	|0 |0 |0 |0 |1 |D |C |B |  
	|d7|d6|d5|d4|d3|d2|d1|d0|

	LCD_DISPLAY_REG default values;
		Display=0, Cursor=0, Blink=0
*/
#define LCD_DISPLAY_REG             0x08	
#define LCD_CURSOR_BLINK_ON 		0x01	
#define LCD_CURSOR_ON			 	0x02	
#define LCD_DISPLAY_ON              0X04	

/*
	Function Set 
	|0 |0 |1 |DL|N |F |x |x |  
	|d7|d6|d5|d4|d3|d2|d1|d0
	FUNCTION_SET_REG default values: 
		DL=0(4bit mode) ,N=0(1 line display mode) F=0(5x8 dots font format)
*/
#define FUNCTION_SET_REG			0x20 
#define FUNTCION_SET_8BIT			0x10	//DL=1
#define FUNTCION_SET_2LINE			0x08 	//N=1
#define FUNCTION_SET_5X11DOT		0x04 	//F=1

/*
	busy flag
	after RS set low and R/W set high, d7 pin would be high if there is an internal process 
	present, low if its available to send another command and/or character.
	|BF|A6|A5|A4|A3|A2|A1|A0|  
	|d7|d6|d5|d4|d3|d2|d1|d0|
*/
#define READ_BUSY_FLAG				0x40 // setting rs=0, rw=1 this 
#define IS_BUSY						0x80 // this is for checking if the returned value's msbit is set(busy) or not(not busy)

//function prototypes


//parameters defined as uint32_t are peripherial of choice (etiher I2C1 or I2C2 for bluepill)
//this is implemented as there would be multiple i2c devices.

//sends a single char
void lcd_send_char(uint8_t,uint32_t);

//for sending command
void lcd_send_cmd(uint8_t,uint32_t);

//can be use to write a string literal (eg. char str[]="string literal")
void lcd_write(char *, uint32_t);

//to initialize lcd
void lcd_init(uint32_t);

// positioning the cursor first parameters are(row,column, I2C periph)
// row can be either 1 or 2
void lcd_cursor_pos(uint8_t, uint8_t, uint32_t);

// clears lcd and returns the cursor home;
void lcd_clear(uint32_t);
//a really humble and unnecessary delay function that is not even able to delay properly
void delay(void);






