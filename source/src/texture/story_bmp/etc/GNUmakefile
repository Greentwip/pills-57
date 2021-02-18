ifndef LOCAL
LOCAL=JAPAN
endif

ifeq ($(LOCAL),CHINA)
ASSET_DIR=../../../i10n/assets/zh/story_bmp/etc
GBI_SOURCES= \
	$(ASSET_DIR)/al_pausemenu.gbi \
	al_pausemenu_alpha.gbi \
	al_pausemenu_alpha2.gbi \
	al_pausemenu_cursor.gbi \
	$(ASSET_DIR)/al_pausemenu2.gbi \
	al_pausemenu2_alpha.gbi \
	$(ASSET_DIR)/al_pausemenu3.gbi \
	$(ASSET_DIR)/al_pausemenu4.gbi \
	$(ASSET_DIR)/al_pausemenu5.gbi \
	$(ASSET_DIR)/al_pausemenu6.gbi \
	stop_a.gbi \
	stop_a_i.gbi \
	$(ASSET_DIR)/stop_b.gbi
else
GBI_SOURCES= \
	al_pausemenu.gbi \
	al_pausemenu_alpha.gbi \
	al_pausemenu_alpha2.gbi \
	al_pausemenu_cursor.gbi \
	al_pausemenu2.gbi \
	al_pausemenu2_alpha.gbi \
	al_pausemenu3.gbi \
	al_pausemenu4.gbi \
	al_pausemenu5.gbi \
	al_pausemenu6.gbi \
	stop_a.gbi \
	stop_a_i.gbi \
	stop_b.gbi
endif

SOURCES= \
	etc_gbi.bin \
	../mes/mes.seg \
	sp.bin

.SUFFIXES: .bin .zzz

.bin.zzz:
	gzip -n -9 -c $< > $<.gz && dd if=$<.gz bs=10 skip=1 of=$@
	rm -f $<.gz

default: etc.zzz

etc.bin: $(SOURCES)
	rm -f $@ $*.tbl
	touch $@
	for f in $(SOURCES); do \
	    echo -e "`stat -c %s $@`,\t/* $$f */" >> $*.tbl; \
	    cat $$f >> $@; \
	done

etc_gbi.bin: $(GBI_SOURCES)
	rm -f $@ $*.tbl
	touch $@
	for f in $(GBI_SOURCES); do \
	    echo -e "`stat -c %s $@`,\t/* $$f */" >> $*.tbl; \
	    cat $$f >> $@; \
	done

etc.zzz: etc.bin

clean:
	rm -f etc.bin etc_gbi.bin *.tbl *.zzz *.gz 
