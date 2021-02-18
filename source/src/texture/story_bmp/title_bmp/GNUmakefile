ifndef LOCAL
LOCAL=JAPAN
endif

ifeq ($(LOCAL),CHINA)
ASSET_DIR=../../../i10n/assets/zh/story_bmp/title_bmp
GBI_SOURCES= \
	ninlogo.gbi \
	$(ASSET_DIR)/press_col.gbi \
	$(ASSET_DIR)/press_i.gbi
else
GBI_SOURCES= \
	ninlogo.gbi \
	press_col.gbi \
	press_i.gbi
endif

.SUFFIXES: .bin .zzz

.bin.zzz:
	gzip -n -9 -c $< > $<.gz && dd if=$<.gz bs=10 skip=1 of=$@
	rm -f $<.gz

default: title_bmp.zzz

title_bmp.bin: $(GBI_SOURCES)
	rm -f $@ $*.tbl
	touch $@
	for f in $(GBI_SOURCES); do \
	    echo -e "`stat -c %s $@`,\t/* $$f */" >> $*.tbl; \
	    cat $$f >> $@; \
	done

title_bmp.zzz: title_bmp.bin

clean:
	rm -f title_bmp.bin *.tbl *.zzz *.gz 
