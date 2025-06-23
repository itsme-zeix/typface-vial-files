#pragma once

#include "quantum.h"
#include "via.h"

#define LED_CONFIG_EEPROM_ADDR (EECONFIG_SIZE + 32) 

enum via_led_value {
    id_led_brightness = 1,
    id_led1_color     = 2,
    id_led2_color     = 3,
    id_led3_color     = 4
};

typedef struct {
    uint8_t brightness;
    uint8_t led1[2];
    uint8_t led2[2];
    uint8_t led3[2];
} led_config;

// Function declarations
void led_config_set_value( uint8_t *data );
void led_config_get_value( uint8_t *data );
void led_config_save(void);
void led_config_load(void);
