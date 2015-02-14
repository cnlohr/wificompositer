#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

int line = 0;


#define RESCALEX 5.6228
#define RESCALEY 5.6228
#define RESCALEZ 5.6228

#define XCT 64
#define YCT 64
#define ZCT 32

#define PLS 4
#define MRANGE 1.5

float * Powers;
float * Counts;
float * OutPowers;

void Raadd( int x, int y, int z, float power, float stren )
{
	Powers[x+y*XCT+z*XCT*YCT] += power*stren;
	Counts[x+y*XCT+z*XCT*YCT] += stren;
}

//X, Y, Z in terms
void Raaplot( float ix, float iy, float iz, float power )
{
	int x, y, z;
	for( x = (int)ix-PLS; x < (int)ix+PLS+1; x++ )
	for( y = (int)iy-PLS; y < (int)iy+PLS+1; y++ )
	for( z = (int)iz-PLS; z < (int)iz+PLS+1; z++ )
	{
		if( x < 0 || y < 0 || z < 0 || x >= XCT || y >= YCT || z >= ZCT ) continue;
		float dist = (x-ix)*(x-ix)+(y-iy)*(y-iy)+(z-iz)*(z-iz);
		float pows = MRANGE - sqrtf( dist );
		if( pows < 0 ) continue;
		Raadd( x, y, z, power, pows );
	}
}

int main()
{
	int x, y, z;
	Powers = malloc( sizeof( float ) * XCT * YCT * ZCT );
	OutPowers = malloc( sizeof( float ) * XCT * YCT * ZCT );
	Counts = malloc( sizeof( float ) * XCT * YCT * ZCT );

	int oldz = -18000;
	float lastpower = 0;
	FILE * f = fopen( "milllog.txt.3d", "r" );
	if( !f )
	{
		fprintf( stderr, "Error: Cannot open mill log data file.\n" );
		return 0;
	}

	while( !feof( f ) )
	{
		float power, x, y, z;
		line++;
		if( fscanf( f, "%f %f %f %f\n", &power, &x, &y, &z ) != 4 )
		{
			fprintf( stderr, "Fault reading file.  Line %d\n", line );
			continue;
		}

		int irz = (-z / RESCALEZ + 0.5);
		if( irz != oldz )
		{
//			printf( "%d %f %d\n", irz, z, line );
		}
		if( irz < oldz )
		{
			fprintf( stderr, "Error: Z not monotonically increasing (%d)\n", line );
		}
		oldz = irz;

		float powerdiff = lastpower - power;

		if( ( powerdiff > 1500.0 || powerdiff < -1500.0 ) && lastpower != 0 )
		{
			fprintf( stderr, "Power glitch LINE: %d / %f %f %f\n", line, lastpower, power, powerdiff );
			continue;
		}
		lastpower = power;

		Raaplot( x/RESCALEX, y/RESCALEY, -z/RESCALEZ, power );
	}
	fclose( f );

	float minpower = 100000;
	float maxpower = -100000;
	for( z = 0; z < ZCT; z++ )
	for( y = 0; y < YCT; y++ )
	for( x = 0; x < XCT; x++ )
	{
		float pwr = Powers[x+y*XCT+z*XCT*YCT];
		float cnt = Counts[x+y*XCT+z*XCT*YCT];		
		float out = 0;
		if( cnt > 0.01 ) out = pwr/cnt;
		OutPowers[x+y*XCT+z*XCT*YCT] = out;
//		printf( "(%d %d %d) %f %f %f\n", x, y, z, pwr, cnt, out );
		if( out < minpower ) minpower = out;
		if( out > maxpower ) maxpower = out;
	}

	printf( "Read min power: %f, Max: %f\n", minpower, maxpower );
	minpower = 6350;
	maxpower = 6780;
	printf( "Set  min power: %f, Max: %f\n", minpower, maxpower );



	FILE * o = fopen( "rdensities.dat", "wb" );
	if (!o )
	{
		fprintf( stderr, "Error: No o file.\n" );
		return -1;
	}
	for( z = 0; z < ZCT; z++ )
	{
	char scr[100];
	sprintf(scr, "slices/%05d.ppm", z );
	FILE * k = fopen( scr, "wb" );
	fprintf( k, "P5\n%d %d\n%d\n", XCT, YCT, 255 );
	for( y = 0; y < YCT; y++ )
	for( x = 0; x < XCT; x++ )
	{
		float op = OutPowers[x+y*XCT+z*XCT*YCT];
		float ot = (op-minpower)/(maxpower-minpower);
		if( ot > 1.0 ) ot = 1.0;
		if( ot < 0 ) ot = 0;
		uint8_t oo = ot * 255;
		fprintf( o, "%c", oo );
		fprintf( k, "%c", oo );
	}
	fclose( k );
	}
	fclose( o );
	return 0;
}

