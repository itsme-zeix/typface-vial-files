#include "quantum.h"
#include "eeprom.h"
#include "rgblight.h"
#include "via.h"
#include "925.h"

led_config g_led_config = {
    .brightness = 150,
    .led1 = {255, 55}, // hue=255, sat=55
    .led2 = {255, 55},
    .led3 = {255, 55}
};

void via_custom_value_command_kb(uint8_t *data, uint8_t length) 
{
    // data = [command_id, channel_id, value_id, value_data]
    uint8_t *command_id        = &(data[0]);
    uint8_t *channel_id        = &(data[1]);
    uint8_t *value_id_and_data = &(data[2]);

    if ( *channel_id == id_custom_channel )
    {
        switch ( *command_id )
        {
            case id_custom_set_value:
            {
                led_config_set_value(value_id_and_data);
                break;
            }
            case id_custom_get_value:
            {
                led_config_get_value(value_id_and_data);
                break;
            }
            case id_custom_save:
            {
                led_config_save();
                break;
            }
            default:
            {
                // Unhandled message.
                *command_id = id_unhandled;
                break;
            }
        }
        return;
    }

    // Return the unhandled state
    *command_id = id_unhandled;

    // DO NOT call raw_hid_send(data,length) here, let caller do this
}

void led_config_set_value( uint8_t *data )
{
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch ( *value_id )
    {
        case id_led_brightness:
        {
            g_led_config.brightness = value_data[0];
            // Nasty code to loop through the 3 LEDs and update them individually.
            for (uint8_t i = 0; i < 3; i++) 
            {
                uint8_t hue = (i == 0)? g_led_config.led1[0]
                            : (i == 1)? g_led_config.led2[0]
                                    : g_led_config.led3[0];
                uint8_t sat = (i == 0)? g_led_config.led1[1]
                            : (i == 1)? g_led_config.led2[1]
                                    : g_led_config.led3[1];

                uint8_t val = ((uint16_t)g_led_config.brightness * RGBLIGHT_LIMIT_VAL) / UINT8_MAX;
                rgblight_sethsv_at(hue, sat, val, i);
            }
            break;
        }
        case id_led1_color:
        {
            g_led_config.led1[0] = value_data[0];
            g_led_config.led1[1] = value_data[1];
            rgblight_sethsv_at(value_data[0], value_data[1], rgblight_get_val(), 0);
            break;
        }
        case id_led2_color:
        {
            g_led_config.led2[0] = value_data[0];
            g_led_config.led2[1] = value_data[1];
            rgblight_sethsv_at(value_data[0], value_data[1], rgblight_get_val(), 1);
            break;
        }
        case id_led3_color:
        {
            g_led_config.led3[0] = value_data[0];
            g_led_config.led3[1] = value_data[1];
            rgblight_sethsv_at(value_data[0], value_data[1], rgblight_get_val(), 2);
            break;
        }
    }
}

void led_config_get_value( uint8_t *data )
{
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch ( *value_id )
    {
        case id_led_brightness:
        {
            value_data[0] = g_led_config.brightness;
            break;
        }
        case id_led1_color:
        {
            value_data[0] = g_led_config.led1[0];
            value_data[1] = g_led_config.led1[1];
            break;
        }
        case id_led2_color:
        {
            value_data[0] = g_led_config.led2[0];
            value_data[1] = g_led_config.led2[1];
            break;
        }
        case id_led3_color:
        {
            value_data[0] = g_led_config.led3[0];
            value_data[1] = g_led_config.led3[1];
            break;
        }
    }
}

void via_init_kb(void)
{
    // If the EEPROM has the magic, the data is good.
    // OK to load from EEPROM
    if (via_eeprom_is_valid()) {
        led_config_load();
    } else {
        led_config_save();
        // DO NOT set EEPROM valid here, let caller do this
    }
}

static void reapply_led_config(void) {
    for (uint8_t i = 0; i < 3; i++) {
        uint8_t hue, sat;
        if (i == 0) {
            hue = g_led_config.led1[0];
            sat = g_led_config.led1[1];
        } else if (i == 1) {
            hue = g_led_config.led2[0];
            sat = g_led_config.led2[1];
        } else {
            hue = g_led_config.led3[0];
            sat = g_led_config.led3[1];
        }
        uint8_t val = ((uint16_t)g_led_config.brightness * RGBLIGHT_LIMIT_VAL) / UINT8_MAX;
        rgblight_sethsv_at(hue, sat, val, i);
    }
}

void led_config_save(void) 
{
    eeprom_update_block( &g_led_config, ((void*)LED_CONFIG_EEPROM_ADDR), sizeof(led_config) );
    reapply_led_config();
}


void led_config_load(void) 
{
    eeprom_read_block( &g_led_config, ((void*)LED_CONFIG_EEPROM_ADDR), sizeof(led_config) );
    reapply_led_config();
}

void keyboard_post_init_user(void) {
    via_init_kb();
}
