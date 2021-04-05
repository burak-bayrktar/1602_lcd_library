
#include <stdio.h>
#include <string.h>
#include <libopencm3/stm32/i2c.h>
#include <1602_i2c.h>

/*
    in this implementation leftmost 4 bits needs to be send first, according to datasheeet.
    the point needs to be  considered is a data is send in a sequence like below
            |d7|d6|d5|d4|b'|e|rw|rs|   (Send sequence )
    but the data we have is 
            |d7|d6|d5|d4|d3|d2|d1|d0   (Data Sequence)
    so thing to do is  d7,d6,d5,d4 goes first inside the d7,d6,d5,d4 of send sequence,
    then our rightmost 4 bits needs to be shifted left to be send after first one.
    the |b'|e|rw|rs| part will be added to the data after shift is performed

    the reason to send the 2 nibble(1 byte) of data with an array of 4 bytes is:
    you can see in the datasheet that there is an enable pin involved while sending data.
    timing diagram clearly shows that enable pin needs to be high before  data sequence 
    starts (min=100ns) on i2c line and then be pulled low before data sending sequence ends.
     so the first byte goes with enable high , and rigth after that, the byte with enable low
     sends and voila! it happened.
*/

void lcd_send_char(uint8_t character, uint32_t I2C){
    uint8_t dataToSend[4];
    uint8_t leftMostBits=(character & 0xF0);
    uint8_t rightMostBits=(character << 4);
    
    dataToSend[0]=(leftMostBits | 0x0D); //with enable
    dataToSend[1]=(leftMostBits | 0x09); //without enable
    dataToSend[2]=(rightMostBits | 0x0D);
    dataToSend[3]=(rightMostBits | 0x09);
    i2c_send_start(I2C);
    i2c_transfer7(I2C, LCD_ADDR, dataToSend, sizeof(dataToSend), NULL, 0);
    i2c_send_stop(I2C);
}

void lcd_send_cmd(uint8_t cmd,uint32_t I2C){
    uint8_t dataToSend[4];
    uint8_t leftMostBits=(cmd & 0xF0);
    uint8_t rightMostBits=(cmd << 4);

    dataToSend[0]=(leftMostBits | 0x0C); //with enable
    dataToSend[1]=(leftMostBits | 0x08); //without enable
    dataToSend[2]=(rightMostBits | 0x0C);
    dataToSend[3]=(rightMostBits | 0x08);
    i2c_send_start(I2C);
    i2c_transfer7(I2C, LCD_ADDR, dataToSend, sizeof(dataToSend), NULL, 0);
    i2c_send_stop(I2C);
}

void lcd_init(uint32_t I2C){
    //there is plenty of information about initialization process and delays
    //however my lcd works like this. and i am planning to use a proper delay
    //method for this part.
    //it actually only needed for initializetions first 4 commands, later 
    //one can check busy flag before sending anything(if i would ever get to write busy flag checking things, i guess)
	lcd_send_cmd(0x03,I2C);
    delay();
	lcd_send_cmd(0x03,I2C);
    delay();
	lcd_send_cmd(0x03,I2C);
    delay();
	lcd_send_cmd(0x02,I2C);
    delay();
    lcd_send_cmd(LCD_CLEAR_DISPLAY,I2C);
    lcd_send_cmd(FUNCTION_SET_REG|FUNTCION_SET_2LINE,I2C);
    lcd_send_cmd(LCD_DISPLAY_REG|LCD_DISPLAY_ON,I2C);
    lcd_send_cmd(LCD_ENTRY_MODE_SET_INC,I2C);
    //wierd enough, below code has been send before but cursor still blinks.
    //second time sending the command to set display on, cursor and blink off, does the job
    lcd_send_cmd(LCD_DISPLAY_REG|LCD_DISPLAY_ON,I2C);
}


void lcd_write(char *str,uint32_t I2C){
    uint32_t i;
    //send string literature 1 one letter per time 
    for (i = 0; i < strlen(str); i++)
    {
        lcd_send_char(str[i],I2C);
    }   
}

void lcd_cursor_pos(uint8_t row, uint8_t column, uint32_t I2C){
    switch (row){
    case 1:
        column |= LCD_POS_IN_FIRST_ROW;
        break;
    case 2:
        column |= LCD_POS_IN_SECOND_ROW;
        break;
    default :
        break;
    }
    lcd_send_cmd(column,I2C);
}

void lcd_clear(uint32_t I2C){
    lcd_send_cmd(LCD_POS_IN_FIRST_ROW,I2C);
    uint8_t i;
    for(i=0;i<32;i++){
        lcd_send_char(' ',I2C);
        if(i==15){
            lcd_send_cmd(LCD_POS_IN_SECOND_ROW,I2C);
        }
    }
    lcd_send_cmd(LCD_RETURN_HOME,I2C);
    delay();
    lcd_send_cmd(LCD_ENTRY_MODE_SET_INC, I2C);
}

//this func. defined because there would be a need for a func. to not to delay properly
void delay(void){
    for (size_t i = 0; i < 100; i++){
        __asm__("nop");
    }
}