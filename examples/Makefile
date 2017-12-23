
#COMPILEFOR = "SE-UM"     # compiled on OII under Debian FS
#COMPILEFOR = "SE-S"      # X86 SE-S
COMPILEFOR = "linux"      # X86 Linux
#COMPILEFOR = "X86_SE_UM" # X86 SE-UM
#COMPILEFOR = "DEBIAN" # Compile on MIPS Debian 

CCFLAGS          = -g -O -Wall -c  -fexceptions   -fasynchronous-unwind-tables
#   for more info on linkign   enter   "info ld"
#   Notw  -W1, (Capital W,lower case L) takes the next argument and passes it t\
o the linker  -v ->
LDFLAGS  =  -Wl,-v -Wl,-Map=a.map -Wl,--cref -Wl,-t
ARFLAGS  = -rcs


INDEX = 0
#################################################################
TMP = ${findstring SE-UM,${COMPILEFOR}}
ifeq ($(TMP),SE-UM)
    INDEX = 1
    CC = gcc
    AR = ar
    #if you use ld for linking cmds will not like -Wl,
    LD = gcc
    STRIP = strip
    OBJDUMP = objdump
    NM = nm

    MPATH = /usr/bin/
endif
#################################################################
TMP = ${findstring X86_SE_UM,${COMPILEFOR}}
ifeq ($(TMP) ,X86_SE_UM)
    INDEX = 2
    CC = mips64-octeon-linux-gnu-gcc 
    AR = mips64-octeon-linux-gnu-ar
    #if you use ld for linking cmds will not like -Wl,
    LD = mips64-octeon-linux-gnu-gcc  
    STRIP = mips64-octeon-linux-gnu-strip
    OBJDUMP = mips64-octeon-linux-gnu-objdump
    NM = mips64-octeon-linux-gnu-nm

    MPATH = /usr/local/Cavium_Networks/OCTEON-SDK/tools/bin/

    CCFLAGS          += -DLINUX
endif
################################################################
TMP = ${findstring SE-S,${COMPILEFOR}}
ifeq ($(TMP),SE-S)
    INDEX = 3
    CC = mipsisa64-octeon-elf-gcc 
    AR = mipsisa64-octeon-elf-ar
    #if you use ld for linking cmds will not like -Wl,
    LD = mipsisa64-octeon-elf-gcc
    STRIP = mipsisa64-octeon-elf-strip
    OBJDUMP = mipsisa64-octeon-elf-objdump
    NM = mipsisa64-octeon-elf-nm

    MPATH = /usr/local/Cavium_Networks/OCTEON-SDK/tools/bin/
endif
#################################################################
TMP = ${findstring linux,${COMPILEFOR}}
ifeq ($(TMP),linux)
    INDEX = 4
    CC = gcc 
    LD = gcc
    AR = ar
    MPATH  =

    CCFLAGS          += -DLINUX
endif 
#################################################################
TMP = ${findstring DEBIAN,${COMPILEFOR}}
ifeq ($(TMP),DEBIAN)
    INDEX = 5
    CC = gcc 
    LD = gcc
    AR = ar
    MPATH  =

    CCFLAGS          += -DLINUX
endif 


all: var main   


main.o:   main.c  Makefile
	 $(MPATH)$(CC) $(CCFLAGS) -o main.o main.c

main:  main.o
#       BOth of the two following links work
	 $(MPATH)$(LD) $(LDFLAGS) -L. main.o -Wl,-Bdynamic  -o main



clean:
	rm -rf *.a
	rm -rf *.o
	rm -rf *.map
	rm -rf main

cleanbuild:
	rm -rf *.a
	rm -rf *.o
	rm -rf *.map




# enter "make var" to see the environment variables
var:
	@echo -e "\n"
	@echo -e "index           $(INDEX)"
	@echo -e "compilefor      $(COMPILEFOR)"
	@echo -e "cc              $(CC)"
	@echo -e "ccflags         $(CCFLAGS)"
	@echo -e "ld              $(LD)"
	@echo -e "ldflags         $(LDFLAGS)"
	@echo -e "ar              $(AR)"
	@echo -e "arflags         $(ARFLAGS)"
	@echo -e "MAPTH           $(MPATH)"
	@echo -e "(I) PATH        $(PATH)"
	@echo -e "(I) VPATH       $(VPATH)"
	@echo -e "(I) vpath       $(vpath)"

