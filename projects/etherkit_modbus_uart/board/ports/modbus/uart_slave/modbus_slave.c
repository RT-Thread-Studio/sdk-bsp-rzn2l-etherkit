
#include <agile_modbus.h>
#include <agile_modbus_slave_util.h>
#include "rs485.h"

#define DBG_TAG "rtu_master"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

static agile_modbus_rtu_t ctx_rtu;
static agile_modbus_t *ctx = &ctx_rtu._ctx;

#define TAB_MAX_NUM 10
static uint8_t _tab_bits[TAB_MAX_NUM] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
static uint8_t _tab_input_bits[TAB_MAX_NUM] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
static uint16_t _tab_registers[TAB_MAX_NUM] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
static uint16_t _tab_input_registers[TAB_MAX_NUM] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

extern const agile_modbus_slave_util_map_t bit_maps[1];
extern const agile_modbus_slave_util_map_t input_bit_maps[1];
extern const agile_modbus_slave_util_map_t register_maps[1];
extern const agile_modbus_slave_util_map_t input_register_maps[1];

static int addr_check(agile_modbus_t *ctx, struct agile_modbus_slave_info *slave_info)
{
    int slave = slave_info->sft->slave;
    if ((slave != ctx->slave) && (slave != AGILE_MODBUS_BROADCAST_ADDRESS) && (slave != 0xFF))
        return -AGILE_MODBUS_EXCEPTION_UNKNOW;

    return 0;
}

static const agile_modbus_slave_util_t _slave_util = {
    bit_maps,
    sizeof(bit_maps) / sizeof(bit_maps[0]),
    input_bit_maps,
    sizeof(input_bit_maps) / sizeof(input_bit_maps[0]),
    register_maps,
    sizeof(register_maps) / sizeof(register_maps[0]),
    input_register_maps,
    sizeof(input_register_maps) / sizeof(input_register_maps[0]),
    addr_check,
    NULL,
    NULL};


/**
 * @brief   从机回调函数
 * @param   ctx modbus 句柄
 * @param   slave_info 从机信息体
 * @return  =0:正常;
 *          <0:异常
 *             (-AGILE_MODBUS_EXCEPTION_UNKNOW(-255): 未知异常，从机不会打包响应数据)
 *             (其他负数异常码: 从机会打包异常响应数据)
 */
static int slave_callback(agile_modbus_t *ctx, struct agile_modbus_slave_info *slave_info, const void *data)
{
    int function = slave_info->sft->function;
    int ret = 0;

    switch (function)
    {
    case AGILE_MODBUS_FC_READ_COILS:
    case AGILE_MODBUS_FC_READ_DISCRETE_INPUTS:
    {
        int address = slave_info->address;
        int nb = slave_info->nb;
        int send_index = slave_info->send_index;
        int is_input = (function == AGILE_MODBUS_FC_READ_DISCRETE_INPUTS);

        for (int now_address = address, i = 0; now_address < address + nb; now_address++, i++)
        {
            if (now_address >= 0 && now_address < TAB_MAX_NUM)
            {
                int index = now_address - 0;
                agile_modbus_slave_io_set(ctx->send_buf + send_index, i,
                                          is_input ? _tab_input_bits[index] : _tab_bits[index]);
            }
        }
    }
    break;

    case AGILE_MODBUS_FC_READ_HOLDING_REGISTERS:
    case AGILE_MODBUS_FC_READ_INPUT_REGISTERS:
    {
        int address = slave_info->address;
        int nb = slave_info->nb;
        int send_index = slave_info->send_index;
        int is_input = (function == AGILE_MODBUS_FC_READ_INPUT_REGISTERS);

        for (int now_address = address, i = 0; now_address < address + nb; now_address++, i++)
        {
            if (now_address >= 0 && now_address < TAB_MAX_NUM)
            {
                int index = now_address - 0;
                agile_modbus_slave_register_set(ctx->send_buf + send_index, i,
                                                is_input ? _tab_input_registers[index] : _tab_registers[index]);
            }
        }
    }
    break;

    case AGILE_MODBUS_FC_WRITE_SINGLE_COIL:
    case AGILE_MODBUS_FC_WRITE_MULTIPLE_COILS:
    {
        int address = slave_info->address;

        if (function == AGILE_MODBUS_FC_WRITE_SINGLE_COIL)
        {
            if (address >= 0 && address < TAB_MAX_NUM)
            {
                int index = address - 0;
                int data = *((int *)slave_info->buf);
                _tab_bits[index] = data;
            }
        }
        else
        {
            int nb = slave_info->nb;
            for (int now_address = address, i = 0; now_address < address + nb; now_address++, i++)
            {
                if (now_address >= 0 && now_address < TAB_MAX_NUM)
                {
                    int index = now_address - 0;
                    uint8_t status = agile_modbus_slave_io_get(slave_info->buf, i);
                    _tab_bits[index] = status;
                }
            }
        }
    }
    break;

    case AGILE_MODBUS_FC_WRITE_SINGLE_REGISTER:
    case AGILE_MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
    {
        int address = slave_info->address;

        if (function == AGILE_MODBUS_FC_WRITE_SINGLE_REGISTER)
        {
            if (address >= 0 && address < TAB_MAX_NUM)
            {
                int index = address - 0;
                int data = *((int *)slave_info->buf);
                _tab_registers[index] = data;
            }
        }
        else
        {
            int nb = slave_info->nb;
            for (int now_address = address, i = 0; now_address < address + nb; now_address++, i++)
            {
                if (now_address >= 0 && now_address < TAB_MAX_NUM)
                {
                    int index = now_address - 0;
                    uint16_t data = agile_modbus_slave_register_get(slave_info->buf, i);
                    _tab_registers[index] = data;
                }
            }
        }
    }
    break;

    case AGILE_MODBUS_FC_MASK_WRITE_REGISTER:
    {
        int address = slave_info->address;
        if (address >= 0 && address < TAB_MAX_NUM)
        {
            int index = address - 0;
            uint16_t data = _tab_registers[index];
            uint16_t and = (slave_info->buf[0] << 8) + slave_info->buf[1];
            uint16_t or = (slave_info->buf[2] << 8) + slave_info->buf[3];

            data = (data & and) | (or &(~and));

            _tab_registers[index] = data;
        }
    }
    break;

    case AGILE_MODBUS_FC_WRITE_AND_READ_REGISTERS:
    {
        int address = slave_info->address;
        int nb = (slave_info->buf[0] << 8) + slave_info->buf[1];
        uint16_t address_write = (slave_info->buf[2] << 8) + slave_info->buf[3];
        int nb_write = (slave_info->buf[4] << 8) + slave_info->buf[5];
        int send_index = slave_info->send_index;

        /* Write first. 7 is the offset of the first values to write */
        for (int now_address = address_write, i = 0; now_address < address_write + nb_write; now_address++, i++)
        {
            if (now_address >= 0 && now_address < TAB_MAX_NUM)
            {
                int index = now_address - 0;
                uint16_t data = agile_modbus_slave_register_get(slave_info->buf + 7, i);
                _tab_registers[index] = data;
            }
        }

        /* and read the data for the response */
        for (int now_address = address, i = 0; now_address < address + nb; now_address++, i++)
        {
            if (now_address >= 0 && now_address < TAB_MAX_NUM)
            {
                int index = now_address - 0;
                agile_modbus_slave_register_set(ctx->send_buf + send_index, i, _tab_registers[index]);
            }
        }
    }
    break;

    default:
        ret = -AGILE_MODBUS_EXCEPTION_ILLEGAL_FUNCTION;
        break;
    }

    return ret;
}

static void modbus_thread_entry(void *parameter)
{
    RT_UNUSED(parameter);
    while (1)
    {
        rt_thread_mdelay(100);

        int read_len = rs485_receive(ctx->read_buf, ctx->read_bufsz, 1000, 20);
        if (read_len == 0)
            continue;

        int send_len = agile_modbus_slave_handle(ctx, read_len, 1, slave_callback,
                                           &_slave_util, NULL);
        if (send_len > 0)
        {
            rs485_send(ctx->send_buf, send_len); // 返回请求应答
        }
    }
}

int modbus_slave_uart_sample(void)
{
    rs485_init();

    uint8_t ctx_send_buf[AGILE_MODBUS_MAX_ADU_LENGTH];
    uint8_t ctx_read_buf[AGILE_MODBUS_MAX_ADU_LENGTH];

    agile_modbus_rtu_init(&ctx_rtu, ctx_send_buf, sizeof(ctx_send_buf), ctx_read_buf, sizeof(ctx_read_buf));
    agile_modbus_set_slave(ctx, 1);

    LOG_I("Running.");
    
    rt_thread_t tid;

    tid = rt_thread_create("modbus", modbus_thread_entry, RT_NULL,
                           2048, 22, 20);
    RT_ASSERT(tid != RT_NULL);
    rt_thread_startup(tid);
    
    return 0;
}
MSH_CMD_EXPORT(modbus_slave_uart_sample, modbus_slave_uart_sample)
