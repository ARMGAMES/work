
#include "constants.h"
#include "bitalgorithm.h"

namespace BitAlgorithm
{
	// move original data
	int CountBit1(char data)
	{
		int count = 0;
		int len = 8 * sizeof(char);

		for (int i = 0; i < len; i++)
		{
			if (data & 1 == 1)
			{
				count ++;
			}
			data = data >> 1;
		}
		return count;
	}

	// left shift bit 1 to compare with original data
	int CountBit1_v2(char data)
	{
		int count = 0;
		int len = 8 * sizeof(char);

		for (int i = 0; i < len; i++)
		{
			char bit_mask = 1 << i;
			if(( data & bit_mask) == bit_mask)
			{
				count ++;
			}
		}
		return count;
	}


	int CountBit1_arr(char data[], int size)
	{
		int count = 0;

		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				char bit_mask = 1 << j;
				if(( *(data+i) & bit_mask) == bit_mask)
				{
					count ++;
				}
			}
		}
		return count;
	}

	int bitsinint(int x)
	{
		x = x - ((x >> 1) & 0x55555555);
		x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
		x = x + (x >> 8);
		x = x + (x >> 16);
		return x & 0x0000003F;
	}

}