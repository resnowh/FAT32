/**
 * 文件名称：xdisk.h
 * 文件功能：磁盘操作相关数据结构和接口定义
 * 作者：查涵睿、傅广文
 */
#ifndef XDISK_H
#define	XDISK_H

#include "xtypes.h"

/**
 * 文件系统类型
 */
typedef enum {
    FS_NOT_VALID = 0x00,            // 无效类型
    FS_FAT32 = 0x01,                // FAT32
    FS_EXTEND = 0x05,               // 扩展分区
    FS_WIN95_FAT32_0 = 0xB,         // FAT32
    FS_WIN95_FAT32_1 = 0xC,         // FAT32
}xfs_type_t;

#pragma pack(1)     //指定结构体或联合体的对齐方式，参数1表示按1字节 即取消对齐

/**
 * MBR的分区表项类型
 */
typedef struct _mbr_part_t {
    u8_t boot_active;               // 引导指示符(Boot Indicator)，0x80可引导，0x00非引导
    u8_t start_header;              // 起始磁头(Starting Head)
    u16_t start_sector : 6;         // 起始扇区(Starting Sector)    // 这种写法分配了6位的位域成员
    u16_t start_cylinder : 10;	    // 起始磁道(Starting Cylinder)  // 这一行10位和上一行共享了16位的u16_t
    u8_t system_id;	                // 文件系统类型
    u8_t end_header;                // 结束磁头(Ending Head)
    u16_t end_sector : 6;           // 结束扇区(Ending Sector)
    u16_t end_cylinder : 10;        // 结束磁道(Ending Cylinder)
    u32_t relative_sectors;	        // 逻辑块地址
    u32_t total_sectors;            // 总的扇区数
}mbr_part_t;

#define MBR_PRIMARY_PART_NR	    4   // 4个分区表

/**
 * MBR区域描述结构
 */
typedef  struct _mbr_t {
    u8_t code[446];                 // 引导代码区
    mbr_part_t part_info[MBR_PRIMARY_PART_NR];
    u8_t boot_sig[2];               // 引导结束标志 55 AA
}mbr_t;

#pragma pack()

// 相关前置声明
struct _xdisk_t;

/**
 * 磁盘驱动接口
 */
typedef struct _xdisk_driver_t {
    xfat_err_t(*open) (struct _xdisk_t* disk, void* init_data);
    xfat_err_t(*close) (struct _xdisk_t* disk);
    xfat_err_t(*read_sector) (struct _xdisk_t* disk, u8_t* buffer, u32_t start_sector, u32_t count);
    xfat_err_t(*write_sector) (struct _xdisk_t* disk, u8_t* buffer, u32_t start_sector, u32_t count);
}xdisk_driver_t;

/**
 * 存储设备类型
 */
typedef struct _xdisk_t {
    const char* name;               // 设备名称
    u32_t sector_size;              // 块大小
    u32_t total_sector;             // 总块数
    xdisk_driver_t* driver;         // 驱动接口
    void* data;                     // 自定义参数
}xdisk_t;

/**
 * 存储设备分区类型
 */
typedef struct _xdisk_part_t {
    u32_t start_sector;             // 相对于整个物理存储区开始的块序号
    u32_t total_sector;             // 总的块数量
    xfs_type_t type;                // 文件系统类型
    xdisk_t* disk;                  // 对应的存储设备
}xdisk_part_t;

xfat_err_t xdisk_open(xdisk_t* disk, const char* name, xdisk_driver_t* driver, void* init_data);
xfat_err_t xdisk_close(xdisk_t* disk);
xfat_err_t xdisk_get_part_count(xdisk_t* disk, u32_t* count);
xfat_err_t xdisk_get_part(xdisk_t* disk, xdisk_part_t* xdisk_part, int part_no);
xfat_err_t xdisk_read_sector(xdisk_t* disk, u8_t* buffer, u32_t start_sector, u32_t count);
xfat_err_t xdisk_write_sector(xdisk_t* disk, u8_t* buffer, u32_t start_sector, u32_t count);

#endif