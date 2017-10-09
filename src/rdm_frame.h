/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * rdm_frame.h
 * Copyright (C) 2015 Simon Newton
 */

/**
 * @addtogroup rdm
 * @{
 * @file rdm_frame.h
 * @brief Remote Device Management
 */

#ifndef FIRMWARE_SRC_RDM_FRAME_H_
#define FIRMWARE_SRC_RDM_FRAME_H_

#include <stdint.h>
#include "rdm.h"

//NUEVA ESTRUCTURA RDM KIRNO
typedef struct {
  uint8_t start_code;
  uint8_t message_length;
  uint8_t dest_uid[UID_LENGTH];
  uint8_t src_uid[UID_LENGTH];
  uint8_t port_id;

  //-- DUB internals --
  uint8_t command_class;
  uint16_t param_id;
  uint8_t param_data_length;
  // optional param data
  // checksum [2];
} RDMKirnoHeader;

//ESTRUCTURA ORIGINAL
typedef struct {
  uint8_t start_code;
  uint8_t sub_start_code;
  uint8_t message_length;
  uint8_t dest_uid[UID_LENGTH];
  uint8_t src_uid[UID_LENGTH];
  uint8_t transaction_number;
  uint8_t port_id;
  uint8_t message_count;
  uint16_t sub_device;
  uint8_t command_class;
  uint16_t param_id;
  uint8_t param_data_length;
  // optional param data
  // checksum [2];
} __attribute__((packed)) RDMHeader;


/**
 * @}
 */

#endif  // FIRMWARE_SRC_RDM_FRAME_H_
