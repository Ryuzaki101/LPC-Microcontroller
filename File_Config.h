/*----------------------------------------------------------------------------
 *      R T L  -  F l a s h   F i l e   S y s t e m
 *----------------------------------------------------------------------------
 *      Name:    FILE_CONFIG.H 
 *      Purpose: Common Definitions
 *      Rev.:    V3.40
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2008 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#ifndef __FILE_CONFIG_H__
#define __FILE_CONFIG_H__

#include <RTL.h>

/* Drive Media Types */
#define DRV_NONE    0
#define DRV_FLASH   1
#define DRV_SPI     2
#define DRV_RAM     3
#define DRV_MCARD   4

/* Entry Types */
#define ENT_FILE    0
#define ENT_DIR     1

/* Absolute memory access macro. */
#define MVAR(object, adr)   (*((object volatile *) (adr)))

/* Flash Device Configuration */
typedef struct DevConf {
   U32 bStart;
   U32 bEnd;
} DEVCONF;

/* Definition of the file control structure for stream. */
typedef struct iob {
   U16   fileID;                        /* File Identification Number        */
   U16   flags;                         /* File status flags                 */
   U8    drive;                         /* Device Drive Media type           */
   U8    attrib;                        /* Attribute Flags                   */
   U16   NumSect;                       /* Number of sectors                 */
   U32   InitVal;                       /* Initial content of Flash/RAM      */
   void *DevCfg;                        /* Device configuration              */
   U16   _fblock;                       /* Current Flash Block index         */
   U16   _fidx;                         /* Current File Block index          */
   U32   _fbot;                         /* Flash Block free space bottom     */
   U32   _ftop;                         /* Flash Block free space top        */
   U32   _firstClus;                    /* FAT First Data Cluster            */
   U32   _lastEntClus;                  /* FAT Entry (last) Cluster          */
   U8    _currDatSect;                  /* FAT Curr Data Sect Offs in Cluster*/
   U32   _currDatClus;                  /* FAT Current Data Cluster          */
   U32   fsize;                         /* FAT File Size                     */
   U32   fpos;                          /* FAT File Position Indicator       */
} IOB;

/* Note: fileID is used as FAT Entry (last) Offset in Cluster */

/* "flags" bit definitions  */
#define _IOREAD         0x0001
#define _IOWRT          0x0002
#define _IOAPPEND       0x0004
#define _IOEOF          0x0008
#define _IOERR          0x0010
#define _IOWALLOC       0x0020
#define _IORBUF         0x0040
#define _IOROOT_1X      0x0080          /* FAT Entry is in Root on FAT12/16  */

/* Flash Block Usage Flags */
#define BlockTEMP       0x03
#define BlockUSED       0x0F
#define BlockFULL       0xFF

/* File Allocation Information struct */
typedef struct falloc {
   U32 end;
   U16 fileID;
   U16 index;
} FALLOC;

/* File Defragmentation Information struct */
typedef struct fdbuf {
   U16 index;
   U16 fblock;
   U32 fbot;
   U32 ftop;
} FDBUF;

/* MMC Flash Card Information struct */
typedef struct fatinfo {
   U32 BootRecSec;                      /* Boot Record Sector Offset         */
   U8  FatType;                         /* File System Fat type.             */
   U8  NumOfFat;                        /* Number of Fat tables              */
   U8  SecPerClus;                      /* Number of Sectors per Cluster     */
   U16 FatSize;                         /* Number of Sectors per FAT table   */
   U16 RsvdSecCnt;                      /* Number of Reserved Sectors        */
   U32 DskSize;                         /* Disk Size Sector Count            */
   U32 DataSecCnt;                      /* Number of Data Sectors            */
   U16 RootEntCnt;                      /* Maximum Root Directory entries    */
   U16 BytesPerSec;                     /* Sector Size in bytes              */
   U32 DataClusCnt;                     /* Data Cluster Count                */
   U16 RootDirAddr;                     /* Root Dir First Sector             */
   U16 RootSecCnt;                      /* Number of Sectors for Root dir    */
   U32 ClusSize;                        /* Cluster Size in bytes             */
   U16 EntsPerClus;                     /* Number of entries per cluster     */
                                        /* Added fields for FAT32            */
   U16 FAT32_ExtFlags;                  /* FAT extended flags                */
   U32 FAT32_RootClus;                  /* Root directory first cluster      */
   U16 FAT32_FSInfo;                    /* FSInfo structure sector number    */
   U16 FAT32_BkBootSec;                 /* Boot Record copy sector number    */
} FATINFO;

/* FAT File Directory Record Entry struct */
typedef struct filerec {
   U8  FileName[11];                    /* File name in format 8.3           */
   U8  Attr;                            /* File Attribute flags              */
   U8  NTRsvd;                          /* NT Reserved                       */
   U8  CrtTimeTenth;                    /* File create Milliseconds          */
   U16 CrtTime;                         /* File create Time                  */
   U16 CrtDate;                         /* File create Date                  */
   U16 LastAccDate;                     /* Last access Date                  */
   U16 FirstClusHI;                     /* First Cluster HI (0 for FAT12/16) */
   U16 WriteTime;                       /* Time of last write                */
   U16 WriteDate;                       /* Date of last write                */
   U16 FirstClusLO;                     /* First Cluster LO                  */
   U32 FileSize;                        /* File Size in bytes                */
} FILEREC;

/* FAT32 Long Name Record Entry struct */
typedef struct lfn_filerec {
   U8  Ordinal;                         /* Ordinal field                     */
   U8  Unichar0[10];                    /* Unicode characters 1  ..  5       */
   U8  Attr;                            /* File Attribute flags              */
   U8  Rsvd;                            /* Reserved                          */
   U8  Checksum;                        /* Checksum of short file name       */
   U8  Unichar1[12];                    /* Unicode characters 6  .. 11       */
   U16 FirstClusLO;                     /* Cluster (unused, always 0)        */
   U8  Unichar2[4];                     /* Unicode characters 12 .. 13       */
} LFN_FILEREC;

#define LFN_REC(rec)    ((LFN_FILEREC *)rec)

/* MMC Format Device Parameter struct */
typedef struct devpar {
   U8  FatType;                         /* File System Fat type              */
   U8  SecClus;                         /* Number of Sectors per Cluster     */
   U8  SecClus32;                       /* Sectors per Cluster for FAT32     */
   U8  NumHeads;                        /* Number of heads for MBR           */
   U16 BootRecSec;                      /* Boot Record Sector Offset         */
} DEVPAR;

/* FAT File Attributes. */
#define ATTR_READ_ONLY      0x01
#define ATTR_HIDDEN         0x02
#define ATTR_SYSTEM         0x04
#define ATTR_VOLUME_ID      0x08
#define ATTR_DIRECTORY      0x10
#define ATTR_ARCHIVE        0x20
#define ATTR_LONG_NAME      (ATTR_READ_ONLY | ATTR_HIDDEN   | \
                             ATTR_SYSTEM    | ATTR_VOLUME_ID)

/* Ordinal last part of long name flag */
#define ORD_LONG_NAME_LAST  0x40

/* FAT type definitions. */
#define FS_RAW          0
#define FS_FAT12        1
#define FS_FAT16        2
#define FS_FAT32        3

/* Card Type definitions */
#define CARD_NONE       0
#define CARD_MMC        1
#define CARD_SD         2

/* FAT Sector Caching structure */
typedef struct fcache {
   U32 sect;
   U8  *buf;
   BIT dirty;
} FCACHE;

/* Data Sector Caching structure */
typedef struct dcache {
   U32 sect;
   U32 csect;
   U8  *buf;
   U8  *cbuf;
   U8  nwr;
   U8  nrd;
} DCACHE;

/* MMC device configuration */
typedef struct mmcfg {
   U32 sernum;
   U32 blocknr;
   U16 read_blen;
   U16 write_blen;
} MMCFG;

/* Variables. */
extern struct iob _iob[];
extern U32    mc_cache[];

/* Constants */
extern struct DevConf const FlashDev [];
extern struct DevConf const SpiDev[];
extern struct DevConf const RamDev [];
extern U16 const _FlashNumSect;
extern U16 const _SpiNumSect;
extern U16 const _RamNumSect;
extern U32 const _BlockFREE;
extern U32 const _SpiBlFREE;
extern U16 const _NFILE;
extern U16 const _DEF_DRIVE;
extern U16 const _MC_CSIZE;

/* Low level file IO functions. */
extern int  _fdelete (IOB *fcb);
extern int  _fcreate (const char *fname, IOB *fcb);
extern int  _frename (const char *filename, IOB *fcb);
extern BOOL _ffind (FINFO *info, IOB *fcb);

/* Functions called from Retarget.c */
extern int  __fopen (const char *fname, int openmode);
extern int  __fclose (int handle);
extern int  __write (int handle, const U8 *buf, U32 len);
extern int  __flushbuf (int handle);
extern int  __read (int handle, U8 *buf, U32 len);
extern int  __setfpos (int handle, U32 pos);
extern U32  __getfsize (IOB *fcb, BOOL set_fidx);
#define __get_flen(h)  __getfsize(&_iob[h],__FALSE)

/* fs_FlashIO.c module */
extern U16  fs_get_freeID (U32 maxID, IOB *fcb);
extern U32  fs_get_freeMem (U32 block, IOB *fcb);
extern BOOL fs_check_fwopen (U32 block, IOB *fcb);
extern void fs_mark_freeMem (IOB *fcb);
extern BOOL fs_mark_fileMem (IOB *fcb);
extern U32  fs_Find_File (const char *name, IOB *fcb);
extern BOOL fs_BlockFull (IOB *fcb);
extern BOOL fs_AllocBlock (IOB *fcb);
extern BOOL fs_BlockInvalid (U32 block, IOB *fcb);
extern BOOL fs_InitBlock (U32 block, IOB *fcb);
extern BOOL fs_WriteBlock (U32 adr, void *buf, U32 cnt, IOB *fcb);
extern BOOL fs_ReadBlock (U32 adr, void *buf, U32 cnt, IOB *fcb);
extern U32  fs_adr_sig (U32 bl, IOB *fcb);
extern U32  fs_rd_alloc (U32 adr, FALLOC *fa);
extern U32  fs_rd_sig (U32 adr, U32 *sig);

/* fs_lib.c module */
extern BOOL fn_cmp (const char *sp, const char *fp);
extern int  fs_strpos (const char *sp, const char ch);
extern int  fs_get_drive (const char *fn);
extern int  fs_find_iob (void);
extern BOOL fs_set_params (IOB *fcb);
extern int  fs_ReadData (U32 adr, U32 cnt, U8 *buf);
extern int  fs_WriteData (U32 adr, U32 cnt, U8 *buf);

/* Low level Flash programming routines */
extern int  fs_Init (U32 adr, U32 clk);
extern int  fs_EraseSector (U32 adr);
extern int  fs_ProgramPage (U32 adr, U32 sz, U8 *buf);

/* Low level SPI Flash programming routines */
extern int  fs_spi_Init (void);
extern int  fs_spi_EraseSector (U32 adr);
extern int  fs_spi_ProgramPage (U32 adr, U32 sz, U8 *buf);
extern int  fs_spi_ReadData (U32 adr, U32 sz, U8 *buf);

/* fs_fat.c module */
extern int  fat_init (void);
extern BOOL fat_find_file (const char *fn, IOB *fcb);
extern BOOL fat_set_fpos (IOB *fcb, U32 pos);
extern U32  fat_read (IOB *fcb, U8 *buf, U32 len);
extern BOOL fat_write (IOB *fcb, const U8 *buf, U32 len);
extern U32  fat_free (void);
extern BOOL fat_delete (const char *fn, IOB *fcb);
extern BOOL fat_close_write (IOB *fcb);
extern BOOL fat_rename (const char *old, const char *new, IOB *fcb);
extern BOOL fat_create (const char *fn, IOB *fcb);
extern BOOL fat_format (const char *label);
extern BOOL fat_ffind  (const char *fn, FINFO *info, IOB *fcb);

/* fs_mmc.c module */
extern BOOL mmc_init (void);
extern BOOL mmc_write_sect (U32 sect, U8 *buf, U32 cnt);
extern BOOL mmc_read_sect (U32 sect, U8 *buf, U32 cnt);
extern BOOL mmc_read_config (MMCFG *cfg);

/* fs_time.c module */
extern U32  fs_get_time (void);
extern U32  fs_get_date (void);

/* SPI.c module */
extern void spi_init (void);
extern U8   spi_send (U8 outb);
extern void spi_ss (U32 ss);
//extern void spi_hi_speed (BOOL on);
//extern void spi_command (U8 *cmd, U8 *tbuf, U8 *rbuf, U32 sz);
#endif
