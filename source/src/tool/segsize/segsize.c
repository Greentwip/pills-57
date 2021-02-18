#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

int pathGetSepaPos(const char *file, int flag)
{
	const char *p = file;

	while(*p && *p != ':' && *p != '/' && *p != '\\') {
		p += mblen(p, MB_CUR_MAX);
	}

	if(flag && *p) {
		p++;
	}

	return p - file;
}

int pathGetNamePos(const char *file)
{
	const char *p, *q;

	p = q = file;

	while(p += pathGetSepaPos(p, 1), *p) {
		q = p;
	}

	return q - file;
}

int pathGetExtPos(const char *file)
{
	const char *p, *q;

	p = q = file + pathGetNamePos(file);

	while(*p) {
		if(*p == '.') {
			q = p;
		}
		p += mblen(p, MB_CUR_MAX);
	}

	return (*q != '.' ? p : q) - file;
}

void pathGetName(const char *file, char *name)
{
	int i, j;
	i = pathGetNamePos(file);
	j = pathGetExtPos(file + i);
	strncpy(name, file + i, j - i);
	name[j - i] = '\0';
}

void StrToUpper(char *str)
{
	int len;

	while(*str) {
		len = mblen(str, MB_CUR_MAX);

		if(len == 1 && islower(*str)) {
			*str = toupper(*str);
		}

		str += len;
	}
}

int FileSize(const char *file)
{
	FILE *fp = fopen(file, "r");
	int size = 0;

	if(fp) {
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fclose(fp);
	}

	return size;
}

int main()
{
	char buf[256], seg[256];
	int count = 0;

	setlocale(LC_CTYPE, "");

	printf(
//		"#pragma once\n"
		"enum {\n");

	while(gets(buf) != NULL) {
		pathGetName(buf, seg);
//		StrToUpper(seg);
		printf("\t_%sSegmentSize = 0x%08x,\n", seg, FileSize(buf));
		count++;
	}

	printf("};\n");

	return 0;
}

