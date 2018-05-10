#pragma once
#include"BitStream.h"
#include"ImageFileBuffer.h"
#include"Image.h"

class Bmp : public Image
{
	InputBitStream _image_content;




public:
	Bmp(const std::string& file_path_)
		: _image_content(ImageFileBuffer(file_path_).Get())
	{
		
	}
};