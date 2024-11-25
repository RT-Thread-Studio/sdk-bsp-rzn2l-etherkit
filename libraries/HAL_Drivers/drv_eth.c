/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-19     SummerGift   first version
 * 2018-12-25     zylx         fix some bugs
 * 2019-06-10     SummerGift   optimize PHY state detection process
 * 2019-09-03     xiaofan      optimize link change detection process
 * 2024-08-30     wangyuqiang  add the netdev driver for EtherCAT EOE
 */

#include "drv_config.h"
#include "drv_eth.h"
#include <hal_data.h>
#include <netif/ethernetif.h>
#include <lwipopts.h>

#if defined(BSP_USING_ETHERCAT_EOE)

#include "eoe_vether_feature.h"

#define ETHER_NETIF_PORT_BIT(x) ((uint32_t)(1) << (x))
#define ETHER_NETIF_PORT_ALL ((uint32_t)(0xFFFFFFFF))

#define UM_ETHER_NETIF_FEATURE_NUMBER_OF_ETHER_PORTS (3)
#define ETHER_NETIF_PORT_MASK ((uint32_t) ~((uint32_t)(0xFFFFFFFF) << (UM_ETHER_NETIF_FEATURE_NUMBER_OF_ETHER_PORTS)))

#define R_GMAC_Open EOE_VETHER_Open
#define R_GMAC_Write EOE_VETHER_Write
#define R_GMAC_Read EOE_VETHER_Read

#endif

/* debug option */
// #define ETH_RX_DUMP
// #define ETH_TX_DUMP
#define MINIMUM_ETHERNET_FRAME_SIZE (60U)
#define ETH_MAX_PACKET_SIZE (2048U)
#define ETHER_GMAC_INTERRUPT_FACTOR_RECEPTION (0x000000C0)
#define ETH_RX_BUF_SIZE ETH_MAX_PACKET_SIZE
#define ETH_TX_BUF_SIZE ETH_MAX_PACKET_SIZE
// #define DRV_DEBUG
#define LOG_TAG "drv.eth"
#ifdef DRV_DEBUG
#define DBG_LVL DBG_LOG
#else
#define DBG_LVL DBG_INFO
#endif /* DRV_DEBUG */
#include <rtdbg.h>

struct rt_eth_dev
{
    /* inherit from ethernet device */
    struct eth_device parent;
#ifndef PHY_USING_INTERRUPT_MODE
    rt_timer_t poll_link_timer;
#endif
};

static rt_uint8_t *Rx_Buff, *Tx_Buff;
// static  ETH_HandleTypeDef EthHandle;
static struct rt_eth_dev ra_eth_device;

static uint8_t g_link_change = 0; ///< Link change (bit0:port0, bit1:port1, bit2:port2)
static uint8_t g_link_status = 0; ///< Link status (bit0:port0, bit1:port1, bit2:port2)
static uint8_t previous_link_status = 0;

#if defined(ETH_RX_DUMP) || defined(ETH_TX_DUMP)
#define __is_print(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')
static void dump_hex(const rt_uint8_t *ptr, rt_size_t buflen)
{
    unsigned char *buf = (unsigned char *)ptr;
    int i, j;

    for (i = 0; i < buflen; i += 16)
    {
        rt_kprintf("%08X: ", i);

        for (j = 0; j < 16; j++)
            if (i + j < buflen)
                rt_kprintf("%02X ", buf[i + j]);
            else
                rt_kprintf("   ");
        rt_kprintf(" ");

        for (j = 0; j < 16; j++)
            if (i + j < buflen)
                rt_kprintf("%c", __is_print(buf[i + j]) ? buf[i + j] : '.');
        rt_kprintf("\n");
    }
}
#endif

static int phy_rtl8211f_led_fixup(ether_phy_instance_ctrl_t *phydev)
{
#define RTL_8211F_PAGE_SELECT 0x1F
#define RTL_8211F_EEELCR_ADDR 0x11
#define RTL_8211F_LED_PAGE 0xD04
#define RTL_8211F_LCR_ADDR 0x10

    uint32_t val1, val2 = 0;

    /* switch to led page */
    R_ETHER_PHY_Write(phydev, RTL_8211F_PAGE_SELECT, RTL_8211F_LED_PAGE);

    /* set led1(green) Link 10/100/1000M, and set led2(yellow) Link 10/100/1000M+Active */
    R_ETHER_PHY_Read(phydev, RTL_8211F_LCR_ADDR, &val1);
    val1 |= (1 << 5);
    val1 |= (1 << 8);
    val1 &= (~(1 << 9));
    val1 |= (1 << 10);
    val1 |= (1 << 11);
    R_ETHER_PHY_Write(phydev, RTL_8211F_LCR_ADDR, val1);

    /* set led1(green) EEE LED function disabled so it can keep on when linked */
    R_ETHER_PHY_Read(phydev, RTL_8211F_EEELCR_ADDR, &val2);
    val2 &= (~(1 << 2));
    R_ETHER_PHY_Write(phydev, RTL_8211F_EEELCR_ADDR, val2);

    /* switch back to page0 */
    R_ETHER_PHY_Write(phydev, RTL_8211F_PAGE_SELECT, 0xa42);

    return 0;
}

void ether_phy_targets_initialize_rtl8211_rgmii(ether_phy_instance_ctrl_t *p_instance_ctrl)
{
   rt_thread_mdelay(100);
    phy_rtl8211f_led_fixup(p_instance_ctrl);
}

extern void phy_reset(void);
/* EMAC initialization function */
static rt_err_t rt_ra_eth_init(void)
{
    fsp_err_t res;

    res = R_GMAC_Open(&g_ether0_ctrl, &g_ether0_cfg);
    if (res != FSP_SUCCESS)
        LOG_W("R_ETHER_Open failed!, res = %d", res);

    return RT_EOK;
}

static rt_err_t rt_ra_eth_open(rt_device_t dev, rt_uint16_t oflag)
{
    LOG_D("emac open");
    return RT_EOK;
}

static rt_err_t rt_ra_eth_close(rt_device_t dev)
{
    LOG_D("emac close");
    return RT_EOK;
}

static rt_ssize_t rt_ra_eth_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    LOG_D("emac read");
    rt_set_errno(-RT_ENOSYS);
    return 0;
}

static rt_ssize_t rt_ra_eth_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    LOG_D("emac write");
    rt_set_errno(-RT_ENOSYS);
    return 0;
}

static rt_err_t rt_ra_eth_control(rt_device_t dev, int cmd, void *args)
{
    switch (cmd)
    {
    case NIOCTL_GADDR:
        /* get mac address */
        if (args)
        {
            SMEMCPY(args, g_ether0_ctrl.p_gmac_cfg->p_mac_address, 6);
        }
        else
        {
            return -RT_ERROR;
        }
        break;

    default:
        break;
    }

    return RT_EOK;
}

/* ethernet device interface */
/* transmit data*/
rt_err_t rt_ra_eth_tx(rt_device_t dev, struct pbuf *p)
{
    fsp_err_t res;
    struct pbuf *q;
    uint8_t *buffer = Tx_Buff;
    uint32_t framelength = 0;
    uint32_t bufferoffset = 0;
    uint32_t byteslefttocopy = 0;
    uint32_t payloadoffset = 0;
    bufferoffset = 0;

    LOG_D("send frame len : %d", p->tot_len);

    /* copy frame from pbufs to driver buffers */
    for (q = p; q != NULL; q = q->next)
    {
        /* Get bytes in current lwIP buffer */
        byteslefttocopy = q->len;
        payloadoffset = 0;

        /* Check if the length of data to copy is bigger than Tx buffer size*/
        while ((byteslefttocopy + bufferoffset) > ETH_TX_BUF_SIZE)
        {
            /* Copy data to Tx buffer*/
            SMEMCPY((uint8_t *)((uint8_t *)buffer + bufferoffset), (uint8_t *)((uint8_t *)q->payload + payloadoffset), (ETH_TX_BUF_SIZE - bufferoffset));

            byteslefttocopy = byteslefttocopy - (ETH_TX_BUF_SIZE - bufferoffset);
            payloadoffset = payloadoffset + (ETH_TX_BUF_SIZE - bufferoffset);
            framelength = framelength + (ETH_TX_BUF_SIZE - bufferoffset);
            bufferoffset = 0;
        }

        /* Copy the remaining bytes */
        SMEMCPY((uint8_t *)((uint8_t *)buffer + bufferoffset), (uint8_t *)((uint8_t *)q->payload + payloadoffset), byteslefttocopy);
        bufferoffset = bufferoffset + byteslefttocopy;
        framelength = framelength + byteslefttocopy;
    }

#ifdef ETH_TX_DUMP
    dump_hex(buffer, p->tot_len);
#endif
#ifdef ETH_RX_DUMP
    if (p)
    {
        LOG_E("******p buf frame *********");
        for (q = p; q != NULL; q = q->next)
        {
            dump_hex(q->payload, q->len);
        }
    }
#endif
    res = R_GMAC_Write(&g_ether0_ctrl, buffer, p->tot_len); //>MINIMUM_ETHERNET_FRAME_SIZE?p->tot_len:MINIMUM_ETHERNET_FRAME_SIZE);
    if (res != FSP_SUCCESS)
        LOG_W("R_ETHER_Write failed!, res = %d", res);
    return RT_EOK;
}

/* receive data*/
struct pbuf *rt_ra_eth_rx(rt_device_t dev)
{
    struct pbuf *p = NULL;
    struct pbuf *q = NULL;
    uint32_t len = 0;
    uint8_t *buffer = Rx_Buff;
    fsp_err_t res;

    res = R_GMAC_Read(&g_ether0_ctrl, buffer, &len);
    if (res != FSP_SUCCESS)
        LOG_D("R_ETHER_Read failed!, res = %d", res);

    uint32_t bufferoffset = 0;
    uint32_t payloadoffset = 0;
    uint32_t byteslefttocopy = 0;

    LOG_D("receive frame len : %d", len);

    if (len > 0)
    {
        /* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
        p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    }

#ifdef ETH_RX_DUMP
    if (p)
    {
        dump_hex(buffer, p->tot_len);
    }
#endif

    if (p != NULL)
    {
        bufferoffset = 0;
        for (q = p; q != NULL; q = q->next)
        {
            byteslefttocopy = q->len;
            payloadoffset = 0;

            /* Check if the length of bytes to copy in current pbuf is bigger than Rx buffer size*/
            while ((byteslefttocopy + bufferoffset) > ETH_RX_BUF_SIZE)
            {
                /* Copy data to pbuf */
                SMEMCPY((uint8_t *)((uint8_t *)q->payload + payloadoffset), (uint8_t *)((uint8_t *)buffer + bufferoffset), (ETH_RX_BUF_SIZE - bufferoffset));

                byteslefttocopy = byteslefttocopy - (ETH_RX_BUF_SIZE - bufferoffset);
                payloadoffset = payloadoffset + (ETH_RX_BUF_SIZE - bufferoffset);
                bufferoffset = 0;
            }
            /* Copy remaining data in pbuf */
            SMEMCPY((uint8_t *)((uint8_t *)q->payload + payloadoffset), (uint8_t *)((uint8_t *)buffer + bufferoffset), byteslefttocopy);
            bufferoffset = bufferoffset + byteslefttocopy;
        }
    }

#ifdef ETH_RX_DUMP
    if (p)
    {
        LOG_E("******p buf frame *********");
        for (q = p; q != NULL; q = q->next)
        {
            dump_hex(q->payload, q->len);
        }
    }
#endif

    return p;
}

static void phy_linkchange()
{
    fsp_err_t res;
    gmac_link_status_t port_status;
    uint8_t port = 0;
    uint8_t port_bit = 0;

#ifndef BSP_USING_ETHERCAT_EOE
    res = R_GMAC_LinkProcess(&g_ether0_ctrl);
    if (res != FSP_SUCCESS)
        LOG_D("R_ETHER_LinkProcess failed!, res = %d", res);

    if (0 == g_ether0.p_cfg->p_callback)
    {
        for (port = 0; port < PING_PORT_COUNT; port++)
        {
            res = R_GMAC_GetLinkStatus(&g_ether0_ctrl, port, &port_status);
            if (FSP_SUCCESS != res)
            {
                /* An error has occurred */
                LOG_E("R_GMAC_GetLinkStatus failed!, res = %d", res);
                break;
            }

            if (GMAC_LINK_STATUS_DOWN != port_status)
            {
                /* Set link up */
                g_link_status |= (uint8_t)(1U << port);
            }
        }
        if (FSP_SUCCESS == res)
        {
            /* Set changed link status */
            g_link_change = previous_link_status ^ g_link_status;
        }
    }

    if (FSP_SUCCESS == res)
    {
        for (port = 0; port < PING_PORT_COUNT; port++)
        {
            port_bit = (uint8_t)(1U << port);

            if (g_link_change & port_bit)
            {
                /* Link status changed */
                g_link_change &= (uint8_t)(~port_bit); // change bit clear

                if (g_link_status & port_bit)
                {
                    /* Changed to Link-up */
                    eth_device_linkchange(&ra_eth_device.parent, RT_TRUE);
                    LOG_I("link up");
                }
                else
                {
                    /* Changed to Link-down */
                    eth_device_linkchange(&ra_eth_device.parent, RT_FALSE);
                    LOG_I("link down");
                }
            }
        }
    }
#else
    res = EOE_VETHER_LinkProcess(&g_ether0_ctrl);
    if (res != FSP_SUCCESS)
        LOG_D("R_ETHER_LinkProcess failed!, res = %d", res);

    g_link_status &= ~ETHER_NETIF_PORT_ALL;

    for (port = 0; port < BSP_FEATURE_GMAC_MAX_PORTS; port++)
    {
        /** Check the link state of single port. */
        res = EOE_VETHER_GetLinkStatus(&g_ether0_ctrl, port, &port_status);

        /** If link status can be gotten correctly and is ready, the port is linking. */
        if (FSP_SUCCESS == res && GMAC_LINK_STATUS_READY == port_status)
        {
            g_link_status |= ETHER_NETIF_PORT_BIT(port);
        }

        /** Otherwise, the port is not linking. */
        else
        {
            g_link_status &= ~ETHER_NETIF_PORT_BIT(port);
        }
    }

    if (g_link_status & ETHER_NETIF_PORT_MASK)
    {
        /* Changed to Link-up */
        if (previous_link_status != g_link_status)
        {
            eth_device_linkchange(&ra_eth_device.parent, RT_TRUE);
            LOG_I("link up");
        }
    }
    else
    {
        /* Changed to Link-down */
        eth_device_linkchange(&ra_eth_device.parent, RT_FALSE);
        LOG_I("link down");
    }
#endif

    previous_link_status = g_link_status;
}

void user_ether0_callback(ether_callback_args_t *p_args)
{
    rt_interrupt_enter();

    switch (p_args->event)
    {
    case ETHER_EVENT_LINK_ON:                          ///< Link up detection event/
        g_link_status |= (uint8_t)p_args->status_link; ///< status up
        g_link_change |= (uint8_t)p_args->status_link; ///< change bit set
        break;

    case ETHER_EVENT_LINK_OFF:                            ///< Link down detection event
        g_link_status &= (uint8_t)(~p_args->status_link); ///< status down
        g_link_change |= (uint8_t)p_args->status_link;    ///< change bit set
        break;

    case ETHER_EVENT_WAKEON_LAN:    ///< Magic packet detection event
    case ETHER_EVENT_SBD_INTERRUPT: ///< BSD Interrupt event
    {
        rt_err_t result;
        result = eth_device_ready(&(ra_eth_device.parent));
        if (result != RT_EOK)
            rt_kprintf("RX err =%d\n", result);
        break;
    }
    case ETHER_EVENT_PMT_INTERRUPT: ///< PMT Interrupt event
        break;

    default:
        break;
    }

    rt_interrupt_leave();
}

/* Register the EMAC device */
static int rt_hw_ra_eth_init(void)
{
    rt_err_t state = RT_EOK;

    /* Prepare receive and send buffers */
    Rx_Buff = (rt_uint8_t *)rt_calloc(1, ETH_MAX_PACKET_SIZE);
    if (Rx_Buff == RT_NULL)
    {
        LOG_E("No memory");
        state = -RT_ENOMEM;
        goto __exit;
    }

    Tx_Buff = (rt_uint8_t *)rt_calloc(1, ETH_MAX_PACKET_SIZE);
    if (Tx_Buff == RT_NULL)
    {
        LOG_E("No memory");
        state = -RT_ENOMEM;
        goto __exit;
    }

    ra_eth_device.parent.parent.init = NULL;
    ra_eth_device.parent.parent.open = rt_ra_eth_open;
    ra_eth_device.parent.parent.close = rt_ra_eth_close;
    ra_eth_device.parent.parent.read = rt_ra_eth_read;
    ra_eth_device.parent.parent.write = rt_ra_eth_write;
    ra_eth_device.parent.parent.control = rt_ra_eth_control;
    ra_eth_device.parent.parent.user_data = RT_NULL;

    ra_eth_device.parent.eth_rx = rt_ra_eth_rx;
    ra_eth_device.parent.eth_tx = rt_ra_eth_tx;

    rt_ra_eth_init();

    /* register eth device */
    state = eth_device_init(&(ra_eth_device.parent), "e0");
    if (RT_EOK == state)
    {
        LOG_D("emac device init success");
    }
    else
    {
        LOG_E("emac device init faild: %d", state);
        state = -RT_ERROR;
        goto __exit;
    }

    ra_eth_device.poll_link_timer = rt_timer_create("phylnk", (void (*)(void *))phy_linkchange,
                                                    NULL, RT_TICK_PER_SECOND, RT_TIMER_FLAG_PERIODIC);
    if (!ra_eth_device.poll_link_timer || rt_timer_start(ra_eth_device.poll_link_timer) != RT_EOK)
    {
        LOG_E("Start link change detection timer failed");
    }
__exit:
    if (state != RT_EOK)
    {
        if (Rx_Buff)
        {
            rt_free(Rx_Buff);
        }

        if (Tx_Buff)
        {
            rt_free(Tx_Buff);
        }
    }

    return state;
}
INIT_DEVICE_EXPORT(rt_hw_ra_eth_init);
