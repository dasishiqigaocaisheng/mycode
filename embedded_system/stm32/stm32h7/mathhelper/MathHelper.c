#include "MathHelper.h"
#include "arm_const_structs.h"
//#include "USART.h"

#ifdef USE_FFT
#include "Memory.h"
#endif

#define MATHHELPER_HEAP heap0

#if (MATHHELPER_HEAP!=heap0)&&(MATHHELPER_HEAP!=heap1)&&(MATHHELPER_HEAP!=heap2)&& \
	(MATHHELPER_HEAP!=heap3)&&(MATHHELPER_HEAP!=heap4)
#error "MathHelper没有指定有效堆空间"
#endif

#if (MATHHELPER_HEAP!=heap0)&&(MATHHELPER_HEAP!=heap1)
#warning "MathHelper使用的堆空间在外部存储器上"
#endif

#ifndef ARM_MATH_CM7
#error "没有宏ARM_MATH_CM7"
#endif

float Global_Save0;

#ifdef USE_FFT
struct 
{
	u8 Has_Been_Built;
	struct FFT_Plan_Information *Plan;
	float *Data_Buffer;
	float *Temporary_Data_Buffer;
	const arm_cfft_instance_f32 *FFT_Struct;
} FFT_Plan_List[4]={{0,NULL,NULL,NULL,NULL},{0,NULL,NULL,NULL,NULL},{0,NULL,NULL,NULL,NULL},{0,NULL,NULL,NULL,NULL}};
#endif

u32 MathHelper_Sqrt_Uint(const u32 X)
{
	arm_sqrt_f32((float)X,&Global_Save0);
	return (u32)Global_Save0;
}

int MathHelper_Sqrt_Int(const int X)
{
	arm_sqrt_f32((float)X,&Global_Save0);
	return (int)Global_Save0;
}

float MathHelper_Sqrt_Float(const float X)
{
	arm_sqrt_f32(X,&Global_Save0);
	return Global_Save0;
}
	
u32 MathHelper_FindMax_Uint32(u32* Source_Array, u16 Lenth, u16* Index)
{
    u16 i;
	u32 Max=Source_Array[0];

	if (Index!=NULL)
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]>Max)
			{
				Max=Source_Array[i];
				*Index=i;
			}
		}
	}
	else
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]>Max)
				Max=Source_Array[i];
		}
	}
		
    return Max;
}
u16 MathHelper_FindMax_Uint16(u16* Source_Array, u16 Lenth, u16* Index)
{
    u16 i;
	u16 Max=Source_Array[0];

	if (Index!=NULL)
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]>Max)
			{
				Max=Source_Array[i];
				*Index=i;
			}
		}
	}
	else
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]>Max)
				Max=Source_Array[i];
		}
	}
		
    return Max;
}
u8 MathHelper_FindMax_Uint8(u8* Source_Array, u16 Lenth, u16* Index)
{
    u16 i;
	u8 Max=Source_Array[0];

	if (Index!=NULL)
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]>Max)
			{
				Max=Source_Array[i];
				*Index=i;
			}
		}
	}
	else
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]>Max)
				Max=Source_Array[i];
		}
	}
		
    return Max;
}
int MathHelper_FindMax_Int32(int* Source_Array, u16 Lenth, u16* Index)
{
    u16 i;
	int Max=Source_Array[0];

	if (Index!=NULL)
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]>Max)
			{
				Max=Source_Array[i];
				*Index=i;
			}
		}
	}
	else
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]>Max)
				Max=Source_Array[i];
		}
	}
		
    return Max;
}
short MathHelper_FindMax_Int16(short* Source_Array, u16 Lenth, u16* Index)
{
    u16 i;
	short Max=Source_Array[0];

	if (Index!=NULL)
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]>Max)
			{
				Max=Source_Array[i];
				*Index=i;
			}
		}
	}
	else
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]>Max)
				Max=Source_Array[i];
		}
	}
		
    return Max;
}
char MathHelper_FindMax_Int8(char* Source_Array, u16 Lenth, u16* Index)
{
    u16 i;
	signed char Max=Source_Array[0];

	if (Index!=NULL)
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]>Max)
			{
				Max=Source_Array[i];
				*Index=i;
			}
		}
	}
	else
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]>Max)
				Max=Source_Array[i];
		}
	}
		
    return Max;
}
float MathHelper_FindMax_Float(float* Source_Array, u16 Lenth, u16* Index)
{
    u16 i;
	float Max=Source_Array[0];

	if (Index!=NULL)
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]>Max)
			{
				Max=Source_Array[i];
				*Index=i;
			}
		}
	}
	else
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]>Max)
				Max=Source_Array[i];
		}
	}
		
    return Max;
}

u32 MathHelper_FindMin_Uint32(u32* Source_Array, u16 Lenth, u16* Index)
{
    u16 i;
    u32 Min=Source_Array[0];

	if (Index!=NULL)
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]<Min)
			{
				Min=Source_Array[i];
				*Index=i;
			}
		}
	}
	else
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]<Min)
				Min=Source_Array[i];
		}
	}
    return Min;
}
u16 MathHelper_FindMin_Uint16(u16* Source_Array, u16 Lenth, u16* Index)
{
    u16 i;
    u16 Min=Source_Array[0];

	if (Index!=NULL)
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]<Min)
			{
				Min=Source_Array[i];
				*Index=i;
			}
		}
	}
	else
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]<Min)
				Min=Source_Array[i];
		}
	}
    return Min;
}
u8 MathHelper_FindMin_Uint8(u8* Source_Array, u16 Lenth, u16* Index)
{
    u16 i;
    u8 Min=Source_Array[0];

	if (Index!=NULL)
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]<Min)
			{
				Min=Source_Array[i];
				*Index=i;
			}
		}
	}
	else
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]<Min)
				Min=Source_Array[i];
		}
	}
    return Min;
}
int MathHelper_FindMin_Int32(int* Source_Array, u16 Lenth, u16* Index)
{
    u16 i;
    int Min=Source_Array[0];

	if (Index!=NULL)
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]<Min)
			{
				Min=Source_Array[i];
				*Index=i;
			}
		}
	}
	else
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]<Min)
				Min=Source_Array[i];
		}
	}
    return Min;
}
short MathHelper_FindMin_Int16(short* Source_Array, u16 Lenth, u16* Index)
{
    u16 i;
    short Min=Source_Array[0];

	if (Index!=NULL)
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]<Min)
			{
				Min=Source_Array[i];
				*Index=i;
			}
		}
	}
	else
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]<Min)
				Min=Source_Array[i];
		}
	}
    return Min;
}
char MathHelper_FindMin_Int8(char* Source_Array, u16 Lenth, u16* Index)
{
    u16 i;
    signed char Min=Source_Array[0];

	if (Index!=NULL)
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]<Min)
			{
				Min=Source_Array[i];
				*Index=i;
			}
		}
	}
	else
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]<Min)
				Min=Source_Array[i];
		}
	}
    return Min;
}
float MathHelper_FindMin_Float(float* Source_Array, u16 Lenth, u16* Index)
{
    u16 i;
    float Min=Source_Array[0];

	if (Index!=NULL)
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]<Min)
			{
				Min=Source_Array[i];
				*Index=i;
			}
		}
	}
	else
	{
		for (i=1;i<Lenth;i++)
		{
			if (Source_Array[i]<Min)
				Min=Source_Array[i];
		}
	}
    return Min;
}

void MathHelper_Sequence_Uint32(u32* Source_Array, u16 Lenth, u8 Direction)
{
    int i, j;
    u16 Change_Times;
    u32 save;

    if (Direction == 0)
    {
        for (i = 0;i < Lenth - 1;i++)
        {
            for (j = 0, Change_Times = 0;j < Lenth - 1;j++)
            {
                if (Source_Array[j] < Source_Array[j + 1])
                {
                    save = Source_Array[j];
                    Source_Array[j] = Source_Array[j + 1];
                    Source_Array[j + 1] = save;
                    Change_Times++;
                }
            }
            if (Change_Times == 0)
                break;
        }
    }
    else
    {
        for (i = 0;i < Lenth - 1;i++)
        {
            for (j = 0, Change_Times = 0;j < Lenth - 1;j++)
            {
                if (Source_Array[j] > Source_Array[j + 1])
                {
                    save = Source_Array[j];
                    Source_Array[j] = Source_Array[j + 1];
                    Source_Array[j + 1] = save;
                    Change_Times++;
                }
            }
            if (Change_Times == 0)
                break;
        }
    }
}
void MathHelper_Sequence_Uint16(u16* Source_Array, u16 Lenth, u8 Direction)
{
    int i, j;
    u16 Change_Times;
    u16 save;

    if (Direction == 0)
    {
        for (i = 0;i < Lenth - 1;i++)
        {
            for (j = 0, Change_Times = 0;j < Lenth - 1;j++)
            {
                if (Source_Array[j] < Source_Array[j + 1])
                {
                    save = Source_Array[j];
                    Source_Array[j] = Source_Array[j + 1];
                    Source_Array[j + 1] = save;
                    Change_Times++;
                }
            }
            if (Change_Times == 0)
                break;
        }
    }
    else
    {
        for (i = 0;i < Lenth - 1;i++)
        {
            for (j = 0, Change_Times = 0;j < Lenth - 1;j++)
            {
                if (Source_Array[j] > Source_Array[j + 1])
                {
                    save = Source_Array[j];
                    Source_Array[j] = Source_Array[j + 1];
                    Source_Array[j + 1] = save;
                    Change_Times++;
                }
            }
            if (Change_Times == 0)
                break;
        }
    }
}
void MathHelper_Sequence_Uint8(u8* Source_Array, u16 Lenth, u8 Direction)
{
    int i, j;
    u16 Change_Times;
    u8 save;

    if (Direction == 0)
    {
        for (i = 0;i < Lenth - 1;i++)
        {
            for (j = 0, Change_Times = 0;j < Lenth - 1;j++)
            {
                if (Source_Array[j] < Source_Array[j + 1])
                {
                    save = Source_Array[j];
                    Source_Array[j] = Source_Array[j + 1];
                    Source_Array[j + 1] = save;
                    Change_Times++;
                }
            }
            if (Change_Times == 0)
                break;
        }
    }
    else
    {
        for (i = 0;i < Lenth - 1;i++)
        {
            for (j = 0, Change_Times = 0;j < Lenth - 1;j++)
            {
                if (Source_Array[j] > Source_Array[j + 1])
                {
                    save = Source_Array[j];
                    Source_Array[j] = Source_Array[j + 1];
                    Source_Array[j + 1] = save;
                    Change_Times++;
                }
            }
            if (Change_Times == 0)
                break;
        }
    }
}
void MathHelper_Sequence_Int32(int* Source_Array, u16 Lenth, u8 Direction)
{
    int i, j;
    u16 Change_Times;
    int save;

    if (Direction == 0)
    {
        for (i = 0;i < Lenth - 1;i++)
        {
            for (j = 0, Change_Times = 0;j < Lenth - 1;j++)
            {
                if (Source_Array[j] < Source_Array[j + 1])
                {
                    save = Source_Array[j];
                    Source_Array[j] = Source_Array[j + 1];
                    Source_Array[j + 1] = save;
                    Change_Times++;
                }
            }
            if (Change_Times == 0)
                break;
        }
    }
    else
    {
        for (i = 0;i < Lenth - 1;i++)
        {
            for (j = 0, Change_Times = 0;j < Lenth - 1;j++)
            {
                if (Source_Array[j] > Source_Array[j + 1])
                {
                    save = Source_Array[j];
                    Source_Array[j] = Source_Array[j + 1];
                    Source_Array[j + 1] = save;
                    Change_Times++;
                }
            }
            if (Change_Times == 0)
                break;
        }
    }
}
void MathHelper_Sequence_Int16(short* Source_Array, u16 Lenth, u8 Direction)
{
    int i, j;
    u16 Change_Times;
    short save;

    if (Direction == 0)
    {
        for (i = 0;i < Lenth - 1;i++)
        {
            for (j = 0, Change_Times = 0;j < Lenth - 1;j++)
            {
                if (Source_Array[j] < Source_Array[j + 1])
                {
                    save = Source_Array[j];
                    Source_Array[j] = Source_Array[j + 1];
                    Source_Array[j + 1] = save;
                    Change_Times++;
                }
            }
            if (Change_Times == 0)
                break;
        }
    }
    else
    {
        for (i = 0;i < Lenth - 1;i++)
        {
            for (j = 0, Change_Times = 0;j < Lenth - 1;j++)
            {
                if (Source_Array[j] > Source_Array[j + 1])
                {
                    save = Source_Array[j];
                    Source_Array[j] = Source_Array[j + 1];
                    Source_Array[j + 1] = save;
                    Change_Times++;
                }
            }
            if (Change_Times == 0)
                break;
        }
    }
}
void MathHelper_Sequence_Int8(char* Source_Array, u16 Lenth, u8 Direction)
{
    int i, j;
    u16 Change_Times;
    char save;

    if (Direction == 0)
    {
        for (i = 0;i < Lenth - 1;i++)
        {
            for (j = 0, Change_Times = 0;j < Lenth - 1;j++)
            {
                if (Source_Array[j] < Source_Array[j + 1])
                {
                    save = Source_Array[j];
                    Source_Array[j] = Source_Array[j + 1];
                    Source_Array[j + 1] = save;
                    Change_Times++;
                }
            }
            if (Change_Times == 0)
                break;
        }
    }
    else
    {
        for (i = 0;i < Lenth - 1;i++)
        {
            for (j = 0, Change_Times = 0;j < Lenth - 1;j++)
            {
                if (Source_Array[j] > Source_Array[j + 1])
                {
                    save = Source_Array[j];
                    Source_Array[j] = Source_Array[j + 1];
                    Source_Array[j + 1] = save;
                    Change_Times++;
                }
            }
            if (Change_Times == 0)
                break;
        }
    }
}
void MathHelper_Sequence_Float(float* Source_Array, u16 Lenth, u8 Direction)
{
    u16 i,j;
    u16 Change_Times;
    float save;

    if (Direction==0)
    {
        for (i=0;i<Lenth-1;i++)
        {
            for (j=0,Change_Times=0;j<Lenth-1;j++)
            {
                if (Source_Array[j]<Source_Array[j+1])
                {
                    save=Source_Array[j];
                    Source_Array[j]=Source_Array[j+1];
                    Source_Array[j+1]=save;
                    Change_Times++;
                }
            }
            if (Change_Times==0)
                break;
        }
    }
    else if (Direction==1)
    {
        for (i=0;i<Lenth-1;i++)
        {
            for (j=0,Change_Times=0;j<Lenth-1;j++)
            {
                if (Source_Array[j]>Source_Array[j+1])
                {
                    save=Source_Array[j];
                    Source_Array[j]=Source_Array[j+1];
                    Source_Array[j+1]=save;
                    Change_Times++;
                }
            }
            if (Change_Times==0)
                break;
        }
    }
}
void MathHelper_Sequence_Double(double* Source_Array, u16 Lenth, u8 Direction)
{
    u16 i,j;
    u16 Change_Times;
    double save;

    if (Direction==0)
    {
        for (i=0;i<Lenth-1;i++)
        {
            for (j=0,Change_Times=0;j<Lenth-1;j++)
            {
                if (Source_Array[j]<Source_Array[j+1])
                {
                    save=Source_Array[j];
                    Source_Array[j]=Source_Array[j+1];
                    Source_Array[j+1]=save;
                    Change_Times++;
                }
            }
            if (Change_Times==0)
                break;
        }
    }
    else if (Direction==1)
    {
        for (i=0;i<Lenth-1;i++)
        {
            for (j=0,Change_Times=0;j<Lenth-1;j++)
            {
                if (Source_Array[j]>Source_Array[j+1])
                {
                    save=Source_Array[j];
                    Source_Array[j]=Source_Array[j+1];
                    Source_Array[j+1]=save;
                    Change_Times++;
                }
            }
            if (Change_Times==0)
                break;
        }
    }
}


u32 MathHelper_Average_Uint32(u32* Source_Array, u16 Lenth)
{
    u16 i;
	float Ave;
	
    for (i = 0;i < Lenth;i++)
        Ave += (float)Source_Array[i];
	Ave/=(float)Lenth;

    return (u32)Ave;
}
u16 MathHelper_Average_Uint16(u16* Source_Array, u16 Lenth)
{
    u16 i;
    float Ave = 0;

    for (i = 0;i < Lenth;i++)
        Ave += (float)Source_Array[i];
	Ave/=(float)Lenth;

    return (u16)Ave;
}
u8 MathHelper_Average_Uint8(u8* Source_Array, u16 Lenth)
{
    u16 i;
    float Ave = 0;

    for (i = 0;i < Lenth;i++)
        Ave += (float)Source_Array[i];
	Ave/=(float)Lenth;

    return (u8)Ave;
}
int MathHelper_Average_Int32(int* Source_Array, u16 Lenth)
{
    u16 i;
    float Ave = 0;

    for (i = 0;i < Lenth;i++)
        Ave += (float)Source_Array[i];
	Ave/=(float)Lenth;

    return (int)Ave;
}
short MathHelper_Average_Int16(short* Source_Array, u16 Lenth)
{
    u16 i;
    float Ave = 0;

    for (i = 0;i < Lenth;i++)
        Ave += (float)Source_Array[i];
	Ave/=(float)Lenth;

    return (short)Ave;
}
char MathHelper_Average_Int8(char* Source_Array, u16 Lenth)
{
    u16 i;
    float Ave = 0;

    for (i = 0;i < Lenth;i++)
        Ave += (float)Source_Array[i];
	Ave/=(float)Lenth;

    return (char)Ave;
}
float MathHelper_Average_Float(float* Source_Array, u16 Lenth)
{
    u16 i;
    float Ave = 0;

    for (i = 0;i < Lenth;i++)
        Ave += Source_Array[i] / Lenth;

    return Ave;
}

void MathHelper_DCRemove_Float(float* Source_Array, u16 Length)
{
    float mean=MathHelper_Average_Float(Source_Array,Length);
    for (int i=0;i<Length;i++)
        Source_Array[i]-=mean;
}

float MathHelper_RMS_Float(float* Source_Array, u16 Length)
{
    float rms;
    arm_rms_f32(Source_Array,Length,&rms);
    return rms;
}


float MathHelper_SquareSum_Uint32(u32* Source_Array, u16 Lenth)
{
    int i;
    float Squ_Sum = 0;

    for (i = 0;i < Lenth;i++)
        Squ_Sum += Source_Array[i] * Source_Array[i];

    return Squ_Sum;
}
float MathHelper_SquareSum_Uint16(u16* Source_Array, u16 Lenth)
{
    int i;
    float Squ_Sum = 0;

    for (i = 0;i < Lenth;i++)
        Squ_Sum += Source_Array[i] * Source_Array[i];

    return Squ_Sum;
}
float MathHelper_SquareSum_Int32(int* Source_Array, u16 Lenth)
{
    int i;
    float Squ_Sum = 0;

    for (i = 0;i < Lenth;i++)
        Squ_Sum += Source_Array[i] * Source_Array[i];

    return Squ_Sum;
}
float MathHelper_SquareSum_Float(float* Source_Array, u16 Lenth)
{
    int i;
    float Squ_Sum = 0;

    for (i = 0;i < Lenth;i++)
        Squ_Sum += Source_Array[i] * Source_Array[i];

    return Squ_Sum;
}

float MathHelper_Variance_Float(float* Source_Array, u16 Length)
{
	arm_var_f32(Source_Array,Length,&Global_Save0);
	return Global_Save0;
}

void MathHelper_Moving_Average_Uint32(u32* Source_Array, float* Target_Array, u16 Lenth, u8 Block_Size)
{
    int j;
    u16 i;
    u8 Half_Block_Size = Block_Size / 2;
    float save = 0;

    for (i = 0;i < Lenth;i++)
    {
        for (j = i;j < i + Half_Block_Size + 1;j++)
        {
            if (j == Lenth)
                break;
            save += Source_Array[j];
        }
        if (j == Lenth)
            save += Source_Array[Lenth - 1] * (i + Half_Block_Size - Lenth + 1);

        for (j = i;j > i - Half_Block_Size - 1;j--)
        {
            if (j == -1)
                break;
            save += Source_Array[j];
        }
        if (j == -1)
            save += Source_Array[0] * (Half_Block_Size - i);
        save -= Source_Array[i];
        save /= Block_Size;
        Target_Array[i] = save;
        save = 0;
    }
}
void MathHelper_Moving_Average_Uint16(u16* Source_Array, float* Target_Array, u16 Lenth, u8 Block_Size)
{
    int j;
    u16 i;
    u8 Half_Block_Size = Block_Size / 2;
    float save = 0;

    for (i = 0;i < Lenth;i++)
    {
        for (j = i;j < i + Half_Block_Size + 1;j++)
        {
            if (j == Lenth)
                break;
            save += Source_Array[j];
        }
        if (j == Lenth)
            save += Source_Array[Lenth - 1] * (i + Half_Block_Size - Lenth + 1);

        for (j = i;j > i - Half_Block_Size - 1;j--)
        {
            if (j == -1)
                break;
            save += Source_Array[j];
        }
        if (j == -1)
            save += Source_Array[0] * (Half_Block_Size - i);
        save -= Source_Array[i];
        save /= Block_Size;
        Target_Array[i] = save;
        save = 0;
    }
}
void MathHelper_Moving_Average_Int32(int* Source_Array, float* Target_Array, u16 Lenth, u8 Block_Size)
{
    int j;
    u16 i;
    u8 Half_Block_Size = Block_Size / 2;
    float save = 0;

    for (i = 0;i < Lenth;i++)
    {
        for (j = i;j < i + Half_Block_Size + 1;j++)
        {
            if (j == Lenth)
                break;
            save += Source_Array[j];
        }
        if (j == Lenth)
            save += Source_Array[Lenth - 1] * (i + Half_Block_Size - Lenth + 1);

        for (j = i;j > i - Half_Block_Size - 1;j--)
        {
            if (j == -1)
                break;
            save += Source_Array[j];
        }
        if (j == -1)
            save += Source_Array[0] * (Half_Block_Size - i);
        save -= Source_Array[i];
        save /= Block_Size;
        Target_Array[i] = save;
        save = 0;
    }
}
void MathHelper_Moving_Average_Float(float* Source_Array, float* Target_Array, u16 Lenth, u8 Block_Size)
{
    int j;
    u16 i;
    u8 Half_Block_Size = Block_Size / 2;
    float save = 0;

    for (i = 0;i < Lenth;i++)
    {
        for (j = i;j < i + Half_Block_Size + 1;j++)
        {
            if (j == Lenth)
                break;
            save += Source_Array[j];
        }
        if (j == Lenth)
            save += Source_Array[Lenth - 1] * (i + Half_Block_Size - Lenth + 1);

        for (j = i;j > i - Half_Block_Size - 1;j--)
        {
            if (j == -1)
                break;
            save += Source_Array[j];
        }
        if (j == -1)
            save += Source_Array[0] * (Half_Block_Size - i);
        save -= Source_Array[i];
        save /= Block_Size;
        Target_Array[i] = save;
        save = 0;
    }
}

void MathHelper_MidValue_Filter_Uint32(const u32* Source_Array, u32* Target_Array, u16 Length)
{
	u8 j;
	u16 i;
	float save[4];
	
	Target_Array[0]=Source_Array[0];
	for (i=1;i<Length-1;i++)
	{
		save[0]=Source_Array[i-1];
		save[1]=Source_Array[i];
		save[2]=Source_Array[i+1];
		
		for (j=0;j<2;j++)
		{
			if (save[0]<save[1])
			{
				save[3]=save[0];
				save[0]=save[1];
				save[1]=save[3];
			}
			if (save[1]<save[2])
			{
				save[3]=save[1];
				save[1]=save[2];
				save[2]=save[3];
			}
		}
		Target_Array[i]=save[1];
	}
	Target_Array[Length-1]=Source_Array[Length-1];
}

void MathHelper_MidValue_Filter_Uint16(const u16* Source_Array, u16* Target_Array, u16 Length)
{
	u8 j;
	u16 i;
	float save[4];
	
	Target_Array[0]=Source_Array[0];
	for (i=1;i<Length-1;i++)
	{
		save[0]=Source_Array[i-1];
		save[1]=Source_Array[i];
		save[2]=Source_Array[i+1];
		
		for (j=0;j<2;j++)
		{
			if (save[0]<save[1])
			{
				save[3]=save[0];
				save[0]=save[1];
				save[1]=save[3];
			}
			if (save[1]<save[2])
			{
				save[3]=save[1];
				save[1]=save[2];
				save[2]=save[3];
			}
		}
		Target_Array[i]=save[1];
	}
	Target_Array[Length-1]=Source_Array[Length-1];
}

void MathHelper_MidValue_Filter_Uint8(const u8* Source_Array, u8* Target_Array, u16 Length)
{
	u8 j;
	u16 i;
	float save[4];
	
	Target_Array[0]=Source_Array[0];
	for (i=1;i<Length-1;i++)
	{
		save[0]=Source_Array[i-1];
		save[1]=Source_Array[i];
		save[2]=Source_Array[i+1];
		
		for (j=0;j<2;j++)
		{
			if (save[0]<save[1])
			{
				save[3]=save[0];
				save[0]=save[1];
				save[1]=save[3];
			}
			if (save[1]<save[2])
			{
				save[3]=save[1];
				save[1]=save[2];
				save[2]=save[3];
			}
		}
		Target_Array[i]=save[1];
	}
	Target_Array[Length-1]=Source_Array[Length-1];
}
	
void MathHelper_MidValue_Filter_Int32(const int* Source_Array, int* Target_Array, u16 Length)
{
	u8 j;
	u16 i;
	float save[4];
	
	Target_Array[0]=Source_Array[0];
	for (i=1;i<Length-1;i++)
	{
		save[0]=Source_Array[i-1];
		save[1]=Source_Array[i];
		save[2]=Source_Array[i+1];
		
		for (j=0;j<2;j++)
		{
			if (save[0]<save[1])
			{
				save[3]=save[0];
				save[0]=save[1];
				save[1]=save[3];
			}
			if (save[1]<save[2])
			{
				save[3]=save[1];
				save[1]=save[2];
				save[2]=save[3];
			}
		}
		Target_Array[i]=save[1];
	}
	Target_Array[Length-1]=Source_Array[Length-1];
}

void MathHelper_MidValue_Filter_Int16(const short* Source_Array, short* Target_Array, u16 Length)
{
	u8 j;
	u16 i;
	float save[4];
	
	Target_Array[0]=Source_Array[0];
	for (i=1;i<Length-1;i++)
	{
		save[0]=Source_Array[i-1];
		save[1]=Source_Array[i];
		save[2]=Source_Array[i+1];
		
		for (j=0;j<2;j++)
		{
			if (save[0]<save[1])
			{
				save[3]=save[0];
				save[0]=save[1];
				save[1]=save[3];
			}
			if (save[1]<save[2])
			{
				save[3]=save[1];
				save[1]=save[2];
				save[2]=save[3];
			}
		}
		Target_Array[i]=save[1];
	}
	Target_Array[Length-1]=Source_Array[Length-1];
}

void MathHelper_MidValue_Filter_Int8(const char* Source_Array, char* Target_Array, u16 Length)
{
	u8 j;
	u16 i;
	float save[4];
	
	Target_Array[0]=Source_Array[0];
	for (i=1;i<Length-1;i++)
	{
		save[0]=Source_Array[i-1];
		save[1]=Source_Array[i];
		save[2]=Source_Array[i+1];
		
		for (j=0;j<2;j++)
		{
			if (save[0]<save[1])
			{
				save[3]=save[0];
				save[0]=save[1];
				save[1]=save[3];
			}
			if (save[1]<save[2])
			{
				save[3]=save[1];
				save[1]=save[2];
				save[2]=save[3];
			}
		}
		Target_Array[i]=save[1];
	}
	Target_Array[Length-1]=Source_Array[Length-1];
}

void MathHelper_MidValue_Filter_Float(float* Source_Array, float* Target_Array, u16 Length)
{
	u8 j;
	u16 i;
	float save[4];
	
	Target_Array[0]=Source_Array[0];
	for (i=1;i<Length-1;i++)
	{
		save[0]=Source_Array[i-1];
		save[1]=Source_Array[i];
		save[2]=Source_Array[i+1];
		
		for (j=0;j<2;j++)
		{
			if (save[0]<save[1])
			{
				save[3]=save[0];
				save[0]=save[1];
				save[1]=save[3];
			}
			if (save[1]<save[2])
			{
				save[3]=save[1];
				save[1]=save[2];
				save[2]=save[3];
			}
		}
		Target_Array[i]=save[1];
	}
	Target_Array[Length-1]=Source_Array[Length-1];
}

u16 MathHelper_BadValue_Remove_Float(const float* Source_Array, float* Target_Array, u16 Length, float Mean, float Variance, float Con_Interval)
{
	u16 i,j;
	
	Global_Save0=MathHelper_Sqrt_Float(Variance)*Con_Interval;
	for (i=0,j=0;i<Length;i++)
	{
		if (MATHHELPER_ABS(Source_Array[i]-Mean)<Global_Save0)
		{
			Target_Array[j]=Source_Array[i];
			j++;
		}
	}
	
	for (i=0;j<Length;j++,i++)
		Target_Array[j]=0;
	
	return Length-i;
}

void MathHelper_Linear_Interpolation(float* Source_Array, float* Target_Array, u16 Source_Length, u16 Target_Length)
{
	u16 i;
	float k,s;
	
	k=((float)Source_Length-1)/((float)Target_Length-1);
	Target_Array[0]=Source_Array[0];
	for (i=1;i<Target_Length-1;i++)
	{
		s=i*k;
		Target_Array[i]=(Source_Array[(u16)(s+1)]-Source_Array[(u16)s])*(s-(u16)s)+Source_Array[(u16)s];
	}
	Target_Array[Target_Length-1]=Source_Array[Source_Length-1];
}

void MathHelper_DFT(float* Samples, float* Spectrum, u16 Sample_Points, u32 Sampling_Frequency)
{
    int k, t;
    float BASIC_FREQUENCY = (2 * MATHHELPER_PI * Sampling_Frequency) / Sample_Points;
    float Real_Part, Imaginary_Part;

    Spectrum[0] = 0;
    for (t = 0;t < Sample_Points;t++)
        Spectrum[0] += Samples[t];
	
    for (k = 1; k < Sample_Points/2; k++)
    {
        Real_Part = 0;
        Imaginary_Part = 0;
        for (t = 0; t < Sample_Points; t++)
        {
            Real_Part += Samples[t] * cosf((-(float)t / Sampling_Frequency) * k * BASIC_FREQUENCY);
            Imaginary_Part += Samples[t] * sinf((-(float)t / Sampling_Frequency) * k * BASIC_FREQUENCY);
        }
        Spectrum[k] = MathHelper_Sqrt_Float(Real_Part*Real_Part+Imaginary_Part*Imaginary_Part) * 2;
    }
}

#ifdef USE_FFT
u8 MathHelper_FFT_Plan_Build(struct FFT_Plan_Information *Execute_Plan)
{
	if ((Execute_Plan->Sample_Number!=16)&&(Execute_Plan->Sample_Number!=32)&&
		(Execute_Plan->Sample_Number!=64)&&(Execute_Plan->Sample_Number!=128)&&
		(Execute_Plan->Sample_Number!=256)&&(Execute_Plan->Sample_Number!=512)&&
		(Execute_Plan->Sample_Number!=1024)&&(Execute_Plan->Sample_Number!=2048))
		return 1;
	
	if ((Execute_Plan->Plan_Order!=0)&&(Execute_Plan->Plan_Order!=1)&&
		(Execute_Plan->Plan_Order!=2)&&(Execute_Plan->Plan_Order!=3))
		return 2;
	
	FFT_Plan_List[Execute_Plan->Plan_Order].Plan=Execute_Plan;
	
	FFT_Plan_List[Execute_Plan->Plan_Order].Data_Buffer=(float*)Memory_Malloc(&MATHHELPER_HEAP,Execute_Plan->Sample_Number*8);

	if (Execute_Plan->Sample_Number==16)
		FFT_Plan_List[Execute_Plan->Plan_Order].FFT_Struct=&arm_cfft_sR_f32_len16;
	else if (Execute_Plan->Sample_Number==32)
		FFT_Plan_List[Execute_Plan->Plan_Order].FFT_Struct=&arm_cfft_sR_f32_len32;
	else if (Execute_Plan->Sample_Number==64)
		FFT_Plan_List[Execute_Plan->Plan_Order].FFT_Struct=&arm_cfft_sR_f32_len64;
	else if (Execute_Plan->Sample_Number==128)
		FFT_Plan_List[Execute_Plan->Plan_Order].FFT_Struct=&arm_cfft_sR_f32_len128;
	else if (Execute_Plan->Sample_Number==256)
		FFT_Plan_List[Execute_Plan->Plan_Order].FFT_Struct=&arm_cfft_sR_f32_len256;
	else if (Execute_Plan->Sample_Number==512)
		FFT_Plan_List[Execute_Plan->Plan_Order].FFT_Struct=&arm_cfft_sR_f32_len512;
	else if (Execute_Plan->Sample_Number==1024)
		FFT_Plan_List[Execute_Plan->Plan_Order].FFT_Struct=&arm_cfft_sR_f32_len1024;
	else if (Execute_Plan->Sample_Number==2048)
		FFT_Plan_List[Execute_Plan->Plan_Order].FFT_Struct=&arm_cfft_sR_f32_len2048;

	/*if ((Execute_Plan->Data_Type==DATA_TYPE_INT16)||(Execute_Plan->Data_Type==DATA_TYPE_UINT16)||
		(Execute_Plan->Data_Type==DATA_TYPE_INT8)||(Execute_Plan->Data_Type==DATA_TYPE_UINT8))
		FFT_Plan_List[Execute_Plan->Plan_Order].Temporary_Data_Buffer=(float*)Malloc(MATHHELPER_HEAP,(Execute_Plan->Sample_Number/2+1)*4);
	*/
	FFT_Plan_List[Execute_Plan->Plan_Order].Has_Been_Built=1;

	return 0;
}

u8 MathHelper_FFT(u8 FFT_Plan_Order)
{
	u16 i;
	u16 Sample_Number=FFT_Plan_List[FFT_Plan_Order].Plan->Sample_Number;
	float* input_addr;
    
	if ((FFT_Plan_Order!=0)&&(FFT_Plan_Order!=1)&&//如果序号不对
		(FFT_Plan_Order!=2)&&(FFT_Plan_Order!=3))
		return 1;
	
	if (FFT_Plan_List[FFT_Plan_Order].Has_Been_Built==0)//如果计划未初始化
		return 2;
	
	
	
    /*********************************运算过程*********************************************/
    //如果不是复变换
	if (!FFT_Plan_List[FFT_Plan_Order].Plan->Is_Complex)
    {
        for (i=0;i<Sample_Number;i++)
            FFT_Plan_List[FFT_Plan_Order].Data_Buffer[i*2]=FFT_Plan_List[FFT_Plan_Order].Plan->Input_Data[i];
        for (i=0;i<Sample_Number;i++)//虚部置0
            FFT_Plan_List[FFT_Plan_Order].Data_Buffer[i*2+1]=0;
        input_addr=FFT_Plan_List[FFT_Plan_Order].Data_Buffer;
    }
    else
        input_addr=FFT_Plan_List[FFT_Plan_Order].Plan->Input_Data;
    
	//if (FFT_Plan_List[FFT_Plan_Order].Plan->Data_Type==DATA_TYPE_FLOAT)
	//{
        //如果不是复变换
        /*if (!FFT_Plan_List[FFT_Plan_Order].Plan->Is_Complex)
        {
            for (i=0;i<Sample_Number;i++)//补全实部
                FFT_Plan_List[FFT_Plan_Order].Data_Buffer[i*2]=((float*)FFT_Plan_List[FFT_Plan_Order].Plan->Input_Data)[i];
		}*/
		//FFT运算
		//arm_cfft_f32(FFT_Plan_List[FFT_Plan_Order].FFT_Struct,FFT_Plan_List[FFT_Plan_Order].Data_Buffer,0,1);
    arm_cfft_f32(FFT_Plan_List[FFT_Plan_Order].FFT_Struct,input_addr,0,1);
		
		//求模值
		//arm_cmplx_mag_f32(FFT_Plan_List[FFT_Plan_Order].Data_Buffer,FFT_Plan_List[FFT_Plan_Order].Plan->Output_Data,Sample_Number/2+1);
    if (!FFT_Plan_List[FFT_Plan_Order].Plan->Original_Output)
    {
        arm_cmplx_mag_f32(input_addr,FFT_Plan_List[FFT_Plan_Order].Plan->Output_Data,Sample_Number/2+1);

		for (i=1;i<Sample_Number/2;i++)//幅度*2
			FFT_Plan_List[FFT_Plan_Order].Plan->Output_Data[i]*=2;
		for (i=0;i<Sample_Number/2+1;i++)//除以数据长度
			FFT_Plan_List[FFT_Plan_Order].Plan->Output_Data[i]/=FFT_Plan_List[FFT_Plan_Order].Plan->Sample_Number;
    }
    else
    {
        for (i=0;i<Sample_Number*2;i++)//除以数据长度
        {
            FFT_Plan_List[FFT_Plan_Order].Plan->Output_Data[i]=input_addr[i];
			FFT_Plan_List[FFT_Plan_Order].Plan->Output_Data[i]/=FFT_Plan_List[FFT_Plan_Order].Plan->Sample_Number;
        }
    }
    
	return 0;
}

void MathHelper_FFT_Plan_Cancel(struct FFT_Plan_Information *Execute_Plan)
{
	Memory_Free(&MATHHELPER_HEAP,FFT_Plan_List[Execute_Plan->Plan_Order].Data_Buffer);
	Memory_Free(&MATHHELPER_HEAP,FFT_Plan_List[Execute_Plan->Plan_Order].Temporary_Data_Buffer);
	FFT_Plan_List[Execute_Plan->Plan_Order].Plan=NULL;
	FFT_Plan_List[Execute_Plan->Plan_Order].Has_Been_Built=0;
}
#endif

void* MathHelper_Get_arm_fft_struct(u16 len)
{
	switch (len)
	{
		case 16:
			return (void*)(&arm_cfft_sR_f32_len16);
		case 32:
			return (void*)(&arm_cfft_sR_f32_len32);
		case 64:
			return (void*)(&arm_cfft_sR_f32_len64);
		case 128:
			return (void*)(&arm_cfft_sR_f32_len128);
		case 256:
			return (void*)(&arm_cfft_sR_f32_len256);
		case 512:
			return (void*)(&arm_cfft_sR_f32_len512);
		case 1024:
			return (void*)(&arm_cfft_sR_f32_len1024);
		case 2048:
			return (void*)(&arm_cfft_sR_f32_len2048);
		default:
			break;
	}
	return 0;
}
