#include<iostream>
#include<iomanip>
#include"Jpeg.h"
#include"BitStream.h"

int main()
{

	std::vector<unsigned char> vec = { 1, 2, 3 };
	
	InputBitStream bs(vec);

	int q;
	while (bs >> q)
	{
		std::cout << q << " ";
	}


}