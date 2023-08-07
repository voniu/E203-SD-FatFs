### 如何使用
1. 将 GPIOA 的 [8:11] 引脚配置为 SPI1：`gpio_iof_config(GPIOA, (IOF_SPI_MASK | ANY_MASK));`
    - 注意，将引脚配置为 SPI1 的操作不可逆，e203 sdk 中 `gpio_iof_config` 源码
2. 挂载 SD 卡： `f_mount`
3. 打开指定文件： `f_open`
4. 读取文件数据： `f_read`

### FatFS 已移植函数列表
- f_mount
- f_open
- f_read