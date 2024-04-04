#pragma once
#include <stdio.h>
#include "pico/stdlib.h"

#define ADDR0_PIN 10
#define ADDR1_PIN 11
#define ADDR2_PIN 12
#define ADDR3_PIN 13

#define DATA0_PIN 14
#define DATA1_PIN 15
#define DATA2_PIN 16
#define DATA3_PIN 17
#define DATA4_PIN 18
#define DATA5_PIN 19
#define DATA6_PIN 20
#define DATA7_PIN 21

#define DEBUG_INPUT 5
#define DEBUG_OUTPUT 15
#define CLK_PIN 28



void init_gpio();

void set_gpio_input_mode(bool set);

void set_address(unsigned char address);

void set_data(unsigned char data);

unsigned char get_data();

unsigned char get_input(unsigned char address);

void set_output(unsigned char address, unsigned char data);

void clear_all();
