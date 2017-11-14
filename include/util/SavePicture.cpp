#include "SavePicture.h"
#include "Windows.h"
#include <time.h>
#include "capture/StringConverter.h"

//extern "C" {
//#include "libjpeg\jpeglib.h"
//}

        std::string SavePicture::SaveRGBAsBmp(unsigned char* pBits,std::string filepath,int nWidth,int nHeight)
            {
			    //_mkdir("D:\\picture");
		     	time_t timep;
			    char s[30];
			    time(&timep);
				strcpy(s, ctime(&timep));
				std::string  str_time(s);

                FILE *fp = NULL;
                std::string utf8_filename(filepath);
				utf8_filename += str_time;
				utf8_filename.replace(utf8_filename.rfind(" 2016\n"), 6, ".bmp");
				utf8_filename.replace(utf8_filename.rfind(":"), 1, "");
				utf8_filename.replace(utf8_filename.rfind(":"), 1, "");
                std::wstring unicode_filename;
                unicode_filename=StringConverter::utf8_to_unicode(utf8_filename);

                if ((fp = _wfopen(unicode_filename.c_str(), L"wb")) == NULL) {
                    return filepath;
                }

				BITMAPINFOHEADER bih = {0};                                              // 位图信息头
				bih.biBitCount = 24;                                        // 每个像素字节大小
				bih.biCompression = BI_RGB;
				bih.biHeight = nHeight;                                             // 高度
				bih.biPlanes = 1;
				bih.biSize = sizeof(BITMAPINFOHEADER);
				bih.biSizeImage = nWidth*nHeight*3;                       // 图像数据大小
				bih.biWidth = nWidth;                                               // 宽度
				BITMAPFILEHEADER bfh = {0};                                              // 位图文件头
				bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);     // 到位图数据的偏移量
				bfh.bfSize = bfh.bfOffBits + nWidth*nHeight*3;            // 文件总的大小
				bfh.bfType = (WORD)0x4d42;

				fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp);                           //写入位图文件头
				fwrite(&bih, 1, sizeof(BITMAPINFOHEADER), fp);                           //写入位图信息头     
				fwrite(pBits, 1, nWidth*nHeight * 3, fp);                       //写入位图数据
				fclose(fp);
				return filepath;
			}

		std::string SavePicture::SaveRGBAsBmp_ex(unsigned char* pBits, std::string filepath, int nWidth, int nHeight,char* index)
		{
			_mkdir(".\\picture");
			time_t timep;
			char s[30];
			time(&timep);
			strcpy(s, ctime(&timep));
			std::string  str_time(s);
			std::string  str_idx(index);
			std::string  str_bmp(".bmp");
				 
			FILE *fp = NULL;
			std::string utf8_filename(filepath);
			utf8_filename += str_idx;
			//utf8_filename += str_time;
			utf8_filename += str_bmp;

			//utf8_filename.replace(utf8_filename.rfind(" 2016\n"), 6, ".bmp");
			//utf8_filename.replace(utf8_filename.rfind(":"), 1, "");
			//utf8_filename.replace(utf8_filename.rfind(":"), 1, "");
			std::wstring unicode_filename;
			unicode_filename = StringConverter::utf8_to_unicode(utf8_filename);

			if ((fp = _wfopen(unicode_filename.c_str(), L"wb")) == NULL) {
				return filepath;
			}

			BITMAPINFOHEADER bih = { 0 };                                              // 位图信息头
			bih.biBitCount = 24;                                        // 每个像素字节大小
			bih.biCompression = BI_RGB;
			bih.biHeight = nHeight;                                             // 高度
			bih.biPlanes = 1;
			bih.biSize = sizeof(BITMAPINFOHEADER);
			bih.biSizeImage = nWidth*nHeight * 3;                       // 图像数据大小
			bih.biWidth = nWidth;                                               // 宽度
			BITMAPFILEHEADER bfh = { 0 };                                              // 位图文件头
			bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);     // 到位图数据的偏移量
			bfh.bfSize = bfh.bfOffBits + nWidth*nHeight * 3;            // 文件总的大小
			bfh.bfType = (WORD)0x4d42;
			fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp);                           //写入位图文件头
			fwrite(&bih, 1, sizeof(BITMAPINFOHEADER), fp);                           //写入位图信息头     
			fwrite(pBits, 1, nWidth*nHeight * 3, fp);                       //写入位图数据
			fclose(fp);
			return filepath;
		}

		std::string SavePicture::SaveARGBAsBmp_ex(unsigned char* pBits, std::string filepath, int nWidth, int nHeight, char* index)
		{
			_mkdir(".\\picture");
			time_t timep;
			char s[30];
			time(&timep);
			strcpy(s, ctime(&timep));
			std::string  str_time(s);
			std::string  str_idx(index);
			std::string  str_bmp(".bmp");

			FILE *fp = NULL;
			std::string utf8_filename(filepath);
			utf8_filename += str_idx;
			//utf8_filename += str_time;
			utf8_filename += str_bmp;

			//utf8_filename.replace(utf8_filename.rfind(" 2016\n"), 6, ".bmp");
			//utf8_filename.replace(utf8_filename.rfind(":"), 1, "");
			//utf8_filename.replace(utf8_filename.rfind(":"), 1, "");
			std::wstring unicode_filename;
			unicode_filename = StringConverter::utf8_to_unicode(utf8_filename);

			if ((fp = _wfopen(unicode_filename.c_str(), L"wb")) == NULL) {
				return filepath;
			}

			BITMAPINFOHEADER bih = { 0 };                                              // 位图信息头
			bih.biBitCount = 32;                                        // 每个像素字节大小
			bih.biCompression = BI_RGB;
			bih.biHeight = nHeight;                                             // 高度
			bih.biPlanes = 1;
			bih.biSize = sizeof(BITMAPINFOHEADER);
			bih.biSizeImage = nWidth*nHeight * 4;                       // 图像数据大小
			bih.biWidth = nWidth;                                               // 宽度
			BITMAPFILEHEADER bfh = { 0 };                                              // 位图文件头
			bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);     // 到位图数据的偏移量
			bfh.bfSize = bfh.bfOffBits + nWidth*nHeight * 4;            // 文件总的大小
			bfh.bfType = (WORD)0x4d42;
			fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp);                           //写入位图文件头
			fwrite(&bih, 1, sizeof(BITMAPINFOHEADER), fp);                           //写入位图信息头     
			fwrite(pBits, 1, nWidth*nHeight * 4, fp);                       //写入位图数据
			fclose(fp);
			return filepath;
		}
		//int SavePicture::SaveRGBAsJpg(char * filename, unsigned char* pBits, int quality, int nWidth, int nHeight)
		//{
		//	_mkdir("D:\\picture");

		//	FILE * outfile;    // target file 
		//	struct jpeg_compress_struct jcs;
		//	 声明错误处理器，并赋值给jcs.err域
		//	struct jpeg_error_mgr jem;
		//	jcs.err = jpeg_std_error(&jem);
		//	jpeg_create_compress(&jcs);

		//	std::string utf8_filename(filename);
		//	std::wstring unicode_filename;
		//	unicode_filename = StringConverter::utf8_to_unicode(utf8_filename);

		//	if ((outfile = _wfopen(unicode_filename.c_str(), L"wb")) == NULL) {
		//		return 1;
		//	}

		//	jpeg_stdio_dest(&jcs, outfile);

		//	jcs.image_width = nWidth;    // 为图的宽和高，单位为像素 
		//	jcs.image_height = nHeight;
		//	jcs.input_components = 3;   // 在此为1,表示灰度图， 如果是彩色位图，则为3 
		//	jcs.in_color_space = JCS_EXT_BGR; //JCS_GRAYSCALE表示灰度图，JCS_RGB表示彩色图像
		//	jpeg_set_defaults(&jcs);
		//	jpeg_set_quality(&jcs, quality, true);

		//	jpeg_start_compress(&jcs, TRUE);
		//	JSAMPROW row_pointer[1];   // 一行位图
		//	int row_stride;      // 每一行的字节数
		//	row_stride = jcs.image_width * 3; // 如果不是索引图,此处需要乘以3
		//	 对每一行进行压缩
		//	int nh = nHeight - 1;
		//	while (jcs.next_scanline < jcs.image_height) {
		//		row_pointer[0] = &pBits[(jcs.next_scanline) * row_stride];
		//		jpeg_write_scanlines(&jcs, row_pointer, 1);

		//	}
		//	jpeg_finish_compress(&jcs);
		//	jpeg_destroy_compress(&jcs);

		//	if (outfile)
		//	{
		//		fclose(outfile);
		//		outfile = NULL;
		//	}

		//	return 0;
		//}

		//int SavePicture::SaveRGBAsJpg_ex(char * filename, unsigned char* pBits, int quality, int nWidth, int nHeight, char* index)
		//{
		//	_mkdir("D:\\picture");

		//	FILE * outfile;    // target file 
		//	struct jpeg_compress_struct jcs;
		//	 声明错误处理器，并赋值给jcs.err域
		//	struct jpeg_error_mgr jem;
		//	jcs.err = jpeg_std_error(&jem);
		//	jpeg_create_compress(&jcs);

		//	std::string  str_idx(index);
		//	std::string  str_jpg(".jpg");

		//	std::string utf8_filename(filename);

		//	utf8_filename += str_idx;
		//	utf8_filename += str_jpg;

		//	std::wstring unicode_filename;
		//	unicode_filename = StringConverter::utf8_to_unicode(utf8_filename);

		//	if ((outfile = _wfopen(unicode_filename.c_str(), L"wb")) == NULL) {
		//		return 1;
		//	}

		//	jpeg_stdio_dest(&jcs, outfile);

		//	jcs.image_width = nWidth;    // 为图的宽和高，单位为像素 
		//	jcs.image_height = nHeight;
		//	jcs.input_components = 3;   // 在此为1,表示灰度图， 如果是彩色位图，则为3 
		//	jcs.in_color_space = JCS_EXT_BGR; //JCS_GRAYSCALE表示灰度图，JCS_RGB表示彩色图像
		//	jpeg_set_defaults(&jcs);
		//	jpeg_set_quality(&jcs, quality, true);

		//	jpeg_start_compress(&jcs, TRUE);
		//	JSAMPROW row_pointer[1];   // 一行位图
		//	int row_stride;      // 每一行的字节数
		//	row_stride = jcs.image_width * 3; // 如果不是索引图,此处需要乘以3
		//	 对每一行进行压缩
		//	int nh = nHeight - 1;
		//	while (jcs.next_scanline < jcs.image_height) {
		//		row_pointer[0] = &pBits[(jcs.next_scanline) * row_stride];
		//		jpeg_write_scanlines(&jcs, row_pointer, 1);

		//	}
		//	jpeg_finish_compress(&jcs);
		//	jpeg_destroy_compress(&jcs);

		//	if (outfile)
		//	{
		//		fclose(outfile);
		//		outfile = NULL;
		//	}

		//	return 0;
		//}

		//int SavePicture::SaveYUVAsJpg_ex(char * filename, unsigned char* pBits, int quality, int nWidth, int nHeight, char* index)
		//{
		//	_mkdir("D:\\picture");

		//	FILE * outfile;    // target file 
		//	struct jpeg_compress_struct jcs;
		//	 声明错误处理器，并赋值给jcs.err域
		//	struct jpeg_error_mgr jem;
		//	jcs.err = jpeg_std_error(&jem);
		//	jpeg_create_compress(&jcs);

		//	std::string  str_idx(index);
		//	std::string  str_jpg(".jpg");

		//	std::string utf8_filename(filename);

		//	utf8_filename += str_idx;
		//	utf8_filename += str_jpg;

		//	std::wstring unicode_filename;
		//	unicode_filename = StringConverter::utf8_to_unicode(utf8_filename);

		//	if ((outfile = _wfopen(unicode_filename.c_str(), L"wb")) == NULL) {
		//		return 1;
		//	}

		//	jpeg_stdio_dest(&jcs, outfile);

		//	jcs.image_width = nWidth;
		//	jcs.image_height = nHeight;
		//	jcs.input_components = 3;    // # of color components per pixel    
		//	jcs.in_color_space = JCS_YCbCr;  //colorspace of input image    
		//	jpeg_set_defaults(&jcs);
		//	jpeg_set_quality(&jcs, quality, TRUE);

		//	////////////////////////////    
		//	  cinfo.raw_data_in = TRUE;    
		//	jcs.jpeg_color_space = JCS_YCbCr;
		//	jcs.comp_info[0].h_samp_factor = 2;
		//	jcs.comp_info[0].v_samp_factor = 2;
		//	///////////////////////    

		//	jpeg_start_compress(&jcs, TRUE);

		//	JSAMPROW row_pointer[1];

		//	 获取y、u、v三个分量各自数据的指针地址  
		//	unsigned char *ybase, *ubase, *vbase;
		//	ybase = pBits;
		//	ubase = pBits + nWidth*nHeight;
		//	vbase = ubase + nHeight*nWidth / 4;

		//	unsigned char *yuvLine = new unsigned char[nWidth * 3];
		//	memset(yuvLine, 0, nWidth * 3);

		//	int j = 0;
		//	while (jcs.next_scanline < jcs.image_height)
		//	{
		//		int idx = 0;
		//		for (int i = 0; i<nWidth; i++)
		//		{
		//			 分别取y、u、v的数据  
		//			yuvLine[idx++] = ybase[i + j * nWidth];
		//			yuvLine[idx++] = ubase[(j >> 1) * nWidth / 2 + (i >> 1)];
		//			yuvLine[idx++] = vbase[(j >> 1) * nWidth / 2 + (i >> 1)];
		//		}
		//		row_pointer[0] = yuvLine;
		//		jpeg_write_scanlines(&jcs, row_pointer, 1);
		//		j++;
		//	}
		//	jpeg_finish_compress(&jcs);
		//	jpeg_destroy_compress(&jcs);
		//	fclose(outfile);

		//	delete[]yuvLine;
		//	return 0;
		//}

		int SavePicture::Bmp_Flip(std::string filepath, int nWidth, int nHeight, bool flip_y, bool flip_x)
		{
			FILE *fp = NULL;
			unsigned char* pBits = new unsigned char[nWidth*nHeight * 3 + 54];
			unsigned char* pTemp = new unsigned char[nWidth*nHeight * 3];
			std::wstring unicode_filename;
			unicode_filename = StringConverter::utf8_to_unicode(filepath);

			if ((fp = _wfopen(unicode_filename.c_str(), L"rb")) == NULL) {
				return 0;
			}

			fread(pBits, 1, nWidth*nHeight * 3 + 54, fp);

			fclose(fp);

			pBits = pBits + 54;

			if (flip_y)
			{
				for (int src_index = nHeight - 1,dst_index = 0; src_index >= 0; src_index--, dst_index++)
				{
					memcpy(pTemp + dst_index*nWidth * 3, pBits + src_index*nWidth * 3, nWidth*3);
				}
			}
			else
			{
				memcpy(pTemp, pBits, nWidth * nHeight * 3);
			}

			if (flip_x)
			{
				char tmp_r, tmp_g, tmp_b;

				for (int i = 0; i < nHeight; i++)
				{
					for (int j = 0; j < nWidth/2; j++)
					{
						tmp_r = pTemp[i*nWidth * 3 + j * 3];
						tmp_g = pTemp[i*nWidth * 3 + j * 3+1];
						tmp_b = pTemp[i*nWidth * 3 + j * 3+2];

						pTemp[i*nWidth * 3 + j * 3] = pTemp[i*nWidth * 3 + (nWidth-j-1) * 3];
						pTemp[i*nWidth * 3 + j * 3+1] = pTemp[i*nWidth * 3 + (nWidth - j - 1) * 3+1];
						pTemp[i*nWidth * 3 + j * 3+2] = pTemp[i*nWidth * 3 + (nWidth - j - 1) * 3+2];

						pTemp[i*nWidth * 3 + (nWidth - j - 1) * 3] = tmp_r;
						pTemp[i*nWidth * 3 + (nWidth - j - 1) * 3 + 1] = tmp_g;
						pTemp[i*nWidth * 3 + (nWidth - j - 1) * 3 + 2] = tmp_b;
					}
				}
			}

			memcpy(pBits, pTemp, nWidth*nHeight * 3);
			

			if ((fp = _wfopen(unicode_filename.c_str(), L"wb")) == NULL) {
				return 0;
			}

			BITMAPINFOHEADER bih = { 0 };                                              // 位图信息头
			bih.biBitCount = 24;                                        // 每个像素字节大小
			bih.biCompression = BI_RGB;
			bih.biHeight = nHeight;                                             // 高度
			bih.biPlanes = 1;
			bih.biSize = sizeof(BITMAPINFOHEADER);
			bih.biSizeImage = nWidth*nHeight * 3;                       // 图像数据大小
			bih.biWidth = nWidth;                                               // 宽度
			BITMAPFILEHEADER bfh = { 0 };                                              // 位图文件头
			bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);     // 到位图数据的偏移量
			bfh.bfSize = bfh.bfOffBits + nWidth*nHeight * 3;            // 文件总的大小
			bfh.bfType = (WORD)0x4d42;
			fwrite(&bfh, 1, sizeof(BITMAPFILEHEADER), fp);                           //写入位图文件头
			fwrite(&bih, 1, sizeof(BITMAPINFOHEADER), fp);                           //写入位图信息头     
			fwrite(pBits, 1, nWidth*nHeight * 3, fp);                       //写入位图数据

			fclose(fp);

			pBits -= 54;
			delete pBits;
			delete pTemp;
		}