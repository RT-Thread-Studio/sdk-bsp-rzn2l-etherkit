/*******************************************************************************
 * Copyright (c) 2018, Rockwell Automation, Inc.
 * All rights reserved.
 *
 ******************************************************************************/
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <netdev.h>
#include "cipstring.h"
#include "networkconfig.h"
#include "cipcommon.h"
#include "ciperror.h"
#include "trace.h"
#include "opener_api.h"

EipStatus IfaceGetMacAddress(const char *iface,
                             uint8_t *const physical_address) {
  struct netdev *netdev = netdev_get_by_name(iface);
  if (netdev == NULL) {
    OPENER_TRACE_ERR("interface not found\n");
    return kEipStatusError;
  }

  memcpy(physical_address, netdev->hwaddr, netdev->hwaddr_len);

  return kEipStatusOk;
}

static EipStatus GetIpAndNetmaskFromInterface(const char *iface,
                                              CipTcpIpInterfaceConfiguration *iface_cfg) {
  struct netdev *netdev = netdev_get_by_name(iface);
  if (netdev == NULL) {
    OPENER_TRACE_ERR("interface not found\n");
    return kEipStatusError;
  }

  iface_cfg->ip_address = netdev->ip_addr.addr;
  iface_cfg->network_mask = netdev->netmask.addr;

  return kEipStatusOk;
}

static EipStatus GetGatewayFromRoute(const char *iface,
                                     CipTcpIpInterfaceConfiguration *iface_cfg) {
  struct netdev *netdev = netdev_get_by_name(iface);
  if (netdev == NULL) {
    OPENER_TRACE_ERR("interface not found\n");
    return kEipStatusError;
  }

  iface_cfg->gateway = netdev->gw.addr;

  return kEipStatusOk;
}

EipStatus IfaceGetConfiguration(const char *iface,
                                CipTcpIpInterfaceConfiguration *iface_cfg) {
  CipTcpIpInterfaceConfiguration local_cfg;
  EipStatus status;

  memset(&local_cfg, 0x00, sizeof local_cfg);

  status = GetIpAndNetmaskFromInterface(iface, &local_cfg);
  if (kEipStatusOk == status) {
    status = GetGatewayFromRoute(iface, &local_cfg);
  }
  if (kEipStatusOk == status) {
    /* Free first and then making a shallow copy of local_cfg.domain_name is
     *  ok, because local_cfg goes out of scope now. */
    ClearCipString(&iface_cfg->domain_name);
    *iface_cfg = local_cfg;
  }
  return status;
}

void GetHostName(CipString *hostname) {
  SetCipStringByCstr(hostname, PKG_OPENER_HOSTNAME);
}
