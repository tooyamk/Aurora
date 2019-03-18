#include "String.h"

namespace aurora {
	int String::UnicodeToUtf8(const wchar_t * in, ui32 inLen, char* out, ui32 outLen) {
		if (out == nullptr || in == nullptr)return -1;

		ui32 totalNum = 0;
		for (ui32 i = 0; i < inLen; ++i) {//����ת�����ʵ�����賤��
			wchar_t unicode = in[i];
			if (unicode >= 0x0000 && unicode <= 0x007f) {
				++totalNum;
			} else if (unicode >= 0x0080 && unicode <= 0x07ff) {
				totalNum += 2;
			} else if (unicode >= 0x0800 && unicode <= 0xffff) {
				totalNum += 3;
			}
		}
		if (outLen < totalNum) return -1;//������Ч���жϣ�
		//------------------------------------------------

		ui32 resultSize = 0;//����������������ʵ�ʴ�С��
		auto tmp = out;
		for (ui32 i = 0; i < inLen; ++i) {
			if (resultSize > outLen) return -1;

			wchar_t unicode = in[i];
			if (unicode >= 0x0000 && unicode <= 0x007f) {
				*tmp = (char)unicode;
				++tmp;
				++resultSize;
			} else if (unicode >= 0x0080 && unicode <= 0x07ff) {
				*tmp = 0xc0 | (unicode >> 6);
				++tmp;
				*tmp = 0x80 | (unicode & (0xff >> 2));
				++tmp;
				resultSize += 2;
			} else if (unicode >= 0x0800 && unicode <= 0xffff) {
				*tmp = 0xe0 | (unicode >> 12);
				++tmp;
				*tmp = 0x80 | (unicode >> 6 & 0x00ff);
				++tmp;
				*tmp = 0x80 | (unicode & (0xff >> 2));
				++tmp;
				resultSize += 3;
			}
		}
		return resultSize;
	}

	int String::Utf8ToUnicode(const i8* in, ui32 inLen, wchar_t* out, ui32 outLen) {
		if (out == nullptr || in == nullptr) return -1;

		ui32 totalNum = 0;
		auto p = in;
		for (ui32 i = 0; i < inLen; ++i) {
			if (*p >= 0x00 && *p <= 0x7f) {//˵�����λΪ'0'������ζ��utf8����ֻ��1���ֽڣ�
				++p;
				++totalNum;
			} else if ((*p & (0xe0)) == 0xc0) {//ֻ���������λ���������λ�ǲ���110�����������ζ��utf8������2���ֽڣ�
				p += 2;
				++totalNum;
			} else if ((*p & (0xf0)) == 0xe0) {//ֻ���������λ���������λ�ǲ���1110�����������ζ��utf8������3���ֽڣ�
				p += 3;
				++totalNum;
			}
		}
		if (outLen < totalNum) return -1;//������Ч���жϣ�
		//------------------------------------------------
		i32 resultSize = 0;
		p = in;
		i8* tmp = (i8*)out;
		while (*p) {
			if (*p >= 0x00 && *p <= 0x7f) {//˵�����λΪ'0'������ζ��utf8����ֻ��1���ֽڣ�
				*tmp = *p;
				tmp += 2;
				++resultSize;
			} else if ((*p & 0xe0) == 0xc0) {//ֻ���������λ���������λ�ǲ���110�����������ζ��utf8������2���ֽڣ�
				wchar_t t = 0;
				i8 t1 = 0, t2 = 0;

				t1 = *p & (0x1f);//��λ�ĺ�5λ����ȥ����ͷ����110�����־λ��
				++p;
				t2 = *p & (0x3f);//��λ�ĺ�6λ����ȥ����ͷ����10�����־λ��

				*tmp = t2 | ((t1 & (0x03)) << 6);
				++tmp;
				*tmp = t1 >> 2;//�����䱣������λ
				++tmp;
				++resultSize;
			} else if ((*p & (0xf0)) == 0xe0) {//ֻ���������λ���������λ�ǲ���1110�����������ζ��utf8������3���ֽڣ�
				wchar_t t = 0, t1 = 0, t2 = 0, t3 = 0;
				t1 = *p & (0x1f);
				++p;
				t2 = *p & (0x3f);
				++p;
				t3 = *p & (0x3f);

				*tmp = ((t2 & (0x03)) << 6) | t3;
				++tmp;
				*tmp = (t1 << 4) | (t2 >> 2);
				++tmp;
				++resultSize;
			}
			++p;
		}
		/*�����ǽ����������������򿪴˶Σ�
		*tmp = '/0';
		tmp++;
		*tmp = '/0';
		resultsize += 2;
		*/
		return resultSize;
	}
}