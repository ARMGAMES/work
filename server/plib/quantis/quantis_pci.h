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
 * \file quantis_pci.h
 * \author M. Pignat / id Quantique
 * header file for the Quantis-PCI board
 */
/*****************************************************************************/

#ifndef QUANTIS_PCI_H
#define QUANTIS_PCI_H
 
#define QUANTIS_PCI_REGION 1
#define QUANTIS_TIMEOUT 1000
#define QUANTIS_MAX_MODULE_PER_BOARD 4


#define QUANTIS_REG(__OFFSET__) ((__OFFSET__))
#define QUANTIS_BIT(__SHIFT__) (1<<(__SHIFT__))

// CC Core Control
#define IDQ_CC_ER QUANTIS_REG(0x00)
#define IDQ_CC_DR QUANTIS_REG(0x01)
#define IDQ_CC_SR QUANTIS_REG(0x03)
 
#define QUANTIS_BIT_CC_SWRST QUANTIS_BIT(0)

// CV Core Version
#define IDQ_CV_SR QUANTIS_REG(0x07)

#define QUANTIS_CV_RELEASE(__X__) ((__X__>> 0)&0xff)
#define QUANTIS_CV_DAY(__X__)     ((__X__>> 8)&0xff)
#define QUANTIS_CV_MONTH(__X__)   ((__X__>>16)&0xff)
#define QUANTIS_CV_YEAR(__X__)    ((__X__>>24)&0xff)

// FS FIFO Status
#define IDQ_FD_SIZE (1024)

#define IDQ_FS_CA QUANTIS_REG(0x08)
#define IDQ_FS_RR QUANTIS_REG(0x0a)
#define IDQ_FD_RR QUANTIS_REG(0x0b)

#define QUANTIS_BIT_FS_FSEMP QUANTIS_BIT(0)
#define QUANTIS_BIT_FS_FSFL1 QUANTIS_BIT(1)
#define QUANTIS_BIT_FS_FSFL2 QUANTIS_BIT(2)
#define QUANTIS_BIT_FS_FSFL3 QUANTIS_BIT(3)
#define QUANTIS_BIT_FS_FSFUL QUANTIS_BIT(6)
#define QUANTIS_BIT_FS_FSERR QUANTIS_BIT(7)

// MX Module X
#define IDQ_MX_ER QUANTIS_REG(0x0c)
#define IDQ_MX_DR QUANTIS_REG(0x0d)
#define IDQ_MX_SR QUANTIS_REG(0x0f)

#define QUANTIS_MODULE_0_SHIFT 0
#define QUANTIS_MODULE_1_SHIFT 8
#define QUANTIS_MODULE_2_SHIFT 16
#define QUANTIS_MODULE_3_SHIFT 24

#define QUANTIS_MX_SD  0
#define QUANTIS_MX_SEN 1
#define QUANTIS_MX_TM  2
#define QUANTIS_MX_HEN 6
#define QUANTIS_MX_HST 7

#ifdef WIN32
#define u32 DWORD
#endif



/***************************************************************************/
/**
 * \struct quantis_regs_t
 * \brief register mapping
 */
/***************************************************************************/
typedef struct
{
  // CC Core Control
  volatile u32 CC_ER;      ///< (0x00) Enable
  volatile u32 CC_DR;      ///< (0x01) Disable
  volatile u32 _reserved1; ///< (0x02) reserved
  volatile u32 CC_SR;      ///< (0x03) Status
  
  // CV Core Version
  volatile u32 _reserved2; ///< (0x04) reserved
  volatile u32 _reserved3; ///< (0x05) reserved
  volatile u32 _reserved4; ///< (0x06) reserved
  volatile u32 CV_SR;      ///< (0x07) version
  
  // FS FIFO Status
  volatile u32 FS_CA;      ///< (0x08) Clear Access
  volatile u32 _reserved5; ///< (0x09) reserved
  volatile u32 FS_RR;      ///< (0x0a) Status read register

  // FD FIFO Data
  volatile u32 FD_RR;      ///< (0x0b) Data

  // MX Module X
  volatile u32 MX_ER;      ///< (0x0c) Enable
  volatile u32 MX_DR;      ///< (0x0d) Disable
  volatile u32 _reserved6; ///< (0x0e) reserved
  volatile u32 MX_SR;      ///< (0x0f) Status
} quantis_regs_t;
/***************************************************************************/

static inline int mask2reg(int mask, int type)
{
  int reg;
  
  reg  = (mask&(1<<0) ? 1<<QUANTIS_MODULE_0_SHIFT : 0)
       | (mask&(1<<1) ? 1<<QUANTIS_MODULE_1_SHIFT : 0)
       | (mask&(1<<2) ? 1<<QUANTIS_MODULE_2_SHIFT : 0)
       | (mask&(1<<3) ? 1<<QUANTIS_MODULE_3_SHIFT : 0);

  return reg<<type;
}

static inline int reg2mask(int reg, int type)
{
  int mask;

  reg>>=type;

  mask = (reg&(1<<QUANTIS_MODULE_0_SHIFT) ? (1<<0) : 0)
       | (reg&(1<<QUANTIS_MODULE_1_SHIFT) ? (1<<1) : 0)
       | (reg&(1<<QUANTIS_MODULE_2_SHIFT) ? (1<<2) : 0)
       | (reg&(1<<QUANTIS_MODULE_3_SHIFT) ? (1<<3) : 0) ;

  return mask;
}

static inline unsigned int possibleRead(u32 fifoStatus)
{
  if (fifoStatus&QUANTIS_BIT_FS_FSFUL)
  {
    return IDQ_FD_SIZE;
  }

  if (fifoStatus&QUANTIS_BIT_FS_FSFL3)
  {
    return (3*IDQ_FD_SIZE)/4;
  }

  if (fifoStatus&QUANTIS_BIT_FS_FSFL2)
  {
    return (2*IDQ_FD_SIZE)/4;
  }

  if (fifoStatus&QUANTIS_BIT_FS_FSFL1)
  {
    return (1*IDQ_FD_SIZE)/4;
  }

  if (!(fifoStatus&QUANTIS_BIT_FS_FSEMP))
  {
    return 1;
  }

  return 0;
}

static inline u32 moduleError(u32 moduleStatus)
{
  u32 test  =reg2mask(moduleStatus, QUANTIS_MX_TM);
  u32 status=reg2mask(moduleStatus, QUANTIS_MX_HST);
  u32 enable=reg2mask(moduleStatus, QUANTIS_MX_SEN);
  
  if (test)
  {
    return 0;
  }
  else
  {
    return (enable&~status);
  }
}

#endif // QUANTIS_PCI_H
