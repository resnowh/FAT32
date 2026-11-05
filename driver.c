/**
 * @file driver.c
 * @brief 硬件磁盘驱动相关函数的实现
 * @author 查涵睿、傅广文
 */
#include <stdio.h>
#include "xdisk.h"
#include "xfat.h"

/**
 * @brief 初始化磁盘设备
 * 
 * @param disk 初始化的设备
 * @param init_data 设备路径
 * @return 错误码
 */
static xfat_err_t xdisk_hw_open(xdisk_t* disk, void* init_data) {
    const char* path = (const char*)init_data;

    FILE* file = fopen(path, "rb+");
    if (file == NULL) {
        printf("open disk failed:%s\n", path);
        return FS_ERR_IO;
    }

    disk->data = file;              // 利用data字段存储文件指针 
    disk->sector_size = 512;        // 扇区大小512字节

    fseek(file, 0, SEEK_END);
    disk->total_sector = ftell(file) / disk->sector_size;   // 计算扇区数量
    return FS_ERR_OK;
}

/**
 * @brief 关闭存储设备
 * 
 * @param disk
 * @return
 */
static xfat_err_t xdisk_hw_close(xdisk_t* disk) {
    FILE* file = (FILE*)disk->data;
    fclose(file);

    return FS_ERR_OK;
}

/**
 * @brief 从设备中读取指定扇区数量的数据
 * 
 * @param disk 读取的磁盘
 * @param buffer 读取数据存储的缓冲区
 * @param start_sector 读取的起始扇区
 * @param count 读取的扇区数量
 * @return 错误码
 */
static xfat_err_t xdisk_hw_read_sector(xdisk_t* disk, u8_t* buffer, u32_t start_sector, u32_t count) {
    u32_t offset = start_sector * disk->sector_size;    // 计算起始位置偏移量
    FILE* file = (FILE*)disk->data;

    int err = fseek(file, offset, SEEK_SET);
    if (err == -1) {
        printf("seek disk failed:0x%x\n", (int)offset);
        return FS_ERR_IO;
    }

    err = (int)fread(buffer, disk->sector_size, count, file);
    if (err == -1) {
        printf("read disk failed:sector:%d, count:%d\n", (int)start_sector, (int)count);
        return FS_ERR_IO;
    }
    return FS_ERR_OK;

}

/**
 * @brief 向设备中写指定的扇区数量的数据
 * 
 * @param disk 写入的存储设备
 * @param buffer 数据源缓冲区
 * @param start_sector 写入的起始扇区
 * @param count 写入的扇区数
 * @return 错误码
 */
static xfat_err_t xdisk_hw_write_sector(xdisk_t* disk, u8_t* buffer, u32_t start_sector, u32_t count) {
    u32_t offset = start_sector * disk->sector_size;
    FILE* file = (FILE*)disk->data;

    int err = fseek(file, offset, SEEK_SET);
    if (err == -1) {
        printf("seek disk failed: 0x%x\n", (int)offset);
        return FS_ERR_IO;
    }

    err = (int)fwrite(buffer, disk->sector_size, count, file);
    if (err == -1) {
        printf("write disk failed:sector:%d, count:%d\n", (int)start_sector, (int)count);
        return FS_ERR_IO;
    }

    fflush(file);       // 刷新，即时写入
    return FS_ERR_OK;
}

/**
 * 虚拟磁盘驱动结构
 */
xdisk_driver_t vdisk_driver = {
    .open = xdisk_hw_open,
    .close = xdisk_hw_close,
    .read_sector = xdisk_hw_read_sector,
    .write_sector = xdisk_hw_write_sector,
};