#include <sys/types.h>
#include <stdio.h>
#include <math.h>

#define WIDTH  1000
#define HEIGHT 1000

#define MAXX 1000
#define MAXY 1000

#define DOTSIZE 11
#define MINV 6200
#define MAXV 7200

#define ROWOFF 2.5

//#define DO_GRAYSCALE

unsigned long HSVtoHEX( float hue, float sat, float value );

double powers[WIDTH*HEIGHT];
double hits[WIDTH*HEIGHT];

float rowoffset;

void DrawAt( float x, float y, float pows )
{
	float relx = x / MAXX * WIDTH;
	float rely = y / MAXY * WIDTH ;

	float i, j;
	for( i = -DOTSIZE; i <= DOTSIZE; i++ )
	{
		for( j = -DOTSIZE; j <= DOTSIZE; j++ )
		{
			float sq =  (i/DOTSIZE ) * (i/DOTSIZE );
			sq += (j/DOTSIZE) * (j/DOTSIZE);
			if( sq >= 1 ) continue;
			int tx = relx + i+ rowoffset;
			int ty = rely + j;
			if( tx < 0 || tx >= WIDTH || ty < 0 || ty >= HEIGHT ) continue;

			sq = 1.0 - sq;
			powers[tx+ty*WIDTH] += pows * sq;
			  hits[tx+ty*WIDTH] += sq;
		}
	}
}

void RenderOut()
{
	int i, j;
	FILE * f = fopen( "picture.ppm", "wb" );
	fprintf( f, "P6\n%d %d\n%d\n", WIDTH, HEIGHT, 255 );
	for( i = 0; i < WIDTH; i++ )
	for( j = 0; j < HEIGHT; j++ )
	{
		float cal;
		float sat = 0;
		if( hits[i+j*WIDTH] > 0 )
		{
			cal = powers[i+j*WIDTH] / hits[i+j*WIDTH];
			sat = 1;
		}
		else
		{
			sat = 0;
			cal = 0;
		}
		float hue = 1.0*(cal - MINV)/(MAXV-MINV);
//		if( hue < 0 ) hue = 0;
//		if( hue > .7 ) hue = .7;

#ifdef DO_GRAYSCALE
		unsigned long data = HSVtoHEX( 0, sat, hue );
#else
		unsigned long data = HSVtoHEX( hue, sat, sat );
#endif
		fprintf(f, "%c%c%c", data,(data>>8)&0xff, (data>>16)&0xff );
	}
	fclose( f );
}




int main()
{
	FILE * f = fopen( "milllog.txt", "r" );
	if( !f )
	{
		fprintf( stderr, "Error cannot open milllog.txt\n" );
		return -1;
	}

	memset( powers, 0, sizeof( powers ) );
	memset( hits, 0, sizeof( hits ) );

	while( !feof( f ) && !ferror( f ) )
	{
		float value, x, y, z;
		int r = fscanf( f, "%f %f %f %f\n", &value, &x, &y, &z );

		rowoffset = fmod( y, 20 );

		if( rowoffset > 9 && rowoffset < 11 )
			rowoffset = ROWOFF;
		else
			rowoffset = -ROWOFF;

		DrawAt(x,y, value);
	}
//		DrawAt(100,100,6000);
	RenderOut();

	return 0;
}







unsigned long HSVtoHEX( float hue, float sat, float value )
{

	float pr = 0;
	float pg = 0;
	float pb = 0;

	short ora = 0;
	short og = 0;
	short ob = 0;

	float ro = fmod( hue * 6, 6. );

	float avg = 0;

	ro = fmod( ro + 6 + 1, 6 ); //Hue was 60* off...

	if( ro < 1 ) //yellow->red
	{
		pr = 1;
		pg = 1. - ro;
	} else if( ro < 2 )
	{
		pr = 1;
		pb = ro - 1.;
	} else if( ro < 3 )
	{
		pr = 3. - ro;
		pb = 1;
	} else if( ro < 4 )
	{
		pb = 1;
		pg = ro - 3;
	} else if( ro < 5 )
	{
		pb = 5 - ro;
		pg = 1;
	} else
	{
		pg = 1;
		pr = ro - 5;
	}

	//Actually, above math is backwards, oops!
	pr *= value;
	pg *= value;
	pb *= value;

	avg += pr;
	avg += pg;
	avg += pb;

	pr = pr * sat + avg * (1.-sat);
	pg = pg * sat + avg * (1.-sat);
	pb = pb * sat + avg * (1.-sat);

	ora = pr * 255;
	og = pb * 255;
	ob = pg * 255;

	if( ora < 0 ) ora = 0;
	if( ora > 255 ) ora = 255;
	if( og < 0 ) og = 0;
	if( og > 255 ) og = 255;
	if( ob < 0 ) ob = 0;
	if( ob > 255 ) ob = 255;

	return (ob<<16) | (og<<8) | ora;
}


