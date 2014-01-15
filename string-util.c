#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <stdarg.h>
#include "usartIO.h"

#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1/UCHAR_MAX)                                                                      
#define HIGHS (ONES * (UCHAR_MAX/2+1))
#define HASZERO(x) ((x)-ONES & ~(x) & HIGHS)

#define PRINT_SIZE 50

#define SS (sizeof(size_t))

void *memset(void *dest, int c, size_t n)
{
	unsigned char *s = dest;
	c = (unsigned char)c;
	for (; ((uintptr_t)s & ALIGN) && n; n--) *s++ = c;
	if (n) {
		size_t *w, k = ONES * c;
		for (w = (void *)s; n>=SS; n-=SS, w++) *w = k;
		for (s = (void *)w; n; n--, s++) *s = c;
	}
	return dest;
}

void *memcpy(void *dest, const void *src, size_t n)
{
	void *ret = dest;
	
	//Cut rear
	uint8_t *dst8 = dest;
	const uint8_t *src8 = src;
	switch (n % 4) {
		case 3 : *dst8++ = *src8++;
		case 2 : *dst8++ = *src8++;
		case 1 : *dst8++ = *src8++;
		case 0 : ;
	}
	
	//stm32 data bus width
	uint32_t *dst32 = (void *)dst8;
	const uint32_t *src32 = (void *)src8;
	n = n / 4;
	while (n--) {
		*dst32++ = *src32++;
	}
	
	return ret;
}

char *strchr(const char *s, int c)
{
	for (; *s && *s != c; s++);
	return (*s == c) ? (char *)s : NULL;
}

char *strcpy(char *dest, const char *src)
{
	const unsigned char *s = (unsigned char *)src;
	unsigned char *d = (unsigned char *)dest;
	while ((*d++ = *s++));
	return dest;
}

char *strncpy(char *dest, const char *src, size_t n)
{
	const unsigned char *s = (unsigned char *)src;
	unsigned char *d = (unsigned char *)dest;
	while (n-- && (*d++ = *s++));
	return dest;
}

int strlen(char *string){
	int len = 0;
	char *current = string;
	while(current[len] != '\0'){
		len++;
	}
	return len;
}

int strcmp(char *str1, char *str2){
	char *cur1 = str1;
	char *cur2 = str2;
	int i = 0;
	while(cur1[i] == cur2[i]){
		if(cur1[i] == '\0'){
			return 0;
		}
		i++;
	}
	return -1;
}

int strcat(char *str1, char *str2){
	int i;
	int str1_end = strlen(str1);
	for(i = 0; i < strlen(str2); i++){
		str1[str1_end++] = str2[i];
	}
	str1[str1_end] = '\0';
	return 1;
}

int printf(const char *format, ...);

char *strtok(char *split_str, const char delim){
	static char *current_ptr;
	static char end_flag;
	char *current_token;
	if(split_str == NULL){
		if(end_flag == 1)return NULL;
		current_ptr++;
	}else{
		current_ptr = split_str;
		end_flag = 0;
	}
	current_token = current_ptr;
	for(;;current_ptr++){
		if( current_ptr[0] == delim){
			current_ptr[0] = '\0';
			break;
		}else if(current_ptr[0] == '\0'){
			end_flag = 1;
			break;
		}
	}
	return current_token;
}

void print(char *print_str){
	send_str(print_str);
	/*
	fio_write(1, print_str, strlen(print_str));
	*/
	if(print_str[strlen(print_str)-1] == '\n')
	{
		//fio_write(1, "\r", 1);
		send_str("\r");
	}
}

int num_to_string(unsigned int num, char*num_str, int base, int sign_or_not){
	int digit = 0;
	int digit_end;
	unsigned int tmp_num = num;
	int minus_or_not = 0;
	int i;
	if(tmp_num == 0){
		num_str[0] = '0';
		num_str[1] = '\0';
		return 1;
	}
	if((int)tmp_num < 0){
		num = (int)-num;
		tmp_num = num;
		minus_or_not = 1;
	}
	while(tmp_num != 0){
		digit++;
		tmp_num /= base;
	}
	tmp_num = num;
	if(base == 16){
		num_str[0] = '0';
		num_str[1] = 'x';
		digit_end = digit + 2;
		num_str[digit_end] = '\0';
	}else if(minus_or_not == 1){
		num_str[0] = '-';
		digit_end = digit + 1;
		num_str[digit_end] = '\0';
	}else{
		digit_end = digit;
		num_str[digit_end] = '\0';
	}
	for(i = 1; i <= digit; i++, tmp_num/=base){
		num_str[digit_end-i] = "0123456789abcdef"[tmp_num % base];
	}
	return 1;
}

int float_to_string(double num, char*num_str, int base, int sign_or_not){
	int digit = 0;
	int digit_end;
	int tmp_num = (int)(num*10000);
	int minus_or_not = 0;
	int i;
	if(tmp_num == 0){
		num_str[0] = '0';
		num_str[1] = '\0';
		return 1;
	}
	if((int)tmp_num < 0){
		num = (int)-num;
		tmp_num = num;
		minus_or_not = 1;
	}
	while(tmp_num != 0){
		digit++;
		tmp_num /= base;
	}
	tmp_num = (int)(num*10000);

	if(digit < 5)digit = 5;

	int runner = 0;
	for ( runner = 0 ; runner <= digit ; runner++)
	{
		num_str[runner]='0';
	}//ensure digit is greater than 5; and filled with 0

	if(minus_or_not == 1){
		num_str[0] = '-';
		digit_end = digit + 2;
		num_str[digit_end] = '\0';
	}else{
		digit_end = digit+1;
		num_str[digit_end] = '\0';
	}
	for(i = 1; i <= digit; i++, tmp_num/=base){
/*		if(i == 5)
		{
			num_str[i] = '.';
			i+=1;
		}*/
		num_str[digit_end-i] = "0123456789abcdef"[tmp_num % base];
		
	}

//	int digit_fraction = (digit_end>5)? 6:digit_end - 2;

	for( i = 0;i <= digit_end - 6;i++ )
	{
		num_str[i] = num_str[i+1];
	}
	num_str[digit_end - 5] = '.';
	return 1;
}

static int common_printf(char *dest, const char *format, va_list param){
	char *pstr = dest;
	while(*format != '\0'){
		if(*format == '%'){
			format++;
			switch(*format){
				case 'C':
				case 'c':
					*pstr++ = (char)va_arg(param, int);
					break;
				case 'D':
				case 'd':
					num_to_string(va_arg(param, int), pstr, 10, 1);
					pstr += strlen(pstr);
					break;
				case 'S':
				case 's':
					strcpy(pstr, va_arg(param, char*));
					pstr += strlen(pstr);
					break;
				case 'f':
				case 'F':
					float_to_string(va_arg(param, double), pstr, 10, 1);
					pstr += strlen(pstr);
					break;
				case 'X':
				case 'x':
					num_to_string(va_arg(param, int), pstr, 16, 1);
					pstr += strlen(pstr);
					break;
				case 'u':
					num_to_string(va_arg(param, unsigned), pstr, 10, 0);
					pstr += strlen(pstr);
					break;
				default:
					*pstr++ = *format;
			}
		}else{
			*pstr++ = *format;
		}
		format++;
	}
	*pstr = '\0';
	return pstr - dest;
}

int printf(const char *format, ...){
	va_list arg_list;
	int print_len;
	char dest[PRINT_SIZE];
	va_start(arg_list, format);
	print_len = common_printf(dest, format, arg_list);
	va_end(arg_list);
	print(dest);
	return print_len;
}

int sprintf(char *dest, const char *format, ...){
	va_list arg_list;
	int print_len;
	va_start(arg_list, format);
	print_len = common_printf(dest, format, arg_list);
	va_end(arg_list);
	return print_len;
}
