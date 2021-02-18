
#include <stdarg.h>
#include "io_file.h"

// ファイルを開く
int io_open(IO_FILE *fp, ...)
{
	va_list va;
	va_start(va, fp);
	fp->ftbl->open(fp, va);
	va_end(va);
}

// ファイルを閉じる
int io_close(IO_FILE *fp)
{
	fp->ftbl->close(fp);
}

// ファイルへ出力
int io_write(IO_FILE *fp, void *buf, int size)
{
	fp->ftbl->write(fp, buf, size);
}

