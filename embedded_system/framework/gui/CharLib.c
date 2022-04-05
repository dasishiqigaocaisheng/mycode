#include "CharLib.h"

//#define VACANCY         0

//#define BASIC_WIDTH		16
//#define BASIC_HEIGHT	30

void CharMold2BMP_Decode(charmold *chmd, uint16_t bcolor, uint16_t ccolor, BMP *p_out)
{
	uint16_t color;
	uint32_t k;

	k = chmd->Width * chmd->Height;
	color = bcolor;
	for (uint16_t j = 0, i = 0; j < k; i++)
	{
		for (uint16_t m = 0; m < chmd->Blocks[i]; m++, j++)
			p_out->Colors[j] = color;
		if (color == bcolor)
			color = ccolor;
		else
			color = bcolor;
	}
	p_out->Height = chmd->Height;
	p_out->Width = chmd->Width;
}

// and_cn.bmp
const uint16_t _and_cn_Blocks[] =
	{
		150, 5, 10, 2, 14, 1, 15, 1, 15, 1, 16, 1, 14, 2, 13, 1, 2, 1, 3, 2, 6, 1, 4, 1, 2, 1, 7, 1, 4, 1, 1, 1, 8, 1, 5, 2, 8, 1, 5, 2, 9, 1, 4, 2, 10, 4, 2, 2, 115};

// antidivision_cn.bmp
const uint16_t _antidivision_cn_Blocks[] =
	{
		66, 1, 15, 1, 16, 1, 15, 1, 16, 1, 15, 1, 16, 1, 15, 1, 15, 2, 15, 1, 15, 2, 15, 1, 15, 2, 15, 1, 15, 1, 16, 1, 15, 1, 16, 1, 15, 1, 16, 1, 15, 1, 84};

// at_cn.bmp
const uint16_t _at_cn_Blocks[] =
	{
		86, 5, 10, 1, 5, 1, 8, 1, 7, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 5, 4, 6, 1, 4, 1, 3, 1, 6, 1, 3, 1, 4, 1, 6, 1, 3, 1, 4, 1, 6, 1, 3, 1, 4, 1, 6, 1, 4, 1, 3, 1, 6, 1, 5, 5, 5, 1, 15,
		1, 16, 1, 15, 1, 16, 1, 5, 1, 10, 5, 101};

// backquote_cn.bmp
const uint16_t _backquote_cn_Blocks[] =
	{
		86, 1, 16, 2, 15, 2, 16, 1, 341};

// caret_cn.bmp
const uint16_t _caret_cn_Blocks[] =
	{
		119, 2, 14, 2, 13, 1, 2, 1, 11, 1, 4, 1, 9, 1, 6, 1, 8, 1, 6, 1, 7, 1, 8, 1, 259};

// colon_cn.bmp
const uint16_t _colon_cn_Blocks[] =
	{
		183, 2, 13, 4, 12, 4, 13, 2, 78, 2, 13, 4, 12, 4, 13, 2, 119};

// comma_cn.bmp
const uint16_t _comma_cn_Blocks[] =
	{
		310, 4, 12, 3, 12, 4, 12, 3, 13, 3, 12, 3, 13, 2, 14, 2, 58};

// division_cn.bmp
const uint16_t _division_cn_Blocks[] =
	{
		75, 1, 15, 1, 14, 1, 15, 1, 14, 1, 15, 1, 14, 1, 15, 1, 14, 2, 14, 1, 14, 2, 14, 1, 14, 2, 14, 1, 15, 1, 14, 1, 15, 1, 14, 1, 15, 1, 14, 1, 15, 1, 93};

// dollar_cn.bmp
const uint16_t _dollar_cn_Blocks[] =
	{
		87, 1, 15, 1, 13, 4, 11, 1, 4, 3, 7, 1, 7, 1, 7, 1, 7, 1, 7, 1, 15, 1, 16, 3, 16, 4, 16, 1, 16, 1, 15, 1, 5, 1, 9, 1, 5, 1, 8, 2, 5, 2, 6, 2, 6, 1, 1, 6, 13, 1, 15, 1, 15, 1, 15, 1, 72};

// dot_cn.bmp
const uint16_t _dot_cn_Blocks[] =
	{
		311, 2, 13, 4, 12, 4, 13, 2, 119};

// dualquotation_cn.bmp
const uint16_t _dualquotation_cn_Blocks[] =
	{
		99, 3, 3, 3, 7, 3, 3, 3, 7, 3, 3, 3, 8, 2, 4, 2, 8, 2, 4, 2, 8, 2, 4, 2, 8, 2, 4, 2, 276};

// equal_cn.bmp
const uint16_t _equal_cn_Blocks[] =
	{
		209, 13, 51, 13, 194};

// exclamation_cn.bmp
const uint16_t _exclamation_cn_Blocks[] =
	{
		104, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 62, 3, 13, 3, 13, 3, 118};

// leftbrace_cn.bmp
const uint16_t _leftbrace_cn_Blocks[] =
	{
		120, 2, 13, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 13, 2, 16, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 16, 2, 70};

// leftbracket_cn.bmp
const uint16_t _leftbracket_cn_Blocks[] =
	{
		107, 1, 14, 2, 14, 1, 14, 2, 14, 1, 15, 1, 14, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 16, 1, 15, 1, 15, 2, 15, 1, 15, 2, 15, 1, 68};

// leftmidbacket_cn.bmp
const uint16_t _leftmidbracket_cn_Blocks[] =
	{
		103, 4, 12, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 4, 69};

// lessthan_cn.bmp
const uint16_t _lessthan_cn_Blocks[] =
	{
		156, 2, 12, 2, 12, 2, 13, 1, 13, 2, 12, 2, 12, 2, 16, 2, 16, 2, 16, 1, 16, 2, 16, 2, 16, 2, 130};

// minus_cn.bmp
const uint16_t _minus_cn_Blocks[] =
	{
		226, 11, 5, 11, 227};

// morethan_cn.bmp
const uint16_t _morethan_cn_Blocks[] =
	{
		146, 2, 16, 2, 16, 2, 16, 1, 16, 2, 16, 2, 16, 2, 12, 2, 12, 2, 13, 1, 13, 2, 12, 2, 12, 2, 140};

// multiply_cn.bmp
const uint16_t _multiply_cn_Blocks[] =
	{
		103, 1, 15, 1, 15, 1, 15, 1, 11, 2, 2, 1, 3, 2, 8, 6, 12, 1, 15, 2, 13, 1, 2, 1, 11, 1, 4, 1, 229};

// num0_cn.bmp
const uint16_t _num0_cn_Blocks[] =
	{
		102, 4, 11, 1, 4, 1, 9, 1, 6, 1, 8, 1, 6, 1, 7, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 7, 1, 6, 1, 8, 1, 6, 1, 9, 1, 4, 1, 11,
		4, 118};

// num1_cn.bmp
const uint16_t _num1_cn_Blocks[] =
	{
		103, 2, 12, 4, 10, 2, 3, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 10, 11, 114};

// num2_cn.bmp
const uint16_t _num2_cn_Blocks[] =
	{
		102, 4, 10, 2, 4, 2, 8, 1, 6, 1, 7, 1, 8, 1, 6, 1, 8, 1, 15, 1, 14, 1, 14, 1, 14, 1, 14, 1, 14, 1, 14, 1, 14, 1, 14, 1, 14, 1, 14, 1, 9, 1, 5, 11, 115};

// num3_cn.bmp
const uint16_t _num3_cn_Blocks[] =
	{
		101, 5, 10, 2, 4, 1, 8, 1, 7, 1, 15, 1, 15, 1, 15, 1, 14, 1, 12, 3, 16, 1, 16, 1, 16, 1, 15, 1, 15, 1, 15, 1, 5, 2, 7, 1, 7, 2, 5, 1, 10, 5, 118};

// num4_cn.bmp
const uint16_t _num4_cn_Blocks[] =
	{
		105, 2, 13, 1, 1, 1, 13, 1, 1, 1, 12, 1, 2, 1, 12, 1, 2, 1, 11, 1, 3, 1, 11, 1, 3, 1, 10, 1, 4, 1, 10, 1, 4, 1, 9, 1, 5, 1, 9, 1, 5, 1, 8, 1, 6, 1, 8, 10, 13, 1, 15, 1, 15, 1, 12, 6, 115};

// num5_cn.bmp
const uint16_t _num5_cn_Blocks[] =
	{
		99, 8, 8, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 1, 5, 9, 2, 5, 2, 15, 1, 16, 1, 15, 1, 15, 1, 15, 1, 15, 1, 5, 1, 8, 1, 7, 1, 6, 1, 9, 6, 118};

// num6_cn.bmp
const uint16_t _num6_cn_Blocks[] =
	{
		105, 5, 9, 2, 4, 1, 8, 1, 14, 1, 14, 1, 15, 1, 15, 1, 15, 1, 3, 4, 8, 1, 1, 2, 4, 1, 7, 1, 1, 1, 5, 1, 7, 2, 7, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 7, 1, 6, 1, 9, 1, 4, 2, 10, 5, 116};

// num7_cn.bmp
const uint16_t _num7_cn_Blocks[] =
	{
		99, 9, 7, 1, 7, 1, 7, 1, 7, 1, 14, 1, 15, 1, 15, 1, 15, 1, 14, 1, 15, 1, 15, 1, 15, 1, 14, 1, 15, 1, 15, 1, 15, 1, 14, 1, 15, 1, 120};

// num8_cn.bmp
const uint16_t _num8_cn_Blocks[] =
	{
		102, 4, 10, 2, 4, 2, 8, 1, 6, 1, 7, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 7, 1, 8, 1, 6, 1, 9, 6, 9, 1, 6, 1, 7, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 7, 1, 6, 1, 8, 2, 5, 1, 10, 5, 117};

// num9_cn.bmp
const uint16_t _num9_cn_Blocks[] =
	{
		102, 5, 10, 2, 4, 1, 9, 1, 6, 1, 7, 1, 7, 1, 7, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 7, 1, 6, 2, 7, 2, 4, 1, 1, 1, 9, 4, 2, 1, 15, 1, 15, 1, 14, 1, 15, 1, 14, 1, 13, 2, 9, 5, 119};

// numsign_cn.bmp
const uint16_t _numsign_cn_Blocks[] =
	{
		87, 1, 3, 1, 11, 1, 3, 1, 11, 1, 3, 1, 11, 1, 3, 1, 10, 1, 3, 1, 11, 1, 3, 1, 11, 1, 3, 1, 8, 11, 8, 1, 3, 1, 11, 1, 3, 1, 11, 1, 3, 1, 11, 1, 3, 1, 8, 11, 8, 1, 3, 1, 11, 1, 3, 1, 11, 1, 3, 1, 10, 1, 3,
		1, 11, 1, 3, 1, 11, 1, 3, 1, 11, 1, 3, 1, 86};

// or_cn.bmp
const uint16_t _or_cn_Blocks[] =
	{
		103, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 72};

// percentage_cn.bmp
const uint16_t _percentage_cn_Blocks[] =
	{
		101, 3, 12, 1, 3, 1, 10, 1, 5, 1, 9, 1, 5, 1, 9, 1, 5, 1, 10, 1, 3, 1, 12, 3, 18, 3, 9, 4, 9, 3, 18, 3, 12, 1, 3, 1, 10, 1, 5, 1, 9, 1, 5, 1, 9, 1, 5, 1, 10, 1, 3, 1, 12, 3, 117};

// plus_cn.bmp
const uint16_t _plus_cn_Blocks[] =
	{
		152, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 9, 13, 9, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 135};

// question_cn.bmp
const uint16_t _question_cn_Blocks[] =
	{
		118, 5, 9, 2, 4, 2, 8, 1, 6, 2, 7, 1, 7, 1, 15, 1, 15, 1, 14, 1, 13, 2, 12, 3, 13, 1, 15, 1, 46, 4, 12, 4, 12, 4, 118};

// quotation_cn.bmp
const uint16_t _quotation_cn_Blocks[] =
	{
		102, 4, 12, 4, 12, 4, 12, 4, 13, 2, 14, 2, 14, 2, 14, 2, 263};

// rightbrace_cn.bmp
const uint16_t _rightbrace_cn_Blocks[] =
	{
		117, 2, 16, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 16, 2, 13, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 13, 2, 73};

// rightbracket_cn.bmp
const uint16_t _rightbracket_cn_Blocks[] =
	{
		100, 1, 15, 2, 15, 1, 15, 2, 15, 1, 15, 1, 16, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 14, 1, 15, 1, 14, 2, 14, 1, 14, 2, 14, 1, 75};

// rightmidbracket_cn.bmp
const uint16_t _rightmidbracket_cn_Blocks[] =
	{
		100, 4, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 12, 4, 72};

// semicolon_cn.bmp
const uint16_t _semicolon_cn_Blocks[] =
	{
		183, 2, 13, 4, 12, 4, 13, 2, 61, 4, 12, 3, 12, 3, 13, 3, 13, 2, 13, 3, 13, 2, 90};

// space_cn.bmp
const uint16_t _space_cn_Blocks[] =
	{
		480};

// tilde_cn.bmp
const uint16_t _tilde_cn_Blocks[] =
	{
		212, 3, 12, 1, 2, 2, 4, 1, 5, 1, 4, 2, 2, 1, 12, 3, 213};

// underline_cn.bmp
const uint16_t _underline_cn_Blocks[] =
	{
		464, 16};

// a_cn.bmp
const uint16_t _a_cn_Blocks[] =
	{
		181, 5, 9, 2, 5, 1, 16, 1, 15, 1, 15, 1, 9, 7, 7, 2, 6, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 7, 2, 7, 1, 5, 1, 1, 1, 8, 5, 2, 3, 114};

// b_cn.bmp
const uint16_t _b_cn_Blocks[] =
	{
		96, 3, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 3, 4, 8, 1, 1, 2, 4, 2, 6, 2, 8, 1, 5, 2, 8, 1, 5, 1, 10, 1, 4, 1, 10, 1, 4, 1, 10, 1, 4, 1, 10, 1, 4, 2, 8, 1, 5, 2, 8, 1, 5, 1, 1, 2, 4, 2, 4, 3, 3, 4, 118};

// c_cn.bmp
const uint16_t _c_cn_Blocks[] =
	{
		181, 6, 1, 1, 7, 1, 6, 2, 6, 1, 8, 1, 5, 2, 8, 1, 5, 1, 15, 1, 15, 1, 15, 1, 15, 1, 16, 1, 9, 1, 6, 1, 6, 2, 8, 6, 117};

// d_cn.bmp
const uint16_t _d_cn_Blocks[] =
	{
		106, 3, 15, 1, 15, 1, 15, 1, 15, 1, 8, 5, 2, 1, 7, 1, 5, 1, 1, 1, 6, 1, 7, 2, 5, 1, 9, 1, 5, 1, 9, 1, 5, 1, 9, 1, 5, 1, 9, 1, 5, 1, 9, 1, 5, 1, 9, 1, 6, 1, 7, 2, 7, 1, 5, 1, 1, 1, 8, 5, 2, 3, 113};

// e_cn.bmp
const uint16_t _e_cn_Blocks[] =
	{
		181, 6, 9, 1, 6, 1, 7, 1, 8, 1, 5, 1, 10, 1, 4, 1, 10, 1, 4, 12, 4, 1, 15, 1, 16, 1, 15, 1, 9, 1, 6, 2, 5, 2, 9, 5, 117};

// f_cn.bmp
const uint16_t _f_cn_Blocks[] =
	{
		104, 6, 9, 1, 14, 1, 15, 1, 15, 1, 12, 10, 9, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 12, 10, 115};

// g_cn.bmp
const uint16_t _g_cn_Blocks[] =
	{
		181, 5, 2, 3, 5, 1, 5, 1, 1, 1, 6, 1, 7, 2, 5, 1, 8, 2, 5, 1, 9, 1, 5, 1, 9, 1, 5, 1, 9, 1, 5, 1, 9, 1, 5, 1, 8, 2, 6, 1, 7, 2, 7, 1, 5, 1, 1, 1, 8, 5, 2, 1, 15, 1, 15, 1, 15, 1, 14, 1, 9, 6, 37};

// h_cn.bmp
const uint16_t _h_cn_Blocks[] =
	{
		97, 3, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 2, 5, 8, 1, 1, 1, 5, 1, 7, 2, 7, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 4, 5, 4, 5, 113};

// i_cn.bmp
const uint16_t _i_cn_Blocks[] =
	{
		86, 2, 14, 2, 14, 2, 59, 5, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 10, 11, 115};

// j_cn.bmp
const uint16_t _j_cn_Blocks[] =
	{
		88, 2, 14, 2, 14, 2, 57, 8, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 14, 1, 8, 7, 39};

// k_cn.bmp
const uint16_t _k_cn_Blocks[] =
	{
		98, 3, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 5, 5, 5, 1, 6, 1, 8, 1, 5, 1, 9, 1, 4, 1, 10, 1, 2, 2, 11, 1, 1, 1, 13, 2, 1, 1, 12, 1, 3, 1, 11, 1, 4, 1, 10, 1, 5, 1, 9, 1, 6, 1, 6, 3, 5, 5, 113};

// l_cn.bmp
const uint16_t _l_cn_Blocks[] =
	{
		99, 5, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 10, 11, 115};

// m_cn.bmp
const uint16_t _m_cn_Blocks[] =
	{
		176, 3, 2, 2, 4, 2, 5, 1, 1, 1, 2, 1, 2, 1, 2, 1, 4, 2, 4, 2, 4, 1, 3, 1, 5, 1, 5, 1, 3, 1, 5, 1, 5, 1, 3, 1, 5, 1, 5, 1, 3, 1, 5, 1, 5, 1, 3, 1, 5, 1, 5, 1, 3, 1, 5, 1, 5, 1, 3, 1, 5, 1, 5,
		1, 3, 1, 5, 1, 5, 1, 1, 5, 3, 3, 3, 2, 112};

// n_cn.bmp
const uint16_t _n_cn_Blocks[] =
	{
		177, 3, 2, 5, 8, 1, 1, 1, 5, 1, 7, 2, 7, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 4, 5, 4, 5, 113};

// o_cn.bmp
const uint16_t _o_cn_Blocks[] =
	{
		182, 4, 10, 2, 4, 2, 7, 1, 8, 1, 5, 1, 9, 1, 5, 1, 10, 1, 4, 1, 10, 1, 4, 1, 10, 1, 4, 1, 10, 1, 4, 1, 9, 1, 6, 1, 8, 1, 7, 1, 6, 1, 9, 6, 117};

// p_cn.bmp
const uint16_t _p_cn_Blocks[] =
	{
		176, 3, 3, 4, 8, 1, 1, 2, 4, 2, 6, 2, 8, 1, 5, 2, 8, 1, 5, 1, 10, 1, 4, 1, 10, 1, 4, 1, 10, 1, 4, 1, 10, 1, 4, 2, 8, 1, 5, 2, 8, 1, 5, 1, 1, 2, 5, 1, 6, 1, 3, 5, 7, 1, 15, 1, 15, 1, 15, 1, 13, 6, 42};

// q_cn.bmp
const uint16_t _q_cn_Blocks[] =
	{
		181, 5, 2, 3, 5, 1, 5, 1, 1, 1, 6, 1, 7, 2, 5, 1, 9, 1, 5, 1, 9, 1, 5, 1, 9, 1, 5, 1, 9, 1, 5, 1, 9, 1, 5, 1, 9, 1, 6, 1, 7, 2, 7, 1, 5, 1, 1, 1, 8, 5, 2, 1, 15, 1, 15, 1, 15, 1, 15, 1, 12, 6, 33};

// r_cn.bmp
const uint16_t _r_cn_Blocks[] =
	{
		179, 3, 3, 4, 8, 1, 2, 1, 4, 1, 7, 1, 1, 1, 13, 2, 14, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 12, 10, 116};

// s_cn.bmp
const uint16_t _s_cn_Blocks[] =
	{
		181, 6, 1, 1, 7, 1, 6, 2, 6, 1, 8, 1, 6, 1, 16, 1, 16, 6, 16, 1, 16, 1, 5, 1, 9, 1, 5, 1, 9, 1, 5, 3, 6, 1, 6, 1, 2, 6, 117};

// t_cn.bmp
const uint16_t _t_cn_Blocks[] =
	{
		117, 1, 15, 1, 15, 1, 15, 1, 12, 11, 8, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 16, 1, 6, 2, 8, 6, 115};

// u_cn.bmp
const uint16_t _u_cn_Blocks[] =
	{
		177, 3, 6, 3, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 7, 2, 7, 1, 5, 1, 1, 1, 8, 5, 2, 3, 113};

// v_cn.bmp
const uint16_t _v_cn_Blocks[] =
	{
		177, 5, 4, 5, 4, 1, 8, 1, 6, 1, 8, 1, 7, 1, 6, 1, 8, 1, 6, 1, 9, 1, 4, 1, 10, 1, 4, 1, 10, 1, 3, 1, 12, 1, 2, 1, 12, 1, 2, 1, 13, 2, 14, 2, 119};

// w_cn.bmp
const uint16_t _w_cn_Blocks[] =
	{
		176, 5, 7, 4, 2, 1, 11, 1, 3, 1, 11, 1, 3, 1, 11, 1, 3, 1, 5, 1, 5, 1, 4, 1, 3, 1, 1, 1, 3, 1, 5, 1, 3, 1, 1, 1, 3, 1, 5, 1, 2, 1, 3, 1, 2, 1, 5, 1, 1, 1, 5, 1, 1, 1, 5, 1, 1, 1, 5, 1, 1, 1, 6,
		1, 7, 1, 7, 1, 7, 1, 115};

// x_cn.bmp
const uint16_t _x_cn_Blocks[] =
	{
		178, 4, 3, 4, 6, 1, 7, 1, 8, 1, 5, 1, 10, 1, 3, 1, 12, 1, 1, 1, 14, 1, 14, 1, 1, 1, 12, 1, 3, 1, 10, 1, 5, 1, 8, 1, 7, 1, 6, 1, 9, 1, 4, 5, 3, 5, 114};

// y_cn.bmp
const uint16_t _y_cn_Blocks[] =
	{
		177, 5, 5, 5, 3, 1, 9, 1, 5, 1, 8, 1, 7, 1, 7, 1, 7, 1, 6, 1, 9, 1, 5, 1, 9, 1, 4, 1, 11, 1, 3, 1, 11, 1, 3, 1, 12, 1, 1, 1, 13, 1, 1, 1, 14, 1, 15, 1, 14, 1, 15, 1, 14, 1, 11, 7, 39};

// z_cn.bmp
const uint16_t _z_cn_Blocks[] =
	{
		179, 10, 6, 1, 8, 1, 6, 1, 7, 1, 14, 1, 14, 1, 14, 1, 14, 1, 14, 1, 14, 1, 14, 1, 7, 1, 6, 1, 8, 1, 6, 10, 115};

// A_cn.bmp
const uint16_t _A_cn_Blocks[] =
	{
		115, 6, 14, 2, 13, 1, 2, 1, 12, 1, 2, 1, 12, 1, 2, 1, 11, 1, 4, 1, 10, 1, 4, 1, 10, 1, 4, 1, 9, 1, 6, 1, 8, 8, 8, 1, 6, 1, 7, 1, 8, 1, 6, 1, 8, 1, 5, 1, 10, 1, 4, 1, 10, 1, 2, 5, 6, 5, 112};

// B_cn.bmp
const uint16_t _B_cn_Blocks[] =
	{
		114, 9, 9, 1, 6, 1, 8, 1, 7, 1, 7, 1, 7, 1, 7, 1, 7, 1, 7, 1, 7, 1, 7, 1, 6, 1, 8, 7, 9, 1, 6, 1, 8, 1, 7, 1, 7, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 7, 1, 5, 10, 116};

// C_cn.bmp
const uint16_t _C_cn_Blocks[] =
	{
		117, 6, 2, 1, 6, 1, 6, 1, 1, 1, 5, 1, 8, 2, 4, 1, 10, 1, 3, 1, 11, 1, 3, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 16, 1, 10, 1, 5, 1, 8, 1, 7, 1, 6, 1, 9, 6, 117};

// D_cn.bmp
const uint16_t _D_cn_Blocks[] =
	{
		114, 9, 9, 1, 6, 2, 7, 1, 7, 1, 7, 1, 8, 1, 6, 1, 9, 1, 5, 1, 9, 1, 5, 1, 9, 1, 5, 1, 9, 1, 5, 1, 9, 1, 5, 1, 9, 1, 5, 1, 9, 1, 5, 1, 9, 1, 5, 1, 8, 1, 6, 1, 7, 1, 7, 1, 6, 2, 5, 9, 117};

// E_cn.bmp
const uint16_t _E_cn_Blocks[] =
	{
		114, 12, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 15, 1, 4, 1, 10, 1, 4, 1, 10, 6, 10, 1, 4, 1, 10, 1, 4, 1, 10, 1, 15, 1, 9, 1, 5, 1, 9, 1, 5, 1, 9, 1, 5, 1, 9, 1, 3, 13, 113};

// F_cn.bmp
const uint16_t _F_cn_Blocks[] =
	{
		114, 12, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 15, 1, 4, 1, 10, 1, 4, 1, 10, 6, 10, 1, 4, 1, 10, 1, 4, 1, 10, 1, 15, 1, 15, 1, 15, 1, 15, 1, 13, 8, 118};

// G_cn.bmp
const uint16_t _G_cn_Blocks[] =
	{
		118, 5, 2, 1, 6, 2, 5, 3, 5, 1, 9, 1, 5, 1, 9, 1, 4, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 5, 8, 2, 1, 10, 1, 4, 1, 10, 1, 5, 1, 9, 1, 5, 1, 9, 1, 6, 2, 6, 2, 8, 6, 116};

// H_cn.bmp
const uint16_t _H_cn_Blocks[] =
	{
		113, 5, 4, 5, 4, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 10, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 4, 5, 4, 5, 113};

// I_cn.bmp
const uint16_t _I_cn_Blocks[] =
	{
		114, 11, 10, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 10, 11, 115};

// J_cn.bmp
const uint16_t _J_cn_Blocks[] =
	{
		118, 9, 12, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 7, 1, 8, 2, 4, 2, 10, 4, 119};

// K_cn.bmp
const uint16_t _K_cn_Blocks[] =
	{
		114, 5, 5, 4, 4, 1, 8, 1, 6, 1, 7, 1, 7, 1, 6, 1, 8, 1, 5, 1, 9, 1, 3, 2, 10, 1, 2, 1, 12, 1, 1, 2, 12, 2, 2, 1, 11, 1, 4, 2, 9, 1, 5, 1, 9, 1, 6, 1, 8, 1, 6, 1, 8, 1, 7, 1, 7, 1, 7, 1, 5, 5, 5,
		3, 113};

// L_cn.bmp
const uint16_t _L_cn_Blocks[] =
	{
		114, 7, 12, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 7, 1, 7, 1, 7, 1, 7, 1, 7, 1, 7, 1, 7, 1, 7, 1, 7, 1, 4, 12, 114};

// M_cn.bmp
const uint16_t _M_cn_Blocks[] =
	{
		112, 4, 8, 4, 2, 2, 8, 2, 4, 1, 1, 1, 6, 1, 1, 1, 4, 1, 1, 1, 6, 1, 1, 1, 4, 1, 2, 1, 4, 1, 2, 1, 4, 1, 2, 1, 4, 1, 2, 1, 4, 1, 3, 1, 3, 1, 2, 1, 4, 1, 3, 1, 2, 1, 3, 1, 4, 1, 4, 1, 1, 1, 3,
		1, 4, 1, 4, 2, 4, 1, 4, 1, 5, 1, 4, 1, 4, 1, 10, 1, 4, 1, 10, 1, 4, 1, 10, 1, 4, 1, 10, 1, 2, 5, 6, 5, 112};

// N_cn.bmp
const uint16_t _N_cn_Blocks[] =
	{
		113, 3, 6, 5, 4, 2, 7, 1, 6, 2, 7, 1, 6, 1, 1, 1, 6, 1, 6, 1, 1, 2, 5, 1, 6, 1, 2, 1, 5, 1, 6, 1, 3, 1, 4, 1, 6, 1, 3, 1, 4, 1, 6, 1, 4, 1, 3, 1, 6, 1, 4, 1, 3, 1, 6, 1, 5, 1, 2, 1, 6, 1, 5,
		2, 1, 1, 6, 1, 6, 1, 1, 1, 6, 1, 7, 2, 6, 1, 7, 2, 4, 5, 6, 1, 115};

// O_cn.bmp
const uint16_t _O_cn_Blocks[] =
	{
		118, 4, 10, 2, 4, 2, 7, 1, 8, 1, 5, 1, 10, 1, 4, 1, 10, 1, 3, 1, 12, 1, 2, 1, 12, 1, 2, 1, 12, 1, 2, 1, 12, 1, 2, 1, 12, 1, 2, 1, 12, 1, 3, 1, 10, 1, 4, 1, 10, 1, 5, 1, 8, 1, 7, 2, 4, 2, 10, 4, 118};

// P_cn.bmp
const uint16_t _P_cn_Blocks[] =
	{
		114, 9, 9, 1, 6, 1, 8, 1, 7, 1, 7, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 7, 1, 7, 1, 6, 1, 8, 7, 9, 1, 15, 1, 15, 1, 15, 1, 15, 1, 13, 8, 118};

// Q_cn.bmp
const uint16_t _Q_cn_Blocks[] =
	{
		118, 5, 9, 2, 4, 2, 7, 1, 8, 1, 5, 1, 10, 1, 4, 1, 10, 1, 3, 1, 12, 1, 2, 1, 12, 1, 2, 1, 12, 1, 2, 1, 12, 1, 2, 1, 12, 1, 2, 1, 12, 1, 3, 1, 10, 1, 4, 1, 10, 1, 5, 1, 8, 1, 7, 2, 4, 2, 9, 5, 12, 1, 15,
		5, 3, 1, 6, 1, 5, 3, 66};

// R_cn.bmp
const uint16_t _R_cn_Blocks[] =
	{
		114, 9, 9, 1, 6, 2, 7, 1, 7, 1, 7, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 7, 1, 7, 1, 6, 2, 7, 7, 9, 1, 5, 1, 9, 1, 6, 1, 8, 1, 7, 1, 7, 1, 8, 1, 6, 1, 8, 1, 6, 1, 9, 1, 3, 5, 7, 2, 112};

// S_cn.bmp
const uint16_t _S_cn_Blocks[] =
	{
		117, 6, 1, 1, 7, 1, 5, 3, 6, 1, 7, 2, 6, 1, 8, 1, 6, 1, 8, 1, 6, 2, 15, 2, 16, 4, 16, 2, 16, 1, 15, 1, 5, 1, 9, 1, 5, 1, 9, 1, 5, 2, 7, 1, 6, 3, 5, 2, 6, 1, 2, 5, 118};

// T_cn.bmp
const uint16_t _T_cn_Blocks[] =
	{
		114, 13, 3, 1, 5, 1, 5, 1, 3, 1, 5, 1, 5, 1, 3, 1, 5, 1, 5, 1, 3, 1, 5, 1, 5, 1, 3, 1, 5, 1, 5, 1, 9, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 12, 7, 116};

// U_cn.bmp
const uint16_t _U_cn_Blocks[] =
	{
		113, 5, 4, 5, 4, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 7, 1, 6, 1, 9, 1, 4, 1, 11, 4, 118};

// V_cn.bmp
const uint16_t _V_cn_Blocks[] =
	{
		112, 5, 6, 5, 2, 1, 10, 1, 4, 1, 10, 1, 5, 1, 8, 1, 6, 1, 8, 1, 6, 1, 8, 1, 7, 1, 6, 1, 8, 1, 6, 1, 9, 1, 4, 1, 10, 1, 4, 1, 10, 1, 4, 1, 11, 1, 2, 1, 12, 1, 2, 1, 12, 1, 2, 1, 13, 2, 14, 2, 119};

// W_cn.bmp
const uint16_t _W_cn_Blocks[] =
	{
		112, 6, 5, 5, 2, 1, 11, 1, 3, 1, 11, 1, 3, 1, 11, 1, 3, 1, 5, 2, 4, 1, 3, 1, 5, 2, 4, 1, 4, 1, 3, 1, 2, 1, 2, 1, 5, 1, 3, 1, 2, 1, 2, 1, 5, 1, 3, 1, 2, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2,
		1, 3, 1, 2, 1, 5, 1, 2, 1, 4, 1, 1, 1, 5, 1, 1, 1, 5, 1, 1, 1, 5, 1, 1, 1, 5, 1, 1, 1, 5, 1, 1, 1, 5, 1, 1, 1, 6, 1, 7, 1, 115};

// X_cn.bmp
const uint16_t _X_cn_Blocks[] =
	{
		113, 4, 5, 4, 5, 1, 7, 1, 8, 1, 5, 1, 9, 1, 5, 1, 10, 1, 3, 1, 12, 1, 1, 1, 13, 1, 1, 1, 14, 1, 14, 1, 1, 1, 12, 1, 3, 1, 11, 1, 3, 1, 10, 1, 5, 1, 8, 1, 7, 1, 7, 1, 7, 1, 6, 1, 9, 1, 4, 5, 4, 5, 113};

// Y_cn.bmp
const uint16_t _Y_cn_Blocks[] =
	{
		114, 4, 5, 4, 4, 1, 9, 1, 6, 1, 7, 1, 8, 1, 5, 1, 9, 1, 5, 1, 10, 1, 3, 1, 12, 1, 1, 1, 13, 1, 1, 1, 14, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 15, 1, 12, 7, 116};

// Z_cn.bmp
const uint16_t _Z_cn_Blocks[] =
	{
		115, 10, 6, 1, 8, 1, 6, 1, 7, 1, 7, 1, 6, 1, 8, 1, 6, 1, 14, 1, 14, 1, 14, 1, 15, 1, 14, 1, 14, 1, 6, 1, 7, 1, 7, 1, 7, 1, 7, 1, 6, 1, 8, 1, 5, 1, 9, 1, 5, 11, 115};

const charmold BasicChar_Courier_New[] =
	{
		{BASIC_WIDTH, BASIC_HEIGHT, _space_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _exclamation_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _dualquotation_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _numsign_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _dollar_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _percentage_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _and_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _quotation_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _leftbracket_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _rightbracket_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _multiply_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _plus_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _comma_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _minus_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _dot_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _division_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _num0_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _num1_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _num2_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _num3_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _num4_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _num5_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _num6_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _num7_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _num8_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _num9_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _colon_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _semicolon_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _lessthan_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _equal_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _morethan_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _question_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _at_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _A_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _B_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _C_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _D_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _E_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _F_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _G_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _H_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _I_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _J_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _K_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _L_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _M_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _N_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _O_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _P_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _Q_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _R_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _S_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _T_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _U_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _V_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _W_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _X_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _Y_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _Z_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _leftmidbracket_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _antidivision_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _rightmidbracket_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _caret_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _underline_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _backquote_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _a_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _b_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _c_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _d_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _e_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _f_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _g_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _h_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _i_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _j_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _k_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _l_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _m_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _n_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _o_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _p_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _q_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _r_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _s_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _t_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _u_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _v_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _w_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _x_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _y_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _z_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _leftbrace_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _or_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _rightbrace_cn_Blocks},
		{BASIC_WIDTH, BASIC_HEIGHT, _tilde_cn_Blocks}};

// antidivision_cn_7x15.bmp
const uint16_t _antidivision_cn_7x15_Blocks[] =
	{
		29, 1, 7, 1, 6, 1, 6, 1, 7, 1, 6, 1, 7, 1, 6, 1, 6, 1, 16};

// at_cn_7x15.bmp
const uint16_t _at_cn_7x15_Blocks[] =
	{
		23, 3, 3, 1, 3, 1, 2, 1, 3, 1, 2, 1, 2, 2, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 2, 2, 2, 1, 6, 1, 3, 1, 3, 3, 16};

// backquote_cn_7x15.bmp
const uint16_t _backquote_cn_7x15_Blocks[] =
	{
		31, 1, 7, 1, 65};

// caret_cn_7x15.bmp
const uint16_t _caret_cn_7x15_Blocks[] =
	{
		31, 1, 6, 1, 5, 1, 1, 1, 3, 1, 3, 1, 50};

// colon_cn_7x15.bmp
const uint16_t _colon_cn_7x15_Blocks[] =
	{
		44, 2, 5, 2, 19, 2, 5, 2, 24};

// comma_cn_7x15.bmp
const uint16_t _comma_cn_7x15_Blocks[] =
	{
		73, 2, 5, 1, 5, 2, 5, 1, 11};

// division_cn_7x15.bmp
const uint16_t _division_cn_7x15_Blocks[] =
	{
		33, 1, 6, 1, 5, 1, 6, 1, 5, 1, 6, 1, 5, 1, 6, 1, 5, 1, 19};

// dollar_cn_7x15.bmp
const uint16_t _dollar_cn_7x15_Blocks[] =
	{
		31, 1, 5, 3, 3, 1, 6, 1, 7, 3, 3, 1, 2, 1, 3, 3, 6, 1, 6, 1, 17};

// dot_cn_7x15.bmp
const uint16_t _dot_cn_7x15_Blocks[] =
	{
		72, 2, 5, 2, 24};

// dualquotation_cn_7x15.bmp
const uint16_t _dualquotation_cn_7x15_Blocks[] =
	{
		29, 2, 1, 2, 2, 1, 2, 1, 3, 1, 2, 1, 58};

// eight_cn_7x15.bmp
const uint16_t _num8_cn_7x15_Blocks[] =
	{
		30, 3, 3, 1, 3, 1, 2, 1, 3, 1, 3, 3, 3, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 3, 3, 23};

// equal_cn_7x15.bmp
const uint16_t _equal_cn_7x15_Blocks[] =
	{
		50, 5, 9, 5, 36};

// exclamation_cn_7x15.bmp
const uint16_t _exclamation_cn_7x15_Blocks[] =
	{
		31, 1, 6, 1, 6, 1, 6, 1, 6, 1, 20, 1, 24};

// five_cn_7x15.bmp
const uint16_t _num5_cn_7x15_Blocks[] =
	{
		30, 4, 3, 1, 6, 1, 6, 3, 7, 1, 6, 1, 2, 1, 3, 1, 3, 3, 23};

// four_cn_7x15.bmp
const uint16_t _num4_cn_7x15_Blocks[] =
	{
		32, 2, 4, 1, 1, 1, 4, 1, 1, 1, 3, 1, 2, 1, 2, 1, 3, 1, 2, 6, 5, 1, 5, 3, 21};

// leftbrace_cn_7x15.bmp
const uint16_t _leftbrace_cn_7x15_Blocks[] =
	{
		32, 1, 5, 1, 6, 1, 6, 1, 6, 1, 5, 1, 7, 1, 6, 1, 6, 1, 7, 1, 9};

// leftbracket_cn_7x15.bmp
const uint16_t _leftbracket_cn_7x15_Blocks[] =
	{
		32, 1, 6, 1, 5, 1, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 7, 1, 6, 1, 9};

// leftmidbracket_cn_7x15.bmp
const uint16_t _leftmidbracket_cn_7x15_Blocks[] =
	{
		30, 3, 4, 1, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 6, 3, 9};

// lessthan_cn_7x15.bmp
const uint16_t _lessthan_cn_7x15_Blocks[] =
	{
		39, 2, 4, 1, 4, 2, 4, 1, 7, 2, 7, 1, 7, 2, 22};

// minus_cn_7x15.bmp
const uint16_t _minus_cn_7x15_Blocks[] =
	{
		57, 5, 43};

// morethan_cn_7x15.bmp
const uint16_t _morethan_cn_7x15_Blocks[] =
	{
		35, 2, 7, 1, 7, 2, 7, 1, 4, 2, 4, 1, 4, 2, 26};

// multiply_cn_7x15.bmp
const uint16_t _multiply_cn_7x15_Blocks[] =
	{
		31, 1, 4, 5, 4, 1, 5, 1, 1, 1, 4, 1, 1, 1, 44};

// nine_cn_7x15.bmp
const uint16_t _num9_cn_7x15_Blocks[] =
	{
		30, 3, 3, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 3, 4, 6, 1, 5, 1, 3, 3, 24};

// numsign_cn_7x15.bmp
const uint16_t _numsign_cn_7x15_Blocks[] =
	{
		31, 1, 1, 1, 4, 1, 1, 1, 3, 1, 1, 1, 3, 5, 3, 1, 1, 1, 3, 5, 3, 1, 1, 1, 3, 1, 1, 1, 4, 1, 1, 1, 17};

// one_cn_7x15.bmp
const uint16_t _num1_cn_7x15_Blocks[] =
	{
		30, 2, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 4, 5, 22};

// or_cn_7x15.bmp
const uint16_t _or_cn_7x15_Blocks[] =
	{
		31, 1, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 17};

// percentage_cn_7x15.bmp
const uint16_t _percentage_cn_7x15_Blocks[] =
	{
		30, 1, 5, 1, 1, 1, 5, 1, 8, 2, 2, 3, 7, 1, 5, 1, 1, 1, 5, 1, 23};

// plus_cn_7x15.bmp
const uint16_t _plus_cn_7x15_Blocks[] =
	{
		38, 1, 6, 1, 6, 1, 3, 7, 3, 1, 6, 1, 6, 1, 24};

// question_cn_7x15.bmp
const uint16_t _question_cn_7x15_Blocks[] =
	{
		38, 2, 4, 1, 2, 1, 6, 1, 5, 1, 5, 1, 12, 2, 24};

// quotation_cn_7x15.bmp
const uint16_t _quotation_cn_7x15_Blocks[] =
	{
		31, 1, 6, 1, 6, 1, 6, 1, 52};

// rightbrace_cn_7x15.bmp
const uint16_t _rightbrace_cn_7x15_Blocks[] =
	{
		30, 1, 7, 1, 6, 1, 6, 1, 6, 1, 7, 1, 5, 1, 6, 1, 6, 1, 5, 1, 11};

// rightbracket_cn_7x15.bmp
const uint16_t _rightbracket_cn_7x15_Blocks[] =
	{
		30, 1, 6, 1, 7, 1, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 5, 1, 6, 1, 11};

// rightmidbracket_cn_7x15.bmp
const uint16_t _rightmidbracket_cn_7x15_Blocks[] =
	{
		30, 3, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 4, 3, 9};

// semicolon_cn_7x15.bmp
const uint16_t _semicolon_cn_7x15_Blocks[] =
	{
		45, 2, 5, 2, 19, 2, 4, 2, 5, 1, 18};

// seven_cn_7x15.bmp
const uint16_t _num7_cn_7x15_Blocks[] =
	{
		29, 5, 2, 1, 3, 1, 6, 1, 5, 1, 6, 1, 6, 1, 5, 1, 6, 1, 24};

// six_cn_7x15.bmp
const uint16_t _num6_cn_7x15_Blocks[] =
	{
		31, 3, 3, 1, 5, 1, 6, 4, 3, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 3, 3, 23};

// space_cn_7x15.bmp
const uint16_t _space_cn_7x15_Blocks[] =
	{
		105};

// three_cn_7x15.bmp
const uint16_t _num3_cn_7x15_Blocks[] =
	{
		30, 3, 3, 1, 3, 1, 6, 1, 4, 2, 7, 1, 6, 1, 2, 1, 3, 1, 3, 3, 23};

// tilde_cn_7x15.bmp
const uint16_t _tilde_cn_7x15_Blocks[] =
	{
		58, 1, 2, 1, 2, 1, 1, 2, 37};

// two_cn_7x15.bmp
const uint16_t _num2_cn_7x15_Blocks[] =
	{
		30, 3, 3, 1, 3, 1, 6, 1, 5, 1, 5, 1, 5, 1, 5, 1, 3, 1, 2, 5, 22};

// underline_cn_7x15.bmp
const uint16_t _underline_cn_7x15_Blocks[] =
	{
		98, 7};

// zero_cn_7x15.bmp
const uint16_t _num0_cn_7x15_Blocks[] =
	{
		30, 3, 3, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 3, 3, 23};

// a_cn_7x15.bmp
const uint16_t _a_cn_7x15_Blocks[] =
	{
		44, 3, 3, 1, 3, 1, 3, 4, 2, 1, 3, 1, 2, 1, 3, 1, 3, 5, 21};

// b_cn_7x15.bmp
const uint16_t _b_cn_7x15_Blocks[] =
	{
		28, 2, 6, 1, 6, 1, 1, 2, 3, 2, 2, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 1, 5, 23};

// c_cn_7x15.bmp
const uint16_t _c_cn_7x15_Blocks[] =
	{
		44, 4, 2, 1, 3, 1, 2, 1, 6, 1, 6, 1, 3, 1, 3, 3, 23};

// d_cn_7x15.bmp
const uint16_t _d_cn_7x15_Blocks[] =
	{
		32, 2, 6, 1, 3, 2, 1, 1, 2, 1, 2, 2, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 3, 5, 21};

// e_cn_7x15.bmp
const uint16_t _e_cn_7x15_Blocks[] =
	{
		44, 3, 3, 1, 3, 1, 2, 5, 2, 1, 6, 1, 7, 4, 22};

// f_cn_7x15.bmp
const uint16_t _f_cn_7x15_Blocks[] =
	{
		31, 3, 3, 1, 5, 5, 3, 1, 6, 1, 6, 1, 6, 1, 5, 5, 22};

// g_cn_7x15.bmp
const uint16_t _g_cn_7x15_Blocks[] =
	{
		44, 2, 1, 2, 1, 1, 2, 2, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 3, 4, 6, 1, 3, 3, 9};

// h_cn_7x15.bmp
const uint16_t _h_cn_7x15_Blocks[] =
	{
		28, 2, 6, 1, 6, 1, 1, 2, 3, 2, 2, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 1, 3, 1, 3, 21};

// i_cn_7x15.bmp
const uint16_t _i_cn_7x15_Blocks[] =
	{
		31, 1, 11, 3, 6, 1, 6, 1, 6, 1, 6, 1, 4, 5, 22};

// j_cn_7x15.bmp
const uint16_t _j_cn_7x15_Blocks[] =
	{
		31, 1, 11, 4, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 3, 3, 10};

// k_cn_7x15.bmp
const uint16_t _k_cn_7x15_Blocks[] =
	{
		28, 2, 6, 1, 6, 1, 1, 3, 2, 1, 2, 1, 3, 3, 4, 1, 1, 1, 4, 1, 2, 1, 2, 2, 1, 3, 22};

// l_cn_7x15.bmp
const uint16_t _l_cn_7x15_Blocks[] =
	{
		30, 2, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 4, 5, 22};

// m_cn_7x15.bmp
const uint16_t _m_cn_7x15_Blocks[] =
	{
		42, 3, 1, 1, 3, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 7, 21};

// n_cn_7x15.bmp
const uint16_t _n_cn_7x15_Blocks[] =
	{
		42, 2, 1, 2, 3, 2, 2, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 1, 3, 1, 3, 21};

// o_cn_7x15.bmp
const uint16_t _o_cn_7x15_Blocks[] =
	{
		44, 3, 3, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 3, 3, 23};

// p_cn_7x15.bmp
const uint16_t _p_cn_7x15_Blocks[] =
	{
		42, 2, 1, 2, 3, 2, 2, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 4, 3, 1, 5, 3, 11};

// q_cn_7x15.bmp
const uint16_t _q_cn_7x15_Blocks[] =
	{
		44, 2, 1, 2, 1, 1, 2, 2, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 3, 4, 6, 1, 5, 3, 7};

// r_cn_7x15.bmp
const uint16_t _r_cn_7x15_Blocks[] =
	{
		43, 2, 1, 2, 3, 2, 5, 1, 6, 1, 6, 1, 5, 5, 22};

// s_cn_7x15.bmp
const uint16_t _s_cn_7x15_Blocks[] =
	{
		44, 4, 2, 1, 3, 1, 3, 3, 7, 1, 2, 1, 3, 1, 2, 4, 23};

// u_cn_7x15.bmp
const uint16_t _u_cn_7x15_Blocks[] =
	{
		42, 2, 2, 2, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 2, 2, 3, 2, 1, 2, 21};

// v_cn_7x15.bmp
const uint16_t _v_cn_7x15_Blocks[] =
	{
		42, 3, 1, 3, 1, 1, 3, 1, 2, 1, 3, 1, 3, 1, 1, 1, 4, 1, 1, 1, 5, 1, 24};

// w_cn_7x15.bmp
const uint16_t _w_cn_7x15_Blocks[] =
	{
		42, 3, 1, 3, 1, 1, 3, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 3, 1, 1, 1, 23};

// x_cn_7x15.bmp
const uint16_t _x_cn_7x15_Blocks[] =
	{
		42, 2, 2, 2, 2, 1, 2, 1, 4, 2, 5, 2, 4, 1, 2, 1, 2, 2, 2, 2, 22};

// y_cn_7x15.bmp
const uint16_t _y_cn_7x15_Blocks[] =
	{
		42, 3, 1, 3, 1, 1, 3, 1, 3, 1, 2, 1, 3, 1, 1, 1, 5, 2, 5, 1, 6, 1, 4, 4, 9};

// z_cn_7x15.bmp
const uint16_t _z_cn_7x15_Blocks[] =
	{
		43, 5, 2, 1, 2, 1, 5, 1, 5, 1, 5, 1, 3, 1, 2, 5, 22};

// A_cn_7x15.bmp
const uint16_t _A_cn_7x15_Blocks[] =
	{
		30, 2, 6, 1, 5, 1, 1, 1, 4, 1, 1, 1, 4, 1, 1, 1, 3, 5, 2, 1, 3, 1, 1, 3, 1, 3, 21};

// B_cn_7x15.bmp
const uint16_t _B_cn_7x15_Blocks[] =
	{
		28, 5, 3, 1, 3, 1, 2, 1, 3, 1, 2, 4, 3, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 1, 5, 23};

// C_cn_7x15.bmp
const uint16_t _C_cn_7x15_Blocks[] =
	{
		30, 4, 2, 1, 3, 1, 2, 1, 6, 1, 6, 1, 6, 1, 6, 1, 3, 1, 3, 3, 23};

// D_cn_7x15.bmp
const uint16_t _D_cn_7x15_Blocks[] =
	{
		28, 4, 4, 1, 2, 1, 3, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 2, 1, 2, 4, 24};

// E_cn_7x15.bmp
const uint16_t _E_cn_7x15_Blocks[] =
	{
		28, 6, 2, 1, 3, 1, 2, 1, 1, 1, 4, 3, 4, 1, 1, 1, 4, 1, 6, 1, 3, 1, 1, 6, 22};

// F_cn_7x15.bmp
const uint16_t _F_cn_7x15_Blocks[] =
	{
		29, 6, 2, 1, 3, 1, 2, 1, 1, 1, 4, 3, 4, 1, 1, 1, 4, 1, 6, 1, 5, 3, 24};

// G_cn_7x15.bmp
const uint16_t _G_cn_7x15_Blocks[] =
	{
		30, 4, 2, 1, 3, 1, 2, 1, 6, 1, 6, 1, 2, 3, 1, 1, 3, 1, 2, 1, 3, 1, 3, 3, 23};

// H_cn_7x15.bmp
const uint16_t _H_cn_7x15_Blocks[] =
	{
		28, 3, 1, 3, 1, 1, 3, 1, 2, 1, 3, 1, 2, 5, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 1, 3, 1, 3, 21};

// I_cn_7x15.bmp
const uint16_t _I_cn_7x15_Blocks[] =
	{
		29, 5, 4, 1, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 4, 5, 22};

// J_cn_7x15.bmp
const uint16_t _J_cn_7x15_Blocks[] =
	{
		30, 4, 5, 1, 6, 1, 6, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 4, 2, 24};

// K_cn_7x15.bmp
const uint16_t _K_cn_7x15_Blocks[] =
	{
		28, 3, 1, 3, 1, 1, 3, 1, 2, 1, 2, 1, 3, 1, 1, 1, 4, 3, 4, 1, 2, 1, 3, 1, 3, 1, 1, 3, 2, 2, 21};

// L_cn_7x15.bmp
const uint16_t _L_cn_7x15_Blocks[] =
	{
		29, 3, 5, 1, 6, 1, 6, 1, 6, 1, 6, 1, 2, 1, 3, 1, 2, 1, 2, 5, 22};

// M_cn_7x15.bmp
const uint16_t _M_cn_7x15_Blocks[] =
	{
		28, 3, 1, 3, 1, 2, 1, 2, 2, 2, 1, 2, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 3, 1, 2, 1, 3, 1, 1, 3, 1, 3, 21};

// N_cn_7x15.bmp
const uint16_t _N_cn_7x15_Blocks[] =
	{
		28, 3, 1, 3, 1, 2, 2, 1, 2, 2, 2, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 2, 2, 1, 3, 1, 2, 22};

// O_cn_7x15.bmp
const uint16_t _O_cn_7x15_Blocks[] =
	{
		30, 3, 3, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 3, 3, 23};

// P_cn_7x15.bmp
const uint16_t _P_cn_7x15_Blocks[] =
	{
		29, 4, 4, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 3, 4, 1, 6, 1, 5, 3, 24};

// Q_cn_7x15.bmp
const uint16_t _Q_cn_7x15_Blocks[] =
	{
		30, 3, 3, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 3, 3, 5, 3, 15};

// R_cn_7x15.bmp
const uint16_t _R_cn_7x15_Blocks[] =
	{
		28, 5, 3, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 4, 3, 1, 2, 1, 3, 1, 3, 1, 1, 3, 3, 1, 21};

// S_cn_7x15.bmp
const uint16_t _S_cn_7x15_Blocks[] =
	{
		30, 2, 1, 1, 2, 1, 2, 2, 2, 1, 7, 3, 7, 1, 6, 1, 2, 2, 2, 1, 2, 1, 1, 2, 23};

// T_cn_7x15.bmp
const uint16_t _T_cn_7x15_Blocks[] =
	{
		28, 8, 2, 1, 2, 1, 3, 1, 6, 1, 6, 1, 6, 1, 6, 1, 5, 3, 23};

// U_cn_7x15.bmp
const uint16_t _U_cn_7x15_Blocks[] =
	{
		28, 3, 1, 3, 1, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 1, 3, 3, 23};

// V_cn_7x15.bmp
const uint16_t _V_cn_7x15_Blocks[] =
	{
		28, 3, 1, 3, 1, 1, 3, 1, 2, 1, 3, 1, 3, 1, 1, 1, 4, 1, 1, 1, 4, 1, 1, 1, 5, 1, 6, 1, 24};

// W_cn_7x15.bmp
const uint16_t _W_cn_7x15_Blocks[] =
	{
		28, 3, 1, 3, 1, 1, 3, 1, 2, 1, 3, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 3, 1, 1, 1, 23};

// X_cn_7x15.bmp
const uint16_t _X_cn_7x15_Blocks[] =
	{
		28, 2, 3, 2, 1, 1, 3, 1, 3, 1, 1, 1, 5, 1, 6, 1, 5, 1, 1, 1, 3, 1, 3, 1, 1, 2, 3, 2, 21};

// Y_cn_7x15.bmp
const uint16_t _Y_cn_7x15_Blocks[] =
	{
		28, 3, 1, 3, 1, 1, 3, 1, 3, 1, 1, 1, 4, 1, 1, 1, 5, 1, 6, 1, 6, 1, 5, 3, 23};

// Z_cn_7x15.bmp
const uint16_t _Z_cn_7x15_Blocks[] =
	{
		29, 5, 2, 1, 3, 1, 5, 1, 5, 1, 6, 1, 5, 1, 5, 1, 3, 1, 2, 5, 22};

// and_cn_7x15.bmp
const uint16_t _and_cn_7x15_Blocks[] =
	{
		45, 2, 4, 1, 6, 1, 5, 1, 1, 1, 1, 1, 2, 1, 2, 1, 4, 2, 1, 1, 22};

// t_cn_7x15.bmp
const uint16_t _t_cn_7x15_Blocks[] =
	{
		37, 1, 5, 5, 3, 1, 6, 1, 6, 1, 6, 1, 3, 1, 3, 3, 22};

const charmold Courier_New_7x15[] =
	{
		{7, 15, _space_cn_7x15_Blocks},
		{7, 15, _exclamation_cn_7x15_Blocks},
		{7, 15, _dualquotation_cn_7x15_Blocks},
		{7, 15, _numsign_cn_7x15_Blocks},
		{7, 15, _dollar_cn_7x15_Blocks},
		{7, 15, _percentage_cn_7x15_Blocks},
		{7, 15, _and_cn_7x15_Blocks},
		{7, 15, _quotation_cn_7x15_Blocks},
		{7, 15, _leftbracket_cn_7x15_Blocks},
		{7, 15, _rightbracket_cn_7x15_Blocks},
		{7, 15, _multiply_cn_7x15_Blocks},
		{7, 15, _plus_cn_7x15_Blocks},
		{7, 15, _comma_cn_7x15_Blocks},
		{7, 15, _minus_cn_7x15_Blocks},
		{7, 15, _dot_cn_7x15_Blocks},
		{7, 15, _division_cn_7x15_Blocks},
		{7, 15, _num0_cn_7x15_Blocks},
		{7, 15, _num1_cn_7x15_Blocks},
		{7, 15, _num2_cn_7x15_Blocks},
		{7, 15, _num3_cn_7x15_Blocks},
		{7, 15, _num4_cn_7x15_Blocks},
		{7, 15, _num5_cn_7x15_Blocks},
		{7, 15, _num6_cn_7x15_Blocks},
		{7, 15, _num7_cn_7x15_Blocks},
		{7, 15, _num8_cn_7x15_Blocks},
		{7, 15, _num9_cn_7x15_Blocks},
		{7, 15, _colon_cn_7x15_Blocks},
		{7, 15, _semicolon_cn_7x15_Blocks},
		{7, 15, _lessthan_cn_7x15_Blocks},
		{7, 15, _equal_cn_7x15_Blocks},
		{7, 15, _morethan_cn_7x15_Blocks},
		{7, 15, _question_cn_7x15_Blocks},
		{7, 15, _at_cn_7x15_Blocks},
		{7, 15, _A_cn_7x15_Blocks},
		{7, 15, _B_cn_7x15_Blocks},
		{7, 15, _C_cn_7x15_Blocks},
		{7, 15, _D_cn_7x15_Blocks},
		{7, 15, _E_cn_7x15_Blocks},
		{7, 15, _F_cn_7x15_Blocks},
		{7, 15, _G_cn_7x15_Blocks},
		{7, 15, _H_cn_7x15_Blocks},
		{7, 15, _I_cn_7x15_Blocks},
		{7, 15, _J_cn_7x15_Blocks},
		{7, 15, _K_cn_7x15_Blocks},
		{7, 15, _L_cn_7x15_Blocks},
		{7, 15, _M_cn_7x15_Blocks},
		{7, 15, _N_cn_7x15_Blocks},
		{7, 15, _O_cn_7x15_Blocks},
		{7, 15, _P_cn_7x15_Blocks},
		{7, 15, _Q_cn_7x15_Blocks},
		{7, 15, _R_cn_7x15_Blocks},
		{7, 15, _S_cn_7x15_Blocks},
		{7, 15, _T_cn_7x15_Blocks},
		{7, 15, _U_cn_7x15_Blocks},
		{7, 15, _V_cn_7x15_Blocks},
		{7, 15, _W_cn_7x15_Blocks},
		{7, 15, _X_cn_7x15_Blocks},
		{7, 15, _Y_cn_7x15_Blocks},
		{7, 15, _Z_cn_7x15_Blocks},
		{7, 15, _leftmidbracket_cn_7x15_Blocks},
		{7, 15, _antidivision_cn_7x15_Blocks},
		{7, 15, _rightmidbracket_cn_7x15_Blocks},
		{7, 15, _caret_cn_7x15_Blocks},
		{7, 15, _underline_cn_7x15_Blocks},
		{7, 15, _backquote_cn_7x15_Blocks},
		{7, 15, _a_cn_7x15_Blocks},
		{7, 15, _b_cn_7x15_Blocks},
		{7, 15, _c_cn_7x15_Blocks},
		{7, 15, _d_cn_7x15_Blocks},
		{7, 15, _e_cn_7x15_Blocks},
		{7, 15, _f_cn_7x15_Blocks},
		{7, 15, _g_cn_7x15_Blocks},
		{7, 15, _h_cn_7x15_Blocks},
		{7, 15, _i_cn_7x15_Blocks},
		{7, 15, _j_cn_7x15_Blocks},
		{7, 15, _k_cn_7x15_Blocks},
		{7, 15, _l_cn_7x15_Blocks},
		{7, 15, _m_cn_7x15_Blocks},
		{7, 15, _n_cn_7x15_Blocks},
		{7, 15, _o_cn_7x15_Blocks},
		{7, 15, _p_cn_7x15_Blocks},
		{7, 15, _q_cn_7x15_Blocks},
		{7, 15, _r_cn_7x15_Blocks},
		{7, 15, _s_cn_7x15_Blocks},
		{7, 15, _t_cn_7x15_Blocks},
		{7, 15, _u_cn_7x15_Blocks},
		{7, 15, _v_cn_7x15_Blocks},
		{7, 15, _w_cn_7x15_Blocks},
		{7, 15, _x_cn_7x15_Blocks},
		{7, 15, _y_cn_7x15_Blocks},
		{7, 15, _z_cn_7x15_Blocks},
		{7, 15, _leftbrace_cn_7x15_Blocks},
		{7, 15, _or_cn_7x15_Blocks},
		{7, 15, _rightbrace_cn_7x15_Blocks},
		{7, 15, _tilde_cn_7x15_Blocks}};

// comma_song_10x19.bmp
const uint16_t _dot_song_10x19_Blocks[] =
	{
		141, 2, 8, 2, 37};

// division_song_10x19.bmp
const uint16_t _division_song_10x19_Blocks[] =
	{
		18, 1, 8, 1, 9, 1, 9, 1, 8, 1, 9, 1, 8, 1, 9, 1, 8, 1, 9, 1, 8, 1, 9, 1, 8, 1, 9, 1, 9, 1, 8, 1, 28};

// num0_song_10x19.bmp
const uint16_t _num0_song_10x19_Blocks[] =
	{
		33, 4, 5, 1, 4, 1, 4, 1, 4, 1, 3, 1, 6, 1, 2, 1, 6, 1, 2, 1, 6, 1, 2, 1, 6, 1, 2, 1, 6, 1, 2, 1, 6, 1, 2, 1, 6, 1, 3, 1, 4, 1, 4, 1, 4, 1, 5, 4, 33};

// num1_song_10x19.bmp
const uint16_t _num1_song_10x19_Blocks[] =
	{
		35, 1, 7, 3, 9, 1, 9, 1, 9, 1, 9, 1, 9, 1, 9, 1, 9, 1, 9, 1, 9, 1, 9, 1, 7, 5, 32};

// num2_song_10x19.bmp
const uint16_t _num2_song_10x19_Blocks[] =
	{
		33, 4, 5, 1, 4, 1, 3, 1, 6, 1, 2, 1, 6, 1, 9, 1, 8, 1, 8, 1, 8, 1, 8, 1, 8, 1, 8, 1, 5, 1, 2, 1, 6, 1, 2, 8, 31};

// num3_song_10x19.bmp
const uint16_t _num3_song_10x19_Blocks[] =
	{
		32, 4, 5, 1, 4, 1, 4, 1, 5, 1, 3, 1, 5, 1, 9, 1, 8, 1, 7, 3, 10, 1, 10, 1, 2, 1, 6, 1, 2, 1, 6, 1, 2, 1, 5, 1, 4, 5, 33};

// num4_song_10x19.bmp
const uint16_t _num4_song_10x19_Blocks[] =
	{
		36, 1, 8, 2, 8, 2, 7, 1, 1, 1, 6, 1, 2, 1, 6, 1, 2, 1, 5, 1, 3, 1, 4, 1, 4, 1, 4, 8, 7, 1, 9, 1, 9, 1, 7, 5, 31};

// num5_song_10x19.bmp
const uint16_t _num5_song_10x19_Blocks[] =
	{
		32, 7, 3, 1, 9, 1, 9, 1, 9, 1, 1, 3, 5, 2, 3, 1, 4, 1, 5, 1, 9, 1, 9, 1, 2, 1, 6, 1, 2, 1, 6, 1, 2, 1, 5, 1, 4, 5, 33};

// num6_song_10x19.bmp
const uint16_t _num6_song_10x19_Blocks[] =
	{
		34, 3, 6, 1, 3, 1, 4, 1, 4, 1, 3, 1, 9, 1, 9, 1, 1, 4, 4, 2, 4, 1, 3, 1, 6, 1, 2, 1, 6, 1, 2, 1, 6, 1, 3, 1, 5, 1, 3, 1, 4, 1, 5, 4, 33};

// num7_song_10x19.bmp
const uint16_t _num7_song_10x19_Blocks[] =
	{
		31, 8, 2, 1, 5, 1, 3, 1, 5, 1, 8, 1, 9, 1, 8, 1, 9, 1, 9, 1, 8, 1, 9, 1, 9, 1, 9, 1, 9, 1, 35};

// num8_song_10x19.bmp
const uint16_t _num8_song_10x19_Blocks[] =
	{
		33, 4, 5, 1, 4, 1, 3, 1, 6, 1, 2, 1, 6, 1, 2, 1, 6, 1, 3, 1, 4, 1, 5, 4, 5, 1, 4, 1, 3, 1, 6, 1, 2, 1, 6, 1, 2, 1, 6, 1, 3, 1, 4, 1, 5, 4, 33};

// num9_song_10x19.bmp
const uint16_t _num9_song_10x19_Blocks[] =
	{
		33, 4, 5, 1, 4, 1, 3, 1, 6, 1, 2, 1, 6, 1, 2, 1, 6, 1, 2, 1, 6, 1, 3, 1, 4, 2, 4, 4, 1, 1, 9, 1, 8, 1, 4, 1, 4, 1, 4, 1, 3, 1, 6, 3, 34};

// percentage_song_10x19.bmp
const uint16_t _percentage_song_10x19_Blocks[] =
	{
		31, 2, 3, 1, 3, 1, 2, 1, 2, 1, 3, 1, 2, 1, 1, 1, 4, 1, 2, 1, 1, 1, 4, 1, 2, 1, 1, 1, 4, 1, 2, 2, 6, 2, 1, 1, 1, 2, 6, 2, 2, 1, 4, 1, 1, 1, 2, 1, 4, 1, 1, 1, 2, 1, 4, 1, 1, 1, 2, 1, 3, 1, 2,
		1, 2, 1, 3, 1, 3, 2, 32};

// minus_song_10x19.bmp
const uint16_t _minus_song_10x19_Blocks[] =
	{
		91, 8, 91};

// tilde_song_10x19.bmp
const uint16_t _tilde_song_10x19_Blocks[] =
	{
		82, 2, 7, 1, 2, 2, 2, 1, 7, 2, 82};

// d_song_10x19.bmp
const uint16_t _d_song_10x19_Blocks[] =
	{
		36, 2, 9, 1, 9, 1, 9, 1, 5, 3, 1, 1, 4, 1, 3, 2, 3, 1, 5, 1, 3, 1, 5, 1, 3, 1, 5, 1, 3, 1, 5, 1, 3, 1, 5, 1, 4, 1, 3, 2, 5, 3, 1, 2, 31};

// k_song_10x19.bmp
const uint16_t _k_song_10x19_Blocks[] =
	{
		31, 2, 9, 1, 9, 1, 9, 1, 9, 1, 2, 3, 4, 1, 3, 1, 5, 1, 2, 1, 6, 1, 1, 1, 7, 2, 1, 1, 6, 1, 2, 1, 6, 1, 3, 1, 5, 1, 4, 1, 3, 3, 2, 3, 31};

// m_song_10x19.bmp
const uint16_t _m_song_10x19_Blocks[] =
	{
		70, 2, 1, 4, 4, 2, 1, 1, 2, 1, 3, 1, 2, 1, 2, 1, 3, 1, 2, 1, 2, 1, 3, 1, 2, 1, 2, 1, 3, 1, 2, 1, 2, 1, 3, 1, 2, 1, 2, 1, 3, 1, 2, 1, 2, 1, 2, 3, 1, 2, 1, 2, 31};

// s_song_10x19.bmp
const uint16_t _s_song_10x19_Blocks[] =
	{
		73, 5, 4, 1, 4, 1, 4, 1, 4, 1, 5, 2, 10, 2, 10, 1, 4, 1, 4, 1, 4, 1, 4, 1, 4, 5, 33};

// z_song_10x19.bmp
const uint16_t _z_song_10x19_Blocks[] =
	{
		71, 7, 3, 1, 4, 1, 4, 1, 3, 1, 9, 1, 8, 1, 8, 1, 8, 1, 5, 1, 3, 1, 5, 1, 2, 7, 32};

// B_song_10x19.bmp
const uint16_t _B_song_10x19_Blocks[] =
	{
		31, 6, 5, 1, 4, 1, 4, 1, 4, 1, 4, 1, 4, 1, 4, 1, 3, 1, 5, 5, 5, 1, 4, 1, 4, 1, 5, 1, 3, 1, 5, 1, 3, 1, 5, 1, 3, 1, 5, 1, 3, 1, 4, 1, 3, 6, 33};

// H_song_10x19.bmp
const uint16_t _H_song_10x19_Blocks[] =
	{
		30, 3, 3, 3, 2, 1, 5, 1, 3, 1, 5, 1, 3, 1, 5, 1, 3, 1, 5, 1, 3, 1, 5, 1, 3, 7, 3, 1, 5, 1, 3, 1, 5, 1, 3, 1, 5, 1, 3, 1, 5, 1, 3, 1, 5, 1, 2, 3, 3, 3, 31};

// M_song_10x19.bmp
const uint16_t _M_song_10x19_Blocks[] =
	{
		30, 3, 3, 3, 2, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 2, 3, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 3, 1, 2, 1, 2, 1, 3,
		1, 2, 1, 2, 1, 3, 1, 2, 1, 2, 1, 2, 3, 1, 1, 1, 3, 31};

// V_song_10x19.bmp
const uint16_t _V_song_10x19_Blocks[] =
	{
		30, 3, 3, 3, 2, 1, 5, 1, 3, 1, 5, 1, 4, 1, 3, 1, 5, 1, 3, 1, 5, 1, 3, 1, 5, 1, 3, 1, 6, 1, 1, 1, 7, 1, 1, 1, 7, 1, 1, 1, 7, 1, 1, 1, 8, 1, 9, 1, 35};

const charmold SongTi_10x19[] =
	{
		{10, 19, _space_cn_Blocks},
		{10, 19, _exclamation_cn_Blocks},
		{10, 19, _dualquotation_cn_Blocks},
		{10, 19, _numsign_cn_Blocks},
		{10, 19, _dollar_cn_Blocks},
		{10, 19, _percentage_song_10x19_Blocks},
		{10, 19, _and_cn_Blocks},
		{10, 19, _quotation_cn_Blocks},
		{10, 19, _leftbracket_cn_Blocks},
		{10, 19, _rightbracket_cn_Blocks},
		{10, 19, _multiply_cn_Blocks},
		{10, 19, _plus_cn_7x15_Blocks},
		{10, 19, _comma_cn_Blocks},
		{10, 19, _minus_song_10x19_Blocks},
		{10, 19, _dot_song_10x19_Blocks},
		{10, 19, _division_song_10x19_Blocks},
		{10, 19, _num0_song_10x19_Blocks},
		{10, 19, _num1_song_10x19_Blocks},
		{10, 19, _num2_song_10x19_Blocks},
		{10, 19, _num3_song_10x19_Blocks},
		{10, 19, _num4_song_10x19_Blocks},
		{10, 19, _num5_song_10x19_Blocks},
		{10, 19, _num6_song_10x19_Blocks},
		{10, 19, _num7_song_10x19_Blocks},
		{10, 19, _num8_song_10x19_Blocks},
		{10, 19, _num9_song_10x19_Blocks},
		{10, 19, _colon_cn_Blocks},
		{10, 19, _semicolon_cn_Blocks},
		{10, 19, _lessthan_cn_Blocks},
		{10, 19, _equal_cn_Blocks},
		{10, 19, _morethan_cn_Blocks},
		{10, 19, _question_cn_Blocks},
		{10, 19, _at_cn_Blocks},
		{10, 19, _A_cn_Blocks},
		{10, 19, _B_song_10x19_Blocks},
		{10, 19, _C_cn_Blocks},
		{10, 19, _D_cn_Blocks},
		{10, 19, _E_cn_Blocks},
		{10, 19, _F_cn_7x15_Blocks},
		{10, 19, _G_cn_Blocks},
		{10, 19, _H_song_10x19_Blocks},
		{10, 19, _I_cn_Blocks},
		{10, 19, _J_cn_Blocks},
		{10, 19, _K_cn_Blocks},
		{10, 19, _L_cn_Blocks},
		{10, 19, _M_song_10x19_Blocks},
		{10, 19, _N_cn_Blocks},
		{10, 19, _O_cn_Blocks},
		{10, 19, _P_cn_Blocks},
		{10, 19, _Q_cn_Blocks},
		{10, 19, _R_cn_Blocks},
		{10, 19, _S_cn_Blocks},
		{10, 19, _T_cn_Blocks},
		{10, 19, _U_cn_Blocks},
		{10, 19, _V_song_10x19_Blocks},
		{10, 19, _W_cn_Blocks},
		{10, 19, _X_cn_Blocks},
		{10, 19, _Y_cn_Blocks},
		{10, 19, _Z_cn_Blocks},
		{10, 19, _leftmidbracket_cn_Blocks},
		{10, 19, _antidivision_cn_Blocks},
		{10, 19, _rightmidbracket_cn_Blocks},
		{10, 19, _caret_cn_Blocks},
		{10, 19, _underline_cn_Blocks},
		{10, 19, _backquote_cn_Blocks},
		{10, 19, _a_cn_Blocks},
		{10, 19, _b_cn_Blocks},
		{10, 19, _c_cn_Blocks},
		{10, 19, _d_song_10x19_Blocks},
		{10, 19, _e_cn_Blocks},
		{10, 19, _f_cn_Blocks},
		{10, 19, _g_cn_Blocks},
		{10, 19, _h_cn_Blocks},
		{10, 19, _i_cn_Blocks},
		{10, 19, _j_cn_Blocks},
		{10, 19, _k_song_10x19_Blocks},
		{10, 19, _l_cn_Blocks},
		{10, 19, _m_song_10x19_Blocks},
		{10, 19, _n_cn_Blocks},
		{10, 19, _o_cn_Blocks},
		{10, 19, _p_cn_Blocks},
		{10, 19, _q_cn_Blocks},
		{10, 19, _r_cn_Blocks},
		{10, 19, _s_song_10x19_Blocks},
		{10, 19, _t_cn_Blocks},
		{10, 19, _u_cn_Blocks},
		{10, 19, _v_cn_Blocks},
		{10, 19, _w_cn_Blocks},
		{10, 19, _x_cn_Blocks},
		{10, 19, _y_cn_Blocks},
		{10, 19, _z_song_10x19_Blocks},
		{10, 19, _leftbrace_cn_Blocks},
		{10, 19, _or_cn_Blocks},
		{10, 19, _rightbrace_cn_Blocks},
		{10, 19, _tilde_song_10x19_Blocks}};

const uint16_t _CourierNew_11_000_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_001_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_002_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_003_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_004_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_005_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_006_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_007_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_008_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_009_Blocks[]=
{
	0
};
const uint16_t _CourierNew_11_010_Blocks[]=
{
	0
};
const uint16_t _CourierNew_11_011_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_012_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_013_Blocks[]=
{
	0
};
const uint16_t _CourierNew_11_014_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_015_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_016_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_017_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_018_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_019_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_020_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_021_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_022_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_023_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_024_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_025_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_026_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_027_Blocks[]=
{
	44,13,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,4,7,13,55
};
const uint16_t _CourierNew_11_028_Blocks[]=
{
	0
};
const uint16_t _CourierNew_11_029_Blocks[]=
{
	0
};
const uint16_t _CourierNew_11_030_Blocks[]=
{
	0
};
const uint16_t _CourierNew_11_031_Blocks[]=
{
	0
};
const uint16_t _CourierNew_11_032_Blocks[]=
{
	220
};
const uint16_t _CourierNew_11_033_Blocks[]=
{
	48,3,8,3,8,3,8,3,8,3,8,3,8,3,30,3,8,3,59
};
const uint16_t _CourierNew_11_034_Blocks[]=
{
	46,7,4,3,1,3,4,3,1,3,4,3,1,3,4,3,1,3,123
};
const uint16_t _CourierNew_11_035_Blocks[]=
{
	37,4,7,4,7,4,6,5,4,9,4,5,6,2,1,2,6,2,1,2,4,9,4,5,6,5,6,4,7,4,48
};
const uint16_t _CourierNew_11_036_Blocks[]=
{
	37,3,7,6,4,3,1,3,3,3,9,2,9,4,9,5,9,2,9,3,2,3,2,3,3,7,7,2,9,2,9,2,38
};
const uint16_t _CourierNew_11_037_Blocks[]=
{
	46,5,5,3,1,2,5,2,2,2,5,3,1,2,6,5,5,9,5,5,6,2,1,3,5,2,2,2,5,2,1,3,5,5,57
};
const uint16_t _CourierNew_11_038_Blocks[]=
{
	69,5,6,2,1,2,6,2,9,3,7,5,6,8,3,2,1,4,4,6,5,8,56
};
const uint16_t _CourierNew_11_039_Blocks[]=
{
	48,3,8,3,8,3,8,3,8,3,125
};
const uint16_t _CourierNew_11_040_Blocks[]=
{
	51,2,8,3,8,2,8,3,8,3,8,2,9,2,9,2,9,3,8,3,9,2,9,3,9,2,35
};
const uint16_t _CourierNew_11_041_Blocks[]=
{
	46,2,9,3,9,3,8,3,9,2,9,2,9,2,9,2,9,2,8,3,8,3,7,3,8,2,40
};
const uint16_t _CourierNew_11_042_Blocks[]=
{
	48,3,8,3,5,9,4,5,6,5,6,5,5,3,1,2,102
};
const uint16_t _CourierNew_11_043_Blocks[]=
{
	59,3,8,3,8,3,8,3,4,10,5,3,8,3,8,3,8,3,70
};
const uint16_t _CourierNew_11_044_Blocks[]=
{
	146,4,7,3,8,3,7,3,8,3,28
};
const uint16_t _CourierNew_11_045_Blocks[]=
{
	100,9,111
};
const uint16_t _CourierNew_11_046_Blocks[]=
{
	146,4,7,4,59
};
const uint16_t _CourierNew_11_047_Blocks[]=
{
	29,3,8,2,8,3,8,2,8,3,8,2,8,3,8,2,8,3,8,2,8,3,8,2,8,3,8,3,51
};
const uint16_t _CourierNew_11_048_Blocks[]=
{
	47,5,5,3,2,2,4,2,3,3,2,3,3,3,2,3,4,2,2,3,4,2,2,3,4,2,2,3,3,3,3,2,3,3,3,3,1,3,5,5,58
};
const uint16_t _CourierNew_11_049_Blocks[]=
{
	47,4,5,6,8,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,5,9,56
};
const uint16_t _CourierNew_11_050_Blocks[]=
{
	46,6,4,4,1,3,3,3,3,2,9,2,8,3,7,3,7,3,7,3,7,3,7,3,3,2,3,8,57
};
const uint16_t _CourierNew_11_051_Blocks[]=
{
	46,6,4,4,1,3,9,3,8,3,7,3,6,4,9,3,9,3,8,3,2,3,2,4,3,7,57
};
const uint16_t _CourierNew_11_052_Blocks[]=
{
	49,3,7,4,7,4,6,5,6,2,1,2,5,3,1,2,4,3,2,2,4,8,8,2,9,2,7,5,57
};
const uint16_t _CourierNew_11_053_Blocks[]=
{
	46,7,4,2,9,2,9,2,9,7,9,3,9,2,9,2,8,3,2,4,1,3,4,6,58
};
const uint16_t _CourierNew_11_054_Blocks[]=
{
	49,5,4,4,6,3,8,2,9,7,4,4,1,3,3,3,3,2,3,2,4,2,3,3,3,2,4,3,1,3,4,6,57
};
const uint16_t _CourierNew_11_055_Blocks[]=
{
	45,9,2,2,4,3,8,2,8,3,8,3,8,2,8,3,8,3,8,2,9,2,8,3,59
};
const uint16_t _CourierNew_11_056_Blocks[]=
{
	47,5,5,3,1,3,3,3,3,2,3,3,3,2,4,2,3,2,4,6,5,2,3,2,3,3,3,2,3,2,4,2,4,2,2,3,5,5,58
};
const uint16_t _CourierNew_11_057_Blocks[]=
{
	47,5,5,3,1,3,4,2,4,2,3,2,4,2,3,2,4,2,3,3,1,4,4,7,9,2,8,3,3,1,3,3,4,5,59
};
const uint16_t _CourierNew_11_058_Blocks[]=
{
	80,4,7,4,51,4,7,4,59
};
const uint16_t _CourierNew_11_059_Blocks[]=
{
	81,4,7,4,39,4,7,4,7,3,7,3,8,3,39
};
const uint16_t _CourierNew_11_060_Blocks[]=
{
	62,3,6,4,5,4,6,4,5,4,9,4,8,4,9,4,9,3,67
};
const uint16_t _CourierNew_11_061_Blocks[]=
{
	88,11,22,11,88
};
const uint16_t _CourierNew_11_062_Blocks[]=
{
	56,3,9,4,9,4,8,4,9,4,5,4,6,4,5,4,6,3,73
};
const uint16_t _CourierNew_11_063_Blocks[]=
{
	46,7,4,3,1,3,4,2,3,3,8,3,8,3,6,4,6,3,8,3,19,4,7,4,58
};
const uint16_t _CourierNew_11_064_Blocks[]=
{
	36,5,5,3,1,3,4,2,3,2,3,3,3,2,3,3,1,4,3,8,3,5,1,2,3,8,3,3,1,5,2,3,9,2,9,3,1,3,5,6,46
};
const uint16_t _CourierNew_11_065_Blocks[]=
{
	56,6,7,5,6,2,1,2,5,3,1,3,4,3,1,3,4,2,3,2,3,9,2,2,4,3,1,3,5,2,1,5,1,5,55
};
const uint16_t _CourierNew_11_066_Blocks[]=
{
	55,9,4,2,3,3,3,2,4,2,3,2,2,4,3,7,4,2,3,3,3,2,4,3,2,2,4,3,2,2,3,3,1,10,56
};
const uint16_t _CourierNew_11_067_Blocks[]=
{
	57,8,2,4,2,3,2,2,5,2,1,3,8,3,8,3,8,3,9,2,5,3,2,3,2,3,4,6,57
};
const uint16_t _CourierNew_11_068_Blocks[]=
{
	55,9,4,2,3,3,3,2,4,3,2,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,2,4,3,2,2,3,3,1,9,57
};
const uint16_t _CourierNew_11_069_Blocks[]=
{
	55,10,3,2,4,2,3,2,4,2,3,2,1,2,6,5,6,2,1,2,6,2,4,2,3,2,4,2,3,2,4,2,1,10,56
};
const uint16_t _CourierNew_11_070_Blocks[]=
{
	56,10,2,3,4,2,2,3,4,2,2,3,1,2,5,6,5,3,1,2,5,3,8,3,8,3,7,7,58
};
const uint16_t _CourierNew_11_071_Blocks[]=
{
	57,8,2,3,3,3,2,2,5,2,1,3,8,3,8,3,2,9,5,2,2,2,5,2,2,3,4,2,3,8,56
};
const uint16_t _CourierNew_11_072_Blocks[]=
{
	56,4,1,4,2,3,3,3,2,3,3,3,2,3,3,3,2,9,2,3,3,3,2,3,3,3,2,3,3,3,2,3,3,3,1,5,1,5,55
};
const uint16_t _CourierNew_11_073_Blocks[]=
{
	56,9,5,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,5,9,56
};
const uint16_t _CourierNew_11_074_Blocks[]=
{
	58,8,7,2,9,2,9,2,9,2,3,2,4,2,3,2,4,2,3,2,4,2,3,4,1,3,4,6,58
};
const uint16_t _CourierNew_11_075_Blocks[]=
{
	55,11,2,2,2,3,4,2,1,3,5,5,6,5,6,6,5,2,2,3,4,2,3,2,4,2,3,3,1,6,2,3,55
};
const uint16_t _CourierNew_11_076_Blocks[]=
{
	55,7,7,2,9,2,9,2,9,2,9,2,9,2,3,3,3,2,3,3,3,2,3,14,55
};
const uint16_t _CourierNew_11_077_Blocks[]=
{
	55,4,3,4,1,3,3,3,2,4,1,4,2,4,1,4,2,9,2,2,1,6,2,2,1,3,1,2,2,2,5,2,2,2,5,2,1,5,1,5,55
};
const uint16_t _CourierNew_11_078_Blocks[]=
{
	55,4,2,5,1,4,3,2,2,4,3,2,2,5,2,2,2,2,1,3,1,2,2,2,1,3,1,2,2,2,2,5,2,2,3,4,2,2,3,4,1,5,2,3,56
};
const uint16_t _CourierNew_11_079_Blocks[]=
{
	58,6,3,4,1,4,2,3,4,2,1,3,5,6,6,5,6,5,5,3,1,3,4,2,2,4,1,4,4,6,57
};
const uint16_t _CourierNew_11_080_Blocks[]=
{
	56,9,3,3,2,3,3,3,3,3,2,3,3,3,2,3,2,3,3,7,4,3,8,3,8,3,7,7,58
};
const uint16_t _CourierNew_11_081_Blocks[]=
{
	58,6,3,4,1,4,2,2,5,6,6,5,6,5,6,5,5,3,1,2,5,2,2,4,1,4,3,7,5,3,8,8,2,9,22
};
const uint16_t _CourierNew_11_082_Blocks[]=
{
	55,9,4,2,3,2,4,2,3,3,3,2,3,3,3,2,2,3,4,6,5,2,1,3,5,2,3,2,4,2,3,3,1,6,2,3,55
};
const uint16_t _CourierNew_11_083_Blocks[]=
{
	57,8,2,3,2,4,2,3,4,2,2,4,8,7,8,4,9,2,2,2,5,2,2,4,2,3,2,8,57
};
const uint16_t _CourierNew_11_084_Blocks[]=
{
	55,10,1,3,1,2,2,2,1,3,1,2,2,2,1,3,1,2,2,2,5,2,9,2,9,2,9,2,9,2,7,7,57
};
const uint16_t _CourierNew_11_085_Blocks[]=
{
	55,5,1,5,1,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,3,3,3,3,3,1,3,5,6,57
};
const uint16_t _CourierNew_11_086_Blocks[]=
{
	55,5,1,5,1,3,4,2,2,3,3,3,3,2,3,2,4,3,1,3,4,3,1,3,5,2,1,2,6,5,6,5,7,3,59
};
const uint16_t _CourierNew_11_087_Blocks[]=
{
	55,5,1,8,5,2,2,2,1,3,1,2,2,2,1,3,1,2,2,6,1,2,2,9,2,9,2,4,1,4,2,4,1,4,2,3,2,4,56
};
const uint16_t _CourierNew_11_088_Blocks[]=
{
	55,4,2,5,1,3,3,3,3,3,1,3,5,5,7,3,7,4,6,6,5,2,2,3,3,3,3,3,1,5,1,5,55
};
const uint16_t _CourierNew_11_089_Blocks[]=
{
	55,5,1,5,1,3,3,3,3,3,1,3,5,5,7,4,7,3,8,3,8,3,8,3,6,7,57
};
const uint16_t _CourierNew_11_090_Blocks[]=
{
	57,8,3,2,3,3,3,2,2,3,4,2,1,3,7,3,8,2,8,3,2,2,3,3,3,2,2,3,4,2,2,9,56
};
const uint16_t _CourierNew_11_091_Blocks[]=
{
	48,5,6,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,8,5,35
};
const uint16_t _CourierNew_11_092_Blocks[]=
{
	34,3,8,3,9,2,9,3,9,2,9,3,9,2,9,3,9,2,9,3,9,2,9,3,9,2,9,3,34
};
const uint16_t _CourierNew_11_093_Blocks[]=
{
	46,5,8,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,6,5,37
};
const uint16_t _CourierNew_11_094_Blocks[]=
{
	38,1,9,3,7,5,5,3,1,3,3,3,3,3,133
};
const uint16_t _CourierNew_11_095_Blocks[]=
{
	209,11
};
const uint16_t _CourierNew_11_096_Blocks[]=
{
	36,3,9,3,9,3,157
};
const uint16_t _CourierNew_11_097_Blocks[]=
{
	78,7,5,2,2,3,9,2,4,7,3,3,3,2,3,2,4,2,3,2,2,4,3,9,56
};
const uint16_t _CourierNew_11_098_Blocks[]=
{
	44,3,9,2,9,2,9,8,3,4,2,3,2,3,4,2,2,2,5,3,1,2,5,3,1,3,4,2,2,4,2,3,1,9,57
};
const uint16_t _CourierNew_11_099_Blocks[]=
{
	80,7,3,3,2,3,2,2,5,2,2,2,9,2,9,2,9,4,2,4,2,8,56
};
const uint16_t _CourierNew_11_100_Blocks[]=
{
	50,4,9,2,9,2,3,8,2,3,2,4,2,2,4,3,1,3,5,2,1,3,5,2,2,2,4,3,2,3,2,4,3,9,55
};
const uint16_t _CourierNew_11_101_Blocks[]=
{
	79,7,3,3,2,4,1,3,5,2,1,10,1,3,9,2,9,4,2,3,3,7,57
};
const uint16_t _CourierNew_11_102_Blocks[]=
{
	48,7,4,2,9,2,6,9,5,2,9,2,9,2,9,2,9,2,9,2,6,9,56
};
const uint16_t _CourierNew_11_103_Blocks[]=
{
	79,9,1,3,2,4,1,3,4,3,1,3,4,3,1,3,4,3,1,3,4,3,2,3,2,4,3,8,8,3,7,3,4,6,25
};
const uint16_t _CourierNew_11_104_Blocks[]=
{
	44,4,8,3,8,3,8,8,3,4,1,3,3,3,3,3,2,3,3,3,2,3,3,3,2,3,3,3,2,3,3,3,1,5,1,5,55
};
const uint16_t _CourierNew_11_105_Blocks[]=
{
	37,3,8,3,28,5,8,3,8,3,8,3,8,3,8,3,8,3,5,9,56
};
const uint16_t _CourierNew_11_106_Blocks[]=
{
	38,2,9,2,27,7,9,2,9,2,9,2,9,2,9,2,9,2,9,2,9,2,8,3,4,6,26
};
const uint16_t _CourierNew_11_107_Blocks[]=
{
	45,4,8,3,8,3,8,3,1,5,2,3,1,3,4,6,5,5,6,5,6,6,5,3,1,3,3,4,1,5,55
};
const uint16_t _CourierNew_11_108_Blocks[]=
{
	46,5,8,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,5,9,56
};
const uint16_t _CourierNew_11_109_Blocks[]=
{
	77,10,2,9,2,2,1,3,1,2,2,2,1,3,1,2,2,2,1,3,1,2,2,2,1,3,1,2,2,2,1,3,1,2,1,11,55
};
const uint16_t _CourierNew_11_110_Blocks[]=
{
	77,9,4,3,1,3,4,2,3,3,3,2,3,3,3,2,3,3,3,2,3,3,3,2,3,3,1,5,1,5,55
};
const uint16_t _CourierNew_11_111_Blocks[]=
{
	79,7,3,3,3,3,2,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,3,3,3,3,7,57
};
const uint16_t _CourierNew_11_112_Blocks[]=
{
	77,9,3,4,2,3,2,3,4,2,2,2,5,3,1,2,5,3,1,3,4,2,2,4,2,3,2,8,3,2,9,2,8,6,27
};
const uint16_t _CourierNew_11_113_Blocks[]=
{
	79,9,1,3,2,4,1,3,4,3,1,3,5,2,1,3,5,2,1,3,4,3,2,3,2,4,3,8,9,2,9,2,6,6,22
};
const uint16_t _CourierNew_11_114_Blocks[]=
{
	78,9,4,4,1,3,3,3,8,2,9,2,9,2,9,2,7,8,57
};
const uint16_t _CourierNew_11_115_Blocks[]=
{
	79,8,3,3,2,3,3,2,9,7,9,3,9,2,2,4,2,3,2,8,57
};
const uint16_t _CourierNew_11_116_Blocks[]=
{
	57,3,8,3,6,9,4,3,8,3,8,3,8,3,8,3,8,3,2,3,4,6,57
};
const uint16_t _CourierNew_11_117_Blocks[]=
{
	77,4,1,5,2,3,3,3,2,3,3,3,2,3,3,3,2,3,3,3,2,3,3,3,3,2,1,5,3,9,55
};
const uint16_t _CourierNew_11_118_Blocks[]=
{
	77,5,1,5,1,3,3,3,3,2,3,2,4,3,1,3,5,2,1,3,5,5,6,5,7,3,59
};
const uint16_t _CourierNew_11_119_Blocks[]=
{
	77,4,3,4,1,2,5,3,1,2,1,3,1,2,2,2,1,3,1,2,2,9,3,8,3,3,1,3,4,3,1,3,57
};
const uint16_t _CourierNew_11_120_Blocks[]=
{
	77,5,1,4,3,2,3,3,4,5,7,3,7,5,5,3,1,3,3,3,3,3,1,5,1,5,55
};
const uint16_t _CourierNew_11_121_Blocks[]=
{
	78,4,2,4,1,3,4,2,3,3,2,3,3,3,2,2,5,6,5,5,7,4,7,3,8,3,8,2,6,7,25
};
const uint16_t _CourierNew_11_122_Blocks[]=
{
	79,8,3,2,3,2,8,2,8,2,8,2,8,2,8,2,4,2,2,9,56
};
const uint16_t _CourierNew_11_123_Blocks[]=
{
	49,3,7,3,8,3,8,3,8,3,8,3,7,3,9,3,8,3,8,3,8,3,8,3,9,3,36
};
const uint16_t _CourierNew_11_124_Blocks[]=
{
	48,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,37
};
const uint16_t _CourierNew_11_125_Blocks[]=
{
	47,3,9,2,9,2,9,2,9,2,9,3,9,3,7,3,8,2,9,2,9,2,9,2,8,3,38
};
const uint16_t _CourierNew_11_126_Blocks[]=
{
	101,4,6,9,6,4,90
};
const uint16_t _CourierNew_11_127_Blocks[]=
{
	12,48,12,8
};
const uint16_t _CourierNew_11_128_Blocks[]=
{
	57,8,2,4,1,4,1,3,5,2,1,8,3,3,8,8,3,3,9,2,5,2,2,4,1,4,4,6,57
};
const uint16_t _CourierNew_11_129_Blocks[]=
{
	0
};
const uint16_t _CourierNew_11_130_Blocks[]=
{
	146,4,7,4,7,3,7,3,8,3,28
};
const uint16_t _CourierNew_11_131_Blocks[]=
{
	49,5,6,2,8,3,8,3,6,7,6,3,8,3,8,3,8,3,8,3,8,3,7,3,6,5,38
};
const uint16_t _CourierNew_11_132_Blocks[]=
{
	146,3,1,3,3,3,1,4,3,3,1,3,4,2,2,2,4,3,1,3,25
};
const uint16_t _CourierNew_11_133_Blocks[]=
{
	143,3,1,3,1,6,1,3,1,3,55
};
const uint16_t _CourierNew_11_134_Blocks[]=
{
	48,3,8,3,8,3,6,8,5,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,48
};
const uint16_t _CourierNew_11_135_Blocks[]=
{
	48,3,8,3,8,3,6,7,6,3,8,3,8,3,8,3,6,7,6,3,8,3,8,3,48
};
const uint16_t _CourierNew_11_136_Blocks[]=
{
	37,3,7,5,5,3,1,3,156
};
const uint16_t _CourierNew_11_137_Blocks[]=
{
	44,4,7,5,6,5,6,4,2,4,3,7,3,5,18,4,1,4,2,9,2,9,2,4,1,4,55
};
const uint16_t _CourierNew_11_138_Blocks[]=
{
	13,3,1,3,5,5,7,3,17,8,2,3,2,4,2,3,4,2,2,4,8,7,8,4,9,2,2,2,5,2,2,4,2,3,2,8,57
};
const uint16_t _CourierNew_11_139_Blocks[]=
{
	81,3,7,3,7,3,6,4,7,4,9,3,9,3,9,3,59
};
const uint16_t _CourierNew_11_140_Blocks[]=
{
	57,13,1,2,2,5,2,2,2,4,3,8,3,4,2,2,3,4,2,2,3,2,4,3,2,2,2,6,1,2,2,2,2,9,55
};
const uint16_t _CourierNew_11_141_Blocks[]=
{
	0
};
const uint16_t _CourierNew_11_142_Blocks[]=
{
	13,3,1,3,5,5,7,3,17,8,3,2,3,3,3,2,2,3,4,2,1,3,7,3,8,2,8,3,2,2,3,3,3,2,2,3,4,2,2,9,56
};
const uint16_t _CourierNew_11_143_Blocks[]=
{
	0
};
const uint16_t _CourierNew_11_144_Blocks[]=
{
	0
};
const uint16_t _CourierNew_11_145_Blocks[]=
{
	48,4,8,3,8,4,8,3,8,3,123
};
const uint16_t _CourierNew_11_146_Blocks[]=
{
	47,4,7,3,7,4,7,3,8,3,127
};
const uint16_t _CourierNew_11_147_Blocks[]=
{
	45,3,1,3,4,4,1,3,4,3,1,3,5,2,2,3,4,3,1,3,122
};
const uint16_t _CourierNew_11_148_Blocks[]=
{
	47,3,1,3,3,3,1,4,3,3,1,3,3,3,1,3,4,3,1,3,124
};
const uint16_t _CourierNew_11_149_Blocks[]=
{
	80,5,6,5,6,5,6,5,102
};
const uint16_t _CourierNew_11_150_Blocks[]=
{
	99,10,111
};
const uint16_t _CourierNew_11_151_Blocks[]=
{
	99,11,110
};
const uint16_t _CourierNew_11_152_Blocks[]=
{
	0
};
const uint16_t _CourierNew_11_153_Blocks[]=
{
	55,22,1,2,2,6,1,2,2,2,2,13,110
};
const uint16_t _CourierNew_11_154_Blocks[]=
{
	35,3,1,3,5,5,7,3,17,8,3,3,2,3,3,2,9,7,9,3,9,2,2,4,2,3,2,8,57
};
const uint16_t _CourierNew_11_155_Blocks[]=
{
	81,3,9,3,9,3,9,4,7,4,6,3,7,3,7,3,59
};
const uint16_t _CourierNew_11_156_Blocks[]=
{
	78,4,1,4,1,13,2,3,2,4,2,9,2,3,4,2,2,3,4,11,1,4,1,4,56
};
const uint16_t _CourierNew_11_157_Blocks[]=
{
	0
};
const uint16_t _CourierNew_11_158_Blocks[]=
{
	35,3,1,3,5,5,7,3,17,8,3,2,3,2,8,2,8,2,8,2,8,2,8,2,4,2,2,9,56
};
const uint16_t _CourierNew_11_159_Blocks[]=
{
	35,3,1,3,13,5,1,5,1,3,3,3,3,3,1,3,5,5,7,4,7,3,8,3,8,3,8,3,6,7,57
};
const uint16_t _CourierNew_11_160_Blocks[]=
{
	220
};
const uint16_t _CourierNew_11_161_Blocks[]=
{
	81,3,8,3,30,3,8,3,8,3,8,3,8,3,8,3,8,3,26
};
const uint16_t _CourierNew_11_162_Blocks[]=
{
	60,2,9,2,7,6,4,3,2,2,3,3,8,3,9,3,1,3,5,6,7,2,9,2,59
};
const uint16_t _CourierNew_11_163_Blocks[]=
{
	58,5,5,3,1,2,5,2,9,3,6,7,7,2,9,2,9,2,8,3,3,2,2,9,56
};
const uint16_t _CourierNew_11_164_Blocks[]=
{
	68,1,6,1,2,9,3,3,1,3,4,2,3,2,4,2,3,2,4,3,1,3,3,9,3,1,6,1,67
};
const uint16_t _CourierNew_11_165_Blocks[]=
{
	55,5,1,4,3,2,3,2,4,3,1,3,5,5,7,3,5,8,6,3,5,8,6,3,6,7,57
};
const uint16_t _CourierNew_11_166_Blocks[]=
{
	48,3,8,3,8,3,8,3,8,3,41,3,8,3,8,3,8,3,8,3,37
};
const uint16_t _CourierNew_11_167_Blocks[]=
{
	47,7,3,3,3,2,3,3,3,2,2,5,6,2,1,4,4,3,2,4,3,4,2,2,5,6,7,4,2,3,3,2,3,3,2,3,3,7,47
};
const uint16_t _CourierNew_11_168_Blocks[]=
{
	46,3,1,3,167
};
const uint16_t _CourierNew_11_169_Blocks[]=
{
	46,7,3,4,1,4,1,3,5,5,1,5,1,3,1,3,1,2,1,3,1,2,6,2,1,2,5,7,4,13,1,4,1,4,3,7,57
};
const uint16_t _CourierNew_11_170_Blocks[]=
{
	36,5,6,2,1,2,5,6,5,6,5,7,134
};
const uint16_t _CourierNew_11_171_Blocks[]=
{
	82,1,8,3,2,3,2,8,2,8,2,8,4,8,4,8,4,3,2,3,5,1,49
};
const uint16_t _CourierNew_11_172_Blocks[]=
{
	99,10,9,2,9,2,9,2,9,2,67
};
const uint16_t _CourierNew_11_173_Blocks[]=
{
	100,9,111
};
const uint16_t _CourierNew_11_174_Blocks[]=
{
	46,7,3,4,1,4,1,13,1,2,1,7,1,5,2,3,1,5,2,3,1,2,1,3,1,7,1,7,5,3,1,4,1,4,3,7,57
};
const uint16_t _CourierNew_11_175_Blocks[]=
{
	22,11,187
};
const uint16_t _CourierNew_11_176_Blocks[]=
{
	25,5,5,3,1,3,4,3,2,2,4,3,1,3,5,5,146
};
const uint16_t _CourierNew_11_177_Blocks[]=
{
	48,3,8,3,8,3,8,3,4,10,5,3,8,3,8,3,8,3,15,10,56
};
const uint16_t _CourierNew_11_178_Blocks[]=
{
	47,5,6,2,1,2,9,2,7,3,7,3,7,6,113
};
const uint16_t _CourierNew_11_179_Blocks[]=
{
	47,5,6,2,1,2,7,4,9,2,5,3,1,3,5,5,113
};
const uint16_t _CourierNew_11_180_Blocks[]=
{
	38,3,7,3,7,3,159
};
const uint16_t _CourierNew_11_181_Blocks[]=
{
	77,4,1,5,3,2,3,3,3,2,3,3,3,2,3,3,3,2,3,3,3,2,3,3,3,8,3,9,2,2,9,2,9,2,29
};
const uint16_t _CourierNew_11_182_Blocks[]=
{
	46,8,2,8,3,2,1,5,3,2,1,5,3,8,4,7,6,5,6,5,6,5,6,5,6,5,4,8,45
};
const uint16_t _CourierNew_11_183_Blocks[]=
{
	94,3,8,3,112
};
const uint16_t _CourierNew_11_184_Blocks[]=
{
	169,3,8,4,8,3,6,4,15
};
const uint16_t _CourierNew_11_185_Blocks[]=
{
	48,3,7,4,8,3,8,3,8,3,7,6,112
};
const uint16_t _CourierNew_11_186_Blocks[]=
{
	36,5,5,3,1,3,4,2,3,2,4,3,1,3,5,5,135
};
const uint16_t _CourierNew_11_187_Blocks[]=
{
	82,1,5,3,2,3,4,8,4,8,4,8,2,8,2,8,2,3,2,3,8,1,49
};
const uint16_t _CourierNew_11_188_Blocks[]=
{
	44,3,8,3,9,2,7,1,1,2,5,3,1,2,3,4,1,11,3,8,1,4,1,5,1,3,2,5,9,2,8,3,55
};
const uint16_t _CourierNew_11_189_Blocks[]=
{
	44,3,8,3,9,2,6,2,1,2,4,4,1,2,3,3,2,11,2,9,1,3,5,2,1,2,5,3,6,3,1,1,6,5,55
};
const uint16_t _CourierNew_11_190_Blocks[]=
{
	44,5,9,2,7,4,4,2,3,2,2,4,3,2,1,3,2,11,3,3,1,4,1,4,1,5,1,2,3,5,9,2,8,3,55
};
const uint16_t _CourierNew_11_191_Blocks[]=
{
	80,4,7,4,19,3,8,3,6,4,7,2,8,3,8,3,3,2,4,3,1,3,5,5,25
};
const uint16_t _CourierNew_11_192_Blocks[]=
{
	14,3,9,3,9,3,15,6,7,5,6,2,1,2,5,3,1,3,4,3,1,3,4,2,3,2,3,9,2,2,4,3,1,3,5,2,1,5,1,5,55
};
const uint16_t _CourierNew_11_193_Blocks[]=
{
	16,3,7,3,7,3,17,6,7,5,6,2,1,2,5,3,1,3,4,3,1,3,4,2,3,2,3,9,2,2,4,3,1,3,5,2,1,5,1,5,55
};
const uint16_t _CourierNew_11_194_Blocks[]=
{
	14,3,7,5,5,3,1,3,15,6,7,5,6,2,1,2,5,3,1,3,4,3,1,3,4,2,3,2,3,9,2,2,4,3,1,3,5,2,1,5,1,5,55
};
const uint16_t _CourierNew_11_195_Blocks[]=
{
	24,6,4,7,15,6,7,5,6,2,1,2,5,3,1,3,4,3,1,3,4,2,3,2,3,9,2,2,4,3,1,3,5,2,1,5,1,5,55
};
const uint16_t _CourierNew_11_196_Blocks[]=
{
	34,3,1,3,15,6,7,5,6,2,1,2,5,3,1,3,4,3,1,3,4,2,3,2,3,9,2,2,4,3,1,3,5,2,1,5,1,5,55
};
const uint16_t _CourierNew_11_197_Blocks[]=
{
	4,4,6,5,6,5,7,4,15,6,7,5,6,2,1,2,5,3,1,3,4,3,1,3,4,2,3,2,3,9,2,2,4,3,1,3,5,2,1,5,1,5,55
};
const uint16_t _CourierNew_11_198_Blocks[]=
{
	56,10,3,4,2,2,2,5,2,2,2,9,2,2,1,4,3,3,1,4,3,6,5,3,1,2,2,2,1,2,2,2,2,13,55
};
const uint16_t _CourierNew_11_199_Blocks[]=
{
	57,8,2,4,2,3,2,2,5,2,1,3,8,3,8,3,8,3,9,2,5,3,2,3,2,3,4,6,6,2,9,3,9,2,7,4,15
};
const uint16_t _CourierNew_11_200_Blocks[]=
{
	14,3,9,3,9,3,14,10,3,2,4,2,3,2,4,2,3,2,1,2,6,5,6,2,1,2,6,2,4,2,3,2,4,2,3,2,4,2,1,10,56
};
const uint16_t _CourierNew_11_201_Blocks[]=
{
	16,3,7,3,7,3,16,10,3,2,4,2,3,2,4,2,3,2,1,2,6,5,6,2,1,2,6,2,4,2,3,2,4,2,3,2,4,2,1,10,56
};
const uint16_t _CourierNew_11_202_Blocks[]=
{
	15,3,7,5,5,3,1,3,13,10,3,2,4,2,3,2,4,2,3,2,1,2,6,5,6,2,1,2,6,2,4,2,3,2,4,2,3,2,4,2,1,10,56
};
const uint16_t _CourierNew_11_203_Blocks[]=
{
	35,3,1,3,13,10,3,2,4,2,3,2,4,2,3,2,1,2,6,5,6,2,1,2,6,2,4,2,3,2,4,2,3,2,4,2,1,10,56
};
const uint16_t _CourierNew_11_204_Blocks[]=
{
	14,3,9,3,9,3,15,9,5,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,5,9,56
};
const uint16_t _CourierNew_11_205_Blocks[]=
{
	16,3,7,3,7,3,17,9,5,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,5,9,56
};
const uint16_t _CourierNew_11_206_Blocks[]=
{
	15,3,7,5,5,3,1,3,14,9,5,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,5,9,56
};
const uint16_t _CourierNew_11_207_Blocks[]=
{
	35,3,1,3,14,9,5,3,8,3,8,3,8,3,8,3,8,3,8,3,8,3,5,9,56
};
const uint16_t _CourierNew_11_208_Blocks[]=
{
	55,8,4,2,3,3,3,2,4,3,2,2,5,2,1,7,1,2,2,2,5,2,2,2,5,2,2,2,4,3,2,2,3,3,2,8,58
};
const uint16_t _CourierNew_11_209_Blocks[]=
{
	25,6,4,7,13,4,2,5,1,4,3,2,2,4,3,2,2,5,2,2,2,2,1,3,1,2,2,2,1,3,1,2,2,2,2,5,2,2,3,4,2,2,3,4,1,5,2,3,56
};
const uint16_t _CourierNew_11_210_Blocks[]=
{
	14,3,9,3,9,3,17,6,3,4,1,4,2,3,4,2,1,3,5,6,6,5,6,5,5,3,1,3,4,2,2,4,1,4,4,6,57
};
const uint16_t _CourierNew_11_211_Blocks[]=
{
	16,3,7,3,8,2,19,6,3,4,1,4,2,3,4,2,1,3,5,6,6,5,6,5,5,3,1,3,4,2,2,4,1,4,4,6,57
};
const uint16_t _CourierNew_11_212_Blocks[]=
{
	15,3,7,5,6,2,1,3,16,6,3,4,1,4,2,3,4,2,1,3,5,6,6,5,6,5,5,3,1,3,4,2,2,4,1,4,4,6,57
};
const uint16_t _CourierNew_11_213_Blocks[]=
{
	25,6,4,7,16,6,3,4,1,4,2,3,4,2,1,3,5,6,6,5,6,5,5,3,1,3,4,2,2,4,1,4,4,6,57
};
const uint16_t _CourierNew_11_214_Blocks[]=
{
	35,3,1,3,16,6,3,4,1,4,2,3,4,2,1,3,5,6,6,5,6,5,5,3,1,3,4,2,2,4,1,4,4,6,57
};
const uint16_t _CourierNew_11_215_Blocks[]=
{
	68,1,5,1,4,2,2,3,4,7,5,5,6,5,5,7,4,2,2,3,4,1,5,1,68
};
const uint16_t _CourierNew_11_216_Blocks[]=
{
	52,3,2,8,2,4,1,4,2,3,2,4,1,3,2,9,1,3,2,5,1,2,3,8,2,3,1,4,3,2,2,4,1,4,1,9,2,2,53
};
const uint16_t _CourierNew_11_217_Blocks[]=
{
	14,3,9,3,9,3,14,5,1,5,1,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,3,3,3,3,3,1,3,5,6,57
};
const uint16_t _CourierNew_11_218_Blocks[]=
{
	17,2,8,3,7,3,15,5,1,5,1,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,3,3,3,3,3,1,3,5,6,57
};
const uint16_t _CourierNew_11_219_Blocks[]=
{
	15,3,7,5,5,3,1,3,13,5,1,5,1,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,3,3,3,3,3,1,3,5,6,57
};
const uint16_t _CourierNew_11_220_Blocks[]=
{
	35,3,1,3,13,5,1,5,1,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,3,3,3,3,3,1,3,5,6,57
};
const uint16_t _CourierNew_11_221_Blocks[]=
{
	16,3,7,3,7,3,16,5,1,5,1,3,3,3,3,3,1,3,5,5,7,4,7,3,8,3,8,3,8,3,6,7,57
};
const uint16_t _CourierNew_11_222_Blocks[]=
{
	56,7,5,3,8,7,4,3,2,3,3,3,3,3,2,3,3,3,2,3,2,3,3,7,4,3,7,7,58
};
const uint16_t _CourierNew_11_223_Blocks[]=
{
	46,6,5,6,5,2,2,2,5,2,2,2,5,6,5,2,2,3,4,2,3,3,3,2,4,2,3,4,2,2,3,8,1,9,57
};
const uint16_t _CourierNew_11_224_Blocks[]=
{
	36,3,9,3,9,3,15,7,5,2,2,3,9,2,4,7,3,3,3,2,3,2,4,2,3,2,2,4,3,9,56
};
const uint16_t _CourierNew_11_225_Blocks[]=
{
	38,3,7,3,7,3,17,7,5,2,2,3,9,2,4,7,3,3,3,2,3,2,4,2,3,2,2,4,3,9,56
};
const uint16_t _CourierNew_11_226_Blocks[]=
{
	37,3,7,5,5,3,1,3,14,7,5,2,2,3,9,2,4,7,3,3,3,2,3,2,4,2,3,2,2,4,3,9,56
};
const uint16_t _CourierNew_11_227_Blocks[]=
{
	47,6,4,7,14,7,5,2,2,3,9,2,4,7,3,3,3,2,3,2,4,2,3,2,2,4,3,9,56
};
const uint16_t _CourierNew_11_228_Blocks[]=
{
	56,3,1,3,15,7,5,2,2,3,9,2,4,7,3,3,3,2,3,2,4,2,3,2,2,4,3,9,56
};
const uint16_t _CourierNew_11_229_Blocks[]=
{
	26,4,6,5,6,5,7,4,15,7,5,2,2,3,9,2,4,7,3,3,3,2,3,2,4,2,3,2,2,4,3,9,56
};
const uint16_t _CourierNew_11_230_Blocks[]=
{
	78,9,2,2,1,4,1,2,4,3,2,15,2,3,4,2,2,3,4,2,2,3,1,14,55
};
const uint16_t _CourierNew_11_231_Blocks[]=
{
	80,7,3,3,2,3,2,2,5,2,2,2,9,2,9,2,9,4,2,4,2,8,5,2,9,3,9,2,7,4,15
};
const uint16_t _CourierNew_11_232_Blocks[]=
{
	36,3,9,3,9,3,16,7,3,3,2,4,1,3,5,2,1,10,1,3,9,2,9,4,2,3,3,7,57
};
const uint16_t _CourierNew_11_233_Blocks[]=
{
	38,3,7,3,7,3,18,7,3,3,2,4,1,3,5,2,1,10,1,3,9,2,9,4,2,3,3,7,57
};
const uint16_t _CourierNew_11_234_Blocks[]=
{
	37,3,7,5,5,3,1,3,15,7,3,3,2,4,1,3,5,2,1,10,1,3,9,2,9,4,2,3,3,7,57
};
const uint16_t _CourierNew_11_235_Blocks[]=
{
	57,3,1,3,15,7,3,3,2,4,1,3,5,2,1,10,1,3,9,2,9,4,2,3,3,7,57
};
const uint16_t _CourierNew_11_236_Blocks[]=
{
	35,3,9,3,9,3,17,5,8,3,8,3,8,3,8,3,8,3,8,3,5,9,56
};
const uint16_t _CourierNew_11_237_Blocks[]=
{
	37,3,7,3,7,3,19,5,8,3,8,3,8,3,8,3,8,3,8,3,5,9,56
};
const uint16_t _CourierNew_11_238_Blocks[]=
{
	36,3,7,5,5,3,1,3,16,5,8,3,8,3,8,3,8,3,8,3,8,3,5,9,56
};
const uint16_t _CourierNew_11_239_Blocks[]=
{
	56,3,1,3,16,5,8,3,8,3,8,3,8,3,8,3,8,3,5,9,56
};
const uint16_t _CourierNew_11_240_Blocks[]=
{
	47,6,5,5,5,7,4,8,2,4,1,4,1,3,5,2,1,3,5,2,1,3,5,2,2,2,5,2,2,4,1,4,3,7,57
};
const uint16_t _CourierNew_11_241_Blocks[]=
{
	47,6,4,7,13,9,4,3,1,3,4,2,3,3,3,2,3,3,3,2,3,3,3,2,3,3,3,2,3,3,1,5,1,5,55
};
const uint16_t _CourierNew_11_242_Blocks[]=
{
	36,3,9,3,9,3,16,7,3,3,3,3,2,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,3,3,3,3,7,57
};
const uint16_t _CourierNew_11_243_Blocks[]=
{
	38,3,7,3,7,3,18,7,3,3,3,3,2,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,3,3,3,3,7,57
};
const uint16_t _CourierNew_11_244_Blocks[]=
{
	37,3,7,5,5,3,1,3,15,7,3,3,3,3,2,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,3,3,3,3,7,57
};
const uint16_t _CourierNew_11_245_Blocks[]=
{
	47,6,4,7,15,7,3,3,3,3,2,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,3,3,3,3,7,57
};
const uint16_t _CourierNew_11_246_Blocks[]=
{
	57,3,1,3,15,7,3,3,3,3,2,2,5,2,2,2,5,2,2,2,5,2,2,2,5,2,2,3,3,3,3,7,57
};
const uint16_t _CourierNew_11_247_Blocks[]=
{
	70,3,27,9,27,3,81
};
const uint16_t _CourierNew_11_248_Blocks[]=
{
	79,9,1,4,1,4,1,3,3,4,1,3,1,3,1,2,1,6,2,2,2,4,3,2,2,4,1,4,1,9,3,1,53
};
const uint16_t _CourierNew_11_249_Blocks[]=
{
	36,3,9,3,9,3,14,4,1,5,2,3,3,3,2,3,3,3,2,3,3,3,2,3,3,3,2,3,3,3,3,2,1,5,3,9,55
};
const uint16_t _CourierNew_11_250_Blocks[]=
{
	38,3,7,3,7,3,16,4,1,5,2,3,3,3,2,3,3,3,2,3,3,3,2,3,3,3,2,3,3,3,3,2,1,5,3,9,55
};
const uint16_t _CourierNew_11_251_Blocks[]=
{
	37,3,7,5,5,3,1,3,13,4,1,5,2,3,3,3,2,3,3,3,2,3,3,3,2,3,3,3,2,3,3,3,3,2,1,5,3,9,55
};
const uint16_t _CourierNew_11_252_Blocks[]=
{
	57,3,1,3,13,4,1,5,2,3,3,3,2,3,3,3,2,3,3,3,2,3,3,3,2,3,3,3,3,2,1,5,3,9,55
};
const uint16_t _CourierNew_11_253_Blocks[]=
{
	39,3,7,3,7,3,16,4,2,4,1,3,4,2,3,3,2,3,3,3,2,2,5,6,5,5,7,4,7,3,8,3,8,2,6,7,25
};
const uint16_t _CourierNew_11_254_Blocks[]=
{
	44,3,9,2,9,2,9,8,3,4,1,4,2,3,4,2,2,2,5,3,1,2,5,3,1,3,4,2,2,4,1,4,2,8,3,2,9,2,8,6,27
};
const uint16_t _CourierNew_11_255_Blocks[]=
{
	57,3,1,3,14,4,2,4,1,3,4,2,3,3,2,3,3,3,2,2,5,6,5,5,7,4,7,3,8,3,8,2,6,7,25
};
const charmold CourierNew_11[]=
{
	{11,20,_CourierNew_11_000_Blocks},
	{11,20,_CourierNew_11_001_Blocks},
	{11,20,_CourierNew_11_002_Blocks},
	{11,20,_CourierNew_11_003_Blocks},
	{11,20,_CourierNew_11_004_Blocks},
	{11,20,_CourierNew_11_005_Blocks},
	{11,20,_CourierNew_11_006_Blocks},
	{11,20,_CourierNew_11_007_Blocks},
	{11,20,_CourierNew_11_008_Blocks},
	{0,20,_CourierNew_11_009_Blocks},
	{0,20,_CourierNew_11_010_Blocks},
	{11,20,_CourierNew_11_011_Blocks},
	{11,20,_CourierNew_11_012_Blocks},
	{0,20,_CourierNew_11_013_Blocks},
	{11,20,_CourierNew_11_014_Blocks},
	{11,20,_CourierNew_11_015_Blocks},
	{11,20,_CourierNew_11_016_Blocks},
	{11,20,_CourierNew_11_017_Blocks},
	{11,20,_CourierNew_11_018_Blocks},
	{11,20,_CourierNew_11_019_Blocks},
	{11,20,_CourierNew_11_020_Blocks},
	{11,20,_CourierNew_11_021_Blocks},
	{11,20,_CourierNew_11_022_Blocks},
	{11,20,_CourierNew_11_023_Blocks},
	{11,20,_CourierNew_11_024_Blocks},
	{11,20,_CourierNew_11_025_Blocks},
	{11,20,_CourierNew_11_026_Blocks},
	{11,20,_CourierNew_11_027_Blocks},
	{0,20,_CourierNew_11_028_Blocks},
	{0,20,_CourierNew_11_029_Blocks},
	{0,20,_CourierNew_11_030_Blocks},
	{0,20,_CourierNew_11_031_Blocks},
	{11,20,_CourierNew_11_032_Blocks},
	{11,20,_CourierNew_11_033_Blocks},
	{11,20,_CourierNew_11_034_Blocks},
	{11,20,_CourierNew_11_035_Blocks},
	{11,20,_CourierNew_11_036_Blocks},
	{11,20,_CourierNew_11_037_Blocks},
	{11,20,_CourierNew_11_038_Blocks},
	{11,20,_CourierNew_11_039_Blocks},
	{11,20,_CourierNew_11_040_Blocks},
	{11,20,_CourierNew_11_041_Blocks},
	{11,20,_CourierNew_11_042_Blocks},
	{11,20,_CourierNew_11_043_Blocks},
	{11,20,_CourierNew_11_044_Blocks},
	{11,20,_CourierNew_11_045_Blocks},
	{11,20,_CourierNew_11_046_Blocks},
	{11,20,_CourierNew_11_047_Blocks},
	{11,20,_CourierNew_11_048_Blocks},
	{11,20,_CourierNew_11_049_Blocks},
	{11,20,_CourierNew_11_050_Blocks},
	{11,20,_CourierNew_11_051_Blocks},
	{11,20,_CourierNew_11_052_Blocks},
	{11,20,_CourierNew_11_053_Blocks},
	{11,20,_CourierNew_11_054_Blocks},
	{11,20,_CourierNew_11_055_Blocks},
	{11,20,_CourierNew_11_056_Blocks},
	{11,20,_CourierNew_11_057_Blocks},
	{11,20,_CourierNew_11_058_Blocks},
	{11,20,_CourierNew_11_059_Blocks},
	{11,20,_CourierNew_11_060_Blocks},
	{11,20,_CourierNew_11_061_Blocks},
	{11,20,_CourierNew_11_062_Blocks},
	{11,20,_CourierNew_11_063_Blocks},
	{11,20,_CourierNew_11_064_Blocks},
	{11,20,_CourierNew_11_065_Blocks},
	{11,20,_CourierNew_11_066_Blocks},
	{11,20,_CourierNew_11_067_Blocks},
	{11,20,_CourierNew_11_068_Blocks},
	{11,20,_CourierNew_11_069_Blocks},
	{11,20,_CourierNew_11_070_Blocks},
	{11,20,_CourierNew_11_071_Blocks},
	{11,20,_CourierNew_11_072_Blocks},
	{11,20,_CourierNew_11_073_Blocks},
	{11,20,_CourierNew_11_074_Blocks},
	{11,20,_CourierNew_11_075_Blocks},
	{11,20,_CourierNew_11_076_Blocks},
	{11,20,_CourierNew_11_077_Blocks},
	{11,20,_CourierNew_11_078_Blocks},
	{11,20,_CourierNew_11_079_Blocks},
	{11,20,_CourierNew_11_080_Blocks},
	{11,20,_CourierNew_11_081_Blocks},
	{11,20,_CourierNew_11_082_Blocks},
	{11,20,_CourierNew_11_083_Blocks},
	{11,20,_CourierNew_11_084_Blocks},
	{11,20,_CourierNew_11_085_Blocks},
	{11,20,_CourierNew_11_086_Blocks},
	{11,20,_CourierNew_11_087_Blocks},
	{11,20,_CourierNew_11_088_Blocks},
	{11,20,_CourierNew_11_089_Blocks},
	{11,20,_CourierNew_11_090_Blocks},
	{11,20,_CourierNew_11_091_Blocks},
	{11,20,_CourierNew_11_092_Blocks},
	{11,20,_CourierNew_11_093_Blocks},
	{11,20,_CourierNew_11_094_Blocks},
	{11,20,_CourierNew_11_095_Blocks},
	{11,20,_CourierNew_11_096_Blocks},
	{11,20,_CourierNew_11_097_Blocks},
	{11,20,_CourierNew_11_098_Blocks},
	{11,20,_CourierNew_11_099_Blocks},
	{11,20,_CourierNew_11_100_Blocks},
	{11,20,_CourierNew_11_101_Blocks},
	{11,20,_CourierNew_11_102_Blocks},
	{11,20,_CourierNew_11_103_Blocks},
	{11,20,_CourierNew_11_104_Blocks},
	{11,20,_CourierNew_11_105_Blocks},
	{11,20,_CourierNew_11_106_Blocks},
	{11,20,_CourierNew_11_107_Blocks},
	{11,20,_CourierNew_11_108_Blocks},
	{11,20,_CourierNew_11_109_Blocks},
	{11,20,_CourierNew_11_110_Blocks},
	{11,20,_CourierNew_11_111_Blocks},
	{11,20,_CourierNew_11_112_Blocks},
	{11,20,_CourierNew_11_113_Blocks},
	{11,20,_CourierNew_11_114_Blocks},
	{11,20,_CourierNew_11_115_Blocks},
	{11,20,_CourierNew_11_116_Blocks},
	{11,20,_CourierNew_11_117_Blocks},
	{11,20,_CourierNew_11_118_Blocks},
	{11,20,_CourierNew_11_119_Blocks},
	{11,20,_CourierNew_11_120_Blocks},
	{11,20,_CourierNew_11_121_Blocks},
	{11,20,_CourierNew_11_122_Blocks},
	{11,20,_CourierNew_11_123_Blocks},
	{11,20,_CourierNew_11_124_Blocks},
	{11,20,_CourierNew_11_125_Blocks},
	{11,20,_CourierNew_11_126_Blocks},
	{4,20,_CourierNew_11_127_Blocks},
	{11,20,_CourierNew_11_128_Blocks},
	{0,20,_CourierNew_11_129_Blocks},
	{11,20,_CourierNew_11_130_Blocks},
	{11,20,_CourierNew_11_131_Blocks},
	{11,20,_CourierNew_11_132_Blocks},
	{11,20,_CourierNew_11_133_Blocks},
	{11,20,_CourierNew_11_134_Blocks},
	{11,20,_CourierNew_11_135_Blocks},
	{11,20,_CourierNew_11_136_Blocks},
	{11,20,_CourierNew_11_137_Blocks},
	{11,20,_CourierNew_11_138_Blocks},
	{11,20,_CourierNew_11_139_Blocks},
	{11,20,_CourierNew_11_140_Blocks},
	{0,20,_CourierNew_11_141_Blocks},
	{11,20,_CourierNew_11_142_Blocks},
	{0,20,_CourierNew_11_143_Blocks},
	{0,20,_CourierNew_11_144_Blocks},
	{11,20,_CourierNew_11_145_Blocks},
	{11,20,_CourierNew_11_146_Blocks},
	{11,20,_CourierNew_11_147_Blocks},
	{11,20,_CourierNew_11_148_Blocks},
	{11,20,_CourierNew_11_149_Blocks},
	{11,20,_CourierNew_11_150_Blocks},
	{11,20,_CourierNew_11_151_Blocks},
	{0,20,_CourierNew_11_152_Blocks},
	{11,20,_CourierNew_11_153_Blocks},
	{11,20,_CourierNew_11_154_Blocks},
	{11,20,_CourierNew_11_155_Blocks},
	{11,20,_CourierNew_11_156_Blocks},
	{0,20,_CourierNew_11_157_Blocks},
	{11,20,_CourierNew_11_158_Blocks},
	{11,20,_CourierNew_11_159_Blocks},
	{11,20,_CourierNew_11_160_Blocks},
	{11,20,_CourierNew_11_161_Blocks},
	{11,20,_CourierNew_11_162_Blocks},
	{11,20,_CourierNew_11_163_Blocks},
	{11,20,_CourierNew_11_164_Blocks},
	{11,20,_CourierNew_11_165_Blocks},
	{11,20,_CourierNew_11_166_Blocks},
	{11,20,_CourierNew_11_167_Blocks},
	{11,20,_CourierNew_11_168_Blocks},
	{11,20,_CourierNew_11_169_Blocks},
	{11,20,_CourierNew_11_170_Blocks},
	{11,20,_CourierNew_11_171_Blocks},
	{11,20,_CourierNew_11_172_Blocks},
	{11,20,_CourierNew_11_173_Blocks},
	{11,20,_CourierNew_11_174_Blocks},
	{11,20,_CourierNew_11_175_Blocks},
	{11,20,_CourierNew_11_176_Blocks},
	{11,20,_CourierNew_11_177_Blocks},
	{11,20,_CourierNew_11_178_Blocks},
	{11,20,_CourierNew_11_179_Blocks},
	{11,20,_CourierNew_11_180_Blocks},
	{11,20,_CourierNew_11_181_Blocks},
	{11,20,_CourierNew_11_182_Blocks},
	{11,20,_CourierNew_11_183_Blocks},
	{11,20,_CourierNew_11_184_Blocks},
	{11,20,_CourierNew_11_185_Blocks},
	{11,20,_CourierNew_11_186_Blocks},
	{11,20,_CourierNew_11_187_Blocks},
	{11,20,_CourierNew_11_188_Blocks},
	{11,20,_CourierNew_11_189_Blocks},
	{11,20,_CourierNew_11_190_Blocks},
	{11,20,_CourierNew_11_191_Blocks},
	{11,20,_CourierNew_11_192_Blocks},
	{11,20,_CourierNew_11_193_Blocks},
	{11,20,_CourierNew_11_194_Blocks},
	{11,20,_CourierNew_11_195_Blocks},
	{11,20,_CourierNew_11_196_Blocks},
	{11,20,_CourierNew_11_197_Blocks},
	{11,20,_CourierNew_11_198_Blocks},
	{11,20,_CourierNew_11_199_Blocks},
	{11,20,_CourierNew_11_200_Blocks},
	{11,20,_CourierNew_11_201_Blocks},
	{11,20,_CourierNew_11_202_Blocks},
	{11,20,_CourierNew_11_203_Blocks},
	{11,20,_CourierNew_11_204_Blocks},
	{11,20,_CourierNew_11_205_Blocks},
	{11,20,_CourierNew_11_206_Blocks},
	{11,20,_CourierNew_11_207_Blocks},
	{11,20,_CourierNew_11_208_Blocks},
	{11,20,_CourierNew_11_209_Blocks},
	{11,20,_CourierNew_11_210_Blocks},
	{11,20,_CourierNew_11_211_Blocks},
	{11,20,_CourierNew_11_212_Blocks},
	{11,20,_CourierNew_11_213_Blocks},
	{11,20,_CourierNew_11_214_Blocks},
	{11,20,_CourierNew_11_215_Blocks},
	{11,20,_CourierNew_11_216_Blocks},
	{11,20,_CourierNew_11_217_Blocks},
	{11,20,_CourierNew_11_218_Blocks},
	{11,20,_CourierNew_11_219_Blocks},
	{11,20,_CourierNew_11_220_Blocks},
	{11,20,_CourierNew_11_221_Blocks},
	{11,20,_CourierNew_11_222_Blocks},
	{11,20,_CourierNew_11_223_Blocks},
	{11,20,_CourierNew_11_224_Blocks},
	{11,20,_CourierNew_11_225_Blocks},
	{11,20,_CourierNew_11_226_Blocks},
	{11,20,_CourierNew_11_227_Blocks},
	{11,20,_CourierNew_11_228_Blocks},
	{11,20,_CourierNew_11_229_Blocks},
	{11,20,_CourierNew_11_230_Blocks},
	{11,20,_CourierNew_11_231_Blocks},
	{11,20,_CourierNew_11_232_Blocks},
	{11,20,_CourierNew_11_233_Blocks},
	{11,20,_CourierNew_11_234_Blocks},
	{11,20,_CourierNew_11_235_Blocks},
	{11,20,_CourierNew_11_236_Blocks},
	{11,20,_CourierNew_11_237_Blocks},
	{11,20,_CourierNew_11_238_Blocks},
	{11,20,_CourierNew_11_239_Blocks},
	{11,20,_CourierNew_11_240_Blocks},
	{11,20,_CourierNew_11_241_Blocks},
	{11,20,_CourierNew_11_242_Blocks},
	{11,20,_CourierNew_11_243_Blocks},
	{11,20,_CourierNew_11_244_Blocks},
	{11,20,_CourierNew_11_245_Blocks},
	{11,20,_CourierNew_11_246_Blocks},
	{11,20,_CourierNew_11_247_Blocks},
	{11,20,_CourierNew_11_248_Blocks},
	{11,20,_CourierNew_11_249_Blocks},
	{11,20,_CourierNew_11_250_Blocks},
	{11,20,_CourierNew_11_251_Blocks},
	{11,20,_CourierNew_11_252_Blocks},
	{11,20,_CourierNew_11_253_Blocks},
	{11,20,_CourierNew_11_254_Blocks},
	{11,20,_CourierNew_11_255_Blocks},
};

