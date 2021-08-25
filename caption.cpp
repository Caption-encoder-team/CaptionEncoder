/*
字幕处理源文件
功能：
1、读取SRT文件，包括ANSI格式和UTF-8格式
2、保存为SRT文件，格式与读入的SRT文件一致，即读入ANSI保存ANSI，读入UTF-8保存UTF-8
3、生成AVS字幕标准规定的字幕ES流
4、暂时只处理CC_type=1的普通字幕，应急广播、手语注释、直播字幕尚未完全实现

20210810 创建 赵海武

*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "caption.h"

int file_format=0;//0=ANSI 1=UTF-8

unsigned short ToUnicode[4] = {

};
/*
4e00 一 丁 丂 七 丄 丅 丆 万 丈 三 上 下 丌 不 与 丏
4e10 丐 丑 丒 专 且 丕 世 丗 丘 丙 业 丛 东 丝 丞 丟
4e20 丠 両 丢 丣 两 严 並 丧 丨 丩 个 丫 丬 中 丮 丯
4e30 丰 丱 串 丳 临 丵 丶 丷 丸 丹 为 主 丼 丽 举 丿
4e40 乀 乁 乂 乃 乄 久 乆 乇 么 义 乊 之 乌 乍 乎 乏
4e50 乐 乑 乒 乓 乔 乕 乖 乗 乘 乙 乚 乛 乜 九 乞 也
4e60 习 乡 乢 乣 乤 乥 书 乧 乨 乩 乪 乫 乬 乭 乮 乯
4e70 买 乱 乲 乳 乴 乵 乶 乷 乸 乹 乺 乻 乼 乽 乾 乿
4e80 亀 亁 亂 亃 亄 亅 了 亇 予 争 亊 事 二 亍 于 亏
4e90 亐 云 互 亓 五 井 亖 亗 亘 亙 亚 些 亜 亝 亞 亟
4ea0 亠 亡 亢 亣 交 亥 亦 产 亨 亩 亪 享 京 亭 亮 亯
4eb0 亰 亱 亲 亳 亴 亵 亶 亷 亸 亹 人 亻 亼 亽 亾 亿
4ec0 什 仁 仂 仃 仄 仅 仆 仇 仈 仉 今 介 仌 仍 从 仏
4ed0 仐 仑 仒 仓 仔 仕 他 仗 付 仙 仚 仛 仜 仝 仞 仟
4ee0 仠 仡 仢 代 令 以 仦 仧 仨 仩 仪 仫 们 仭 仮 仯
4ef0 仰 仱 仲 仳 仴 仵 件 价 仸 仹 仺 任 仼 份 仾 仿
*/
//-----------------------------------------------------------------------------
void TimeInfo::set_start_time(uint64_t st)
{
	buf[1] = (((st >> 30) & 0x07) << 1) + 0xF1;
	buf[2] = ((st >> 22) & 0xFF);
	buf[3] = (((st >> 15) & 0x7F) << 1) + 0x01;
	buf[4] = ((st >> 7) & 0xFF);
	buf[5] = (((st) & 0x7F) << 1) + 0x01;
}
void TimeInfo::set_start_time(unsigned char h, unsigned char m, unsigned char s, unsigned short ms)
{
	buf[1] = h + 1; buf[2] = m + 1; buf[3] = s + 1;
	ms++;
	buf[4] = ms >> 8;
	buf[5] = ms & 0xFF;
}
void TimeInfo::set_end_time(uint64_t et)
{
	buf[6] = (((et >> 30) & 0x07) << 1) + 0xF1;
	buf[7] = ((et >> 22) & 0xFF);
	buf[8] = (((et >> 15) & 0x7F) << 1) + 0x01;
	buf[9] = ((et >> 7) & 0xFF);
	buf[10] = (((et)& 0x7F) << 1) + 0x01;
}
void TimeInfo::set_end_time(unsigned char h, unsigned char m, unsigned char s, unsigned short ms)
{
	buf[6] = h + 1; buf[7] = m + 1; buf[8] = s + 1;
	ms++;
	buf[9] = ms >> 8;
	buf[10] = ms & 0xFF;
}
int TimeInfo::ReadSrt(FILE *fin)
{
	unsigned char str[256],h,m,s;
	unsigned short ms;
	int r=0;
	r = fscanf(fin, "%[^\n]", str); fscanf(fin, "%*c");
	if (r <= 0) return r;
	h = (str[0] - 0x30) * 10 + (str[1] - 0x30);
	m = (str[3] - 0x30) * 10 + (str[4] - 0x30);
	s = (str[6] - 0x30) * 10 + (str[7] - 0x30);
	ms = (str[9] - 0x30) * 100 + (str[10] - 0x30) * 10 + (str[11] - 0x30);
	set_start_time(h, m, s, ms);
	h = (str[17] - 0x30) * 10 + (str[18] - 0x30);
	m = (str[20] - 0x30) * 10 + (str[21] - 0x30);
	s = (str[23] - 0x30) * 10 + (str[24] - 0x30);
	ms = (str[26] - 0x30) * 100 + (str[27] - 0x30) * 10 + (str[28] - 0x30);
	set_end_time(h, m, s, ms);
	buf[0] = 0xA3;
	return r;
}
void TimeInfo::printScn()
{ 
	printf("\n%02d:%02d:%02d,%03d", buf[1] - 1, buf[2] - 1, buf[3] - 1, buf[4] * 256 + buf[5] - 1);
	printf(" --> ");
	printf("%02d:%02d:%02d,%03d", buf[6] - 1, buf[7] - 1, buf[8] - 1, buf[9] * 256 + buf[10] - 1);
}

void TimeInfo::printSrt(FILE *fsrt)
{
	fprintf(fsrt, "\n%02d:%02d:%02d,%03d", buf[1] - 1, buf[2] - 1, buf[3] - 1, buf[4] * 256 + buf[5] - 1);
	fprintf(fsrt, " --> ");
	fprintf(fsrt, "%02d:%02d:%02d,%03d", buf[6] - 1, buf[7] - 1, buf[8] - 1, buf[9] * 256 + buf[10] - 1);
}
void TimeInfo::printAVS(FILE *favs)
{
	fprintf(favs, "\n%d",buf[0]);
	fprintf(favs, "\n%02d:%02d:%02d,%03d", buf[1] - 1, buf[2] - 1, buf[3] - 1, buf[4] * 256 + buf[5] - 1);
	if (get_endtype() == 0)
	{
		fprintf(favs, " --> ");
	}
	else
		fprintf(favs, " dur ");
	fprintf(favs, "%02d:%02d:%02d,%03d", buf[6] - 1, buf[7] - 1, buf[8] - 1, buf[9] * 256 + buf[10] - 1);
}
//-----------------------------------------------------------------------------
int CCLine::Read(FILE *fin)
{
	int r;
	r = fscanf(fin, "%[^\n]", buf); 
	if (r == EOF)
		return r;
	if (r == 1)
	{
		LinChar = strlen(buf);
		for (LinByte = 0; LinByte < MaxLineLen; LinByte++)
		{
			if (buf[LinByte] == 0)
			{
				LinByte++; break;
			}
		}
		fscanf(fin, "%*c");//如果成功读入一行，则跳过换行符
	}
	return r;
}
int CCString::Read(FILE *fin)
{
	int r;
	LineNum = 0;
	while ((r = Line[LineNum].Read(fin)) == 1)
	{
		LineNum++;
	}
	return r;
}
void CCString::print()
{
	for (int i = 0; i < LineNum; i++)
		Line[i].print();
}
void CCString::print2f(FILE *f)
{
	for (int i = 0; i < LineNum; i++)
		Line[i].print2f(f);
}
void CCString::printES(FILE *fes)
{
	for (int i = 0; i < LineNum; i++)
		Line[i].printES(fes);
}
void CCSample::print()
{
	if (cctype != 255)//不是应急广播，才有格式等信息
	{
		if (counter == 0)
		{
			dd.print();
			ci.print();
			pd.print();
			fd.print();
			sd.print();
		}
	}
	printf("\n%d", counter);
	if (cctype != 255 && cctype != 4)//不是应急广播和直播字幕，才有时间信息
	{
		ti.printScn();
	}
	st.print();
	printf("\n");
}
//fes应该是用二进制方式打开的文件
void CCSample::printES(FILE *fes)
{
	unsigned char buf[9];
	buf[0] = buf[1] = 0;
	buf[2] = 1;
	buf[3] = 0xC0;
	buf[4] = cctype;
	buf[5] = language[0]; buf[6] = language[1]; buf[7] = language[2];
	buf[8] = string_offset;
	fwrite(buf, 1, 9, fes);
	if (cctype != 255 && cctype != 4)//不是应急广播和直播字幕，才有时间信息
	{
		ti.printES(fes);
	}
	if (cctype != 255)//不是应急广播，才有格式等信息
	{
		pd.printES(fes);
		dd.printES(fes);
		ci.printES(fes);
		fd.printES(fes);
		sd.printES(fes);
	}
	st.printES(fes);
}
void CCSample::printSrt(FILE *fsrt)
{
	char bom[4] = { 0xEF, 0xBB, 0xBF, 0x00 };
	if (counter == 0)
	{
		if (file_format == 1)//utf-8
		{
			fprintf(fsrt, "%s", bom);
			fseek(fsrt, 3, SEEK_SET);
		}
	}
	if (cctype != 255)//不是应急广播，才有格式等信息
	{
		if (counter == 0)
		{
			fprintf(fsrt, "%d", counter);
		}
		else
		{
			fprintf(fsrt, "\n%d", counter);
		}
	}
	if (cctype != 255 && cctype != 4)//不是应急广播和直播字幕，才有时间信息
	{
		ti.printSrt(fsrt);
	}
	st.print2f(fsrt);
	fprintf(fsrt,"\n");
}

int CCSample::ReadSrt(FILE *fin)
{
	int r;
	char buf[256], c[3] = { 0xEF, 0xBB, 0xBF };
	//if (counter == 0)
	if (ftell(fin)==0)
	{
		r = fscanf(fin, "%s", buf); fscanf(fin, "%*c");
		if (buf[0] == c[0] && buf[1] == c[1] && buf[2] == c[2])//UTF-8格式
		{
			counter = buf[3] - 0x30;
			file_format = 1;
		}
		else if (buf[0] == 0x30)
		{
			counter = buf[3] - 0x30;
			file_format = 0;
		}
		else
		{
			printf("\n Unknown input srt file format!");
		}
	}
	else
	{
		r = fscanf(fin, "%d", &counter); fscanf(fin, "%*c");
		if (r <= 0)		return r;
	}

	r = ti.ReadSrt(fin);
	if (r <= 0)		return r;

	r = st.Read(fin);
	return r;
}
void test_caption(char *fname, char *foutsrtname, char *fesname)
{
	CCSample ccs;
	FILE *fes;
	FILE *fsrt;
	FILE *foutsrt;
	int r;
	ccs.dd.SetWriteDirection(0);//值为1表示从上到下，值为0表示从左到右。
	ccs.dd.SetHJustification(1);//水平对齐方式，0表示左对齐，1表示居中，2表示右对齐
	ccs.dd.SetVJustification(1);//垂直对齐方式，0表示顶端对齐，1表示居中，2表示底端对齐
	ccs.ci.set_background_color(128, 128, 128, 50);
	ccs.ci.set_background_width(255);
	ccs.ci.set_foreground_color(250, 250, 100, 100);//t=100表示不透明
	ccs.pd.SetOrigin(2);//1=屏幕左上角，2=视频显示窗口左上角
	ccs.pd.SetAbsOrRelative(1);//1=绝对的，单位是像素，2=相对的，千分比
	ccs.pd.SetFormat(2);//指定方式：1=指定中心点，2=指定左上角和右下角坐标
	ccs.pd.SetWindow(100, 900, 1820, 1024);//
	ccs.fd.SetLanguange("chn");
	ccs.fd.SetFontID(0);//具体含义在标准中规定
	ccs.fd.SetFontSize(32);//字符高度，单位由abs_or_relative的值决定
	ccs.sd.SetBold(1);
	ccs.sd.SetItalic(0);
	ccs.sd.SetUnderline(0);
	if ((fsrt = fopen(fname, "r")) == NULL)
	{
		printf("Error! opening file");//如果文件指针返回NULL，报错并退出。
		return;
	}
	if ((foutsrt = fopen(foutsrtname, "w")) == NULL)
	{
		printf("Error! opening file");//如果文件指针返回NULL，报错并退出。
		return;
	}
	if ((fes = fopen(fesname, "wb")) == NULL)
	{
		printf("Error! opening file");//如果文件指针返回NULL，报错并退出。
		return;
	}
	do
	{
		r = ccs.ReadSrt(fsrt);
		ccs.print();
		ccs.printSrt(foutsrt);
		ccs.printES(fes);
	} while (r != EOF);

	fclose(fes);
	fclose(fsrt);
	fclose(foutsrt);
	return;
}
//-----------------------------------------------------------------------------
