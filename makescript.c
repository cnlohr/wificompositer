#include <stdio.h>

#define X_SIZE 1000
#define Y_SIZE 1000
#define X_STEP 10
#define Y_STEP 10

int main()
{
	int i, j, k;
	printf( "layer zigzag\n" );
	for( i = 0; i < Y_SIZE; i+=Y_STEP )
	{
		printf( "section %d\n", i );
		if( !((i/Y_STEP) & 1) )
		{
			printf( "goto3 %d %d 0\n", 0, i );
			printf( "goto3 %d %d 0\n", X_SIZE, i );
		}
		else
		{
			printf( "goto3 %d %d 0\n", X_SIZE, i );
			printf( "goto3 %d %d 0\n", 0, i );
		}
	}
	return 0;
}

