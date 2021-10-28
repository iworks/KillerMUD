CCACHE  := $(shell which ccache 2>/dev/null)
CC      = $(CCACHE) gcc
COMMON  = -ggdb -lz

ifdef INSANE_OPT
	COMMON += -Wall -Wp,-D_FORTIFY_SOURCE=2 -mtune=native -march=native -O2 -Wextra
endif

C_FLAGS = $(COMMON) -DHASHSTR -DMCCP -std=gnu89
L_FLAGS =  $(COMMON) -lpcre -pthread
CYG_C_FLAGS = $(COMMON) -DHASHSTR -DCYGWIN
CYG_L_FLAGS = $(COMMON) /bin/pcre.dll
CYG = cyg/
OBJ = obj/
OCAML = ocm/

# wersja OFFLINE:
ifdef OFFLINE
	C_FLAGS += -DOFFLINE 
	CYG_C_FLAGS += -DOFFLINE
endif

O_FILES := $(addprefix $(OBJ),$(patsubst %.c,%.o,$(wildcard *.c)))
CYG_O_FILES := $(addprefix $(CYG),$(patsubst %.c,%.o,$(wildcard *.c)))

rom:  $(EXTRA_TARGETS) $(O_FILES)
	rm -f rom
	#$(CC) $(L_FLAGS) -o rom $(O_FILES) $(L_FILES) $(STATIC_LIBS)
	$(CC) -o rom $(O_FILES) $(L_FILES) $(L_FLAGS) $(STATIC_LIBS)

rom-tc:  $(EXTRA_TARGETS) $(O_FILES)
	rm -f rom-tc
	$(LINK_HASKELL)
	$(CC) $(L_FLAGS) -ltcmalloc -lprofiler -o rom-tc $(O_FILES) $(L_FILES)


cygwin: $(CYG_O_FILES)
	rm -f rom
	$(CC) $(CYG_L_FLAGS) -o rom $(CYG_O_FILES)

ocaml:
	ocamlopt str.cmxa spell_syn.ml -o $(OCAML)syn.exe
	cp $(OCAML)* ../area/
	rm -f *.cmx *.cmi *.o

$(OBJ)%.o: %.c
	$(CC) -c $(C_FLAGS) $< -o $@

$(CYG)%.o: %.c
	$(CC) -c $(CYG_C_FLAGS) $< -o $@

clean:
	rm -f *.o $(OBJ)*.o $(CYG)*.o rom rom.exe *.cmx *.cmi

cclean: clean
	$(CCACHE) -c

.PHONY: clean rom
	.DEFAULT_GOAL := rom
