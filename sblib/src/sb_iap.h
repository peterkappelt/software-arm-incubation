/*
 *  Copyright (c) 2013 Martin Glück <martin@mangari.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#ifndef SB_IAP_H_
#define SB_IAP_H_

#ifndef IAP_EMULATION
#define SB_FLASH_BASE_ADDRESS         0x0000
#else
extern unsigned char FLASH [];
#define SB_FLASH_BASE_ADDRESS   ((int) FLASH)
#endif

#define SB_EEP_FLASH_SECTOR_ADDRESS   (SB_FLASH_BASE_ADDRESS + 0x7000)

/**
 * Returns the number of the FLASH sector based on the passed address.
 * @param    address     the address inside the FLASH
 * @return               the sector number
 */
int sb_iap_address2sector (unsigned int address);

/**
 * Erase the specified sector.
 *  @param sector       number of the sector which should be erased
 *  @return             status code (0 == OK)
 */
int sb_iap_erase_sector   (int sector);

/**
 * Programs the specified number of bytes from the RAM to the specified location
 * inside the FLASH.
 * @param rom           start address of inside the FLASH
 * @param ram           start address if the buffer
 * @param size          number of bytes ot program
 *  @return             status code (0 == OK)
 */
int sb_iap_program (unsigned int rom, unsigned int ram, unsigned int size);

/**
 * Returns the part identification number and unique id of the CPU.
 *  @return             status code (0 == OK)
 */
int sb_iap_read_uid       (unsigned int * uid, unsigned int * part_id);

 #endif /* SB_IAP_H_ */