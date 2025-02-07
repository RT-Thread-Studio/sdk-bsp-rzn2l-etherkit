/*******************************************************************************
 * Copyright (c) 2009, Rockwell Automation, Inc.
 * All rights reserved.
 *
 ******************************************************************************/

/** @file POSIX/opener_error.c
 *  @author Martin Melik Merkumians
 *  @brief This file includes the prototypes for error resolution functions like strerror or WSAGetLastError
 *
 */

#undef _GNU_SOURCE  /* Force the use of the XSI compliant strerror_r() function. */

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "opener_error.h"

/* There are some places where the memory is not freed */
static char kErrorMessageBuffer[256];

int GetSocketErrorNumber(void) {
  return errno;
}

char* GetErrorMessage(int error_number) {
  const char *error_message = strerror(error_number);

  strncpy(kErrorMessageBuffer, error_message, sizeof(kErrorMessageBuffer) - 1);
  kErrorMessageBuffer[sizeof(kErrorMessageBuffer) - 1] = '\0';

  return kErrorMessageBuffer;
}

void FreeErrorMessage(char *error_message) {
  (void)error_message;
}

