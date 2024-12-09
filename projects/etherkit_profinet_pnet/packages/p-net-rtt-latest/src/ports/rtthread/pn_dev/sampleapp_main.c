/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2021 rt-labs AB, Sweden.
 *
 * This software is dual-licensed under GPLv3 and a commercial
 * license. See the file LICENSE.md distributed with this software for
 * full license information.
 ********************************************************************/

#include "sampleapp_common.h"
#include "app_utils.h"
#include "app_log.h"
#include "app_gsdml.h"

#include "osal_log.h"
#include "osal.h"
#include <pnet_api.h>

#include <lwip/netif.h>

#include <string.h>

#include <rtthread.h>
#include <netdev.h>

#ifdef RT_USING_DFS
#include <dfs_fs.h>
#include "dfs_ramfs.h"
#endif

#define APP_DEFAULT_FILE_DIRECTORY     "/"

#if !defined(APP_DEFAULT_ETHERNET_INTERFACE)
#define APP_DEFAULT_ETHERNET_INTERFACE "e00"
#endif

#define APP_LOG_LEVEL                  APP_LOG_LEVEL_DEBUG

#define APP_BG_WORKER_THREAD_PRIORITY  19
#define APP_BG_WORKER_THREAD_STACKSIZE 4096 /* bytes */

/********************************** Globals ***********************************/

static app_data_t * sample_app = NULL;
static pnet_cfg_t pnet_cfg = {0};
app_args_t app_args = {0};

static bool app_status = 0;

/****************************** Main ******************************************/

static void pnet_app(void);

static void netdev_status_callback(struct netdev *netdev, enum netdev_cb_type up)
{
    if (up)
    {
        pnet_app();
    }
    else
    {
        return;
    }
}

int netdev_monitor_init(void)
{
    struct netdev *netdev = netdev_get_by_name("e0");
    if (netdev == RT_NULL)
    {
        APP_LOG_ERROR("Failed to get network device.\n");
    }

    netdev_set_status_callback(netdev, netdev_status_callback);
    return RT_EOK;
}
INIT_APP_EXPORT(netdev_monitor_init);

static void pnet_main (void *parameter)
{
   int ret;
   app_utils_netif_namelist_t netif_name_list;
   pnet_if_cfg_t netif_cfg = {0};
   uint16_t number_of_ports;

   strcpy (app_args.eth_interfaces, APP_DEFAULT_ETHERNET_INTERFACE);
   strcpy (app_args.station_name, APP_GSDML_DEFAULT_STATION_NAME);
   app_log_set_log_level (APP_LOG_LEVEL);

   APP_LOG_INFO ("\n** Starting P-Net sample application " PNET_VERSION
                 " **\n");
   APP_LOG_INFO (
      "Number of slots:      %u (incl slot for DAP module)\n",
      PNET_MAX_SLOTS);
   APP_LOG_INFO ("P-net log level:      %u (DEBUG=0, FATAL=4)\n", LOG_LEVEL);
   APP_LOG_INFO ("App log level:        %u (DEBUG=0, FATAL=4)\n", APP_LOG_LEVEL);
   APP_LOG_INFO ("Max number of ports:  %u\n", PNET_MAX_PHYSICAL_PORTS);
   APP_LOG_INFO ("Network interfaces:   %s\n", app_args.eth_interfaces);
   APP_LOG_INFO ("Default station name: %s\n", app_args.station_name);

   app_pnet_cfg_init_default (&pnet_cfg);

   /* Note: station name is defined by app_gsdml.h */

   strcpy (pnet_cfg.file_directory, APP_DEFAULT_FILE_DIRECTORY);

   ret = app_utils_pnet_cfg_init_netifs (
      app_args.eth_interfaces,
      &netif_name_list,
      &number_of_ports,
      &netif_cfg);
   if (ret != 0)
   {
      return;
   }

   pnet_cfg.if_cfg = netif_cfg;
   pnet_cfg.num_physical_ports = number_of_ports;

   app_utils_print_network_config (&netif_cfg, number_of_ports);

   pnet_cfg.pnal_cfg.bg_worker_thread.prio = APP_BG_WORKER_THREAD_PRIORITY;
   pnet_cfg.pnal_cfg.bg_worker_thread.stack_size =
      APP_BG_WORKER_THREAD_STACKSIZE;

   /* Initialize profinet stack */
   sample_app = app_init (&pnet_cfg, &app_args);
   if (sample_app == NULL)
   {
      printf ("Failed to initialize P-Net.\n");
      printf ("Aborting application\n");
      return;
   }

   /* Start main loop */
   if (app_start (sample_app, RUN_IN_MAIN_THREAD) != 0)
   {
      printf ("Failed to start\n");
      printf ("Aborting application\n");
      return;
   }

   app_loop_forever (sample_app);
}

static void pnet_app(void)
{
    if(app_status == 0)
    {
#if defined(PNET_USING_RAMFS)
        rt_align(PNET_RAMFS_SIZE)
       static char ramfs_buf[PNET_RAMFS_SIZE];
       if (dfs_mount(RT_NULL, "/", "ram", 0, dfs_ramfs_create(ramfs_buf, PNET_RAMFS_SIZE)) == 0)
       {
          rt_kprintf("RAM file system initializated!\n");
       }
       else
       {
          rt_kprintf("RAM file system initializate failed!\n");
       }
#endif

        rt_thread_t pnet_thread = rt_thread_create("pnet", pnet_main, RT_NULL, 5124, 19, 10);
        rt_thread_startup(pnet_thread);
        app_status = 1;
        return;
    }

    return;
}
