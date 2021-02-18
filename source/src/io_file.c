
#include <stdarg.h>
#include "io_file.h"

// �t�@�C�����J��
int io_open(IO_FILE *fp, ...)
{
	va_list va;
	va_start(va, fp);
	fp->ftbl->open(fp, va);
	va_end(va);
}

// �t�@�C�������
int io_close(IO_FILE *fp)
{
	fp->ftbl->close(fp);
}

// �t�@�C���֏o��
int io_write(IO_FILE *fp, void *buf, int size)
{
	fp->ftbl->write(fp, buf, size);
}

