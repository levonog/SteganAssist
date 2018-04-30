#include<iostream>
#include"Jpeg.h"
#include"BitStream.h"

int main()
{
	std::vector<unsigned char> vec = { 255, 2, 3 };
	
	InputBitStream bs(vec);

	int a;
	while (bs >> a)
	{
		std::cout << a;
	}


}