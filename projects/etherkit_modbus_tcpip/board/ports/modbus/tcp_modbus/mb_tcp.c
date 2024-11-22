#include "mb_tcp.h"
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <dfs_file.h>
#include <unistd.h>
#include <rthw.h>

#define DBG_TAG "mb_tcp"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#define MBTCP_SEVER_THREAD_PRI        13
#define MBTCP_SEVER_THREAD_STACK_SIZE 2048

#define MBTCP_SESSION_THREAD_PRI        12
#define MBTCP_SESSION_THREAD_STACK_SIZE 4096

#define MBTCP_SESSION_TIMEOUT 60

#define MBTCP_LISTEN_BACKLOG 6

#define TAB_MAX_NUM 10
static uint8_t _tab_bits[TAB_MAX_NUM] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
static uint8_t _tab_input_bits[TAB_MAX_NUM] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
static uint16_t _tab_registers[TAB_MAX_NUM] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
static uint16_t _tab_input_registers[TAB_MAX_NUM] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

static mbtcp_server_t _server = {0};

extern const agile_modbus_slave_util_map_t bit_maps[1];
extern const agile_modbus_slave_util_map_t input_bit_maps[1];
extern const agile_modbus_slave_util_map_t register_maps[1];
extern const agile_modbus_slave_util_map_t input_register_maps[1];

const agile_modbus_slave_util_t slave_util = {
    bit_maps,
    sizeof(bit_maps) / sizeof(bit_maps[0]),
    input_bit_maps,
    sizeof(input_bit_maps) / sizeof(input_bit_maps[0]),
    register_maps,
    sizeof(register_maps) / sizeof(register_maps[0]),
    input_register_maps,
    sizeof(input_register_maps) / sizeof(input_register_maps[0]),
    NULL,
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
static int slave_callback(agile_modbus_t *ctx, struct agile_modbus_slave_info *slave_info,const void *data)
{
    int function = slave_info->sft->function;
    int ret = 0;
    switch (function) {
    case AGILE_MODBUS_FC_READ_COILS:
    case AGILE_MODBUS_FC_READ_DISCRETE_INPUTS: {
        int address = slave_info->address;//读取开始位数
        int nb = slave_info->nb;//要读取的位数
        int send_index = slave_info->send_index;//有效数据开始字节数
        int is_input = (function == AGILE_MODBUS_FC_READ_DISCRETE_INPUTS);
        /*
        for (int now_address = address, i = 0; now_address < address + nb; now_address++, i++) {
            if (now_address >= 0 && now_address < TAB_MAX_NUM) {
                int index = now_address - 0;
                agile_modbus_slave_io_set(ctx->send_buf + send_index, i,
                                          is_input ? _tab_input_bits[index] : _tab_bits[index]);
            }
        }*/
        uint8_t *buf=is_input ? _tab_input_bits : _tab_bits;//获取位存储指针
        for (int now_address = address, i = 0; now_address < address + nb; now_address++, i++) {
            if (now_address >= 0 && now_address < TAB_MAX_NUM) {
                agile_modbus_slave_io_set(ctx->send_buf + send_index, i,
                        agile_modbus_slave_io_get(buf,now_address));//复制位存储到返回存储
            }
        }
    } break;
    case AGILE_MODBUS_FC_READ_HOLDING_REGISTERS:
    case AGILE_MODBUS_FC_READ_INPUT_REGISTERS: {
        int address = slave_info->address;
        int nb = slave_info->nb;
        int send_index = slave_info->send_index;
        int is_input = (function == AGILE_MODBUS_FC_READ_INPUT_REGISTERS);
        for (int now_address = address, i = 0; now_address < address + nb; now_address++, i++) {
            if (now_address >= 0 && now_address < TAB_MAX_NUM) {
                int index = now_address - 0;
                agile_modbus_slave_register_set(ctx->send_buf + send_index, i,
                                                is_input ? _tab_input_registers[index] : _tab_registers[index]);
            }
        }
    } break;
    case AGILE_MODBUS_FC_WRITE_SINGLE_COIL:
    case AGILE_MODBUS_FC_WRITE_MULTIPLE_COILS: {
        int address = slave_info->address;
        if (function == AGILE_MODBUS_FC_WRITE_SINGLE_COIL) {
            if (address >= 0 && address < TAB_MAX_NUM) {
                int index = address - 0;
                int data = *((int *)slave_info->buf);
                //_tab_bits[index] = data;
                agile_modbus_slave_io_set(_tab_bits,index,data);
            }
        } else {
            int nb = slave_info->nb;
            for (int now_address = address, i = 0; now_address < address + nb; now_address++, i++) {
                if (now_address >= 0 && now_address < TAB_MAX_NUM) {
                    int index = now_address - 0;
                    uint8_t status = agile_modbus_slave_io_get(slave_info->buf, i);
                    //_tab_bits[index] = status;
                    agile_modbus_slave_io_set(_tab_bits,index,status);
                }
            }
        }
    } break;
    case AGILE_MODBUS_FC_WRITE_SINGLE_REGISTER:
    case AGILE_MODBUS_FC_WRITE_MULTIPLE_REGISTERS: {
        int address = slave_info->address;
        if (function == AGILE_MODBUS_FC_WRITE_SINGLE_REGISTER) {
            if (address >= 0 && address < TAB_MAX_NUM) {
                int index = address - 0;
                int data = *((int *)slave_info->buf);
                _tab_registers[index] = data;
            }
        } else {
            int nb = slave_info->nb;
            for (int now_address = address, i = 0; now_address < address + nb; now_address++, i++) {
                if (now_address >= 0 && now_address < TAB_MAX_NUM) {
                    int index = now_address - 0;
                    uint16_t data = agile_modbus_slave_register_get(slave_info->buf, i);
                    _tab_registers[index] = data;
                }
            }
        }
    } break;
    case AGILE_MODBUS_FC_MASK_WRITE_REGISTER: {
        int address = slave_info->address;
        if (address >= 0 && address < TAB_MAX_NUM) {
            int index = address - 0;
            uint16_t data = _tab_registers[index];
            uint16_t and = (slave_info->buf[0] << 8) + slave_info->buf[1];
            uint16_t or = (slave_info->buf[2] << 8) + slave_info->buf[3];
            data = (data & and) | (or &(~and));
            _tab_registers[index] = data;
        }
    } break;
    case AGILE_MODBUS_FC_WRITE_AND_READ_REGISTERS: {
        int address = slave_info->address;
        int nb = (slave_info->buf[0] << 8) + slave_info->buf[1];
        uint16_t address_write = (slave_info->buf[2] << 8) + slave_info->buf[3];
        int nb_write = (slave_info->buf[4] << 8) + slave_info->buf[5];
        int send_index = slave_info->send_index;
        /* Write first. 7 is the offset of the first values to write */
        for (int now_address = address_write, i = 0; now_address < address_write + nb_write; now_address++, i++) {
            if (now_address >= 0 && now_address < TAB_MAX_NUM) {
                int index = now_address - 0;
                uint16_t data = agile_modbus_slave_register_get(slave_info->buf + 7, i);
                _tab_registers[index] = data;
            }
        }
        /* and read the data for the response */
        for (int now_address = address, i = 0; now_address < address + nb; now_address++, i++) {
            if (now_address >= 0 && now_address < TAB_MAX_NUM) {
                int index = now_address - 0;
                agile_modbus_slave_register_set(ctx->send_buf + send_index, i, _tab_registers[index]);
            }
        }
    } break;
    default:
        ret = -AGILE_MODBUS_EXCEPTION_ILLEGAL_FUNCTION;
        break;
    }
    return ret;
}

static mbtcp_session_t *get_free_session(void) {
    for (int i = 0; i < MBTCP_MAX_CONNECTIONS; i++) {
        rt_base_t level = rt_hw_interrupt_disable();
        int socket = _server.sessions[i].socket;
        int active = _server.sessions[i].active;
        rt_hw_interrupt_enable(level);

        if ((socket < 0) && (active == 0)) return &_server.sessions[i];
    }

    return RT_NULL;
}

static int mbtcp_session_receive(mbtcp_session_t *session, uint8_t *buf, int bufsz, int timeout) {
    int len = 0;
    int rc = 0;
    fd_set rset, exset;
    struct timeval tv;

    while (bufsz > 0) {
        FD_ZERO(&rset);
        FD_ZERO(&exset);
        FD_SET(session->socket, &rset);
        FD_SET(session->socket, &exset);

        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;
        rc = select(session->socket + 1, &rset, RT_NULL, &exset, &tv);
        if (rc <= 0) break;

        if (FD_ISSET(session->socket, &exset)) {
            rc = -1;
            break;
        }

        rc = recv(session->socket, buf + len, bufsz, MSG_DONTWAIT);
        if (rc < 0) break;
        if (rc == 0) {
            if (len == 0) rc = -1;
            break;
        }

        len += rc;
        bufsz -= rc;
    }

    if (rc >= 0) rc = len;

    return rc;
}

static int mbtcp_session_send(mbtcp_session_t *session, uint8_t *buf, int len) {
    if (!session->connected || session->quit) {
        LOG_W("session not connected or quit");
        return -1;
    }

    int rc = send(session->socket, buf, len, 0);
    if (rc != len) {
        LOG_W("send %d data failed, rc = %d", len, rc);
        rc = -1;
        session->quit = 1;
    }

    return rc;
}

static void mbtcp_session_entry(void *param) {
    rt_base_t level;
    mbtcp_session_t *session = (mbtcp_session_t *)param;
    int active;
    struct timeval send_timeout;
    int rc;
    agile_modbus_t *ctx;
    int remain_length = 0;
    uint8_t tmp_buf[100];

_session_start:
    while (1) {
        rt_thread_mdelay(200);

        level = rt_hw_interrupt_disable();
        active = session->active;
        rt_hw_interrupt_enable(level);

        if (active) break;
    }

    if (session->socket < 0) {
        LOG_W("session socket invalid");
        goto _session_restart;
    }

    send_timeout.tv_sec = 5;
    send_timeout.tv_usec = 0;
    rc = setsockopt(session->socket, SOL_SOCKET, SO_SNDTIMEO, (const void *)&send_timeout,
                    sizeof(struct timeval));
    if (rc < 0) {
        LOG_W("setsockopt SO_SNDTIMEO failed");
        goto _session_restart;
    }

    session->tick_timeout = rt_tick_get() + rt_tick_from_millisecond(MBTCP_SESSION_TIMEOUT * 1000);

    ctx = &session->mb_ctx._ctx;
    agile_modbus_tcp_init(&session->mb_ctx, session->send_buf, sizeof(session->send_buf),
                          session->read_buf, sizeof(session->read_buf));
    agile_modbus_set_slave(ctx, 1);
    remain_length = 0;

    session->connected = 1;

    while (1) {
        if ((rt_tick_get() - session->tick_timeout) < (RT_TICK_MAX / 2)) {
            LOG_W("session timeout");
            break;
        }

        if (session->quit) {
            LOG_I("session quit");
            break;
        }

        int rt = mbtcp_session_receive(session, tmp_buf, sizeof(tmp_buf), 50);
        if (rt < 0) {
            LOG_W("receive data failed");
            break;
        }

        int read_len = rt;
        if (rt > (ctx->read_bufsz - remain_length)) {
            read_len = ctx->read_bufsz - remain_length;
        }
        if (read_len > 0) {
            memcpy(ctx->read_buf + remain_length, tmp_buf, read_len);
        }

        int total_len = read_len + remain_length;
        int is_reset = 0;

        if (total_len > 0 && read_len == 0) {
            is_reset = 1;
        }

        while (total_len > 0) {
            int frame_length = 0;
            rc = agile_modbus_slave_handle(ctx, total_len, 1, slave_callback,
                                           &slave_util, &frame_length);
            if (rc >= 0) {
                ctx->read_buf = ctx->read_buf + frame_length;
                ctx->read_bufsz = ctx->read_bufsz - frame_length;
                remain_length = total_len - frame_length;
                total_len = remain_length;

                if (rc > 0) {
                    session->tick_timeout =
                        rt_tick_get() + rt_tick_from_millisecond(MBTCP_SESSION_TIMEOUT * 1000);

                    if (mbtcp_session_send(session, ctx->send_buf, rc) < 0) {
                        LOG_W("send data failed");
                        goto _session_restart;
                    }
                }
            } else {
                if (total_len > AGILE_MODBUS_MAX_ADU_LENGTH || is_reset == 1) {
                    ctx->read_buf++;
                    ctx->read_bufsz--;
                    total_len--;
                    continue;
                }

                if (ctx->read_buf != session->read_buf) {
                    for (int i = 0; i < total_len; i++) {
                        session->read_buf[i] = ctx->read_buf[i];
                    }
                }

                ctx->read_buf = session->read_buf;
                ctx->read_bufsz = sizeof(session->read_buf);

                remain_length = total_len;

                break;
            }
        }

        if (total_len == 0) {
            remain_length = 0;
            ctx->read_buf = session->read_buf;
            ctx->read_bufsz = sizeof(session->read_buf);
        }
    }

_session_restart:
    LOG_W("session restart");
    session->connected = 0;
    session->quit = 0;
    if (session->socket >= 0) {
        closesocket(session->socket);
    }

    level = rt_hw_interrupt_disable();
    session->socket = -1;
    session->active = 0;
    rt_hw_interrupt_enable(level);

    goto _session_start;
}

static void mbtcp_server_entry(void *param) {
    int enable = 1;
    unsigned long ul = 1;
    struct sockaddr_in addr;
    struct sockaddr_in cliaddr;
    socklen_t addrlen;
    uint16_t port = 502;

    // select使用
    fd_set readset, exceptset;
    // select超时时间
    struct timeval select_timeout;
    select_timeout.tv_sec = 1;
    select_timeout.tv_usec = 0;

    rt_thread_mdelay(5000);

_server_start:
    _server.socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_server.socket < 0) {
        LOG_W("create server socket failed");
        goto _server_restart;
    }

    if (setsockopt(_server.socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&enable,
                   sizeof(enable)) < 0) {
        LOG_W("setsockopt REUSEADDR failed");
        goto _server_restart;
    }

    rt_memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(_server.socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        LOG_W("bind server socket failed");
        goto _server_restart;
    }

    if (listen(_server.socket, MBTCP_LISTEN_BACKLOG) < 0) {
        LOG_W("listen server socket failed");
        goto _server_restart;
    }

    if (ioctlsocket(_server.socket, FIONBIO, (unsigned long *)&ul) < 0) {
        LOG_W("ioctlsocket server failed");
        goto _server_restart;
    }

    LOG_I("server socket listen on port %d", port);
    while (1) {
        FD_ZERO(&readset);
        FD_ZERO(&exceptset);

        FD_SET(_server.socket, &readset);
        FD_SET(_server.socket, &exceptset);

        int rc = select(_server.socket + 1, &readset, RT_NULL, &exceptset, &select_timeout);
        if (rc < 0) break;
        if (rc > 0) {
            if (FD_ISSET(_server.socket, &exceptset)) break;
            if (FD_ISSET(_server.socket, &readset)) {
                addrlen = sizeof(struct sockaddr_in);
                rt_memset(&cliaddr, 0, sizeof(cliaddr));
                int client_fd = accept(_server.socket, (struct sockaddr *)&cliaddr, &addrlen);
                if (client_fd < 0) {
                    LOG_W("accept client socket failed");
                } else {
                    mbtcp_session_t *session = get_free_session();
                    if (session == RT_NULL) {
                        LOG_W("no free session, close");
                        closesocket(client_fd);
                    } else {
                        LOG_I("new client connected");

                        rt_base_t level = rt_hw_interrupt_disable();
                        session->socket = client_fd;
                        session->active = 1;
                        rt_hw_interrupt_enable(level);
                    }
                }
            }
        }
    }

_server_restart:
    LOG_W("server socket restart");
    if (_server.socket >= 0) {
        closesocket(_server.socket);
        _server.socket = -1;
    }

    rt_thread_mdelay(1000);
    goto _server_start;
}

int modbus_tcp_test(void) {
    rt_thread_t tid = RT_NULL;

    _server.socket = -1;
    for (int i = 0; i < MBTCP_MAX_CONNECTIONS; i++) {
        _server.sessions[i].socket = -1;
        _server.sessions[i].active = 0;
        _server.sessions[i].quit = 0;
        _server.sessions[i].connected = 0;

        tid = rt_thread_create("mbtcp_ses", mbtcp_session_entry, &_server.sessions[i],
                               MBTCP_SESSION_THREAD_STACK_SIZE, MBTCP_SESSION_THREAD_PRI, 100);
        if (tid == RT_NULL) {
            LOG_E("create mbtcp_session thread failed.");
            return -RT_ERROR;
        }
        rt_thread_startup(tid);
    }

    tid = rt_thread_create("mbtcp_srv", mbtcp_server_entry, RT_NULL, MBTCP_SEVER_THREAD_STACK_SIZE,
                           MBTCP_SEVER_THREAD_PRI, 100);
    if (tid == RT_NULL) {
        LOG_E("create mbtcp_server thread failed.");
        return -RT_ERROR;
    }
    rt_thread_startup(tid);

    return RT_EOK;
}
MSH_CMD_EXPORT(modbus_tcp_test, modbus_tcp_test)
