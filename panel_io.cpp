#include "panel_io.hpp"

bool is_gpio_input = false;

void init_gpio() {
    gpio_init(ADDR0_PIN);
    gpio_set_dir(ADDR0_PIN, GPIO_OUT);

    gpio_init(ADDR1_PIN);
    gpio_set_dir(ADDR1_PIN, GPIO_OUT);

    gpio_init(ADDR2_PIN);
    gpio_set_dir(ADDR2_PIN, GPIO_OUT);

    gpio_init(ADDR3_PIN);
    gpio_set_dir(ADDR3_PIN, GPIO_OUT);

    // data
    gpio_init(DATA0_PIN);
    gpio_set_dir(DATA0_PIN, GPIO_OUT);

    gpio_init(DATA1_PIN);
    gpio_set_dir(DATA1_PIN, GPIO_OUT);

    gpio_init(DATA2_PIN);
    gpio_set_dir(DATA2_PIN, GPIO_OUT);

    gpio_init(DATA3_PIN);
    gpio_set_dir(DATA3_PIN, GPIO_OUT);

    gpio_init(DATA4_PIN);
    gpio_set_dir(DATA4_PIN, GPIO_OUT);

    gpio_init(DATA5_PIN);
    gpio_set_dir(DATA5_PIN, GPIO_OUT);

    gpio_init(DATA6_PIN);
    gpio_set_dir(DATA6_PIN, GPIO_OUT);

    gpio_init(DATA7_PIN);
    gpio_set_dir(DATA7_PIN, GPIO_OUT);

    gpio_init(CLK_PIN);
    gpio_set_dir(CLK_PIN, GPIO_OUT);
}



void set_gpio_input_mode(bool set) {
    if (set && !is_gpio_input) {
        gpio_set_dir(DATA0_PIN, GPIO_IN);
        gpio_set_dir(DATA1_PIN, GPIO_IN);
        gpio_set_dir(DATA2_PIN, GPIO_IN);
        gpio_set_dir(DATA3_PIN, GPIO_IN);
        gpio_set_dir(DATA4_PIN, GPIO_IN);
        gpio_set_dir(DATA5_PIN, GPIO_IN);
        gpio_set_dir(DATA6_PIN, GPIO_IN);
        gpio_set_dir(DATA7_PIN, GPIO_IN);
        is_gpio_input = true;
    } else if (!set && is_gpio_input) {
        gpio_set_dir(DATA0_PIN, GPIO_OUT);
        gpio_set_dir(DATA1_PIN, GPIO_OUT);
        gpio_set_dir(DATA2_PIN, GPIO_OUT);
        gpio_set_dir(DATA3_PIN, GPIO_OUT);
        gpio_set_dir(DATA4_PIN, GPIO_OUT);
        gpio_set_dir(DATA5_PIN, GPIO_OUT);
        gpio_set_dir(DATA6_PIN, GPIO_OUT);
        gpio_set_dir(DATA7_PIN, GPIO_OUT);
        is_gpio_input = false;
    }
}
void set_address(unsigned char address) {
    gpio_put(ADDR0_PIN,(address & (1 << 0)) != 0);
    gpio_put(ADDR1_PIN,(address & (1 << 1)) != 0);
    gpio_put(ADDR2_PIN,(address & (1 << 2)) != 0);
    gpio_put(ADDR3_PIN,(address & (1 << 3)) != 0);
}

void set_data(unsigned char data) {
    set_gpio_input_mode(false);
    gpio_put(DATA0_PIN,(data & (1 << 0)) != 0);
    gpio_put(DATA1_PIN,(data & (1 << 1)) != 0);
    gpio_put(DATA2_PIN,(data & (1 << 2)) != 0);
    gpio_put(DATA3_PIN,(data & (1 << 3)) != 0);
    gpio_put(DATA4_PIN,(data & (1 << 4)) != 0);
    gpio_put(DATA5_PIN,(data & (1 << 5)) != 0);
    gpio_put(DATA6_PIN,(data & (1 << 6)) != 0);
    gpio_put(DATA7_PIN,(data & (1 << 7)) != 0);
}

unsigned char get_data() {
    set_gpio_input_mode(true);
    unsigned char data = 0;
    data |= (gpio_get(DATA0_PIN) << 0);
    data |= (gpio_get(DATA1_PIN) << 1);
    data |= (gpio_get(DATA2_PIN) << 2);
    data |= (gpio_get(DATA3_PIN) << 3);
    data |= (gpio_get(DATA4_PIN) << 4);
    data |= (gpio_get(DATA5_PIN) << 5);
    data |= (gpio_get(DATA6_PIN) << 6);
    data |= (gpio_get(DATA7_PIN) << 7);

    return data;
}
unsigned char get_input(unsigned char address) {
    set_address(address);
    return get_data();

    // return 0x0f;

}

void set_output(unsigned char address, unsigned char data) {
    set_address(address);
    gpio_put(CLK_PIN, 0);
    set_data(data);
    sleep_us(1);
    gpio_put(CLK_PIN, 1);

}

void clear_all() {
    set_output(8, 0);
    set_output(9, 0);
    set_output(10, 0);
    set_output(11, 0);
    set_output(12, 0);
    set_output(13, 0);
    set_output(14, 0);
    set_output(15, 0);
}
