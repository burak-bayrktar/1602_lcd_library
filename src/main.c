#include <errno.h>
#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/gpio.h>
#include <1602_i2c.h>


static void gpio_setup(void){
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,GPIO13);
}

static void clock_setup(void){
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_set_ppre1(RCC_CFGR_PPRE1_HCLK_DIV2);
}

static void i2c_setup(void){
    rcc_periph_clock_enable(RCC_I2C1);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_AFIO);
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN, (GPIO_I2C1_SCL | GPIO_I2C1_SDA));
    i2c_peripheral_disable(I2C1);
    i2c_set_clock_frequency(I2C1, I2C_CR2_FREQ_36MHZ);
    i2c_set_standard_mode(I2C1);
    i2c_set_ccr(I2C1, 0xB3); 
    i2c_set_trise(I2C1, 0x24);
    i2c_peripheral_enable(I2C1);
}

int main(void){

    clock_setup();
    gpio_setup();
    i2c_setup();
    char str[]="Burak Bayraktar";
    lcd_init(I2C1);
    lcd_write(str, I2C1);
    lcd_cursor_pos(2, 2, I2C1);
    lcd_write(str, I2C1);

    while (1) {
        gpio_toggle(GPIOC, GPIO13);
        for (size_t i = 0; i < 900000; i++){
            __asm__("nop");
        }
    }
    return 0;
}