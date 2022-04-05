#ifndef _MATHHELPER_H_
#define _MATHHELPER_H_
/*
	适合于STM32F4的数学运算库
	使用时要加宏：ARM_MATH_CM4
	一些函数最后标明了该函数的适用数据类型
	arm_cortexM4lf_math.lib文件需要加入到project中
*/
#include "sys.h"
#include <math.h>
#include "arm_math.h"

//指明是否使用FFT功能
#define USE_FFT

//排序函数的顺序
//POSITIVE：正序
//NEGATIVE：逆序
#define POSITIVE 0
#define NEGATIVE 1

#define MATHHELPER_PI (3.141592653f)
#define MATHHELPER_E  (2.718281828f)

//FFT计划的序号，同时最多存在4个FFT计划
#define FFT_PLAN_ORDER0 0
#define FFT_PLAN_ORDER1 1
#define FFT_PLAN_ORDER2 2
#define FFT_PLAN_ORDER3 3

//指明FFT原始数据的类型
#define DATA_TYPE_UINT32 0
#define DATA_TYPE_UINT16 1
#define DATA_TYPE_UINT8  2
#define DATA_TYPE_INT32  3
#define DATA_TYPE_INT16  4
#define DATA_TYPE_INT8   5
#define DATA_TYPE_FLOAT  6

typedef struct Data_Type_Struct_Complex_Float
{
	float Real_Part;
	float Imaginary_Part;
} Complex_Float;

typedef struct Data_Type_Struct_Complex_Int32
{
	float Real_Part;
	float Imaginary_Part;
} Complex_Int32;

typedef struct Data_Type_Struct_Complex_Int16
{
	float Real_Part;
	float Imaginary_Part;
} Complex_Int16;

typedef struct Data_Type_Struct_Complex_Int8
{
	float Real_Part;
	float Imaginary_Part;
} Complex_Int8;

//FFT计划信息结构体，包含了执行一个FFT运算的基本信息。
//在执行一个FFT运算之前必须先声明一个这样的结构体，并填入相关信息，然后以该结构体指针为参数引用MathHelper_FFT_Plan_Build函数。
//当连续执行多个FFT运算时，同一个结构体的Plan_Order、Sample_Number、Data_Type不应该改变。
//但是Input_Data和Output_Data中的内容都可以改变。
//Input_Data和Output_Data必须是同一类型的数据
typedef struct FFT_Plan_Information
{
	u8 Plan_Order;		//该计划的序号
	u8 Is_IFFT;			//是否是IFFT运算
    u8 Is_Complex;      //是否是复变换
	u8 Original_Output;	//是否输出原始的复数数据（但除过样本数）
	u16 Sample_Number;	//样本数量，只能是2的幂
	float *Input_Data;	//输入数据地址
	float *Output_Data;	//输出数据地址
} fft_plan;

/*求绝对值*/
#define MATHHELPER_ABS(x) (x>0?x:-x)

#define MathHelper_Slope(x1,y1,x2,y2)		(((y2)-(y1))/((x2)-(x1)))
#define MathHelper_Intercept(x1,y1,x2,y2)	((y1)-(x1)*MathHelper_Slope(x1,y1,x2,y2))
#define MathHelper_Round(x)                 ((int)((x)+0.5f))

/*求平方根
**参数：X：数据
**返回值：平方根值（正数）*/
u32 MathHelper_Sqrt_Uint(const u32 X);
int MathHelper_Sqrt_Int(const int X);
float MathHelper_Sqrt_Float(const float X);

/*返回数组中的最大值
**参数：Source_Array：源数组，函数将返回这个数组里的最大值
**		Lenth：数组长度
**返回值：最大值*/
u32 MathHelper_FindMax_Uint32(u32* Source_Array, u16 Lenth, u16* Index);
u16 MathHelper_FindMax_Uint16(u16* Source_Array, u16 Lenth, u16* Index);
u8 MathHelper_FindMax_Uint8(u8* Source_Array, u16 Lenth, u16* Index);
int MathHelper_FindMax_Int32(int* Source_Array, u16 Lenth, u16* Index);
short MathHelper_FindMax_Int16(short* Source_Array, u16 Lenth, u16* Index);
char MathHelper_FindMax_Int8(char* Source_Array, u16 Lenth, u16* Index);
float MathHelper_FindMax_Float(float* Source_Array, u16 Lenth, u16* Index);

/*返回数组中的最小值
**参数：Source_Array：源数组，函数将返回这个数组中的最小值
**		Lenth：数组长度
**返回值：最小值*/
u32 MathHelper_FindMin_Uint32(u32* Source_Array, u16 Lenth, u16* Index);
u16 MathHelper_FindMin_Uint16(u16* Source_Array, u16 Lenth, u16* Index);
u8 MathHelper_FindMin_Uint8(u8* Source_Array, u16 Lenth, u16* Index);
int MathHelper_FindMin_Int32(int* Source_Array, u16 Lenth, u16* Index);
short MathHelper_FindMin_Int16(short* Source_Array, u16 Lenth, u16* Index);
char MathHelper_FindMin_Int8(char* Source_Array, u16 Lenth, u16* Index);
float MathHelper_FindMin_Float(float* Source_Array, u16 Lenth, u16* Index);

/*对一个数组进行排序
**参数：Source_Array：源数组，函数将会对这个数组进行排序，并将排好序的数据放在这个数组里
**		Lenth：数组长度
**		Direction：排序方向，文件头处的宏有定义*/
void MathHelper_Sequence_Uint32(u32* Source_Array, u16 Lenth, u8 Direction);
void MathHelper_Sequence_Uint16(u16* Source_Array, u16 Lenth, u8 Direction);
void MathHelper_Sequence_Uint8(u8* Source_Array, u16 Lenth, u8 Direction);
void MathHelper_Sequence_Int32(int* Source_Array, u16 Lenth, u8 Direction);
void MathHelper_Sequence_Int16(short* Source_Array, u16 Lenth, u8 Direction);
void MathHelper_Sequence_Int8(char* Source_Array, u16 Lenth, u8 Direction);
void MathHelper_Sequence_Float(float* Source_Array, u16 Lenth, u8 Direction);
void MathHelper_Sequence_Double(double* Source_Array, u16 Lenth, u8 Direction);

/*求一个数组中所有数据的平均值
**参数：Source_Array：源数组，函数将对这个数组求平均值
**		Lenth：数组长度
**返回值：平均数*/
u32 MathHelper_Average_Uint32(u32* Source_Array, u16 Lenth);
u16 MathHelper_Average_Uint16(u16* Source_Array, u16 Lenth);
u8 MathHelper_Average_Uint8(u8* Source_Array, u16 Lenth);
int MathHelper_Average_Int32(int* Source_Array, u16 Lenth);
short MathHelper_Average_Int16(short* Source_Array, u16 Lenth);
char MathHelper_Average_Int8(char* Source_Array, u16 Lenth);
float MathHelper_Average_Float(float* Source_Array, u16 Lenth);

void MathHelper_DCRemove_Float(float* Source_Array, u16 Length);

float MathHelper_RMS_Float(float* Source_Array, u16 Length);

/*求一个数组中所有数据的平方和
**参数：Source_Array：源数组，函数将对这个数组求平方和
**		Lenth：数组长度
**返回值：平方和*/
float MathHelper_SquareSum_Uint32(u32* Source_Array, u16 Lenth);
float MathHelper_SquareSum_Uint16(u16* Source_Array, u16 Lenth);
float MathHelper_SquareSum_Int32(int* Source_Array, u16 Lenth);
float MathHelper_SquareSum_Float(float* Source_Array, u16 Lenth);

/*方差*/
float MathHelper_Variance_Float(float* Source_Array, u16 Length);

/*求滑动平均
**参数：Source_Array：源数组
**		Target_Array：结果将放在这个数组里面
**		Lenth：数组长度
**		Block_Size：滑动窗口的大小（只能是奇数）
**解释：Target_Array[i]=Source_Array[i-(Block_Size-1)/2]+Source_Array[i-(Block_Size-1)/2+1]+....+
**						Source_Array[i-1]+Source_Array[i]+Source_Array[i+1]+....+
**						Source_Array[i+(Block_Size-1)-1]+Source_Array[i+(Block_Size-1)/2]
**						i<(Block_Size-1)/2+1时，空缺项会以Source_Array[0]补充
**						i>Lenth-(Block_Size)/2时，空缺项会以Source_Array[Lenth-1]补充*/
void MathHelper_Moving_Average_Uint32(u32* Source_Array, float* Target_Array, u16 Lenth, u8 Block_Size);
void MathHelper_Moving_Average_Uint16(u16* Source_Array, float* Target_Array, u16 Lenth, u8 Block_Size);
void MathHelper_Moving_Average_Int32(int* Source_Array, float* Target_Array, u16 Lenth, u8 Block_Size);
void MathHelper_Moving_Average_Float(float* Source_Array, float* Target_Array, u16 Lenth, u8 Block_Size);

/*中值滤波*/
void MathHelper_MidValue_Filter_Uint32(const u32* Source_Array, u32* Target_Array, u16 Length);
void MathHelper_MidValue_Filter_Uint16(const u16* Source_Array, u16* Target_Array, u16 Length);
void MathHelper_MidValue_Filter_Uint8(const u8* Source_Array, u8* Target_Array, u16 Length);
void MathHelper_MidValue_Filter_Int32(const int* Source_Array, int* Target_Array, u16 Length);
void MathHelper_MidValue_Filter_Int16(const short* Source_Array, short* Target_Array, u16 Length);
void MathHelper_MidValue_Filter_Int8(const char* Source_Array, char* Target_Array, u16 Length);
void MathHelper_MidValue_Filter_Float(float* Source_Array, float* Target_Array, u16 Length);

void MathHelper_Linear_Interpolation(float* Source_Array, float* Target_Array, u16 Source_Length, u16 Target_Length);

/*坏值剔除*/
u16 MathHelper_BadValue_Remove_Float(const float* Source_Array, float* Target_Array, u16 Length, float Mean, float Variance, float Con_Interval);

/*求一个样本的离散傅里叶变换
**参数：Samples：样本数组的地址
**		Spectrum：频谱数据存放地址，变换结果存放在这个数组里，这个数组大小应保证至少为Samples的一半
**		Sample_Points：样本数组大小
**		Sampling_Frequency：采样频率（指每秒采样次数）
**解释：Spectrum中的第i项，代表频率为i*Sampling_Frequency/Sample_Points的信号的振幅，首项就是直流分量*/
void MathHelper_DFT(float* Samples, float* Spectrum, u16 Sample_Points, u32 Sampling_Frequency);

/**********************************************************************
*功能：初始化一个FFT运算计划										  *
*参数：Execute_Plan：将要初始化的计划结构体地址						  *
*返回值：0：成功													  *
*		 1：样本数量不是指定的数									  *
*		 2：计划序号不是指定的数									  *
***********************************************************************/
u8 MathHelper_FFT_Plan_Build(struct FFT_Plan_Information *Execute_Plan);

/************************************************************************************
*功能：求一个样本的快速傅里叶变换													*
*参数：FFT_Plan_Order：要执行运算的计划序号（该计划必须被初始化过）					*   						
*返回值：0：正常  																	*
*		 1：计划序号不是指定的数													*
*		 2：计划未被初始化															*
*解释：对一个具有N个样本的序列做FFT运算，输出序列长度为N/2+1。其中第一项为直流幅度，*
*	   第k项是频率为(k-1)*(N/Sample_Frequency)的分量的幅度							*
*************************************************************************************/
u8 MathHelper_FFT(u8 FFT_Plan_Order);

/*************************************************************************
*功能：撤销一个FFT运算计划并释放内存									 *
*参数：Execute_Plan：要执行撤销的计划结构体地址							 *
**************************************************************************/
void MathHelper_FFT_Plan_Cancel(struct FFT_Plan_Information *Execute_Plan);

void* MathHelper_Get_arm_fft_struct(u16 len); 

#endif // _MATHHELPER_H_
