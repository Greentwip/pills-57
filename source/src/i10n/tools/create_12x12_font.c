/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
       Create 12x12 GB font table 

  Input(stdin): all charactors you want to use in this app
  Output(stdout) : 
      (1) # of total charactors.
      (2) Sorted code map. 
      (3) 12 x 12 font bitmap.        
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#define GB_CODE_SIZE   94
#define GB_CODE_START  0xA1
#define GB_12x12_SIZE  (12*12/8)

char GBcode[GB_CODE_SIZE][GB_CODE_SIZE];
extern unsigned char GUO_GB2312_12X12_FONT_BITMAP[];

void output_12x12_line(int v)
{
    int i, out;

    printf("    ");
    for (i=11; i>=0; i-=2) {
        out = ((v >> i) & 1) ? 0xF0:0;
        out |= ((v >> (i-1)) & 1) ? 0xF:0;
        printf("0x%02x, ", out);
    }
    printf("\n");
}

void output_12x12(unsigned char *font)
{
    unsigned char *p = font;
    int i;

    for (i=0; i<6; i++, p+=3) {
        output_12x12_line( ((int) p[0]) << 4 | ((p[1] >> 4) & 0xf));
        output_12x12_line( ((int) p[2]) << 4 | (p[1]  & 0xf));
    }
}

int main(int argc, char **argv)
{
    int i, j, num;
    unsigned char line[1024], *p, comment;
 
    if (argc != 2) {
        printf("Usage: create_12x12_font prefix\n");
        printf("     Reading all charactors from stdin, then \n");
        printf("     output sorted font table.\n");
	return -1;
    }

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

    printf("/* * * This code is generate by create_12x12_font * * * */\n\n"); 
    printf("int %s_num = %d; \n", argv[1], num);
    printf("u16 %s_code[] = { \n", argv[1]);
    for (i=num=0; i<GB_CODE_SIZE; i++) 
         for (j=0; j<GB_CODE_SIZE; j++) 
              if (GBcode[i][j]) {
                  printf("0x%02x%02x, ", i+0xA1, j+0xA1);
                  if (!((++num) % 8)) printf("\n");       
              }
    printf("};\n\n"); 

    printf("/* Font bitmap here */\n");
    printf("u8 %s_bitmap[] __attribute__((aligned(16))) = { \n", argv[1]);
    for (i=0, p=GUO_GB2312_12X12_FONT_BITMAP; i<GB_CODE_SIZE; i++)
        for (j=0; j<GB_CODE_SIZE; j++, p+=GB_12x12_SIZE) 
            if (GBcode[i][j]) {
                 printf("\n      /* GB code 0x%x 0x%x */ \n", i+0xA1, j+0xA1);
                 output_12x12(p);
            }
    printf("};\n\n"); 
    return 0;
}






