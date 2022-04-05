#ifndef _GRAPHIC_CONFIG_H_
#define _GRAPHIC_CONFIG_H_

//图形库专属缓存0所在的堆
#define BMPDECODER_BUFFER_HEAP HEAP1

//图形库专属缓存0大小，该值越大在显示较大的压缩格式的BMP格式图象时，显示速度越快
//建议该值大于2048
#define BMPDECODER_BUFFER_SIZE 4096

//图形库专属缓存1所在的堆
#define RASTERIZATION_BUFFER_HEAP HEAP1

//图形库专属缓存1大小，此缓存区用来显示线/圆
//建议该值大于max(屏幕宽度,屏幕高度)*2
#define RASTERIZATION_BUFFER_SIZE 2048

#define PRINT_BUFFER_HEAP HEAP0

// printf函数所使用的暂存字符串的缓存大小
//实际上会开辟Print_Buffer0和Print_Buffer1两个缓存区
//每个的大小都是PRINT_BUFFER_SIZE
//其中Print_Buffer0会存在于GRAPHIC_BUFFER0_HEAP
//而Print_Buffer1会存在于GRAPHIC_BUFFER1_HEAP
#define PRINT_BUFFER_SIZE 256

//显存所在的堆空间，此空间用来存储画布开辟的存储区
#define GRAM_HEAP HEAP2

//保护空间大小
//该值代表在每一块显存的首位都会有PROTECTED_ROOM*Canvas.Width*2长度空闲
//显存空间，该部分空间也被视为显存的一部分，虽然并不会向其中写图像信息。这么做是为
//了防止对显存写的时候超出了显存地址而错误地写入了相邻的画布显存内，造成图像损坏。
#define PROTECTED_ROOM 1

#define MAX_CACHED_COLUMN 128

#define MAX_CACHED_ROW 8

//将TAB键转换为空格
#define SPACE_TAB 1

#define TAB_SIZE 4

#endif
