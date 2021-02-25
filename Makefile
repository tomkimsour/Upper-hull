# generation de programme PVM
#  TP algorithmique parallele
#  maitrise
#  LL
#  07/10/97
# 

CC = gcc

# executable directory
#BDIR  =  $(HOME)/bin/$(PVM_ARCH)
BDIR = $(PWD)

# Programme sequentiel
PROG = upper

#PVM_ROOT = /usr/share/pvm3
#PVM_ARCH = LINUXI386

#PVM_ROOT = /home/commun_depinfo/enseignants/lemarchand/TPPVMM1/PvmLinux/pvm3
#PVM_ARCH = LINUX64


#### do not edit #####

EXEC = $(PROG:%=$(BDIR)/%)

#### generation rules #####

all: $(EXEC)
	
$(BDIR)/%: point.o %.o 
	$(CC) -o $@ upper.o point.o pb.o

.c.o: point.h pb.h upper.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(EXEC) points upper_hull.pdf
