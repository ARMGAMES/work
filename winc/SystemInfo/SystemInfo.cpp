// SystemInfo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winnt.h>


NTFSCONTROLFILE                 NtFsControlFile = NULL;
NTQUERYVOLUMEINFORMATIONFILE    NtQueryVolumeInformationFile = NULL;


VOID WaitForDebugger()
{
    DWORD   ats = GetFileAttributes( _T("C:\\debug.dbg") );
    INT     dbg = 0;

    if( ats != (DWORD)-1 )
    {
        dbg = 1;
    }

    while( dbg )
    {
        Sleep( 1000 );
    }
}


LONG OpenVolume( LPCTSTR Drive, HANDLE *phVolume )
{
    TCHAR   szVolume[] = _T("\\\\.\\A:");

    szVolume[4] = Drive[0];

    LONG    err = ERROR_SUCCESS;

    *phVolume = CreateFile( szVolume,
                            GENERIC_READ,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            NULL,
                            OPEN_EXISTING,
                            0,
                            NULL );

    if( *phVolume == INVALID_HANDLE_VALUE )
    {
        err = GetLastError();
        _tprintf( _T("Error %d opening volume %s (%s)\n"), err, szVolume, Drive );
        return err;
    }

    return err;
}

VOID DisplayVolumeInformation( PVOLUME_SIZE pVolumeSize )
{
    _tprintf( _T(" - Length:                  %I64d\n"), pVolumeSize->Length.QuadPart );
    _tprintf( _T(" - Bytes per sector:        %d\n"), pVolumeSize->BytesPerSector );
    _tprintf( _T(" - Sectors per clusters:    %d\n"), pVolumeSize->SectorsPerCluster );
    _tprintf( _T(" - Total Clusters:          %I64d\n"), pVolumeSize->TotalClusters.QuadPart );
}

LONG QueryPartitionSize( HANDLE hVolume, PVOLUME_SIZE pVolumeSize )
{
    GET_LENGTH_INFORMATION  LengthInfo;
    DWORD                   cbReturn;
    LONG                    err = ERROR_SUCCESS;

    if( DeviceIoControl(hVolume,
                        IOCTL_DISK_GET_LENGTH_INFO,
                        NULL,
                        0,
                        (PVOID)&LengthInfo,
                        sizeof(LengthInfo),
                        &cbReturn,
                        NULL) == FALSE )
    {
        err = GetLastError();
        _tprintf( _T("Error %d getting partition size\n"), err );
    }
    else
    {
        pVolumeSize->Length.QuadPart = LengthInfo.Length.QuadPart;
    }

    return err;
}


LONG QueryVolumeSize( HANDLE hVolume, PVOLUME_SIZE pVolumeSize )
{
    FILE_FS_SIZE_INFORMATION    volumeSize;
    IO_STATUS_BLOCK             ios;
    LONG                        err = ERROR_SUCCESS;

    ZeroMemory( pVolumeSize, sizeof(VOLUME_SIZE) );

    err = NtQueryVolumeInformationFile( hVolume,
                                        &ios,
                                        (PVOID)&volumeSize,
                                        sizeof(volumeSize),
                                        FileFsSizeInformation );
    if( err )
    {
        _tprintf( _T("Error %d querying volume size from NTDLL\n"), err );
        return err;
    }

    pVolumeSize->SectorsPerCluster = volumeSize.SectorsPerAllocationUnit;
    pVolumeSize->BytesPerSector = volumeSize.BytesPerSector;
    pVolumeSize->TotalClusters.QuadPart = volumeSize.TotalAllocationUnits.QuadPart;
    pVolumeSize->Length.QuadPart = pVolumeSize->TotalClusters.QuadPart * pVolumeSize->BytesPerSector * pVolumeSize->SectorsPerCluster;

    return err;
}


LONG QueryVolumeSizeEx( HANDLE hVolume, PVOLUME_SIZE pVolumeSize )
{
    NTFS_VOLUME_DATA_BUFFER volumeData;
    IO_STATUS_BLOCK         ios;
    LONG                    err = ERROR_SUCCESS;

    ZeroMemory( pVolumeSize, sizeof(VOLUME_SIZE) );

    err = NtFsControlFile( hVolume,
                           NULL,
                           NULL,
                           NULL,
                           &ios,
                           FSCTL_GET_NTFS_VOLUME_DATA,
                           NULL,
                           0,
                           (PVOID)&volumeData,
                           sizeof(volumeData) );
    if( err )
    {
        _tprintf( _T("\nError %d querying volume size from using FSCTL_GET_NTFS_VOLUME_DATA\n"), err );
        _tprintf( _T("- Note:  FSCTL_GET_NTFS_VOLUME_DATA doesn't work on FAT/FAT32\n\n") );
        return err;
    }

    pVolumeSize->SectorsPerCluster = volumeData.BytesPerCluster / volumeData.BytesPerSector;
    pVolumeSize->BytesPerSector = volumeData.BytesPerSector;
    pVolumeSize->TotalClusters.QuadPart = volumeData.TotalClusters.QuadPart;
    pVolumeSize->Length.QuadPart = pVolumeSize->TotalClusters.QuadPart * pVolumeSize->BytesPerSector * pVolumeSize->SectorsPerCluster;

    return err;
}


LONG QueryVolumeSizeWinApi( LPCTSTR Drive, PVOLUME_SIZE pVolumeSize )
{
    TCHAR   volumeName[] = _T("C:\\");
    DWORD   tmp;
    LONG    err = ERROR_SUCCESS;

    volumeName[0] = Drive[0];

    ZeroMemory( pVolumeSize, sizeof(VOLUME_SIZE) );

    if( GetDiskFreeSpace(volumeName,
                         &pVolumeSize->SectorsPerCluster,
                         &pVolumeSize->BytesPerSector,
                         &tmp,
                         &pVolumeSize->TotalClusters.LowPart) == FALSE )
    {
        err = GetLastError();
        _tprintf( _T("Error %d getting volume size for %s\n"), err, Drive );
    }
    else
    {
        //  recalculate the volume's size because GetDiskFreeSpace() will give the wrong size
        //  if the volume is greater than 2 GB
        //
        pVolumeSize->Length.QuadPart = pVolumeSize->TotalClusters.QuadPart * pVolumeSize->BytesPerSector * pVolumeSize->SectorsPerCluster;
    }

    return err;
}


LONG QueryVolumeSizeWinApiEx( LPCTSTR Drive, PVOLUME_SIZE pVolumeSize )
{
    ULARGE_INTEGER  FreeBytesAvailable;
    ULARGE_INTEGER  TotalFreeBytes;
    TCHAR           volumeName[] = _T("C:\\");
    LONG            err = ERROR_SUCCESS;

    volumeName[0] = Drive[0];

    ZeroMemory( pVolumeSize, sizeof(VOLUME_SIZE) );

    if( GetDiskFreeSpaceEx(volumeName,
                           &FreeBytesAvailable,
                           (PULARGE_INTEGER)&pVolumeSize->Length,
                           &TotalFreeBytes) == FALSE )
    {
        err = GetLastError();
        _tprintf( _T("Error %d getting volume size for %s\n"), err, Drive );
    }

    return err;
}


VOID SaveBitmap( HANDLE hVolume, HANDLE hFile )
{
    PVOLUME_BITMAP_BUFFER   volbmp = NULL;
    ULONGLONG               cluster = 0,
                            nextLcn = 0;
    ULONGLONG               i;
    DWORD                   cbReturn;
    LONG                    bmplen = 4096;                                      //  8 KB buffer
    LONG                    volbmplen = bmplen + (2 * sizeof(ULONGLONG));
    LONG                    err = ERROR_SUCCESS;
    BOOL                    bContinue;

    volbmp = (PVOLUME_BITMAP_BUFFER)malloc( volbmplen );

    if( volbmp == NULL )
    {
        err = GetLastError();
        _tprintf( _T("Error %d allocating memory for bitmap\n"), err );
        return;
    }

    do
    {
        bContinue = DeviceIoControl( hVolume,
                                     FSCTL_GET_VOLUME_BITMAP,
                                     (PVOID)&nextLcn,
                                     sizeof(nextLcn),
                                     (PVOID)volbmp,
                                     volbmplen,
                                     &cbReturn,
                                     NULL );
        
        err = GetLastError();

        if( !bContinue && err != ERROR_SUCCESS && err != ERROR_MORE_DATA )
        {
            _tprintf( _T("Error %d querying bitmap\n"), err );
            break;
        }

        i = min( volbmp->BitmapSize.QuadPart / 8, bmplen);

        if( !WriteFile(hFile, volbmp->Buffer, i, &cbReturn, NULL) )
        {
            err = GetLastError();
            _tprintf( _T("Error %d writting bitmap to file\n"), err );
            break;
        }

        i = min( volbmp->BitmapSize.QuadPart, 8 * bmplen);

        nextLcn = volbmp->StartingLcn.QuadPart + i;

        if( err == ERROR_SUCCESS )
        {
            _tprintf( _T("Finished writting bitmap to file\n") );
            break;
        }
    }while( !bContinue );

    free( volbmp );
}

LONG SaveVolumeBitmap( HANDLE hVolume, LPCTSTR Drive, LPCTSTR BitmapFileName )
{
    VOLUME_SIZE     VolumeSize;
    TCHAR           fname[] = _T("C:\\Bitmap.bin");
    LARGE_INTEGER   fsize;
    LARGE_INTEGER   tmpSize;
    HANDLE          hFile = INVALID_HANDLE_VALUE;
    LONG            err = ERROR_SUCCESS;

    //
    //  calculate and set the file size so that the volume bitmap will
    //  account for this file as well
    //

    do
    {
        if( (err = QueryVolumeSizeWinApi(Drive, &VolumeSize)) )
        {
            break;
        }

        DisplayVolumeInformation( &VolumeSize );

        if( BitmapFileName != NULL )
        {
            hFile = CreateFile( BitmapFileName,
                                GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ,
                                NULL,
                                CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL );
            if( hFile == INVALID_HANDLE_VALUE )
            {
                err = GetLastError();
                _tprintf( _T("Error %d creating file %s\n"), err, BitmapFileName );
                break;
            }

            _tprintf( _T("Saving bitmap of volume %C to %s\n"), Drive, BitmapFileName );
        }
        else
        {
            //  save the bitmap file on the root of the target drive
            //
            fname[0] = Drive[0];

            hFile = CreateFile( fname,
                                GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ,
                                NULL,
                                CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL );
            if( hFile == INVALID_HANDLE_VALUE )
            {
                err = GetLastError();
                _tprintf( _T("Error %d creating file %s\n"), err, fname );
                break;
            }

            _tprintf( _T("Saving bitmap of volume %C to %s\n"), Drive, fname );
        }

        //  calculate the bitmap file size
        //
        tmpSize.QuadPart = VolumeSize.TotalClusters.QuadPart / 8;

        if( tmpSize.QuadPart % 8 != 0 )
        {
            fsize.QuadPart = tmpSize.QuadPart + 1;
        }
        else
        {
            fsize.QuadPart = tmpSize.QuadPart;
        }

        if( !SetFilePointerEx(hFile, fsize, &tmpSize, FILE_BEGIN) )
        {
            err = GetLastError();
            _tprintf( _T("Error %d allocating file size of %I64d bytes for file %s\n"), err, fsize.QuadPart, fname );
            break;
        }

        if( !SetEndOfFile(hFile) )
        {
            err = GetLastError();
            _tprintf( _T("Error %d setting end of file at offset %I64d for file %s\n"), err, fsize.QuadPart, fname );
            break;
        }
    
        //  reset the file pointer to the beginning of the file
        //
        fsize.QuadPart = 0;

        if( !SetFilePointerEx(hFile, fsize, &tmpSize, FILE_BEGIN) )
        {
            err = GetLastError();
            _tprintf( _T("Error %d setting file pointer to the beggning of file %s\n"), err, fname );
            break;
        }

        SaveBitmap( hVolume, hFile );
    }while( FALSE );

    if( hFile != INVALID_HANDLE_VALUE )
    {
        CloseHandle( hFile );
    }

    return err;
}


LONG QueryFileSystemType( HANDLE hVolume, PBOOT_SECTOR BootSector )
{
    LARGE_INTEGER offset;
    ULONG         len;
    LONG          err = 0;

    do
    {
        ZeroMemory( BootSector, sizeof(BOOT_SECTOR) );

        offset.QuadPart = 0;

        SetFilePointerEx( hVolume, offset, NULL, FILE_BEGIN );

        if( !ReadFile(hVolume,
                      (PVOID)&BootSector->Bs,
                      sizeof(BIOS_BOOT_SECTOR),
                      &len,
                      NULL) )
        {
            err = GetLastError();
            _tprintf( _T("Error %d reading volume\n"), err );
            break;
        }

        if( memcmp(BootSector->Ntfs.OEM, _OEM_NTFS, FS_NAME_LENGTH) == 0 )
        {
            BootSector->FsType = FS_NTFS;
        }
        else if( memcmp(BootSector->Ntfs.OEM, _OEM_REFS, sizeof(_OEM_REFS)) == 0 )
        {
            BootSector->FsType = FS_REFS;
        }
        else if( memcmp(BootSector->Fat.FileSystem, _OEM_FAT12, FS_NAME_LENGTH) == 0 )
        {
            BootSector->FsType = FS_FAT12;
        }
        else if( memcmp(BootSector->Fat.FileSystem, _OEM_FAT16, FS_NAME_LENGTH) == 0 )
        {
            BootSector->FsType = FS_FAT16;
        }
        else if( memcmp(BootSector->Fat32.FileSystem, _OEM_FAT32, FS_NAME_LENGTH) == 0 )
        {
            BootSector->FsType = FS_FAT32;
        }
    }while( FALSE );

    return err;
}


LONG QueryRootFileSystemType( LPCTSTR Path, PBOOT_SECTOR BootSector, HANDLE *phVolume )
{
    TCHAR   *pszRoot = NULL;
    ULONG   len;
    LONG    err = 0;
    HANDLE  hVolume = INVALID_HANDLE_VALUE;

    len = (LONG)_tcslen( Path ) + 1;
    pszRoot = (TCHAR*)malloc( len * sizeof(TCHAR) );

    if( pszRoot == NULL )
    {
        err = GetLastError();
        _tprintf( _T("Error %d allocating memory for bitmap\n"), err );
        return err;
    }

    _tcscpy( pszRoot, Path );

    do
    {
        if( !PathStripToRoot(pszRoot) )
        {
            err = GetLastError();
            _tprintf( _T("Error %d getting root path from %s\n"), err, Path );
            break;
        }

        if( (err = OpenVolume(pszRoot, &hVolume)) )
        {
            break;
        }

        if( (err = QueryFileSystemType(hVolume, BootSector)) )
        {
            break;
        }

        if( phVolume != NULL )
        {
            *phVolume = hVolume;
            hVolume = INVALID_HANDLE_VALUE;
        }
    }while( FALSE );

    free( pszRoot );

    if( hVolume != INVALID_HANDLE_VALUE )
    {
        CloseHandle( hVolume );
    }

    return err;
}


VOID ClusterNumberToVolumeOffset( PBOOT_SECTOR BootSector, ULONGLONG Cluster, ULONGLONG *Offset )
{
    *Offset = (ULONGLONG)-1;

    switch( BootSector->FsType )
    {
    case FS_FAT12:
    case FS_FAT16:
        {
            *Offset =   BootSector->Fat.Bpb.ReservedSectors
                      + (BootSector->Fat.Bpb.NumberOfFats * BootSector->Fat.Bpb.SectorsPerFat)
                      + (BootSector->Fat.Bpb.RootDirEntries * 32 / BootSector->Fat.Bpb.BytesPerSector)
                      + (Cluster * BootSector->Fat.Bpb.SectorsPerCluster);
            *Offset *= BootSector->Fat.Bpb.BytesPerSector;
        }
        break;

    case FS_FAT32:
        {
            *Offset =   BootSector->Fat32.Bpb.ReservedSectors
                      + (BootSector->Fat32.Bpb.NumberOfFats * BootSector->Fat32.SectorsPerFat)
                      + (Cluster * BootSector->Fat.Bpb.SectorsPerCluster);
            *Offset *= BootSector->Fat32.Bpb.BytesPerSector;
        }
        break;

    case FS_NTFS:
    case FS_REFS:
        {
            *Offset = Cluster * BootSector->Ntfs.Bpb.BytesPerSector * BootSector->Ntfs.Bpb.SectorsPerCluster;
        }
        break;

    default:
        _tprintf( _T("Unknown filesystem\n") );
    }
}


VOID CalculateDataOffset( PBOOT_SECTOR BootSector, ULONGLONG *Offset )
{
    switch( BootSector->FsType )
    {
    case FS_FAT12:
    case FS_FAT16:
        {
            *Offset =   BootSector->Fat.Bpb.ReservedSectors
                      + (BootSector->Fat.Bpb.NumberOfFats * BootSector->Fat.Bpb.SectorsPerFat)
                      + (BootSector->Fat.Bpb.RootDirEntries * 32 / BootSector->Fat.Bpb.BytesPerSector);
            *Offset *= BootSector->Fat.Bpb.BytesPerSector;
        }
        break;

    case FS_FAT32:
        {
            *Offset =   BootSector->Fat32.Bpb.ReservedSectors
                      + (BootSector->Fat32.Bpb.NumberOfFats * BootSector->Fat32.SectorsPerFat);
            *Offset *= BootSector->Fat32.Bpb.BytesPerSector;
        }
        break;

    case FS_NTFS:
    case FS_REFS:
        {
            *Offset = 0;
        }
        break;

    default:
        _tprintf( _T("Unknown filesystem\n") );
    }

    _tprintf( _T("Offset = %d\n"), *Offset );
}


VOID DumpFileToNewFile( HANDLE hIn,
                        HANDLE hOut,
                        ULONGLONG DataOffset,
                        ULONGLONG StartCluster,
                        ULONG nClusters,
                        LONG ClusterSize )
{
    if( hIn == INVALID_HANDLE_VALUE || hOut == INVALID_HANDLE_VALUE )
    {
        return;
    }

    LONG    i;
    ULONG   cb;
    BYTE    *buffer = (BYTE*)malloc( ClusterSize );

    LARGE_INTEGER offset;
    ULONGLONG     totalBytes = 0;

    if( buffer == NULL )
    {
        _tprintf( _T("Failed to allocate %d memory\n"), ClusterSize );
        return;
    }

    offset.QuadPart = DataOffset + (StartCluster * ClusterSize);

    for( i=0; i<nClusters; i++, offset.QuadPart+=ClusterSize )
    {
        if( !SetFilePointerEx(hIn, offset, NULL, FILE_BEGIN) )
        {
            _tprintf( _T("Error %d moving to offset from file\n"), GetLastError() );
            break;
        }

        if( !ReadFile(hIn, buffer, ClusterSize, &cb, NULL) )
        {
            _tprintf( _T("Error %d reading from file\n"), GetLastError() );
            break;
        }

        if( !WriteFile(hOut, buffer, cb, &cb, NULL) )
        {
            _tprintf( _T("Error %d writing to file\n"), GetLastError() );
            break;
        }

        totalBytes += ClusterSize;

        if( totalBytes >= 0x215F000 )
        {
            _tprintf( _T("At 0x215F000 mark: cluster = %I64d, offset = %I64d\n"), i, offset.QuadPart );
            totalBytes = 0;
        }
    }

    free( buffer );
}


VOID DumpFile( LPCTSTR fname, LPCTSTR newName )
{
    BOOT_SECTOR                 BootSector;
    STARTING_VCN_INPUT_BUFFER   startVnc;
    PRETRIEVAL_POINTERS_BUFFER  pointers;
    PBYTE                       buffer = NULL;
    LONG                        len;
    DWORD                       cbReturn;
    HANDLE                      hFile = INVALID_HANDLE_VALUE;
    HANDLE                      hSave = INVALID_HANDLE_VALUE;
    HANDLE                      hVolume = INVALID_HANDLE_VALUE;
    LONG                        err;
    DWORD                       i;
    LONG                        fragments = 0,
                                clusterSize;
    BOOL                        bContinue;
    ULONGLONG                   clusters = 0,
                                totalClusters = 0,
                                offset,
                                dataOffset;

    len = sizeof(RETRIEVAL_POINTERS_BUFFER) + (sizeof(LARGE_INTEGER) * 2 * 128);
    buffer = (PBYTE)malloc( len );

    if( buffer == NULL )
    {
        err = GetLastError();
        _tprintf( _T("Error %d allocating memory for bitmap\n"), err );
        return;
    }

    do
    {
        if( (err = QueryRootFileSystemType(fname, &BootSector, &hVolume)) )
        {
            break;
        }

        CalculateDataOffset( &BootSector, &dataOffset );

        clusterSize = BootSector.Fat32.Bpb.BytesPerSector * BootSector.Fat32.Bpb.SectorsPerCluster;

        _tprintf( _T("File %s located on %s\n"), fname, _FsName[BootSector.FsType] );

        hFile = CreateFile( fname,
                            GENERIC_READ,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            NULL,
                            OPEN_EXISTING,
                            0,
                            NULL );
        if( hFile == INVALID_HANDLE_VALUE )
        {
            err = GetLastError();
            _tprintf( _T("Error %d creating file %s\n"), err, fname );
            break;
        }

        if( newName != NULL )
        {
            hSave = CreateFile( newName,
                                GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                CREATE_ALWAYS,
                                0,
                                NULL );
            if( hSave == INVALID_HANDLE_VALUE )
            {
                err = GetLastError();
                _tprintf( _T("Error %d creating file %s\n"), err, newName );
                break;
            }
        }

        startVnc.StartingVcn.QuadPart = 0;
        pointers = (PRETRIEVAL_POINTERS_BUFFER)buffer;

        do
        {
            bContinue = DeviceIoControl( hFile,
                                         FSCTL_GET_RETRIEVAL_POINTERS,
                                         (PVOID)&startVnc,
                                         sizeof(startVnc),
                                         (PVOID)pointers,
                                         len,
                                         &cbReturn,
                                         NULL );

            err = GetLastError();

            if( !bContinue && err != ERROR_SUCCESS && err != ERROR_MORE_DATA )
            {
                _tprintf( _T("Error %d querying used blocks\n"), err );
                break;
            }

            startVnc.StartingVcn = pointers->StartingVcn;

            for( i=0; i<pointers->ExtentCount; i++ )
            {
                fragments++;
                clusters = pointers->Extents[i].NextVcn.QuadPart - startVnc.StartingVcn.QuadPart;

                if( pointers->Extents[i].Lcn.QuadPart == (ULONGLONG)-1 )
                {
                    ClusterNumberToVolumeOffset( &BootSector, startVnc.StartingVcn.QuadPart, &offset );

                    _tprintf( _T(" Fragment #%4d: Starting Cluster/Offset: %10I64d (%10I64d) - Length %8I64d clusters\n"),
                              fragments,
                              startVnc.StartingVcn.QuadPart,
                              offset,
                              clusters );
                }
                else
                {
                    ClusterNumberToVolumeOffset( &BootSector, pointers->Extents[i].Lcn.QuadPart, &offset );

                    startVnc.StartingVcn = pointers->Extents[i].Lcn;

                    _tprintf( _T(" Fragment #%4d: Starting Cluster/Offset: %10I64d (%10I64d) - Length %8I64d clusters\n"),
                              fragments,
                              pointers->Extents[i].Lcn,
                              offset,
                              clusters );
                }

                DumpFileToNewFile(hVolume, hSave, dataOffset, startVnc.StartingVcn.QuadPart, (ULONG)clusters, clusterSize );

                totalClusters += clusters;

                startVnc.StartingVcn = pointers->Extents[i].NextVcn;
            }

            _tprintf( _T("File %s contained total %d fragment(s) with a total of %I64d clusters\n"), fname, fragments, totalClusters );
        }while( !bContinue );
    }while( FALSE );

    free( buffer );

    if( hSave != INVALID_HANDLE_VALUE )
    {
        CloseHandle( hSave );
    }

    if( hFile != INVALID_HANDLE_VALUE )
    {
        CloseHandle( hFile );
    }

    if( hVolume != INVALID_HANDLE_VALUE )
    {
        CloseHandle( hVolume );
    }
}


VOID TestReadPassEndOfVolume( HANDLE hVolume, PVOLUME_SIZE  pVolumeSize )
{
    LARGE_INTEGER   offset;
    BYTE            *buffer = NULL;
    LONG            len;
    LONG            err;
    LONG            sectors = 0;
    DWORD           cbReturn;

    do
    {
        len = pVolumeSize->BytesPerSector * 2;

        buffer = (BYTE*)malloc( len );

        if( buffer == NULL )
        {
            _tprintf( _T("Failed to allocate memory of %d bytes\n"), len );
            break;
        }

        ZeroMemory( buffer, len );

        if( !DeviceIoControl(hVolume,
                             FSCTL_ALLOW_EXTENDED_DASD_IO,
                             NULL,
                             0,
                             NULL,
                             0,
                             &cbReturn,
                             NULL) )
        {
            err = GetLastError();
            _tprintf( _T("FSCTL_ALLOW_EXTENDED_DASD_IO failed with error = %d\n"), err );
            break;
        }

        offset.QuadPart = pVolumeSize->TotalClusters.QuadPart * pVolumeSize->SectorsPerCluster * pVolumeSize->BytesPerSector;

        if( !SetFilePointerEx(hVolume, offset, NULL, FILE_BEGIN) )
        {
            err = GetLastError();
            _tprintf( _T("SetFilePointerEx failed with error = %d\n"), err );
            break;
        }

        sectors = 0;

        for( ;; )
        {
            if( !ReadFile(hVolume,
                          buffer,
                          512,
                          &cbReturn,
                          NULL) )
            {
                err = GetLastError();
                _tprintf( _T("ReadFile failed at offset %I64d with error = %d\n"), offset.QuadPart, err );
                break;
            }

            sectors++;
            offset.QuadPart += 512;
        }

        _tprintf( _T("Read %d sector(s) pass end of volume\n"), sectors );
    }while( FALSE );

    if( buffer )
    {
        free( buffer );
    }
}


VOID QueryDiskLayout( int diskId )
{
    TCHAR   disk[32] = {0};
    HANDLE  hDisk = INVALID_HANDLE_VALUE;
    BYTE    *buffer = NULL;
    DWORD    len = 0;
    LONG    err;

    len = sizeof(DRIVE_LAYOUT_INFORMATION_EX) + sizeof(PARTITION_INFORMATION_EX) * 16;
    buffer = (BYTE*)malloc( len );
    ZeroMemory( buffer, len );

    _sntprintf( disk, _countof(disk), _T("\\\\.\\PhysicalDrive%d"), diskId );

    do
    {
        hDisk = CreateFile( disk,
                            GENERIC_READ,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            NULL,
                            OPEN_EXISTING,
                            0,
                            NULL );
        if( hDisk == INVALID_HANDLE_VALUE )
        {
            err = GetLastError();
            _tprintf( _T("Failed to open disk %d, error = %d\n"), diskId, err );
            break;
        }

        if( !DeviceIoControl(hDisk,
                             IOCTL_DISK_GET_DRIVE_LAYOUT_EX,
                             NULL,
                             0,
                             buffer,
                             len,
                             &len,
                             NULL) )
        {
            err = GetLastError();
            _tprintf( _T("Failed to send IOCTL_DISK_GET_DRIVE_LAYOUT_EX to disk %d, error = %d\n"), diskId, err );
            break;
        }
        else
        {
            PDRIVE_LAYOUT_INFORMATION_EX layout = (PDRIVE_LAYOUT_INFORMATION_EX)buffer;

            _tprintf( _T("Disk %d has %d partition - style %d\n"), diskId, layout->PartitionCount, layout->PartitionStyle );

            for( DWORD i=0; i<layout->PartitionCount; i++ )
            {
                PPARTITION_INFORMATION_EX   part = &layout->PartitionEntry[i];

                if (part->PartitionStyle == PARTITION_STYLE_MBR)
                {
                  _tprintf( _T(" - Partition %d, style %d length:%I64d offset:%I64d type %d\n"), part->PartitionNumber, part->PartitionStyle, part->PartitionLength.QuadPart, part->StartingOffset.QuadPart, part->Mbr.PartitionType);
                }
                else if (part->PartitionStyle == PARTITION_STYLE_GPT)
                {
                  _tprintf( _T(" - Partition %d, style %d length:%I64d offset:%I64d\n"), part->PartitionNumber, part->PartitionStyle, part->PartitionLength.QuadPart, part->StartingOffset.QuadPart);
                }

                
            }
        }
    }while( FALSE );

    free( buffer );

    if( hDisk )
    {
        CloseHandle( hDisk );
    }
}

VOID QueryVolumeInformation( LPCTSTR pszVolume )
{
    BOOT_SECTOR bs;
    VOLUME_SIZE Size;
    ULONGLONG   PartitionSize;
    ULONGLONG   TotalSectors;
    ULONGLONG   SectorsInVolume;
    ULONG       SlackSectors;
    LONG        err;
    HANDLE      hVolume = INVALID_HANDLE_VALUE;

    do
    {
        if( (err = OpenVolume(pszVolume, &hVolume)) )
        {
            break;
        }

        if( (err = QueryPartitionSize(hVolume, &Size)) )
        {
            break;
        }

        PartitionSize = Size.Length.QuadPart;

        if( (err = QueryVolumeSize(hVolume, &Size)) )
        {
            break;
        }

        if( (err = QueryFileSystemType(hVolume, &bs)) )
        {
            break;
        }

        SectorsInVolume = Size.Length.QuadPart / Size.BytesPerSector;
        TotalSectors = PartitionSize / Size.BytesPerSector;;
        SlackSectors = TotalSectors % Size.SectorsPerCluster;
        
        _tprintf( _T("Volume %s is %s file system\n"), pszVolume, _FsName[bs.FsType] );
        
        DisplayVolumeInformation( &Size );

        _tprintf( _T(" - Total Sectors:           %I64d\n"), SectorsInVolume );

        _tprintf( _T("Partition Information:\n") );
        _tprintf( _T(" - Size:                    %I64d\n"), PartitionSize );
        _tprintf( _T(" - Total sectors:           %I64d\n"), TotalSectors );
        _tprintf( _T(" - Total slack sectors:     %I64d\n"), SlackSectors );

        _tprintf( _T("---------------------\n") );

        ULONGLONG sizedif = PartitionSize - Size.Length.QuadPart;
        ULONGLONG sizedifinsector = sizedif/Size.BytesPerSector;
        _tprintf( _T(" - Partition Size - Volume size:     %I64d\n"), sizedif );
        _tprintf( _T(" - Size diff in sector:              %I64d\n"), sizedif/Size.BytesPerSector );
        _tprintf( _T(" - Size diff in cluster:             %I64d\n"), sizedifinsector/Size.SectorsPerCluster );

        //_tprintf( _T(" - Total Clusters:          %I64d\n"), pVolumeSize->TotalClusters.QuadPart );
    }while( FALSE );

    if( hVolume != INVALID_HANDLE_VALUE )
    {
        CloseHandle( hVolume );
    }
}

void QueryVolumeDiskInformation(LPCTSTR pszVolume)
{
  VOLUME_SIZE Size;
  LONG        err;
  HANDLE      hVolume = INVALID_HANDLE_VALUE;
  BYTE        *buffer = NULL; 
  DWORD       len = sizeof(DRIVE_LAYOUT_INFORMATION_EX) + sizeof(PARTITION_INFORMATION_EX) * 16;

  buffer = (BYTE*)malloc( len );

  if( (err = OpenVolume(pszVolume, &hVolume)) )
  {
    return;
  }

  if( !DeviceIoControl(hVolume,
    IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,
    NULL,
    0,
    buffer,
    len,
    &len,
    NULL) )
  {
    err = GetLastError();
    _tprintf( _T("Failed to send IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS to disk =n"));
    return;
  }
  else
  {
    PVOLUME_DISK_EXTENTS disk_extents = (PVOLUME_DISK_EXTENTS)buffer;

    for( DWORD i=0; i<disk_extents->NumberOfDiskExtents; i++ )
    {
      DISK_EXTENT  extent = disk_extents->Extents[0];

      _tprintf( _T(" - DiskNumber %d, length:%I64d offset:%I64d\n"), extent.DiskNumber, extent.ExtentLength, extent.StartingOffset);
    }
  }

  CloseHandle(hVolume);


}

VOID PrintUsage()
{
    _tprintf( _T("Missing required argument\n") );
    _tprintf( _T("SystemInfo DriveLetter:      To display information of the volume\n") );
    _tprintf( _T("SystemInfo /disk DiskNumber: To display disk layout\n") );
    _tprintf( _T("SystemInfo /file FileName:   To display cluster number and byte offset occurpied by the file\n") );
    _tprintf( _T("SystemInfo /detail DriveLetter: To display volume's information\n") );
    _tprintf( _T("SystemInfo /bitmap DriveLetter [FileName]: To query volume bitmap and save in [FileName]\n") );
}


int _tmain(int argc, _TCHAR* argv[])
{
    if( argc < 2 )
    {
        PrintUsage();

        return ERROR_INVALID_PARAMETER;
    }

    HANDLE      hVolume = INVALID_HANDLE_VALUE;
    LONG        err;
    HMODULE hLib = LoadLibrary( _T("ntdll.dll") );

    NtFsControlFile = (NTFSCONTROLFILE)GetProcAddress( hLib, "NtFsControlFile" );
    NtQueryVolumeInformationFile = (NTQUERYVOLUMEINFORMATIONFILE)GetProcAddress( hLib, "NtQueryVolumeInformationFile" );

    WaitForDebugger();

    if( _tcsicmp(argv[1], _T("/?")) == 0 )
    {
        PrintUsage();
        return 0;
    }
    else if( _tcsicmp(argv[1], _T("/disk")) == 0 )
    {
        if( argc > 2 )
        {
            int diskId = _tstoi( argv[2] );

            QueryDiskLayout( diskId );

            return 0;
        }
    }
    else if( _tcsicmp(argv[1], _T("/file")) == 0 )
    {
        if( argc > 2 )
        {
            if( argc > 3 )
            {
                DumpFile( argv[2], argv[3] );
            }
            else
            {
                DumpFile( argv[2], NULL );
            }

            return 0;
        }
    }
    else if( _tcsicmp(argv[1], _T("/detail")) == 0 )
    {
        if( argc > 2 )
        {
            QueryVolumeInformation( argv[2] );
            return 0;
        }
    }
    else if( _tcsicmp(argv[1], _T("/volume")) == 0 )
    {
      if( argc > 2 )
      {
        QueryVolumeDiskInformation( argv[2] );
        return 0;
      }
    }
    else if( _tcsicmp(argv[1], _T("/bitmap")) == 0 )
    {
        if( argc > 2 )
        {
            TCHAR   *volume = argv[2];
            TCHAR   *fname = NULL;

            if( argc > 3 )
            {
                fname = argv[3];
            }

            err = OpenVolume( volume, &hVolume );
            err = SaveVolumeBitmap( hVolume, volume, fname );
            CloseHandle( hVolume );

            return 0;
        }
    }

    TCHAR   *volume = argv[1];

    VOLUME_SIZE Size;

    do
    {
        err = OpenVolume( volume, &hVolume );

        if( err )
        {
            break;
        }

        if( (err = QueryVolumeSizeWinApi(volume, &Size)) == ERROR_SUCCESS )
        {
        }

        if( (err = QueryVolumeSizeWinApiEx(volume, &Size)) == ERROR_SUCCESS )
        {
        }

        if( (err = QueryVolumeSize(hVolume, &Size)) == ERROR_SUCCESS )
        {
        }

        if( (err = QueryVolumeSizeEx(hVolume, &Size)) == ERROR_SUCCESS )
        {
        }

        if( (err = QueryPartitionSize(hVolume, &Size)) == ERROR_SUCCESS )
        {
        }

        //  the output bitmap file should be identical to the file "$Bitmap" located in
        //  the root of the target volume
        //  the file "$Bitmap" can be extracted using tool like WinHex
        //
        //err = SaveVolumeBitmap( hVolume, volume );

        //
        //  test read pass end of volume
        //
        //TestReadPassEndOfVolume( hVolume, &Size );
    }while( FALSE );

    if( hVolume != INVALID_HANDLE_VALUE )
    {
        CloseHandle( hVolume );
    }

	return 0;
}

