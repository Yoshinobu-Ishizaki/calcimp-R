/*
 $Id: radimp.c 3 2013-08-03 07:37:16Z yoshinobu $
*/

#include <stdio.h>
#include "cephes.h"
#include <complex.h>
#include <math.h>
#include "kutils.h"

#include "zmensur.h"

double temperature=24.0;
extern double c0,rho,rhoc0;

#if 0
/*
 * 放射インピーダンスを計算する 
 * 無限バッフル中の円盤による放射としての計算を行っている
 * 城戸健一編著 日本音響学会編纂 基礎音響工学の90p
 * k : 波数
 * d : 直径
 * zr : 放射インピーダンス = p/u, pは音圧,uは体積速度
 */
void rad_imp( double frq,double d,_Complex double * zr )
{
    double a,x;
    double re,im;
    double k;

    k = PI2*frq/c0;

    if( d <= 0.0 ) return; /* エラー処理がない! */

    a = 0.5*d;
    x = k * d;
#if 0 
    printf( "k=%f,f=%f,2ka=%f\n",k,f,x);
#endif
    
    /* j1は1次のベッセル関数。struveはストルーブ関数 */
    re = rhoc0 * ( 1 - j1(x)/(k*a) );
    im = rhoc0 * struve( 1,x ) / (k*a);
#if 0    
    printf( "radiation impedance is %f,%f at freq %f\n",c->r,c->i,\
	    PI2 / k );
#endif
    *zr = im*I + re;
}
#endif

int main()
{
    double f;
    double complex z;

    c0 = 331.45 * sqrt(temperature / 273.16 + 1);
    rho = 1.2929 * ( 273.16 / (273.16 + temperature) );
    rhoc0 = rho * c0;

    for( f=0; f <= 2000; f += 2.5){
	rad_imp(f,0.25,&z);
	printf("%f,%f,%f\n",f,creal(z),cimag(z));
    }
}
