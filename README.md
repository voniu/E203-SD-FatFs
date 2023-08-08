## E203-SD-FatFs
适用于 [hbird-sdk](https://github.com/riscv-mcu/hbird-sdk) 的 SD 卡 SPI 模式驱动及 FatFs 文件系统移植。

可在蜂鸟 RISC-V 处理器 [E203](https://github.com/riscv-mcu/e203_hbirdv2) 上借助 SPI 外设实现对 FAT32 格式 SD 卡进行读写。

### 移植清单
#### FatFs Application Interface
- [x] f_open
- [x] f_close
- [x] f_read
- [ ] f_write
- [x] f_mount
- [x] f_close

#### FatFs Media Access Interface
- [x] disk_status
- [x] disk_initialize
- [x] disk_read
- [x] disk_write

#### SD Card Driver
- [x] sd_goIdleState
- [x] sd_init
- [x] sd_sendCmd
- [x] sd_readSingleBlock
- [ ] sd_writeSingleBlock
- [ ] sd_readMultiBlock
- [ ] sd_writeMultiBlock
- [ ] sd_stopTransmission
- [x] sd_readRes1 & sd_readRes1b & sd_readRes3_7

#### SPI Driver
- [x] spi_send8 & spi_send16 & spi_send32
- [ ] spi_receive8

### 如何使用
> 可直接参考 `src/demo.c`
> 
> 函数定义见：[FatFs - Generic FAT Filesystem Module](http://elm-chan.org/fsw/ff/00index_e.html)
1. 将 GPIOA 的 [8:11] 引脚配置为 SPI1：`gpio_iof_config(GPIOA, (IOF_SPI_MASK | ANY_MASK));`
    - 注意：将引脚配置为 SPI1 的操作不可逆，程序后续不可将 [8:11] 作为 IO 引脚配置使用，详见 e203 sdk 中 [gpio_iof_config](https://github.com/riscv-mcu/hbird-sdk/blob/master/SoC/hbirdv2/Common/Source/Drivers/hbirdv2_gpio.c#L4) 源码
2. 挂载 SD 卡： `f_mount`
3. 打开指定文件： `f_open`
4. 读取文件数据： `f_read`
5. 关闭文件：`f_close`
6. 卸载 SD 卡：`f_unmount`

### 开发平台
ZedBoard + Vivado + NucleiStudio + hbird_sdk@0.1.4

### 参考资源
- [FatFs - Generic FAT Filesystem Module](http://elm-chan.org/fsw/ff/00index_e.html)
