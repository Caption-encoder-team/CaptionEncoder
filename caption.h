#ifndef _CAPTION_
#define _CAPTION_
/*
字幕处理头文件
20210810 创建 赵海武
*/
#include <stdio.h>
#include <stdint.h>

class TimeInfo
{
	unsigned char buf[11];
	void set_time(uint64_t t,unsigned char *buf)
	{
		buf[0] = (((t >> 30) & 0x07) << 1) + 0xF1;
		buf[1] = ((t >> 22) & 0xFF);
		buf[2] = (((t >> 15) & 0x7F) << 1) + 0x01;
		buf[3] = ((t >> 7) & 0xFF);
		buf[4] = (((t)& 0x7F) << 1) + 0x01;
	}
	void set_time(unsigned char h, unsigned char m, unsigned char s, unsigned short ms, unsigned char *buf)
	{
		buf[0] = h + 1; 
		buf[1] = m + 1; 
		buf[2] = s + 1;
		//buf[3] = (ms + 1) >> 8;
		//buf[4] = (ms + 1) & 0xFF;
		buf[3] = (ms + 1) >> 2;
		buf[4] = ((ms + 1) << 6) | 0x3F;
	}
public:
	TimeInfo()
	{
		buf[0] = 0xA3;//相对于节目开始时刻，时分秒毫秒，指定结束时刻
		for (int i = 1; i < 11; i++) buf[i] = 0x00;
	}
	TimeInfo(unsigned char ref, unsigned char format, unsigned char endtype)
	{
		buf[0] = (ref << 6) + (format << 4) + (endtype << 2) + 0x03;
		for (int i = 1; i < 11; i++) buf[i] = 0x00;
	}
	void SetRef(unsigned char ref){ buf[0] = (buf[0] & 0x3F) + (ref << 6); }
	void SetFormat(unsigned char format){ buf[0] = (buf[0] & 0xCF) + (format << 4); }
	void SetEndtype(unsigned char endtype){ buf[0] = (buf[0] & 0xF3) + (endtype << 2); }
	unsigned char GetRef(){ return buf[0] >> 6; }
	unsigned char GetFormat(){ return (buf[0] >> 4) & 0x03; }
	unsigned char GetEndtype(){ return (buf[0] >> 2) & 0x03; }
	void SetStartTime(uint64_t st){ set_time(st, buf + 1); }
	void SetEndTime(uint64_t et){ set_time(et, buf + 6); }
	void SetStartTime(unsigned char h, unsigned char m, unsigned char s, unsigned short ms)
	{
		set_time(h,m,s,ms, buf + 1);
	}
	void SetEndTime(unsigned char h, unsigned char m, unsigned char s, unsigned short ms)
	{
		set_time(h, m, s, ms, buf + 6);
	}

	int ReadSrt(FILE *fin);
	void printScn();
	void printSrt(FILE *fsrt);
	void printAVS(FILE *favs);//AVS字幕文件
	void printES(FILE *fes){ fwrite(buf, 1, 11, fes); }
};
//-----------------------------------------------------------------------------
class PositionDescription
{
	unsigned char buf[9];
public:
	PositionDescription(){ for (int i = 0; i < 9; i++) buf[i] = 0xFF; }
	void SetOrigin(unsigned char origin){ buf[0] = (buf[0] & 0x3F) | (origin << 6); }
	void SetAbsOrRelative(unsigned char ar){ buf[0] = (buf[0] & 0xCF) | ((ar & 0x03) << 4); }
	void SetFormat(unsigned char pf){ buf[0] = (buf[0] & 0xF0) | ((pf & 0x0F)); }
	unsigned char GetOrigin(){ return (buf[0] >> 6); }
	unsigned char GetAbsOrRelative(){ return (buf[0] >> 4) & 0x03; }
	unsigned char GetFormat(){ return (buf[0]) & 0x0F; }
	void SetCoordinate(unsigned short c, int i){ buf[i] = (c >> 7) & 0xFF; buf[i + 1] = (c << 1) & 0xFE + 1; }
	void SetCenter(unsigned short x, unsigned short y){ SetCoordinate(x, 1); SetCoordinate(y, 3); }
	void SetWindow(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1){ SetCoordinate(x0, 1); SetCoordinate(y0, 3); SetCoordinate(x1, 5); SetCoordinate(y1, 7); }
	unsigned short GetCoordinate(int i){ return (buf[i] * 256 + buf[i + 1]) >> 1; }
	void print()
	{ 
		printf("\norigin=%d\nabs_or_relative=%d\nposition_format=%d", GetOrigin(), GetAbsOrRelative(), GetFormat()); 
		if (GetFormat() == 1)
		{
			printf("\nCenter(%d,%d)", GetCoordinate(1), GetCoordinate(3));
		}
		if (GetFormat() == 2)
		{
			printf("\nWindow(%d,%d,%d,%d)", GetCoordinate(1), GetCoordinate(3), GetCoordinate(5), GetCoordinate(7));
		}
	}
	void printES(FILE *fes){ fwrite(buf, 1, 9, fes); }
};
//-----------------------------------------------------------------------------
class DisplationDescription
{
	unsigned char buf[2];
public:
	DisplationDescription(){ buf[0] = buf[1] = 0xFF; }
	void SetWriteDirection(unsigned char wd){ buf[0] = (buf[0] & 0x7F) | (wd << 7); }
	void SetHJustification(unsigned char hj){ buf[0] = (buf[0] & 0x9F) | ((hj & 0x03) << 5); }
	void SetVJustification(unsigned char vj){ buf[0] = (buf[0] & 0xE7) | ((vj & 0x03) << 3); }
	unsigned char GetWriteDirection(){ return (buf[0] >> 7); }
	unsigned char GetHJustification(){ return (buf[0] >> 5) & 0x03; }
	unsigned char GetVJustification(){ return (buf[0] >> 3) & 0x03; }
	void print(){ printf("\nwrite_direction=%d\nhorizantal_justification=%d\nvertical_justification=%d", GetWriteDirection(), GetHJustification(), GetVJustification()); }
	void printES(FILE *fes){ fwrite(buf, 1, 2, fes); }
};
//-----------------------------------------------------------------------------
class ColorInfo
{
	unsigned char buf[13];
public:
	ColorInfo(){ for (int i = 0; i < 13; i++) buf[i] = 0xFF; }
	void set_background_color(unsigned char r, unsigned char g, unsigned char b, unsigned char t)
	{
		buf[0] = r; buf[1] = g; buf[2] = t + 128; buf[3] = b;
	}
	void set_background_width(unsigned char bw){ buf[4] = bw; }
	void set_foreground_color(unsigned char r, unsigned char g, unsigned char b, unsigned char t)
	{
		buf[5] = r; buf[6] = g; buf[7] = t + 128; buf[8] = b;
	}
	unsigned int get_background_color(){ return ((buf[0] * 256 + buf[1]) * 256 + buf[3]) * 256 + buf[2] - 128; }
	unsigned int get_foreground_color(){ return ((buf[5] * 256 + buf[6]) * 256 + buf[8]) * 256 + buf[7] - 128; }
	unsigned char get_background_width(){ return buf[4]; }
	void print(){ printf("\nbackground(%3d,%3d,%3d,%3d) %d foreground(%3d,%3d,%3d,%3d)", buf[0], buf[1], buf[3], buf[2] - 128, buf[4], buf[5], buf[6], buf[8], buf[7] - 128); }
	void printES(FILE *fes){ fwrite(buf, 1, 13, fes); }
};
//-----------------------------------------------------------------------------
class FontDescription
{
	unsigned char buf[6];
public:
	FontDescription(){ for (int i = 0; i < 6; i++) buf[i] = 0xFF; }
	void SetLanguange(char *lan){ buf[0] = lan[0]; buf[1] = lan[1]; buf[2] = lan[2]; }
	void SetFontID(unsigned char id){ buf[3] = id; }
	void SetFontSize(unsigned char fs){ buf[4] = fs; }
	char *GetLanguange(char *lan){ lan[0] = buf[0]; lan[1] = buf[1]; lan[2] = buf[2]; return lan; }
	unsigned char GetFontID(){ return buf[3]; }
	unsigned char GetFontSize(){ return buf[4]; }
	void print()
	{
		char s[4]; s[3] = 0;
		printf("\nlanguage =%s", GetLanguange(s));
		printf("\nfont_id=%d", GetFontID());
		printf("\nfont_size=%d", GetFontSize());
	}
	void printES(FILE *fes){ fwrite(buf, 1, 6, fes); }
};
//-----------------------------------------------------------------------------
class StyleDescription
{
	unsigned char buf[2];
public:
	StyleDescription(){ for (int i = 0; i < 2; i++) buf[i] = 0xFF; }
	void SetBold(unsigned char flag){ buf[0] = (buf[0] & 0x7F) | (flag << 7); }
	void SetItalic(unsigned char flag){ buf[0] = (buf[0] & 0xBF) | (flag << 6); }
	void SetUnderline(unsigned char flag){ buf[0] = (buf[0] & 0xDF) | (flag << 5); }
	unsigned char GetBold(){ return (buf[0] & 0x80) >> 7; }
	unsigned char GetItalic(){ return (buf[0] & 0x40) >> 6; }
	unsigned char GetUnderline(){ return (buf[0] & 0x20) >> 5; }
	void print()
	{
		printf("\nbold_flag =%d", GetBold());
		printf("\nitalic_flag =%d", GetItalic());
		printf("\nunderline_flag =%d", GetUnderline());
	}
	void printES(FILE *fes){ fwrite(buf, 1, 2, fes); }
};
//-----------------------------------------------------------------------------
const int MaxLineLen = 256;
const int MaxLineNum = 4;
class CCLine
{public:
	char buf[MaxLineLen];
	int LinChar;
	int LinByte;
	CCLine(){ LinChar = 0; LinByte = 0; for (int i = 0; i < MaxLineLen; i++) buf[i] = 0; }
	int Read(FILE *fin);
	void print(){ printf("\n%s", buf); }
	void print2f(FILE *f){ fprintf(f,"\n%s", buf); }
	void printES(FILE *fes){ fwrite(buf, 1, LinByte, fes); }
};
class CCString
{
	CCLine Line[MaxLineNum];
	int LineNum;
public:
	CCString(){ LineNum = 0; }
	int Read(FILE *fin);
	void print();
	void print2f(FILE *f);
	void printES(FILE *fes);
};
//-----------------------------------------------------------------------------
class CCSample
{
	unsigned int startcode;
	unsigned char cctype;
	char language[4];
	unsigned char string_offset;
	int length;
	int counter;
public:
	TimeInfo ti;
	DisplationDescription dd;
	ColorInfo ci;
	PositionDescription pd;
	FontDescription fd;
	StyleDescription sd;
	CCString st;
	CCSample()
	{ 
		startcode = 0x1C0; 
		cctype = 1;
		strcpy(language, "chn");
		string_offset = 43;//时间和各种格式描述共43个字节，没有用户数据时这个偏移等于43.
		length = 0; 
		counter = 0; 
	}
	int ReadSrt(FILE *fin);
	int GetLength(){ return length; }
	void print();
	void printAVS(FILE *favs);
	void printES(FILE *fes);
	void printSrt(FILE *fsrt);
};
//-----------------------------------------------------------------------------

void test_caption(char *fname, char *foutsrtname, char *fesname);

#endif // !_CAPTION_
