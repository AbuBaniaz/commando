#################################################
# some magic for using linux kernel settings
# when compiling module(s)

M_OBJS = dummyfe.o dummyadap.o
obj-m  = $(M_OBJS)

#remove __ASSEMBLY__ option from kernel makefile
AFLAGS=
EXTRA_CFLAGS=-I../dvb-core -I../../../../include -I../../../../../include -DEXPORT_SYMTAB -DUSE_DVB_I2C

here:
	DIR=`pwd`; (cd $(KERNEL_LOCATION); make SUBDIRS=$$DIR modules)

install:
	cp -v $(M_OBJS) $(MODULE_DEST)

clean:
	-rm -f $(M_OBJS) $(O_TARGET) .*.o.flags *~

include $(KERNEL_LOCATION)/Rules.make
