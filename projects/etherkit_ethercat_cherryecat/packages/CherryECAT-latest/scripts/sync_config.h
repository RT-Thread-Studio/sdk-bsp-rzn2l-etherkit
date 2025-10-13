/*
 * Generated CherryECAT PDO configuration from ENI file
 * Auto-generated - do not modify manually
 */

#include "ec_master.h"

// Slave 1: Drive 1 (ECAT_CIA402)
// Vendor ID: 0x0048504D
// Product Code: 0x00000003

static ec_pdo_entry_info_t eni_0003_1602[] = {
    { 0x6040, 0x00, 0x10 },  // Control Word
    { 0x60ff, 0x00, 0x20 },  // TargetVelocity
    { 0x0000, 0x00, 0x10 },  // Padding
};

static ec_pdo_entry_info_t eni_0003_1a02[] = {
    { 0x6041, 0x00, 0x10 },  // Status Word
    { 0x6064, 0x00, 0x20 },  // ActualPosition
    { 0x0000, 0x00, 0x10 },  // Padding
};

static ec_pdo_info_t eni_0003_rxpdos[] = {
    { 0x1602, 3, &eni_0003_1602[0] },
};

static ec_pdo_info_t eni_0003_txpdos[] = {
    { 0x1a02, 3, &eni_0003_1a02[0] },
};

static ec_sync_info_t eni_0003_syncs[] = {
    { 2, EC_DIR_OUTPUT, 1, eni_0003_rxpdos },
    { 3, EC_DIR_INPUT, 1, eni_0003_txpdos },
};
