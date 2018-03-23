// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <Windows.h>
#include <WinIoCtl.h>
#include <Shlwapi.h>
#include <stdio.h>
#include <tchar.h>



//--------------------------------------------------------------------
//     N T F S C O N T R O L F I L E   D E F I N I T I O N S
//--------------------------------------------------------------------

//
// Prototype for NtFsControlFile and data structures
// used in its definition
//

//
// Io Status block (see NTDDK.H)
//
typedef struct _IO_STATUS_BLOCK {
    NTSTATUS Status;
    ULONG Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;


//
// Apc Routine (see NTDDK.H)
//
typedef VOID (*PIO_APC_ROUTINE) (
                                 PVOID ApcContext,
                                 PIO_STATUS_BLOCK IoStatusBlock,
                                 ULONG Reserved
                                 );


//
// The undocumented NtFsControlFile
//
// This function is used to send File System Control (FSCTL)
// commands into file system drivers. Its definition is 
// in ntdll.dll (ntdll.lib), a file shipped with the NTDDK.
//
typedef NTSTATUS (__stdcall *NTFSCONTROLFILE)( 
                                      HANDLE FileHandle,
                                      HANDLE Event,                                   // optional
                                      PIO_APC_ROUTINE ApcRoutine,             // optional
                                      PVOID ApcContext,                               // optional
                                      PIO_STATUS_BLOCK IoStatusBlock, 
                                      ULONG FsControlCode,
                                      PVOID InputBuffer,                              // optional
                                      ULONG InputBufferLength,
                                      PVOID OutputBuffer,                             // optional
                                      ULONG OutputBufferLength
                                      );

typedef enum _FSINFOCLASS {
    FileFsVolumeInformation       = 1,
    FileFsLabelInformation,      // 2
    FileFsSizeInformation,       // 3
    FileFsDeviceInformation,     // 4
    FileFsAttributeInformation,  // 5
    FileFsControlInformation,    // 6
    FileFsFullSizeInformation,   // 7
    FileFsObjectIdInformation,   // 8
    FileFsDriverPathInformation, // 9
    FileFsVolumeFlagsInformation,// 10
    FileFsMaximumInformation
} FS_INFORMATION_CLASS, *PFS_INFORMATION_CLASS;

typedef struct _FILE_FS_SIZE_INFORMATION {
    LARGE_INTEGER TotalAllocationUnits;
    LARGE_INTEGER AvailableAllocationUnits;
    ULONG SectorsPerAllocationUnit;
    ULONG BytesPerSector;
} FILE_FS_SIZE_INFORMATION, *PFILE_FS_SIZE_INFORMATION;

typedef NTSTATUS (__stdcall * NTQUERYVOLUMEINFORMATIONFILE)(
  _In_   HANDLE FileHandle,
  _Out_  PIO_STATUS_BLOCK IoStatusBlock,
  _Out_  PVOID FsInformation,
  _In_   ULONG Length,
  _In_   FS_INFORMATION_CLASS FsInformationClass
);


typedef struct _VOLUME_SIZE
{
    LARGE_INTEGER   Length;
    LARGE_INTEGER   TotalClusters;
    ULONG           SectorsPerCluster;
    ULONG           BytesPerSector;
} VOLUME_SIZE, *PVOLUME_SIZE;


#pragma pack( 1 )

typedef struct _BPB
{
    USHORT  BytesPerSector;
    BYTE    SectorsPerCluster;
    USHORT  ReservedSectors;
    BYTE    NumberOfFats;
    USHORT  RootDirEntries;
    USHORT  TotalSectorsSmall;           //  only used for small volumes (under 32 MB)
    BYTE    MediaDescriptor;
    USHORT  SectorsPerFat;
    USHORT  SectorsPerTrack;
    USHORT  Heads;
    ULONG   HiddenSectors;
    ULONG   TotalSectorsLarge;          //  used on large volumes
} BPB, *PBPB;

typedef struct _FAT
{
    BYTE    bJmp[3];
    BYTE    OEM[8];
    BPB     Bpb;
    BYTE    Id;
    BYTE    Unused;                     // 0
    BYTE    ExtBootSignature;           //  0x29
    ULONG   SerialNumber;
    BYTE    Label[11];
    BYTE    FileSystem[8];
    BYTE    BootCode[448];
    USHORT  BiosSignature;              //  0x55AA
} FAT, *PFAT;

typedef struct _FAT32
{
    BYTE    bJmp[3];
    BYTE    OEM[8];
    BPB     Bpb;
    ULONG   SectorsPerFat;
    BYTE    ExtendedFlags;
    BYTE    FatMirrorDisabled;
    USHORT  Version;
    ULONG   RootDir;
    USHORT  FsInfo;
    USHORT  BackupBootSector;
    BYTE    Reserved[12];
    BYTE    Id;
    BYTE    Unused;                     // 0
    BYTE    ExtBootSignature;           //  0x29
    ULONG   SerialNumber;
    BYTE    Label[11];
    BYTE    FileSystem[8];
    BYTE    BootCode[420];
    USHORT  BiosSignature;              //  0x55AA
} FAT32, *PFAT32;

typedef struct _NTFS
{
    BYTE        bJmp[3];
    BYTE        OEM[8];
    BPB         Bpb;
    ULONG       Unused1;
    ULONGLONG   TotalSectors;
    ULONGLONG   StartMFT;
    ULONGLONG   StartMFTMir;
    BYTE        FileRecordSize;
    BYTE        Unused2[3];
    BYTE        IndexBufferSize;
    BYTE        Pad[3];
    ULONGLONG   SerialNumber;
    BYTE        CheckSum;
    BYTE        BootCode[429];
    USHORT      BiosSignature;              //  0x55AA
} NTFS, *PNTFS;

typedef struct _BIOS_BOOT_SECTOR
{
    BYTE    Buffer[510];
    USHORT  BiosSignature;              //  0x55AA
} BIOS_BOOT_SECTOR, *PBIOS_BOOT_SECTOR;

typedef enum _FS_TYPE
{
    FS_UNKNOWN,
    FS_FAT12,
    FS_FAT16,
    FS_FAT32,
    FS_NTFS,
    FS_REFS,
    FS_MAX
} FS_TYPE;

typedef struct _BOOT_SECTOR
{
    FS_TYPE FsType;
    union
    {
        FAT                 Fat;
        FAT32               Fat32;
        NTFS                Ntfs;
        BIOS_BOOT_SECTOR    Bs;
    };
} BOOT_SECTOR, *PBOOT_SECTOR;

static const CHAR   _OEM_REFS[]  = "ReFS";
static const CHAR   _OEM_NTFS[]  = "NTFS    ";
static const CHAR   _OEM_FAT12[] = "FAT12   ";
static const CHAR   _OEM_FAT16[] = "FAT16   ";
static const CHAR   _OEM_FAT32[] = "FAT32   ";

#define FS_NAME_LENGTH      8

static const TCHAR  *_FsName[] = {
    _T("Unknown FileSystem"),
    _T("FAT12"),
    _T("FAT16"),
    _T("FAT32"),
    _T("NTFS"),
    _T("ReFS"),
    NULL
};

#define SECTOR_SIZE   512