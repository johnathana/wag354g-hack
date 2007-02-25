EXTRA_DEFINE 	+= -DVX_COMPILATION

EXTSWITCH_OBJ 	=  CSL_switch.o CSL_mdio.o DDC_switch.o

MACH_EXTRA  	+=  $(EXTSWITCH_OBJ)

EXTRA_INCLUDE   +=  -I$(EXTSWITCH_DIR)\include \
            -I$(EXTSWITCH_DIR)\src\ddc \
            -I$(EXTSWITCH_DIR)\src\csl \
            -I$(EXTSWITCH_DIR)\src\dda\os_vxworks
            
# Search paths for the MAKE utility
VPATH 		+=  \
            $(EXTSWITCH_DIR)\include \
            $(EXTSWITCH_DIR)\src\ddc \
            $(EXTSWITCH_DIR)\src\csl \
            $(EXTSWITCH_DIR)\src\dda\os_vxworks \
            
ifeq "$(EXT_SWITCH)" "KENDIN"
EXTRA_DEFINE 	+= -DCONFIG_MIPS_AVALANCHE_KENDIN
EXTSWITCH_OBJ	+= CSL_kendin8995x.o
EXTRA_INCLUDE   += -I$(EXTSWITCH_DIR)\src\csl\csl_kendin8995x
VPATH 		+=  $(EXTSWITCH_DIR)\src\csl\csl_kendin8995x
endif

ifeq "$(EXT_SWITCH)" "MARVELL"
EXTRA_DEFINE 	+= -DCONFIG_MIPS_AVALANCHE_MARVELL
EXTSWITCH_OBJ 	+= CSL_marvell6063.o
EXTRA_INCLUDE   += -I$(EXTSWITCH_DIR)\src\csl\csl_marvell6063
VPATH 		+=  $(EXTSWITCH_DIR)\src\csl\csl_marvell6063
endif

