#include <agile_modbus.h>
#include <agile_modbus_slave_util.h>
#include <string.h>

static uint16_t _tab_input_registers[10] = {0, 1, 2, 3, 4, 9, 8, 7, 6, 5};

static int get_map_buf(void *buf, int bufsz)
{
    uint16_t *ptr = (uint16_t *)buf;

    for (int i = 0; i < sizeof(_tab_input_registers) / sizeof(_tab_input_registers[0]); i++) {
        ptr[i] = _tab_input_registers[i];
    }

    return 0;
}

const agile_modbus_slave_util_map_t input_register_maps[1] = {
    {0xFFF6, 0xFFFF, get_map_buf, NULL}};
