/*
	Copyright (c) 1995,1996 by 京都マイクロコンピュータ株式会社
	All Rights Reserved.

	Modified by Yasunori Kobayashi.
*/

#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>
#include <explog.h>
#include "io_file.h"

/* float,double,long long のサポート時 1 , サポートしない時 0 */
#define	FLOAT_LLONG_ON	1

/* printf flags */
#define MINUS			1		/* - */
#define PLUS			2		/* + */
#define ZERO			4		/* 0 */
#define SPACE			8		/*   */

/* printf type size */
#define LONG_TYPE		0x20
#define SHORT_TYPE		0x40
#define LDOUBLE_TYPE	0x80

#define SIGNE_M			0x100	/* minus value */

static char basc[]="0123456789abcdefx";
static char BASC[]="0123456789ABCDEFX";

static char *cvt_radix(char *asc_buf,unsigned val,int radix,char *binasc)
{
	char *p;

	p=asc_buf+ABUFSIZE;
	do{
		*(--p)=binasc[val % radix];
		val /= radix;
	}while(val);
	return p;
}

#if FLOAT_LLONG_ON /* { */
static char *cvtl_radix(char *asc_buf,unsigned long long val,int radix,char *binasc)
{
	char *p;
	volatile int index;

	p=asc_buf+ABUFSIZE;
	do{
		index=val % radix;
		*(--p)=binasc[index];
		val /= radix;
	}while(val);
	return p;
}
#endif /* } */

static int round_asc(char *f_asc,int exp,int n)
{
	char *p,*pbak;

	p=f_asc+n;
	if(*p<'5'){
		*p='\0';
		return exp;
	}
	*(pbak=p)='\0';
	for(;;){
		--n;
		if(n<0) break;
		--p;
		if(*p!='9'){
		   ++(*p);
		   return exp;
		}
		*p='0';
	}
	*p='1';
	*pbak='0';
	*(pbak+1)='\0';
	return exp+1;
}

static void eprt_sub(char *s,int ndig,int exp,char *d,int letter_e,int sharp_flg)
{
	*(d++)=*(s++);
	*(d++)='.';
	memcpy(d,s,(ndig-1));
	d += (ndig-1);
	if(sharp_flg==0){
		while(*(d-1)=='0'){
			--d;
		}
		if(*(d-1)=='.') --d;
	}
	*(d++)=letter_e;
	if(exp<0) {
		exp = -exp;
		*(d++)='-';
	}
	else{
		*(d++)='+';
	}
	if(exp>=100){
		*(d++)=exp/100+'0';
		exp %= 100;
	}
	*(d++)=exp/10+'0';
	exp %= 10;
	*(d++)=exp+'0';
	*d='\0';
}

#if FLOAT_LLONG_ON /* { */

static double _div_data[]={1e1,1e2,1e4,1e8,1e16,1e32,1e64,1e128,1e256};
static double _mul_data[]={1e-1,1e-2,1e-4,1e-8,1e-16,1e-32,1e-64,1e-128,1e-256};

static char fbuf[FBUF_SIZE];

static char *ecvt(double x,int ndig,int *dec,int *sign)
{
	int i,ndig1;
	int exp,exp_index;
	double ipart;
	volatile double x1;
	char *p;
	DFLOAT val;

	exp=0;
	exp_index=256;
	*sign=0;
	ndig1=ndig;
	if(ndig>VALID_CT){
		ndig=VALID_CT+1;
		ndig1=VALID_CT;
	}
	val.df=x;
	switch(val.ui[EXP_INDEX] & 0x7ff00000){
	case 0:
		p=fbuf;
		for(i=0;i<ndig;++i){
			*(p++)='0';
		}
		*dec=1;
		return fbuf;
	case 0x7ff00000:
		*dec=0x7fffffff;
		if((val.ui[EXP_INDEX] & 0xfffff)!=0 || val.ui[(EXP_INDEX+1) & 1]!=0){
			strcpy(fbuf,"NAN");
			*sign=0;
			return fbuf;
		}
		strcpy(fbuf,"INF");
		return fbuf;
	}
	if(x<0){
		x = -x;
		*sign=1;
	}
	if(x>=1.0){
		i=8;
		do{
			if(x>=_div_data[i]){
				x *= _mul_data[i];
				exp += exp_index;
			}
			exp_index >>= 1;
			--i;
		}while(i>=0) ;
	}
	else{
		i=8;
		do{
			if(x<_mul_data[i]){
				x *= _div_data[i];
				exp -= exp_index;
			}
			exp_index >>= 1;
			--i;
		}while(i>=0) ;
		x *= 10.0;
		--exp;
	}
	x1=x;
loop:
	if(x1>=10.0){
		x1 *= 0.1;
		++exp;
		goto loop;
	}
	else if(x1<1.0){
		x1 *= 10.0;
		--exp;
		goto loop;
	}
	p=fbuf;
	do{
		x1=modf(x1,&ipart)*10.0;
		*(p++)=(int)ipart+'0';
	}while(--ndig1>=0) ;
	if(ndig>VALID_CT){
		*dec=exp+1;
	}
	else{
		*dec=round_asc(fbuf,exp,ndig)+1;
	}
	return fbuf;
}

static char *fcvt(double x,int ndig,int *dec,int *sign)
{
	char *p;
	int exp,epos;

	p=ecvt(x,VALID_CT+1,dec,sign);
	if((exp=*dec)==0x7fffffff) return fbuf;
	epos=exp+ndig;
	if(epos>=0){
		if(epos<=VALID_CT){
			*dec=round_asc(fbuf,exp,epos);
		}
		else{
			if(epos>FBUF_SIZE) epos=FBUF_SIZE;
			p += VALID_CT+1;
			epos -= VALID_CT+1;
			while(epos>0){
				*(p++)='0';
				--epos;
			}
		}
	}
	return fbuf;
}

static char *gcvt(double x,int ndig,char *bufp,int letter_e,int sharp_flg)
{
	char *p,*q;
	int exp,sign;

	p=ecvt(x,ndig,&exp,&sign);
	q=bufp;
	if(sign){
		*(q++)='-';
	}
	if(exp==0x7fffffff){
		strcpy(q,p);
		return bufp;
	}
	if(exp<=0){
		if(exp>-4){
			*(q++)='0';
			*(q++)='.';
			while(exp){
				*(q++)='0';
				++exp;
			}
			memcpy(q,p,ndig);
			q += ndig;
del_zero:
			if(sharp_flg==0){
				while(*(q-1)=='0'){
					--q;
				}
				if(*(q-1)=='.') --q;
			}
			*q='\0';
			return bufp;
		}
	}
	else if(exp<ndig){
		while(ndig--){
			*(q++)=*(p++);
			if(--exp==0) *(q++)='.';
		}
		goto del_zero;
	}
	else if(exp<=(ndig+1)){
		memcpy(q,p,exp);
		*(q+ndig)='0';
		if(sharp_flg==0){
			*(q+exp)='\0';
		}
		else{
			*(q+exp)='.';
			*(q+exp+1)='\0';
		}
		return bufp;
	}
	eprt_sub(p,ndig,exp-1,q,letter_e,sharp_flg);
	return bufp;
}

#endif /* } */

int io_vprintf(IO_FILE *fp,const char *f,va_list argp)
{
	int flg;
	int width;
	int prec,fprec;
	int sign;
	int radix;
	char c;
	char *asc;
	int len;
	int heder_len;
#if FLOAT_LLONG_ON /* { */
	double dval;
#endif /* } */
	int float_flg;
	int n_ct;
	char *binasc;
	int letter_e;
	int sharp_flg;
	char asc_buf[ABUFSIZE];

	n_ct=0;
	letter_e='e';
	binasc=basc;
	sharp_flg=0;

	for(;;){
		if((c=*(f++))!='%'){
			if(c=='\0'){
				sharp_flg=0;
				return n_ct;
			}
			if(iskanji(c)){
				io_write(fp,&c,1);
				++n_ct;
				c=*(f++);
			}
			io_write(fp,&c,1);
			++n_ct;
		}
		else{
/* printf flag  */
			flg=0;
			sharp_flg=0;
flag_loop:
			switch(c=*(f++)){
			case '-':
				flg |= MINUS;
				goto flag_loop;
			case '+':
				flg |= PLUS;
				goto flag_loop;
			case '0':
				flg |= ZERO;
				goto flag_loop;
			case ' ':
				flg |= SPACE;
				goto flag_loop;
			case '#':
				sharp_flg=1;
				goto flag_loop;
			}

/* printf width */
			if(c=='*'){
				width=va_arg(argp, int);
				if(width<0){
					flg |= MINUS;
					width= -width;
				}
				c=*(f++);
			}
			else{
				width=0;
				while(isdigit(c)){
					width=width*10+(c-'0');
					c=*(f++);
				}
			}

/* precision */
			prec=0;
			fprec=6;
			if(c=='.'){
				if((c=*(f++))=='*'){
					prec=va_arg(argp, int);
					c=*(f++);
				}
				else{
					while(isdigit(c)){
						prec=prec*10+(c-'0');
						c=*(f++);
					}
				}
				fprec=prec;
			}
/* type */
			sign=float_flg=0;
			binasc=basc;	/* 0123456789abcdef */
			radix=10;
type_loop:
			switch(c){
			case 'i':
			case 'd':
				sign=1;
				break;
			case 'u':
				break;
			case 'o':
				radix=8;
				flg &= ~(PLUS | SPACE);
				break;
			case 'X':
				binasc=BASC;	/* 0123456789ABCDEF */
			case 'x':
				radix=16;
				flg &= ~(PLUS | SPACE);
				break;
			case 'c':
				*(asc=asc_buf)=(char)va_arg(argp,int);
				len=1;
				goto prt_all;
			case 's':
				asc=va_arg(argp,char *);
				if(asc==0){
					asc="(null)";
				}
				len=strlen(asc);
				if(prec!=0 && len>prec){
					len=prec;
				}
				goto prt_all;
#if FLOAT_LLONG_ON /* { */
			case 'f':
				{
					int fsign,exp,n;
					char *p,*q;

					dval=va_arg(argp,double);
					p=fcvt(dval,fprec,&exp,&fsign);
					if(fsign){
						flg |= SIGNE_M;
					}
					q=asc=asc_buf+1;
					if(exp==0x7fffffff){
						strcpy(q,p);
					}
					else{
						if(exp<=0){
							*(q++)='0';
							*(q++)='.';
							n=0;
							while(exp){
								*(q++)='0';
								++exp;
								if(++n==fprec) break;
							}
							while(n!=fprec){
								*(q++)=*(p++);
								++n;
							}
							*q='\0';
						}
						else if(exp+fprec<FBUF_SIZE){
							memcpy(q,p,exp);
							p += exp;
							q += exp;
							if(fprec){
								*(q++)='.';
								memcpy(q,p,fprec);
							}
							else if(sharp_flg){
								*(q++)='.';
							}
							*(q+fprec)='\0';
						}
						else if(exp+fprec<ABUFSIZE-1){
							memcpy(q,p,FBUF_SIZE);
							if(exp<=FBUF_SIZE){
								q += exp;
							}
							else{
								q += FBUF_SIZE;
								n=exp-FBUF_SIZE;
								do{
									*(q++)='0';
								}while(--n) ;
							}
							if(fprec){
								*(q++)='.';
								n=fprec;
								do{
									*(q++)='0';
								}while(--n) ;
							}
							else if(sharp_flg){
								*(q++)='.';
							}
							*q='\0';
						}
						else{
							fprec=VALID_CT+1;
							c='e';
							goto format_e;
						}
					}
					len=strlen(asc);
					prec=0;
					float_flg=1;
					goto prt_float;
				}
			case 'e':
			case 'E':
				{
					int fsign,exp,n;
					char *p,*q;

					dval=va_arg(argp,double);
format_e:
					p=ecvt(dval,fprec,&exp,&fsign);
					q=asc=asc_buf+1;
					if(fsign){
						flg |= SIGNE_M;
					}
					if(exp==0x7fffffff){
						strcpy(q,p);
					}
					else{
						letter_e=c;
						eprt_sub(p,fprec,exp-1,q,letter_e,sharp_flg);
						letter_e='e';
					}
					len=strlen(asc);
					prec=0;
					float_flg=1;
					goto prt_float;
				}
			case 'g':
			case 'G':
				{
					double dval;

					dval=va_arg(argp,double);
					if(dval<0){
						dval = -dval;
						flg |= SIGNE_M;
					}
					letter_e=c-2;
					asc=gcvt(dval,fprec,asc_buf+1,letter_e,sharp_flg);
					letter_e='e';
					len=strlen(asc);
					prec=0;
					float_flg=1;
					goto prt_float;
				}
#endif /* } */
			case 'n':
				{
					int *np;

					np=va_arg(argp,int *);
					*np=n_ct;
					goto next;
				}
			case 'p':
				flg=0;
				prec=8;
				binasc=BASC;	/* 0123456789ABCDEF */
				radix=16;
				break;
			case 'h':
				flg |= SHORT_TYPE;
				c=*(f++);
				goto type_loop;
			case 'l':
				if((c=*(f++))=='l'){
					flg |= LONG_TYPE;
					c=*(f++);
				}
				goto type_loop;
			case 'L':
				flg |= LDOUBLE_TYPE;
				c=*(f++);
				goto type_loop;
			default:
				io_write(fp,&c,1);
				++n_ct;
				goto next;
			}
			{
				if(prec!=0){
					 flg &= ~ZERO;
				}
				if(flg & LONG_TYPE){
#if FLOAT_LLONG_ON /* { */
					long long lval;

					lval=va_arg(argp,long long);
					if(sign){
						if(lval<0){
							lval = -lval;
							flg |= SIGNE_M;
						}
					}
					asc=cvtl_radix(asc_buf,lval,radix,binasc);
#endif /* } */
				}
				else{
					long val;

					val=va_arg(argp, int);
					if(flg & SHORT_TYPE){
						if(sign){
							val=(short int)val;
						}
						else{
							val=(unsigned short int)val;
						}
					}
					if(sign){
						if(val<0){
							val = -val;
							flg |= SIGNE_M;
						}
					}
					asc=cvt_radix(asc_buf,val,radix,binasc);
				}
				len=asc_buf-asc+ABUFSIZE;
prt_float:
				heder_len=0;
				if(sharp_flg && radix!=10 && sign==0 && *asc!='0'){	/* type==(#o,#x,#X) && val==0 */
					heder_len=(radix==16) ? 2:1;
				}
				if((flg & ZERO) && (flg & MINUS)==0){
					prec=width;
				}
				if(prec){
					if(flg & (PLUS | SPACE | SIGNE_M)){
						--prec;
						if(float_flg){
							char c;
							--width;
							++n_ct;
							if(flg & SIGNE_M){
								c = '-';
								io_write(fp,&c,1);
							}
							else{
								c = (flg & PLUS) ? '+' : ' ';
								io_write(fp,&c,1);
							}
							flg &= ~(PLUS | SPACE | SIGNE_M);
						}
					}
					prec -= heder_len;
					while(len<prec){
						if(float_flg){
							char c = '0';
							io_write(fp,&c,1);
							--width;
							++n_ct;
							--prec;
						}
						else{
						   *(--asc)='0';
						   ++len;
						}
					}
				}
				if(heder_len){
					if(heder_len==2){	/* type==(x,X) */
						*(--asc)=binasc[16];
					}
					*(--asc)='0';
					len += heder_len;
				}
				if(flg & (PLUS | SPACE | SIGNE_M)){
					++len;
					--asc;
					if(flg & SIGNE_M){
						*asc='-';
					}
					else{
						*asc=(flg & PLUS) ? '+':' ';
					}
				}
			}
prt_all:
			{
				if((flg & MINUS)==0){
					while(width>len){
						char c = ' ';
						io_write(fp,&c,1);
						--width;
						++n_ct;
					}
				}
				width -= len;
				n_ct += len;
				while(len--){
					io_write(fp,asc++,1);
				}
				while(width>0){
					char c = ' ';
					io_write(fp,&c,1);
					--width;
					++n_ct;
				}
			}
		}
next:
		;
	}
}

