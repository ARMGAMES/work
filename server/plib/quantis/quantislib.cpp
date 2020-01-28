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

/***************************************************************************/
/**
 * \file quantislib.cpp
 * \author M. Pignat / id Quantique
 * Quantis-PCI library source file.
 */
/***************************************************************************/

#include <windows.h>  
#include "quantis.h"
#include "quantis_pci.h"
#include "ezpcidll.h"

#define QUANTIS_LIB_VERSION 11
#define QUANTIS_MAX_CARDS 10
#define MIN(__A__,__B__) (__A__<__B__) ? __A__ : __B__

static int initialized=0;
static int quantis_count=0;
static quantis_regs_t *quantis_regs[QUANTIS_MAX_CARDS];

int quantisInit()
{
	int i;
	EZH cardHandle;

      if (!initialized)
      {
        for(i=0;i<sizeof(quantis_regs)/sizeof(quantis_regs[0]);i++)
        {
          quantis_regs[i]=0x0;
        }

        quantis_count=MIN(Ez_Count_Cards(),QUANTIS_MAX_CARDS);
        
        // Open a handler for each PCI board found
        for(i=0;i<quantis_count;i++) 
        {
          cardHandle=Ez_Open(Ez_Get_CardID (i+1));
          quantis_regs[i]=(quantis_regs_t*)Ez_Get_Address(cardHandle, QUANTIS_PCI_REGION);
          quantisBoardReset(i);
        }
      }
      initialized++;

	  return quantis_count>0;
}

int quantisGetModules(int cardNumber)
{
  int moduleMask;

  // Check for parameter validity
	if (cardNumber>=quantis_count)
	{
		return 0;
	}
 
  // Get modules mask
  moduleMask=reg2mask(quantis_regs[cardNumber]->MX_SR, QUANTIS_MX_HEN);

  return moduleMask;
}



int quantisBoardVersion(int cardNumber)
{
	int cardVersion;
	
  // Check for parameter validity
  if (cardNumber >= quantis_count)
	{
		return 0; 
	}

  // Get board vhdl version
  cardVersion=quantis_regs[cardNumber]->CV_SR;

  return cardVersion;
}



int quantisLibVersion(void)
{
  return QUANTIS_LIB_VERSION;
}



int quantisDriverVersion(void)
{
  DWORD major;
  DWORD minor;
  
  // get ezPCI driver version
  Ez_Get_Version(&major, &minor); 

  return 10*major+minor;
}



int quantisRead(int cardNumber, void *buf, unsigned int size)
{
	//EZH cardHandle;
  DWORD temp;
  unsigned int toRead;
  DWORD status;
  quantis_regs_t *regs;

  char *buffer=(char *)buf;
  int timeOut;
  unsigned int possibleCount;

  int fifocount=0;
  
  // Check for parameter validity
  if (cardNumber>=quantis_count)
	{
		return -1; 
	}

	regs=quantis_regs[cardNumber];

	if (regs==NULL)
	{
		return -1;
	}

  if( quantisModulesStatus(cardNumber) )
  {
    return -1;
  }

  toRead=size;

  while(toRead!=0)
  {
again:
    timeOut=QUANTIS_TIMEOUT;

    // wait while empty
    do
    {
      status=regs->FS_RR;

      timeOut--;

      if (timeOut<=0)
      {
        break;
      }
    }
    while(status&QUANTIS_BIT_FS_FSEMP);

    if(timeOut<=0)
    {
      return size-toRead;
    }

    possibleCount=MIN(possibleRead(status)*4,toRead);

    while(possibleCount!=0)
    {
      temp=regs->FD_RR;
      if (regs->FS_RR & QUANTIS_BIT_FS_FSERR) {
  	  quantis_regs[cardNumber]->FS_CA=0x0;
	  goto again;
      }

      if(possibleCount>3)
      {
        *(unsigned int*)buffer=temp;
        buffer+=sizeof(temp);
        toRead-=sizeof(temp);
        possibleCount-=4;
      }
      else
      {
        memcpy(buffer,&temp,possibleCount);
        buffer+=toRead;
        possibleCount-=toRead;
        toRead-=toRead;
      }
    }
  }
  
  return size;
}



int quantisBoardReset(int cardNumber)
{
  // Check for parameter validity
  if (cardNumber>=quantis_count)
	{
		return -1; 
	}

  if(quantis_regs[cardNumber]==NULL)
  {
    return -2;
  }
  
  quantis_regs[cardNumber]->CC_ER=QUANTIS_BIT_CC_SWRST;
  quantis_regs[cardNumber]->CC_DR=QUANTIS_BIT_CC_SWRST;

  return 0;
}



int quantisModulesEnable(int cardNumber, int moduleMask)
{

  // Check for parameter validity
  if (cardNumber>=quantis_count)
	{
		return -1; 
	}
  
  // Enable requiered modules
  quantis_regs[cardNumber]->MX_ER=mask2reg(moduleMask,QUANTIS_MX_SEN);
  
	return 0;
}



int quantisModulesDisable(int cardNumber, int moduleMask)
{
  // Check for parameter validity
  if (cardNumber>=quantis_count)
	{
		return -1; 
	}
  
  // Enable disable modules
  quantis_regs[cardNumber]->MX_DR=mask2reg(moduleMask,QUANTIS_MX_SEN);

	return 0;
}



int quantisModulesReset(int cardNumber, int moduleMask)
{
  quantisModulesDisable(cardNumber, moduleMask);
  return quantisModulesEnable(cardNumber, moduleMask);
}



int quantisSetTestMode(int cardNumber, int moduleMask)
{
  // Check for parameter validity
  if (cardNumber>=quantis_count)
	{
		return -1; 
	}
  
  quantis_regs[cardNumber]->MX_DR=mask2reg(0xf,QUANTIS_MX_SEN)|mask2reg(0xf,QUANTIS_MX_TM);
  
  // Reset FIFO and counter
  quantis_regs[cardNumber]->FS_CA, 0x0;
  
  //empty fifo
  while(!quantis_regs[cardNumber]->FS_RR&QUANTIS_BIT_FS_FSEMP )
  {
    (void)quantis_regs[cardNumber]->FD_RR;
  }

  // Enable test mode
  quantis_regs[cardNumber]->MX_ER=mask2reg(moduleMask,QUANTIS_MX_TM);

  // Reset FIFO and counter
  quantis_regs[cardNumber]->FS_CA=0x0;

	return 0;
}

int quantisModulesStatus(int cardNumber)
{
  unsigned int enable;
  unsigned int status;
  unsigned int reg;

  // Check for parameter validity
  if (cardNumber>=quantis_count)
	{
		return -1; 
	}

 	reg=quantis_regs[cardNumber]->MX_SR;

  enable=reg2mask(reg, QUANTIS_MX_SEN);
  status=reg2mask(reg, QUANTIS_MX_HST);

  return (enable&~status);
}

int quantisCount(void)
{
	return Ez_Count_Cards();
}

