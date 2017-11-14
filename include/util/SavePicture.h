#pragma  once
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <string>


class SavePicture
{
public:
    std::string SaveRGBAsBmp(unsigned char* pBits,std::string filepath,int nWidth,int nHeight);
	std::string SaveRGBAsBmp_ex(unsigned char* pBits, std::string filepath, int nWidth, int nHeight, char* index);
	std::string SaveARGBAsBmp_ex(unsigned char* pBits, std::string filepath, int nWidth, int nHeight, char* index);
	int Bmp_Flip(std::string filepath, int nWidth, int nHeight, bool flip_y, bool flip_x);
	//int SaveRGBAsJpg(char * filename, unsigned char* pBits, int quality, int nWidth, int nHeight);
	//int SaveRGBAsJpg_ex(char * filename, unsigned char* pBits, int quality, int nWidth, int nHeight, char* index);
	//int SaveYUVAsJpg_ex(char * filename, unsigned char* pBits, int quality, int nWidth, int nHeight, char* index);
};
