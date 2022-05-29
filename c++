#include <bits/stdc++.h>
#include <iostream>
#include <stdio.h>
#include <utility>
#include <vector>
#include <windows.h>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iomanip>
#define AC return 0;
using namespace std;
//#define ll long long
//#define int long long

//---------------- bmp -> pic ---------------//
//---------------- bmp -> pic ---------------//
//---------------- bmp -> pic ---------------//


int biBitCount = 24; //传递比特数     //默认按原图为24
struct pic_infor
{
    bool isColor;             //1表示为彩色图像，0表示为灰度图像
    bool M;                   //保留
    short int Notes_size;     //注释区字节数
    short int Rows;           //行       ->Height
    short int Columns;        //列       ->Width
    short int Rows_start;     //行起点
    short int Columns_start;  //列起点
    short int T;              //保留
    char some_information[48]; //其他参数

};

struct pic_Text_Notes       //注释
{
    char *Text_Notes;
};

struct pic_RGB
{
    //--------色盘区-------//
    struct RGB  //红、绿、蓝
    {
        uint8_t rgbRed;     //指定红色强度
        uint8_t rgbGreen;   //指定绿色强度
        uint8_t rgbBlue;    //指定蓝色强度
    }ColorTable[256];
};

struct pic_data
{
    unsigned char *data_pic;
};

struct pic
{
    pic_infor infor;
    pic_Text_Notes Note;
    pic_RGB RGB;
    pic_data data;
};

pair<bool, pic> read_bmp(char *path)
{
    pic v;   //存放想要的信息

    FILE *fp = fopen(path, "rb");
    if (fp == 0)  return {0,v};

    fseek(fp, sizeof(BITMAPFILEHEADER), 0);//跳过位图文件头，因为没有想要的信息

    BITMAPINFOHEADER head;  //取出位图信息头
    fread(&head, sizeof(BITMAPINFOHEADER), 1, fp);


    RGBQUAD *pColorTable = new RGBQUAD[256];
    fread(pColorTable, sizeof(RGBQUAD), 256, fp);

    //存放RGB
    bool fl = true; //判断是否是灰度图
    for(int i = 0; i < 256; i++)
    {
        if(pColorTable[i].rgbBlue != pColorTable[i].rgbGreen || pColorTable[i].rgbGreen != pColorTable[i].rgbRed)
            fl = false;
        v.RGB.ColorTable[i].rgbRed = pColorTable[i].rgbRed;
        v.RGB.ColorTable[i].rgbGreen = pColorTable[i].rgbGreen;
        v.RGB.ColorTable[i].rgbBlue = pColorTable[i].rgbBlue;
    }
    if(fl)  v.infor.isColor = 0;
    else    v.infor.isColor = 1;

    v.infor.Rows = head.biHeight;
    v.infor.Columns = head.biWidth;

    biBitCount = head.biBitCount;

    v.infor.Notes_size = 0;
    v.Note.Text_Notes = 0;

    int line_byte = (head.biWidth * head.biBitCount / 8 + 3) / 4 * 4;

    v.data.data_pic = new unsigned char[line_byte * head.biHeight];
    fread(v.data.data_pic, 1, line_byte * head.biHeight, fp);

    fclose(fp);
    return {1,v};
}

bool save_pic(char *path, pic t)
{
    FILE *fp = fopen(path, "wb");
    if(fp == 0) {cout << "error in path" << endl;return 0;}

    //信息区读入(除注释区)
    fwrite(&t.infor, 64 * sizeof(unsigned char), 1, fp);

    //注释区读入
    fwrite(&t.infor + 64, t.infor.Notes_size * sizeof(char), 1, fp);

	  //色盘区读入
    fwrite(&t.RGB, sizeof(pic_RGB), 1, fp);

    //数值区读入
    int line_byte = (t.infor.Columns * biBitCount / 8 + 3) / 4 * 4;
    fwrite(t.data.data_pic, t.infor.Rows * line_byte * sizeof(unsigned char), 1, fp);

    fclose(fp);
    return 1;
}

void bmp_to_pic()
{
    char readPath[] = "C:/Users/HQL/Desktop/test.bmp";
    auto t = read_bmp(readPath);
    if(t.first == 0)    {cout << "error in path" << endl;return;}
    char writePath[] = "C:/Users/HQL/Desktop/test.pic";
    // cout << biBitCount << endl;
    save_pic(writePath, t.second);

}

//---------------- pic -> bmp ---------------//
//---------------- pic -> bmp ---------------//
//---------------- pic -> bmp ---------------//
unsigned char *pBmpBuf;
pair<bool, pic> read_pic(char *path)
{
    pic v;
    FILE *fp = fopen(path, "rb");
    if (fp == 0)  return {0,v};

    fread(&v.infor, sizeof(pic_infor), 1, fp);
    fread(&v.Note, v.infor.Notes_size * sizeof(char), 1, fp);
    fread(&v.RGB, sizeof(pic_RGB), 1, fp);

    int line_byte = (v.infor.Columns * biBitCount / 8 + 3) / 4 * 4;

    pBmpBuf = new unsigned char[line_byte * v.infor.Rows];
    fread(pBmpBuf, 1, line_byte * v.infor.Rows, fp);
    fclose(fp);
    return {1,v};
}

void save_bmp(char *path,pic v)
{
    FILE *fp = fopen(path, "wb");
    if(fp == 0) {cout << "error in path (save)" << endl;return;}

    int line_byte = (v.infor.Columns * biBitCount / 8 + 3) / 4 * 4;
    // cout << line_byte << endl;
    int colorTablesize = 0;

    if (biBitCount == 8)
        colorTablesize = 1024;

    BITMAPFILEHEADER fileHead;
    fileHead.bfType = 0x4D42;

    fileHead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTablesize + line_byte * v.infor.Rows;
    fileHead.bfReserved1 = 0;
    fileHead.bfReserved2 = 0;
    fileHead.bfOffBits = 56;
    fwrite(&fileHead, sizeof(BITMAPFILEHEADER), 1, fp);

    BITMAPINFOHEADER head;
    head.biBitCount = biBitCount;
    head.biClrImportant = 0;
    head.biClrUsed = 0;
    head.biCompression = 0;
    head.biHeight = v.infor.Rows;
    head.biPlanes = 1;
    head.biSize = 40;
    head.biSizeImage = line_byte * v.infor.Rows;
    head.biWidth = v.infor.Columns;
    head.biXPelsPerMeter = 0;
    head.biYPelsPerMeter = 0;

    fwrite(&head, sizeof(BITMAPINFOHEADER), 1, fp);
    // if (biBitCount == 8)
    // {
        RGBQUAD now_col[256];
        for(int i = 0; i < 256; i++)
        {
            now_col[i].rgbReserved = 0;
            now_col[i].rgbRed = v.RGB.ColorTable[i].rgbRed;
            now_col[i].rgbBlue = v.RGB.ColorTable[i].rgbBlue;
            now_col[i].rgbGreen = v.RGB.ColorTable[i].rgbGreen;
        }
        fwrite(&now_col, sizeof(RGBQUAD), 256, fp);

    // }
    // fwrite(pColorTable, sizeof(RGBQUAD), 256, fp);
    // fwrite(&v.data.data_pic, line_byte * v.infor.Rows, 1, fp);

    fwrite(pBmpBuf, v.infor.Rows * line_byte, 1, fp);
    fclose(fp);
}


void pic_to_bmp()
{
    char readPath[] = "C:/Users/HQL/Desktop/test.pic";
    auto t = read_pic(readPath);
    if(!t.first) {cout << "error in path" << endl;return;}
    char writePath[] = "C:/Users/HQL/Desktop/test_cpy.bmp";
    save_bmp(writePath, t.second);
}

signed main()
{
    // ios::sync_with_stdio(false);cin.tie(0);cout.tie(0);
    bmp_to_pic();
    //pic_to_bmp();
    AC
}
