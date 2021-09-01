#
# Makefile
#
CC ?= gcc
LVGL_DIR_NAME ?= lvgl
LVGL_DIR ?= ${CURDIR}
CFLAGS ?= -O3 -g0 -I$(LVGL_DIR)/ -Wall -Wshadow -Wundef -Wmissing-prototypes -Wno-discarded-qualifiers -Wall -Wextra -Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing -Wno-error=cpp -Wuninitialized -Wmaybe-uninitialized -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated -Wempty-body -Wtype-limits -Wshift-negative-value -Wstack-usage=2048 -Wno-unused-value -Wno-unused-parameter -Wno-missing-field-initializers -Wuninitialized -Wmaybe-uninitialized -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits -Wsizeof-pointer-memaccess -Wno-format-nonliteral -Wpointer-arith -Wno-cast-qual -Wmissing-prototypes -Wunreachable-code -Wno-switch-default -Wreturn-type -Wmultichar -Wno-discarded-qualifiers -Wformat-security -Wno-ignored-qualifiers -Wno-sign-compare
LDFLAGS ?= -lm
BIN = demo
sudo ?= sudo

#Collect the files to compile
MAINSRC = ./main.c

makefiles?=lvgl/lvgl.mk lv_drivers/lv_drivers.mk lv_demos/lv_demo.mk

-include $(LVGL_DIR)/lvgl/lvgl.mk
-include $(LVGL_DIR)/lv_drivers/lv_drivers.mk
-include $(LVGL_DIR)/lv_demos/lv_demo.mk


OBJEXT ?= .o

AOBJS = $(ASRCS:.S=$(OBJEXT))
COBJS = $(CSRCS:.c=$(OBJEXT))

MAINOBJ = $(MAINSRC:.c=$(OBJEXT))

SRCS = $(ASRCS) $(CSRCS) $(MAINSRC)
OBJS = $(AOBJS) $(COBJS)

## MAINOBJ -> OBJFILES

default: all
	-sync

%.o: %.c ${makefiles} 
	$(CC)  $(CFLAGS) -c $< -o $@
	@echo "CC $<"

all: ${BIN}

${BIN}: $(AOBJS) $(COBJS) $(MAINOBJ) ${makefiles}
	$(CC) -o $@ $(MAINOBJ) $(AOBJS) $(COBJS) $(LDFLAGS)

clean: 
	rm -f $(BIN) $(AOBJS) $(COBJS) $(MAINOBJ)

${makefiles}: .gitmodules
	@echo "info: Preparing sources"
	git submodule init
	git submodule update
	@ls ${makefiles} > $@
	@echo "info: Sources prepared ready to build again"
	@exit 1

run: ${BIN}
	${sudo} -E ${<D}/${<F}

install: ${BIN}
	install -d ${DESTDIR}/usr/lib/lvgl/bin
	install $< ${DESTDIR}/usr/lib/lvgl/bin/
