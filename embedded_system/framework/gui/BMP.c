#include "BMP.h"

/*��rBMP��ʽ�������ж�λͼ���ĳһ�����ݵ���ʼλ��*/
uint8_t _rBMP_Locate_Line(const rBMP* Pic, uint16_t Line, uint32_t* Index, uint16_t* Remain);

/*��rBMP��ʽ�������ж�ȡͼ���ĳ��������*/
uint8_t _rBMP_Read_Lines(const rBMP* Pic, uint16_t* Out, uint16_t Lines, uint32_t Index, uint16_t Remain);

/*��prBMP��ʽ�������ж�λͼ���ĳһ�����ݵ���ʼλ��*/
uint8_t _prBMP_Locate_Line(const prBMP* Pic, uint16_t Line, uint32_t* Index, uint16_t* Remain);

/*��prBMP��ʽ�������ж�ȡͼ���ĳ��������*/
uint8_t _prBMP_Read_Lines(const prBMP* Pic, uint16_t* Out, uint16_t Lines, uint32_t Index, uint16_t Remain);

uint8_t _rBMP_Locate_Line(const rBMP* Pic, uint16_t Line, uint32_t* Index, uint16_t* Remain)
{
	uint32_t i,j;
	uint32_t save;
	
	save=Pic->Width*Line;
	for (i=0,j=0;j+Pic->Colors[i]<save;i+=2)
		j+=Pic->Colors[i];
	
	*Index=i;
	*Remain=Pic->Colors[i]-save+j;
	
	return 0;
}

uint8_t _rBMP_Read_Lines(const rBMP* Pic, uint16_t* Out, uint16_t Lines, uint32_t Index, uint16_t Remain)
{
	uint32_t k;
	uint32_t i,j;
	uint32_t save;
	
	j=Index;
	for (i=0;i<Remain;i++)
		Out[i]=Pic->Colors[j+1];
	
	save=Pic->Width*Lines;
	for (j+=2;i<save;j+=2)
	{
		for (k=0;(k<Pic->Colors[j])&&(i<save);i++,k++)
			Out[i]=Pic->Colors[j+1];
	}
	
	return 0;
}

uint8_t _prBMP_Locate_Line(const prBMP* Pic, uint16_t Line, uint32_t* Index, uint16_t* Remain)
{
	uint32_t i,j;
	uint32_t save;
	
	save=Pic->Width*Line;
	for (i=0,j=0;j+Pic->Color_Blocks[i]<save;i++)
		j+=Pic->Color_Blocks[i];
	
	*Index=i;
	*Remain=Pic->Color_Blocks[i]-save+j;
	
	return 0;
}

uint8_t _prBMP_Read_Lines(const prBMP* Pic, uint16_t* Out, uint16_t Lines, uint32_t Index, uint16_t Remain)
{
	uint32_t k;
	uint32_t i,j;
	uint32_t save;
	
	j=Index;
	for (i=0;i<Remain;i++)
		Out[i]=Pic->Color_Palette[Pic->Colors[j]];
	
	save=Pic->Width*Lines;
	for (j++;i<save;j++)
	{
		for (k=0;(k<Pic->Color_Blocks[j])&&(i<save);i++,k++)
			Out[i]=Pic->Color_Palette[Pic->Colors[j]];
	}
	
	return 0;
}

uint8_t rBMP2BMP_Decode(const rBMP* In, BMP* Out, uint16_t Start, uint16_t Lines)
{
	uint32_t m;
	uint16_t n;
	
	_rBMP_Locate_Line(In,Start,&m,&n);
	_rBMP_Read_Lines(In,Out->Colors,Lines,m,n);
	
	Out->Height=Lines;
	Out->Width=In->Width;
	
	return 0;
}

uint8_t pBMP2BMP_Decode(const pBMP* In, BMP* Out, uint16_t Start, uint16_t Lines)
{
	uint16_t i,j;
	uint16_t* save;
	uint32_t Index;
	
	save=Out->Colors;
	Index=0;
	for (i=0;i<Lines;i++)
	{
		for (j=0;j<In->Width;j++)
			save[j]=In->Color_Palette[In->Colors[Index+j]];
		save+=In->Width;
		Index+=In->Width;
	}
	Out->Height=Lines;
	Out->Width=In->Width;
	
	return 0;
}

uint8_t prBMP2BMP_Decode(const prBMP* In, BMP* Out, uint16_t Start, uint16_t Lines)
{
	uint32_t m;
	uint16_t n;
	
	_prBMP_Locate_Line(In,Start,&m,&n);
	_prBMP_Read_Lines(In,Out->Colors,Lines,m,n);
	
	Out->Height=Lines;
	Out->Width=In->Width;
	
	return 0;
}

