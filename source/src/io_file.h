#if !defined(_IO_FILE_H_)
#define _IO_FILE_H_

typedef struct IO_FILE     IO_FILE;
typedef struct IO_FUNC_TBL IO_FUNC_TBL;

struct IO_FILE {
	IO_FUNC_TBL *ftbl;
	void        *farg;
};

struct IO_FUNC_TBL {
	int (*open)(IO_FILE *fp, va_list va);
	int (*close)(IO_FILE *fp);
	int (*write)(IO_FILE *fp, void *buf, int size);
};

extern int io_open(IO_FILE *fp, ...);
extern int io_close(IO_FILE *fp);
extern int io_write(IO_FILE *fp, void *buf, int size);

extern int io_vprintf(IO_FILE *fp, const char *f, va_list argp);

#endif // _IO_FILE_H_
