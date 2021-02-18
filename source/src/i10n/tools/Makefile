### Makefile of font tools

TARGET = create_12x12_font gb_mes_font.all create_16x16_bm gb_font
CFILE = create_12x12_font.c gb12x12font.c
OBJECTS = create_12x12_font.o gb12x12font.o create_16x16_bm.o

LCOPTS  = -g
LCFLAGS = $(LCOPTS) $(LCINCS)

defaults: $(TARGET)
.c.o:
	cc $(LCFLAGS) -c $<

create_12x12_font: create_12x12_font.o gb12x12font.o
	cc -o $@ $^  $(LCOPTS)

create_16x16_bm: create_16x16_bm.o
	cc -o $@ $^  $(LCOPTS)

gb_mes_font.all:
	cat ../assets/zh/mes/*.mes | ./create_12x12_font gb_mes_code > ../assets/zh/font/gb_mes_font.all

gb_font:
	cat ../assets/zh/mes/*.mes | ./create_16x16_bm gbfont gbfont.raw > ../assets/zh/font/gbfont.h 

clean:
	rm -rf $(OBJECTS) $(TARGET) 

