// EzPciDll.h: interface for eZPCI dynamic link library.
//
//////////////////////////////////////////////////////////////////////

#if !defined(EzPciDll_H__3FB31078_72EF_11D2_87A3_00105A20D903__INCLUDED_)
#define EzPciDll_H__3FB31078_72EF_11D2_87A3_00105A20D903__INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#define  MAX_MEM_REGISTERS    9
#define  MAX_IO_PORTS         20
#define  MAX_IRQS             7
#define  MAX_DMA_CHANNELS     7

#define  DEBUGSTRMAX 256

typedef struct CardConfig
{
   DWORD wNumMemWindows;               // Num memory windows
   DWORD dMemBase[MAX_MEM_REGISTERS];  // Memory window base
   DWORD dMemLength[MAX_MEM_REGISTERS];// Memory window length
   DWORD wMemAttrib[MAX_MEM_REGISTERS];// Memory window Attrib
   DWORD wNumIOPorts;                  // Num IO ports
   DWORD wIOPortBase[MAX_IO_PORTS];    // I/O port base
   DWORD wIOPortLength[MAX_IO_PORTS];  // I/O port length
   DWORD wNumIRQs;                     // Num IRQ info
   DWORD bIRQRegisters[MAX_IRQS];      // IRQ list
   DWORD bIRQAttrib[MAX_IRQS];         // IRQ Attrib list
   DWORD wNumDMAs;                     // Num DMA channels
   DWORD bDMALst[MAX_DMA_CHANNELS];    // DMA list
   DWORD wDMAAttrib[MAX_DMA_CHANNELS]; // DMA Attrib list
   DWORD bReserved1[3];                // Reserved
   DWORD devnode;                      // Adress of Devnode in Registry
   DWORD RTCIRQHandle;                 // Handle of Interrupt
}  CARDCONFIG;                         // Config buffer info


typedef struct PciConfig
{
   WORD  VendorID;
   WORD  DeviceID;
   WORD  Command; 
   WORD  Status;
   BYTE  RevisionID;
   BYTE  ProgIf;    
   BYTE  SubClass;  
   BYTE  BaseClass; 
   BYTE  CacheLineSize;
   BYTE  LatencyTimer; 
   BYTE  HeaderType;   
   BYTE  BIST;          
   DWORD BaseAddresses[6];
   DWORD CardBusCISPtr;
   WORD  SubsystemVendorID;
   WORD  SubsystemID;
   DWORD ROMBaseAddress;
   DWORD Reserved2[2];
   BYTE  InterruptLine; 
   BYTE  InterruptPin;  
   BYTE  MinimumGrant;  
   BYTE  MaximumLatency;
} PCICONFIG;

typedef struct EzH
{
   HANDLE handle;
   HANDLE pmynode;
   DWORD  rndnbr;
} EZHHANDLE;

#define EZH EZHHANDLE*

// library
 BOOL Ez_Library_Init(void);
// main
 EZH   _stdcall Ez_Open(DWORD cardid);
 EZH   _stdcall Ez_Open_Exclusive(DWORD cardid);
 BOOL  _stdcall Ez_Close(EZH cardh);
// 2th main
 DWORD      _stdcall Ez_Count_Cards (void);
 DWORD      _stdcall Ez_Get_CardID (DWORD cardnr);
 EZH        _stdcall Ez_Open_Mode(DWORD cardid, BOOL exclusive);
 BOOL       _stdcall Ez_Set_Exclusive(EZH cardh);
 BOOL       _stdcall Ez_Get_Exclusive(EZH cardh);
 BOOL       _stdcall Ez_Unset_Exclusive(EZH cardh);
 DWORD      _stdcall Ez_Get_CID(EZH cardh);
 DWORD      _stdcall Ez_Get_SID(EZH cardh);
 CARDCONFIG _stdcall Ez_Get_Config (EZH cardh);
 PCICONFIG  _stdcall Ez_Get_PciConfig (EZH cardh);
 BOOL       _stdcall Ez_Get_Version(DWORD *dwMajor, DWORD *dwMinor); 

// write
 BOOL    _stdcall Ez_Write_Dword(EZH cardh, WORD port, DWORD dwoffset, DWORD data);
 BOOL    _stdcall Ez_Write_Block(EZH cardh, WORD port, DWORD dwoffset, DWORD *buff, DWORD count);
 BOOL    _stdcall Ez_Write_Serial(EZH cardh, WORD port, DWORD dwoffset, DWORD *buff, DWORD count);
 BOOL    _stdcall Ez_Write_Bit(EZH cardh, WORD port, DWORD dwoffset, DWORD data, DWORD mask);
// read
 DWORD   _stdcall Ez_Read_Dword(EZH cardh, WORD port, DWORD dwoffset, BOOL *ok);
 BOOL    _stdcall Ez_Read_Block(EZH cardh, WORD port, DWORD dwoffset, DWORD *buff, DWORD count);
 BOOL    _stdcall Ez_Read_Serial(EZH cardh, WORD port, DWORD dwoffset, DWORD *buff, DWORD count);
 DWORD   _stdcall Ez_Read_Bit(EZH cardh, WORD port, DWORD dwoffset, DWORD mask, BOOL *ok);
// interrupt functions
 HANDLE  _stdcall Ez_Start_Irq(EZH cardh);
 BOOL    _stdcall Ez_Stop_Irq(EZH cardh);
 DWORD   _stdcall Ez_Get_Irq_Count(EZH cardh);
 BOOL    _stdcall Ez_Set_Irq_Count(EZH cardh, DWORD count);
// development functions
 void    _stdcall Ez_Test_Interface(void);
 void    _stdcall Ez_Test_Device(EZH cardh);
 void    _stdcall Ez_Debug_Comment(char *str);
// direct memory
 PVOID   _stdcall Ez_Get_Address(EZH cardh, WORD port);
// development
 void  _stdcall EzPrintCardConfig(CARDCONFIG * cardconfig, BOOL withwindow);
 void  _stdcall EzPrintPciConfig(PCICONFIG *pciconfig, BOOL withwindow);
 DWORD _stdcall Ez_Read_PCIConfig(EZH cardh, DWORD dwoffset, BOOL *ok);
 BOOL  _stdcall Ez_Write_PCIConfig(EZH cardh, DWORD dwoffset, DWORD data); 

#ifdef __cplusplus
}
#endif
#endif // !defined(EzPciDll_H__3FB31078_72EF_11D2_87A3_00105A20D903__INCLUDED_)
// EzPciDll.h: interface for eZPCI dynamic link library.
//
//////////////////////////////////////////////////////////////////////
