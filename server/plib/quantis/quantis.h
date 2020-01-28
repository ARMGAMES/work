/*
 * Copyright (c) 2004, 2005, id Quantique SA, Switzerland
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * Neither the name of id Quantique nor the names of its contributors may be
 * used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*****************************************************************************/
/**
 * \file quantis.h
 * \author M. Pignat / id Quantique 
 * \brief header file for the quantis library
 *
 * The quantis library can be used to access Quantis-PCI boards.
 * This library is OS indepenant and has been tested with Windows 2000 and
 * Linux.
 */
/*****************************************************************************/
#ifndef QUANTIS_H
#define QUANTIS_H

/*****************************************************************************/

/**
 * \def LINK_TYPE
 * \brief Macro for linking with C or C++.
 * \internal
 */
#ifdef __cplusplus
#define LINK_TYPE extern "C"
#else
#define LINK_TYPE extern
#endif

/*****************************************************************************/

LINK_TYPE int quantisInit(void);

/*****************************************************************************/
/**
 * \fn int quantisCount(void);
 * \brief count the number of quantis PCI board.
 *
 * \return the number of PCI card found.
 *
 */
/*****************************************************************************/

LINK_TYPE int quantisCount(void);

/*****************************************************************************/

/*****************************************************************************/
/**
 * \fn int quantisGetModules(int cardNumber);
 * \brief Detect modules present on the boards.
 *
 * \param cardNumber The number of the PCI board to select(first is 0).
 *
 * \return a bit field of modules present.
 *
 * bit 0 is set to 1 when module 0 is detected
 * bit 1 is set to 1 when module 1 is detected
 * bit 2 is set to 1 when module 2 is detected
 * bit 3 is set to 1 when module 3 is detected
 * for instance if the bit field is 5 in decimal (0b101 in binary)
 * then module 0 and 2 are present.
 *
 */
/*****************************************************************************/

LINK_TYPE int quantisGetModules(int cardNumber);

/*****************************************************************************/

/*****************************************************************************/
/**
 * \fn int quantisBoardReset(int cardNumber);
 * \brief Reset the board.
 *
 * \param cardNumber The number of the PCI board to select(first is 0).
 *
 * \return 0 for no problem.
 *
 */
/*****************************************************************************/

LINK_TYPE int quantisBoardReset(int cardNumber);

/*****************************************************************************/

/*****************************************************************************/
/**
 * \fn int quantisBoardVersion(int cardNumber);
 * \brief Get the version of the board.
 *
 * \param cardNumber The number of the PCI board to select(first is 0).
 *
 * \return The version of the board.
 *
 */
/*****************************************************************************/

LINK_TYPE int quantisBoardVersion(int cardNumber);

/*****************************************************************************/

/*****************************************************************************/
/**
 * \fn int quantisLibVersion(void);
 * \brief Get the library version.
 *
 * \return the version of the library.
 */
/*****************************************************************************/

LINK_TYPE int quantisLibVersion(void);

/*****************************************************************************/

/*****************************************************************************/
/**
 * \fn int quantisDriverVersion(void);
 * \brief Get the driver version.
 *
 * \return the version of the driver.
 */
/*****************************************************************************/

LINK_TYPE int quantisDriverVersion(void);

/*****************************************************************************/

/*****************************************************************************/
/**
 * \fn int quantisModulesReset(int cardNumber, int moduleMask);
 * \brief Reset modules on a board.
 *
 * \param cardNumber The number of the PCI board to select(first is 0).
 * \param moduleMask The mask representing wich modules to reset.
 *
 * \return 0 for no problem.
 *
 */
/*****************************************************************************/

LINK_TYPE int quantisModulesReset(int cardNumber, int moduleMask);

/*****************************************************************************/

/*****************************************************************************/
/**
 * \fn int quantisModulesEnable(int cardNumber, int moduleMask);
 * \brief Enable modules on a board.
 *
 * \param cardNumber The number of the PCI board to select(first is 0).
 * \param moduleMask The mask representing wich modules to enable.
 *
 * \return 0 for no problem.
 *
 */
/*****************************************************************************/


LINK_TYPE int quantisModulesEnable(int cardNumber, int moduleMask);

/*****************************************************************************/

/*****************************************************************************/
/**
 * \fn int quantisModulesDisable(int cardNumber, int moduleMask);
 * \brief Disable modules on a board.
 *
 * \param cardNumber The number of the PCI board to select(first is 0).
 * \param moduleMask The mask representing wich modules to disable.
 *
 * \return 0 for no problem.
 *
 */
/*****************************************************************************/

LINK_TYPE int quantisModulesDisable(int cardNumber, int moduleMask);

/*****************************************************************************/

/*****************************************************************************/
/**
 * \fn int quantisRead(int cardNumber, void *buffer, unsigned int size);
 * \brief Read data from a board
 *
 * \param cardNumber The number of the PCI board to select(first is 0).
 * \param buffer the address of the destination data
 * \param size the size to be read (in bytes)
 * \return the size read or a negative errno.
 *
 */
/*****************************************************************************/

LINK_TYPE int quantisRead(int cardNumber, void *buffer, unsigned int size);

/*****************************************************************************/

/*****************************************************************************/
/**
 * \fn int quantisSetTestMode(int cardNumber, int enable);
 * \brief Read data from a board
 *
 * \param cardNumber The number of the PCI board to select(first is 0).
 * \param enable if non zero test mode will be enable
 *
 */
/*****************************************************************************/

LINK_TYPE int quantisSetTestMode(int cardNumber, int enable);

/*****************************************************************************/

/*****************************************************************************/
/**
 * \fn int int quantisModulesStatus(int cardNumber)
 * \brief Get status of the modules on a board
 *
 * \param cardNumber The number of the PCI board to select(first is 0).
 *
 */
/*****************************************************************************/

LINK_TYPE int quantisModulesStatus(int cardNumber);

/*****************************************************************************/
#endif // QUANTIS_H
