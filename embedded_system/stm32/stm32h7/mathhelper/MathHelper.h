#ifndef _MATHHELPER_H_
#define _MATHHELPER_H_
/*
	�ʺ���STM32F4����ѧ�����
	ʹ��ʱҪ�Ӻ꣺ARM_MATH_CM4
	һЩ�����������˸ú�����������������
	arm_cortexM4lf_math.lib�ļ���Ҫ���뵽project��
*/
#include "sys.h"
#include <math.h>
#include "arm_math.h"

//ָ���Ƿ�ʹ��FFT����
#define USE_FFT

//��������˳��
//POSITIVE������
//NEGATIVE������
#define POSITIVE 0
#define NEGATIVE 1

#define MATHHELPER_PI (3.141592653f)
#define MATHHELPER_E  (2.718281828f)

//FFT�ƻ�����ţ�ͬʱ������4��FFT�ƻ�
#define FFT_PLAN_ORDER0 0
#define FFT_PLAN_ORDER1 1
#define FFT_PLAN_ORDER2 2
#define FFT_PLAN_ORDER3 3

//ָ��FFTԭʼ���ݵ�����
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

//FFT�ƻ���Ϣ�ṹ�壬������ִ��һ��FFT����Ļ�����Ϣ��
//��ִ��һ��FFT����֮ǰ����������һ�������Ľṹ�壬�����������Ϣ��Ȼ���Ըýṹ��ָ��Ϊ��������MathHelper_FFT_Plan_Build������
//������ִ�ж��FFT����ʱ��ͬһ���ṹ���Plan_Order��Sample_Number��Data_Type��Ӧ�øı䡣
//����Input_Data��Output_Data�е����ݶ����Ըı䡣
//Input_Data��Output_Data������ͬһ���͵�����
typedef struct FFT_Plan_Information
{
	u8 Plan_Order;		//�üƻ������
	u8 Is_IFFT;			//�Ƿ���IFFT����
    u8 Is_Complex;      //�Ƿ��Ǹ��任
	u8 Original_Output;	//�Ƿ����ԭʼ�ĸ������ݣ���������������
	u16 Sample_Number;	//����������ֻ����2����
	float *Input_Data;	//�������ݵ�ַ
	float *Output_Data;	//������ݵ�ַ
} fft_plan;

/*�����ֵ*/
#define MATHHELPER_ABS(x) (x>0?x:-x)

#define MathHelper_Slope(x1,y1,x2,y2)		(((y2)-(y1))/((x2)-(x1)))
#define MathHelper_Intercept(x1,y1,x2,y2)	((y1)-(x1)*MathHelper_Slope(x1,y1,x2,y2))
#define MathHelper_Round(x)                 ((int)((x)+0.5f))

/*��ƽ����
**������X������
**����ֵ��ƽ����ֵ��������*/
u32 MathHelper_Sqrt_Uint(const u32 X);
int MathHelper_Sqrt_Int(const int X);
float MathHelper_Sqrt_Float(const float X);

/*���������е����ֵ
**������Source_Array��Դ���飬�����������������������ֵ
**		Lenth�����鳤��
**����ֵ�����ֵ*/
u32 MathHelper_FindMax_Uint32(u32* Source_Array, u16 Lenth, u16* Index);
u16 MathHelper_FindMax_Uint16(u16* Source_Array, u16 Lenth, u16* Index);
u8 MathHelper_FindMax_Uint8(u8* Source_Array, u16 Lenth, u16* Index);
int MathHelper_FindMax_Int32(int* Source_Array, u16 Lenth, u16* Index);
short MathHelper_FindMax_Int16(short* Source_Array, u16 Lenth, u16* Index);
char MathHelper_FindMax_Int8(char* Source_Array, u16 Lenth, u16* Index);
float MathHelper_FindMax_Float(float* Source_Array, u16 Lenth, u16* Index);

/*���������е���Сֵ
**������Source_Array��Դ���飬������������������е���Сֵ
**		Lenth�����鳤��
**����ֵ����Сֵ*/
u32 MathHelper_FindMin_Uint32(u32* Source_Array, u16 Lenth, u16* Index);
u16 MathHelper_FindMin_Uint16(u16* Source_Array, u16 Lenth, u16* Index);
u8 MathHelper_FindMin_Uint8(u8* Source_Array, u16 Lenth, u16* Index);
int MathHelper_FindMin_Int32(int* Source_Array, u16 Lenth, u16* Index);
short MathHelper_FindMin_Int16(short* Source_Array, u16 Lenth, u16* Index);
char MathHelper_FindMin_Int8(char* Source_Array, u16 Lenth, u16* Index);
float MathHelper_FindMin_Float(float* Source_Array, u16 Lenth, u16* Index);

/*��һ�������������
**������Source_Array��Դ���飬����������������������򣬲����ź�������ݷ������������
**		Lenth�����鳤��
**		Direction���������ļ�ͷ���ĺ��ж���*/
void MathHelper_Sequence_Uint32(u32* Source_Array, u16 Lenth, u8 Direction);
void MathHelper_Sequence_Uint16(u16* Source_Array, u16 Lenth, u8 Direction);
void MathHelper_Sequence_Uint8(u8* Source_Array, u16 Lenth, u8 Direction);
void MathHelper_Sequence_Int32(int* Source_Array, u16 Lenth, u8 Direction);
void MathHelper_Sequence_Int16(short* Source_Array, u16 Lenth, u8 Direction);
void MathHelper_Sequence_Int8(char* Source_Array, u16 Lenth, u8 Direction);
void MathHelper_Sequence_Float(float* Source_Array, u16 Lenth, u8 Direction);
void MathHelper_Sequence_Double(double* Source_Array, u16 Lenth, u8 Direction);

/*��һ���������������ݵ�ƽ��ֵ
**������Source_Array��Դ���飬�����������������ƽ��ֵ
**		Lenth�����鳤��
**����ֵ��ƽ����*/
u32 MathHelper_Average_Uint32(u32* Source_Array, u16 Lenth);
u16 MathHelper_Average_Uint16(u16* Source_Array, u16 Lenth);
u8 MathHelper_Average_Uint8(u8* Source_Array, u16 Lenth);
int MathHelper_Average_Int32(int* Source_Array, u16 Lenth);
short MathHelper_Average_Int16(short* Source_Array, u16 Lenth);
char MathHelper_Average_Int8(char* Source_Array, u16 Lenth);
float MathHelper_Average_Float(float* Source_Array, u16 Lenth);

void MathHelper_DCRemove_Float(float* Source_Array, u16 Length);

float MathHelper_RMS_Float(float* Source_Array, u16 Length);

/*��һ���������������ݵ�ƽ����
**������Source_Array��Դ���飬�����������������ƽ����
**		Lenth�����鳤��
**����ֵ��ƽ����*/
float MathHelper_SquareSum_Uint32(u32* Source_Array, u16 Lenth);
float MathHelper_SquareSum_Uint16(u16* Source_Array, u16 Lenth);
float MathHelper_SquareSum_Int32(int* Source_Array, u16 Lenth);
float MathHelper_SquareSum_Float(float* Source_Array, u16 Lenth);

/*����*/
float MathHelper_Variance_Float(float* Source_Array, u16 Length);

/*�󻬶�ƽ��
**������Source_Array��Դ����
**		Target_Array����������������������
**		Lenth�����鳤��
**		Block_Size���������ڵĴ�С��ֻ����������
**���ͣ�Target_Array[i]=Source_Array[i-(Block_Size-1)/2]+Source_Array[i-(Block_Size-1)/2+1]+....+
**						Source_Array[i-1]+Source_Array[i]+Source_Array[i+1]+....+
**						Source_Array[i+(Block_Size-1)-1]+Source_Array[i+(Block_Size-1)/2]
**						i<(Block_Size-1)/2+1ʱ����ȱ�����Source_Array[0]����
**						i>Lenth-(Block_Size)/2ʱ����ȱ�����Source_Array[Lenth-1]����*/
void MathHelper_Moving_Average_Uint32(u32* Source_Array, float* Target_Array, u16 Lenth, u8 Block_Size);
void MathHelper_Moving_Average_Uint16(u16* Source_Array, float* Target_Array, u16 Lenth, u8 Block_Size);
void MathHelper_Moving_Average_Int32(int* Source_Array, float* Target_Array, u16 Lenth, u8 Block_Size);
void MathHelper_Moving_Average_Float(float* Source_Array, float* Target_Array, u16 Lenth, u8 Block_Size);

/*��ֵ�˲�*/
void MathHelper_MidValue_Filter_Uint32(const u32* Source_Array, u32* Target_Array, u16 Length);
void MathHelper_MidValue_Filter_Uint16(const u16* Source_Array, u16* Target_Array, u16 Length);
void MathHelper_MidValue_Filter_Uint8(const u8* Source_Array, u8* Target_Array, u16 Length);
void MathHelper_MidValue_Filter_Int32(const int* Source_Array, int* Target_Array, u16 Length);
void MathHelper_MidValue_Filter_Int16(const short* Source_Array, short* Target_Array, u16 Length);
void MathHelper_MidValue_Filter_Int8(const char* Source_Array, char* Target_Array, u16 Length);
void MathHelper_MidValue_Filter_Float(float* Source_Array, float* Target_Array, u16 Length);

void MathHelper_Linear_Interpolation(float* Source_Array, float* Target_Array, u16 Source_Length, u16 Target_Length);

/*��ֵ�޳�*/
u16 MathHelper_BadValue_Remove_Float(const float* Source_Array, float* Target_Array, u16 Length, float Mean, float Variance, float Con_Interval);

/*��һ����������ɢ����Ҷ�任
**������Samples����������ĵ�ַ
**		Spectrum��Ƶ�����ݴ�ŵ�ַ���任������������������������СӦ��֤����ΪSamples��һ��
**		Sample_Points�����������С
**		Sampling_Frequency������Ƶ�ʣ�ָÿ�����������
**���ͣ�Spectrum�еĵ�i�����Ƶ��Ϊi*Sampling_Frequency/Sample_Points���źŵ�������������ֱ������*/
void MathHelper_DFT(float* Samples, float* Spectrum, u16 Sample_Points, u32 Sampling_Frequency);

/**********************************************************************
*���ܣ���ʼ��һ��FFT����ƻ�										  *
*������Execute_Plan����Ҫ��ʼ���ļƻ��ṹ���ַ						  *
*����ֵ��0���ɹ�													  *
*		 1��������������ָ������									  *
*		 2���ƻ���Ų���ָ������									  *
***********************************************************************/
u8 MathHelper_FFT_Plan_Build(struct FFT_Plan_Information *Execute_Plan);

/************************************************************************************
*���ܣ���һ�������Ŀ��ٸ���Ҷ�任													*
*������FFT_Plan_Order��Ҫִ������ļƻ���ţ��üƻ����뱻��ʼ������					*   						
*����ֵ��0������  																	*
*		 1���ƻ���Ų���ָ������													*
*		 2���ƻ�δ����ʼ��															*
*���ͣ���һ������N��������������FFT���㣬������г���ΪN/2+1�����е�һ��Ϊֱ�����ȣ�*
*	   ��k����Ƶ��Ϊ(k-1)*(N/Sample_Frequency)�ķ����ķ���							*
*************************************************************************************/
u8 MathHelper_FFT(u8 FFT_Plan_Order);

/*************************************************************************
*���ܣ�����һ��FFT����ƻ����ͷ��ڴ�									 *
*������Execute_Plan��Ҫִ�г����ļƻ��ṹ���ַ							 *
**************************************************************************/
void MathHelper_FFT_Plan_Cancel(struct FFT_Plan_Information *Execute_Plan);

void* MathHelper_Get_arm_fft_struct(u16 len); 

#endif // _MATHHELPER_H_
