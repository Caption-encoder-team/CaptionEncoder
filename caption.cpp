/*
��Ļ����Դ�ļ�
���ܣ�
1����ȡSRT�ļ�������ANSI��ʽ��UTF-8��ʽ
2������ΪSRT�ļ�����ʽ������SRT�ļ�һ�£�������ANSI����ANSI������UTF-8����UTF-8
3������AVS��Ļ��׼�涨����ĻES��
4����ʱֻ����CC_type=1����ͨ��Ļ��Ӧ���㲥������ע�͡�ֱ����Ļ��δ��ȫʵ��

20210810 ���� �Ժ���

*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "caption.h"

int file_format=0;//0=ANSI 1=UTF-8

unsigned short ToUnicode[4] = {

};
/*
4e00 һ �� �@ �� �A �B �C �� �� �� �� �� آ �� �� �D
4e10 ؤ �� �E ר �� ا �� �F �� �� ҵ �� �� ˿ ة �G
4e20 �H �I �� �J �� �� �K ɥ ح �L �� Ѿ �� �� �M �N
4e30 �� �O �� �P �� �Q ؼ �R �� �� Ϊ �� �S �� �� د
4e40 �T �U �V �� �W �� �X ر ô �� �Y ֮ �� է �� ��
4e50 �� �Z ƹ �� �� �[ �� �\ �� �� �] �^ ؿ �� �� Ҳ
4e60 ϰ �� �_ �` �a �b �� �c �d �� �e �f �g �h �i �j
4e70 �� �� �k �� �l �m �n �o �p �q �r �s �t �u Ǭ �v
4e80 �w �x �y �z �{ �| �� �} �� �� �~ �� �� ء �� ��
4e90 �� �� �� �� �� �� �� �� ب �� �� Щ �� �� �� ؽ
4ea0 �� �� �� �� �� �� �� �� �� Ķ �� �� �� ͤ �� ��
4eb0 �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��
4ec0 ʲ �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��
4ed0 �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� Ǫ
4ee0 �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��
4ef0 �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��
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
		fscanf(fin, "%*c");//����ɹ�����һ�У����������з�
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
	if (cctype != 255)//����Ӧ���㲥�����и�ʽ����Ϣ
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
	if (cctype != 255 && cctype != 4)//����Ӧ���㲥��ֱ����Ļ������ʱ����Ϣ
	{
		ti.printScn();
	}
	st.print();
	printf("\n");
}
//fesӦ�����ö����Ʒ�ʽ�򿪵��ļ�
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
	if (cctype != 255 && cctype != 4)//����Ӧ���㲥��ֱ����Ļ������ʱ����Ϣ
	{
		ti.printES(fes);
	}
	if (cctype != 255)//����Ӧ���㲥�����и�ʽ����Ϣ
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
	if (cctype != 255)//����Ӧ���㲥�����и�ʽ����Ϣ
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
	if (cctype != 255 && cctype != 4)//����Ӧ���㲥��ֱ����Ļ������ʱ����Ϣ
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
		if (buf[0] == c[0] && buf[1] == c[1] && buf[2] == c[2])//UTF-8��ʽ
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
	ccs.dd.SetWriteDirection(0);//ֵΪ1��ʾ���ϵ��£�ֵΪ0��ʾ�����ҡ�
	ccs.dd.SetHJustification(1);//ˮƽ���뷽ʽ��0��ʾ����룬1��ʾ���У�2��ʾ�Ҷ���
	ccs.dd.SetVJustification(1);//��ֱ���뷽ʽ��0��ʾ���˶��룬1��ʾ���У�2��ʾ�׶˶���
	ccs.ci.set_background_color(128, 128, 128, 50);
	ccs.ci.set_background_width(255);
	ccs.ci.set_foreground_color(250, 250, 100, 100);//t=100��ʾ��͸��
	ccs.pd.SetOrigin(2);//1=��Ļ���Ͻǣ�2=��Ƶ��ʾ�������Ͻ�
	ccs.pd.SetAbsOrRelative(1);//1=���Եģ���λ�����أ�2=��Եģ�ǧ�ֱ�
	ccs.pd.SetFormat(2);//ָ����ʽ��1=ָ�����ĵ㣬2=ָ�����ϽǺ����½�����
	ccs.pd.SetWindow(100, 900, 1820, 1024);//
	ccs.fd.SetLanguange("chn");
	ccs.fd.SetFontID(0);//���庬���ڱ�׼�й涨
	ccs.fd.SetFontSize(32);//�ַ��߶ȣ���λ��abs_or_relative��ֵ����
	ccs.sd.SetBold(1);
	ccs.sd.SetItalic(0);
	ccs.sd.SetUnderline(0);
	if ((fsrt = fopen(fname, "r")) == NULL)
	{
		printf("Error! opening file");//����ļ�ָ�뷵��NULL�������˳���
		return;
	}
	if ((foutsrt = fopen(foutsrtname, "w")) == NULL)
	{
		printf("Error! opening file");//����ļ�ָ�뷵��NULL�������˳���
		return;
	}
	if ((fes = fopen(fesname, "wb")) == NULL)
	{
		printf("Error! opening file");//����ļ�ָ�뷵��NULL�������˳���
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
