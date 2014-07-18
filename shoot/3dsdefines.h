#ifndef _3DS_DEFINES_H_
#define _3DS_DEFINES_H_
/* random but useful 3dbrew.org wiki info thrown together */
/* by Joseph LoManto */

#define MAIN_SCREEN_WIDTH 400
#define MAIN_SCREEN_HEIGHT 240
#define MAIN_SCREEN_SIZE (MAIN_SCREEN_WIDTH*MAIN_SCREEN_HEIGHT)
#define SUB_SCREEN_WIDTH 320
#define SUB_SCREEN_HEIGHT 240
#define SUB_SCREEN_SIZE (SUB_SCREEN_WIDTH*SUB_SCREEN_HEIGHT)

#define SHARED_MEM	0x10000000

/* Physical / Virtual Address */ /*most likely unused*/
#define PA(reg)	(0x10000000+reg)
#define VA(reg)	(0x1EB00000+reg)

/* LCD Fill Color */
#define REG_LCDCOLORFILLMAIN	0x202204
#define REG_LCDCOLORFILLSUB	0x202A04
#define RGB_COLOR(r,g,b)	((r)|(g<<8)|(b<<16))
#define RGB_FILL_COLOR(r,g,b)	RGB_COLOR(r,g,b)|(1<<24)

/* Framebuffers */
#define REG_MAINLEFTFB	0x400468
#define REG_MAINRIGHTFB	0x400494
#define REG_SUBFB	0x400568
/* convert PA to VA (assuming FB in VRAM) */
#define FB_PA_TO_VA(fb)	fb+=0x7000000

#define REG_MAINFB_SIZE 0x40045C
#define REG_MAINFB_WIDTH 0x40045C
#define REG_MAINFB_HEIGHT 0x40045E
#define REG_MAINFB_FORMAT 0x400470
#define REG_MAINFB_INDEX 0x400478
#define REG_MAINFB_BYTEWIDTH 0x400490	/* fb width * pixel byte size  */

#define REG_SUBFB_SIZE 0x40055C
#define REG_SUBFB_WIDTH 0x40055C
#define REG_SUBFB_HEIGHT 0x40055E
#define REG_SUBFB_FORMAT 0x400570
#define REG_SUBFB_INDEX 0x400578
#define REG_SUBFB_BYTEWIDTH 0x400590

/* Framebuffer Color Formats */
typedef enum ColorFormat {
	GL_RGBA8_OES,
	GL_RGB8_OES,	//BGR
	GL_RGB565_OES,	//RGB
	GL_RGB5_A1_OES,
	GL_RGBA4_OES,
} ColorFormat;

/* SVC Types and Structures */

typedef enum MemoryState {
	MEMST_FREE = 0,
	MEMST_RESERVED,
	MEMST_IO,
	MEMST_STATIC,
	MEMST_CODE,
	MEMST_PRIVATE,
	MEMST_SHARED,
	MEMST_CONTINUOUS,
	MEMST_ALIASED,
	MEMST_ALIAS,
	MEMST_ALIAS_CODE,
	MEMST_LOCKED,
} MemoryState;

typedef enum PageFlags {
	PF_LOCKED,
	PF_CHANGED,
} PageFlags;

/* MEMORY_OPERATION defined in ctr/svc.h */
#ifdef SVC_H	/* for naming consistancy */
typedef MEMORY_OPERATION MemoryOperation;
#endif
/* additional memop for mapping the gsp heap */
#define MEMOP_MAP_GSP_HEAP	0x10003
#define GSP_HEAP_SIZE	0x2000000

typedef enum MemoryPermission {
	MEMPER_NONE		= 0,
	MEMPER_READ		= 1,
	MEMPER_WRITE		= 2,
	MEMPER_READWRITE	= 3,
	MEMPER_DONTCARE	= 0x10000000,
} MemoryPermission;

typedef enum ResetType {
	RT_ONESHOT,
	RT_STICKY,
	RT_PULSE,
} ResetType;

typedef struct MemoryInfo {
	u32 base_address;
	u32 size;
	u32 permission;
	MemoryState state;
} MemoryInfo;

typedef struct PageInfo {
	u32 flags;
} PageInfo;

typedef struct StartupInfo {
	s32 priority;
	u32 stack_size;
	s32 argc;
	s16* argv;
	s16* envp;
} StartupInfo;

typedef struct CreateProcessInfo {
	/* All addresses are given virtual for the process to be created. */
	/* All sizes/offsets are in given in 0x1000-pages. */
	u8  codeset_name[8];
	u32 text_addr;
	u32 text_size;
	u32 ro_addr;
	u32 ro_size;
	u32 data_addr;
	u32 data_size;
	u32 ro_offset;
	u32 data_offset;
	u32 bss_size_plus_data_size;
	u8  program_id[8];
	u32 unknown[2];
} CreateProcessInfo;

/* ARM11 Kernel Configuaration Memory */
#define REG_KERNEL_		(*(u8*)0x1FF80000)
#define REG_KERNEL_VERSIONREVISION	(*(u8*)0x1FF80001)
#define REG_KERNEL_VERSIONMINOR		(*(u8*)0x1FF80002)
#define REG_KERNEL_VERSIONMAJOR		(*(u8*)0x1FF80003)
#define REG_UPDATEFLAG	(*(u32*)0x1FF80004)
#define REG_NSTID	(*(u64*)0x1FF80008)
#define REG_NSTID_L	(*(u32*)0x1FF80008)
#define REG_NSTID_H	(*(u32*)0x1FF8000C)
#define REG_SYSCOREVER	(*(u32*)0x1FF80010)
#define REG_UNITINFO	(*(u8*)0x1FF80014)
#define REG_KERNEL_CTRSDKVERSION	(*(u32*)0x1FF80018)
#define REG_APPMEMTYPE	(*(u32*)0x1FF80030)
#define REG_APPMEMALLOC	(*(u32*)0x1FF80040)
#define REG_FIRM_	(*(u4*)0x1FF80060)
#define REG_FIRM_VERSIONREVISION	(*(u8*)0x1FF80061)
#define REG_FIRM_VERSIONMINOR		(*(u8*)0x1FF80062)
#define REG_FIRM_VERSIONMAJOR		(*(u8*)0x1FF80063)
#define REG_FIRM_SYSCOREVER		(*(u32*)0x1FF80064)
#define REG_FIRM_CTRSDKVERSION		(*(u32*)0x1FF80068)

/* Shared Memory Page for ARM11 Processes */
#define REG_WIFI_MACADDR	(*(u64*)0x1FF81060)	/* 6 bytes */
#define REG_WIFI_MACADDR_L	(*(u32*)0x1FF81060)
#define REG_WIFI_MACADDR_H	(*(u16*)0x1FF81064)
#define REG_3D_SLIDERSTATE	(*(float*)0x1FF81080)
#define REG_3D_LEDSTATE	(*(u8*)0x1FF81084)
#define REG_MENUTID		(*(u64*)0x1FF810A0)
#define REG_MENUTID_L	(*(u32*)0x1FF810A0)
#define REG_MENUTID_H	(*(u32*)0x1FF810A4)
#define REG_ACTIVEMENUTID	(*(u64*)0x1FF810A8)
#define REG_ACTIVEMENUTID_L	(*(u32*)0x1FF810A8)
#define REG_ACTIVEMENUTID_H	(*(u32*)0x1FF810AC)

typedef enum LedState {
	LED_ENABLED,
	LED_DISABLED,
} LedState;

/* FSFileWrite */
/* Flush Flags */
#define FS_DONT_FLUSH	0x1
#define FS_ENABLE_FLUSH	0x10001

/* Archive idcodes */
/* See http://3dbrew.org/wiki/FS:OpenArchive */
typedef enum ArchiveId {
	ARCID_ROMFS = 0x3,
	ARCID_SAVEDATA = 0x4,
	ARCID_EXTSAVEDATA = 0x6,
	ARCID_SHARED_EXTSAVEDATA = 0x7,
	ARCID_SYSTEMSAVEDATA =  0x8,
	ARCID_SDMC = 0x9,
	ARCID_SDMC_WO = 0xA,
	ARCID_EXTSAVEDATA_BOSS = 0x12345678,
	ARCID_CARD_SPI_FS = 0x12345679,
	ARCID_EXTSAVEDATA_AND_EXTSAVEDATA_BOSS = 0x1234567B,
	ARCID_SYSTEMSAVEDATA_MIRROR = 0x1234567C,
	ARCID_NAND_RW = 0x1234567D,
	ARCID_NAND_RO = 0x1234567E,
	ARCID_NAND_RO_WRITE_FS = 0x1234567F,
} ArchiveId;

#define WORD(mem) (*(u32*)(mem))
/* Relative to HID SharedMem (0x10000000)*/
#define PAD_STATE	0x1C
#define CIRCLE_PAD_INFO	0x34
#define TOUCH_SCREEN_INFO	0xC0

typedef struct TouchScreenInfo {
	u16 x,y;
	u32 contains_data;
} TouchScreenInfo;

#define REG_PAD_STATE WORD(SHARED_MEM+PAD_STATE)

#endif
