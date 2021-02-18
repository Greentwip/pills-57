/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    create 16x16 raw bitmap and index file
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#define GB_CODE_SIZE   94
#define GB_CODE_START  0xA1
#define GB_FONT_SIZE  (16*16/8)

void output_to_bitmap(unsigned int code, FILE *bitmap)
{
    int i;
    if (bitmap == NULL)
        return;

    for (i=7; i>=0; i--)
        fputc(((code >> i) & 1) ? 0xff : 0,  bitmap);

    return; 
}

static char GBcode[GB_CODE_SIZE][GB_CODE_SIZE];
int main(int argc, char **argv)
{
    int i, j, num, comment, offset, k;
    unsigned char line[1024], *p;
    FILE *font, *bitmap;
 
    if (argc != 3) {
        printf("Usage: create_16x16_bm prefix raw_bitmap_file_name\n");
        printf("     Reading all charactors from stdin, then \n");
        printf("     output sorted font table.\n");
	return -1;
    }

    if ((font = fopen("font16", "rb")) == NULL) {
        printf("Please cp/ln font16 (16x16 GB font) in current dir \n");
        return -2;
    }

    if ((bitmap = fopen(argv[2], "wb")) == NULL) {
        printf("Cannot create %s to store bitmap file \n", argv[2]);
        fclose(font);
        return -2;
    }
        
        /* lazy sorting program since we do care about performance */
    for (i=0; i<GB_CODE_SIZE; i++) 
        for (j=0; j<GB_CODE_SIZE; j++) 
            GBcode[i][j] = 0;

    num = comment = 0;
    while (fgets(line, sizeof line, stdin) != NULL) {
        p = line;
        while ((*p != '\n') && (*p != '\0')) {
            if (comment) { /* skip all charactors in comment */
                if ( p[0] == '*' && p[1] == '/') {
		   comment = 0;
                   p += 2;
                } else p++;

                continue; 
	    }

            if ((p[0] >= GB_CODE_START) && (p[1] >= GB_CODE_START)) { /* GB code */
                i = p[0] - GB_CODE_START; 
                j = p[1] - GB_CODE_START;
		if (!GBcode[i][j]) {
                    GBcode[i][j] = 1;
                    num++;
                } 
                p += 2;
            } else {
                if (p[0] == '/' && p[1] == '/') break;
                if (p[0] == '/' && p[1] == '*') {
                    comment = 1;
                    p += 2;
                } else p++;
            }
        }
    }

    printf("int %s_num = %d; \n", argv[1], num);
    printf("u16 %s_code[] = { \n", argv[1]);
    for (i=num=0; i<GB_CODE_SIZE; i++) 
         for (j=0; j<GB_CODE_SIZE; j++) 
              if (GBcode[i][j]) {
                  printf("0x%02x%02x, ", i+0xA1, j+0xA1);
                  if (!((++num) % 8)) printf("\n");       
              }
    printf("};\n\n"); 

    //fprintf(bitmap, "p5\n%d %d\n255\n", 16, 16*num);
    for (i=0, offset=0; i<GB_CODE_SIZE; i++)
        for (j=0; j<GB_CODE_SIZE; j++, offset+=GB_FONT_SIZE) 
            if (GBcode[i][j]) {
                 fseek(font, offset, SEEK_SET); 
                 for (k=0; k<GB_FONT_SIZE; k++) 
                      output_to_bitmap((unsigned int) fgetc(font), bitmap);
            }
    fclose(font);
    fclose(bitmap);
    return 0;
}






