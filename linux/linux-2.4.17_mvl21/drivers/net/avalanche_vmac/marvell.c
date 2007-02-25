/*
 * marvell.c --  Marvell Ethernet Switch Driver
 * 
 */

#include "marvell_mdio.h"
#include <asm/io.h>
#include <asm/uaccess.h>

#include <asm/avalanche/avalanche_map.h>

/* constants */
#ifndef TRUE
#define TRUE (1==1)
#endif

#ifndef FALSE
#define FALSE (1==2)
#endif

#undef SET_MARVELL_PORT_MAC_ADDRESS

#define MARVELL_PORT_ADDR_OFFSET  			0x00

#define MARVELL_SWITCH_GLOBAL_REG_OFFSET 		0xF


#define MARVELL_SWITCH_GLOBAL_CONTROL_REG  	0x04
#define MARVELL_PULL_UP_RESISTOR_OFFSET   	0x10

#define MARVELL_SWITCH_MAC_ADDR1_REG		0x1
#define MARVELL_SWITCH_MAC_ADDR2_REG		0x2
#define MARVELL_SWITCH_MAC_ADDR3_REG		0x3

#define NUMBER_OF_MARVELL_PORTS 			6
#define DBNUM_FIELD_OFFSET  				12

#define MARVELL_PORT_STATUS_REG 			0x00
#define MARVELL_SWITCH_ID_REG 		  		0x03
#define MARVELL_PORT_CONTROL_REG 			0x04
#define MARVELL_PORT_VLAN_MAP_REG			0x06
#define MARVELL_PORT_RX_COUNTER				0x10
#define MARVELL_PORT_TX_COUNTER				0x11

#define MARVELL_HEADER_MODE_MASK 	  	        0x0800 
#define MARVELL_DEV_ID_MASK  				0xFFF0
#define MARVELL_DEV_ID_SHIFT 				4
#define MARVELL6052_DEV_ID_VALUE   			0x052
#define MARVELL6060_DEV_ID_VALUE   			0x060

#define MARVELL_PORT_MODE_MASK  			0x0800
#define MARVELL_PORT_MODE_SHIFT				11
#define MARVELL_PORT_MODE_VALUE				1 /* MII 10/100 Mbps mode*/

#define MARVELL_PHY_MODE_MASK  				0x0400
#define MARVELL_PHY_MODE_SHIFT 				10
#define MARVELL_PHY_MODE_VALUE 				1 /* MII PHY mode*/

#define MARVELL_PORT_COUNTER_VALUE			0x0000 /* Use 0x100 to count errors and 0x0000 for frames */

#define MARVELL_EGRESS_TRAILER_MODE_MASK 	0x4000
#define MARVELL_EGRESS_TRAILER_LENGTH  		4  /* in Bytes*/

#define MARVELL_PORT_FLOW_CONTROL_MASK		0x8000	

#define SPID_OFFSET	 			3 /* 3 is for Marvell trailer 2nd 3rd 4th bytes*/
#define SPID_PORT_MASK 				7
#define DEFAULT_CPU_PORT_DOMAIN LAN_DOMAIN


/* enums */
#if 0
typedef enum
{
	WAN_PORT_ADDR   = 0x08 + MARVELL_PORT_ADDR_OFFSET,
	LAN_PORT_1_ADDR = 0x09 + MARVELL_PORT_ADDR_OFFSET,
	LAN_PORT_2_ADDR = 0x0A + MARVELL_PORT_ADDR_OFFSET,
	LAN_PORT_3_ADDR = 0x0B + MARVELL_PORT_ADDR_OFFSET,
	LAN_PORT_4_ADDR = 0x0C + MARVELL_PORT_ADDR_OFFSET,
	CPU_PORT_ADDR = 0x0D + MARVELL_PORT_ADDR_OFFSET
} MARVELL_PORT_ADDRESS;

#else
/* enums */

typedef enum
{
	LAN_PORT_1_ADDR = 0x08 + MARVELL_PORT_ADDR_OFFSET,
	LAN_PORT_2_ADDR = 0x09 + MARVELL_PORT_ADDR_OFFSET,
	LAN_PORT_3_ADDR = 0x0A + MARVELL_PORT_ADDR_OFFSET,
	LAN_PORT_4_ADDR = 0x0B + MARVELL_PORT_ADDR_OFFSET,
	WAN_PORT_ADDR = 0x0C + MARVELL_PORT_ADDR_OFFSET,
	CPU_PORT_ADDR = 0x0D + MARVELL_PORT_ADDR_OFFSET
} MARVELL_PORT_ADDRESS;
#endif

/* enums */                  

typedef enum
{
  LAN_DOMAIN = 1,
  WAN_DOMAIN = 2
} PORT_DOMAIN;

/* types */
typedef struct
{
	u16 portVlan;  /* Port VLAN Setting */
	u16 portDbNum; /* DataBase number */
} PORT_VLAN_DBNUM;

/* Global variables */ 
int CurrentDomain = LAN_DOMAIN;

/* local variables */ 


#if 0
static PORT_VLAN_DBNUM marvell_ports[NUMBER_OF_MARVELL_PORTS] = 
{
	/* VLAN, DBNUM*/
	{0x20, LAN_DOMAIN},   /* Wan port#0*/
	{0x3C, LAN_DOMAIN},   /* Lan port#1*/
	{0x3A, LAN_DOMAIN},   /* Lan port#2*/ 
	{0x36, LAN_DOMAIN},   /* Lan port#3*/ 
	{0x2E, LAN_DOMAIN},   /* Lan port#4*/ 
	{0x1F, DEFAULT_CPU_PORT_DOMAIN}    /* CPU port#5*/ 
};

#else
static PORT_VLAN_DBNUM marvell_ports[NUMBER_OF_MARVELL_PORTS] = 
{
	/* VLAN, DBNUM*/
	{0x2E, LAN_DOMAIN},   /* Lan port#1*/
	{0x2D, LAN_DOMAIN},   /* Lan port#2*/ 
	{0x2B, LAN_DOMAIN},   /* Lan port#3*/ 
	{0x27, LAN_DOMAIN},   /* Lan port#4*/ 
	{0x20, WAN_DOMAIN},   /* Wan port#0*/
	{0x1F, DEFAULT_CPU_PORT_DOMAIN}    /* CPU port#5*/ 
};
#endif



/* static functions */
static void marvellMacPhyInit (void);
static int marvellDeviceIdCheck (void);
static int marvellPortsModeCheck (void);
static int marvellPortsVlanDBnumSet (void);
static int marvellHeaderSet (void);
static int marvell_set_port_counters(void);
#ifdef SET_MARVELL_PORT_MAC_ADDRESS
static int marvell_set_mac_address (void);
#endif
static int MarvellSetDomain (int new_domain);

/* Global Functions */ 
int marvellInit (void);
int marvell_receive (unsigned char *data_ptr, unsigned int *packet_length);
int marvell_read_counters (char* buf, char **start, off_t offset, int count,int *eof, void *data);
int marvellSwitchProbe (void);

/**************************************************************************
 * FUNCTION NAME : marvellInit
 **************************************************************************
 * DESCRIPTION  : This function is called from the EMAC hardware Init to 
 * 		  initialize the Marvell switch.
 *	
 * RETURNS	: TRUE	- successfull
 * 		  FALSE	- ERROR
 * 		
 ***************************************************************************/
int marvellInit (void)
{
	static  int marvell_Init = FALSE;
  
	/* Check if initialized */
	if (marvell_Init == TRUE)
	{
		printk ("ERROR marvellInit: driver is already initialized\n");   
		return(FALSE);
	}

	/* Check the Marvell switch device ID */
	if (marvellDeviceIdCheck () != TRUE)
		return (FALSE);
	
	/* Check the ports */
	if (marvellPortsModeCheck () != TRUE)
		return (FALSE);
	
	/* Set the Switch's VLAN and DBnum for all ports */
	if (marvellPortsVlanDBnumSet () != TRUE)
		return (FALSE);

	/* Set CPU port to Egress/Ingress Header mode */
	if ( marvellHeaderSet() != TRUE)
		return (FALSE);

#ifdef SET_MARVELL_PORT_MAC_ADDRESS
	/* Set the device MAC address */	
	if (marvell_set_mac_address() != TRUE)
		return FALSE;
#endif

	/* Set the ports to count Tx and Rx errors */	
	if (marvell_set_port_counters() != TRUE)
		return FALSE;

	/* Enable the emac device driver to be a Multi Phy driver */
	marvellMacPhyInit ();
	
	marvell_Init = TRUE;

	return (TRUE);
}

/**************************************************************************
 * FUNCTION NAME : marvellMacPhyInit
 **************************************************************************
 * DESCRIPTION  : This function sets EMAC phy to behave as a multi-phy device
 *	
 * RETURNS	: TRUE	- successfull
 * 		  FALSE	- ERROR
 * 		
 ***************************************************************************/
static void marvellMacPhyInit (void)
{
	u32   base_address;
	volatile u32*	ptr;

	base_address = AVALANCHE_LOW_EMAC_BASE;

  	ptr	= (u32*) (base_address + 0x0390);
  
	/* toggle the ln bit of the avalanche MDIO MAC PHY register*/
	*ptr=0x05;
	*ptr=0x85;
	*ptr=0x05;
}

/**************************************************************************
 * FUNCTION NAME : marvellSwitchProbe
 **************************************************************************
 * DESCRIPTION  : This function probes to check if Marvell Switch is present
 *	
 * RETURNS	: TRUE	- successfull
 * 		  FALSE	- ERROR
 * 		
 ***************************************************************************/
int marvellSwitchProbe (void) 
{
	u32 readVal;
	u32 base_address = AVALANCHE_LOW_EMAC_BASE;

	/* read the Marvell device ID*/
	readVal = marvell_phy_read (base_address, CPU_PORT_ADDR, MARVELL_SWITCH_ID_REG);
  
	/* check the read result*/
	if (readVal & 0x80000000)
	{
		return (FALSE);
	}
	else
	{
    	/* check the Device ID field value*/
		readVal &= 0xFFFF;
	
			if ((((readVal & MARVELL_DEV_ID_MASK) >> MARVELL_DEV_ID_SHIFT) != MARVELL6052_DEV_ID_VALUE) &&
				(((readVal & MARVELL_DEV_ID_MASK) >> MARVELL_DEV_ID_SHIFT) != MARVELL6060_DEV_ID_VALUE))
	    	{
			/* read the Marvell device ID - again BUT assume a pull down resistor is NOT connected */
			/* to the marvell device EE_CLK pin */
			readVal = marvell_phy_read (base_address, CPU_PORT_ADDR + MARVELL_PULL_UP_RESISTOR_OFFSET, 
							MARVELL_SWITCH_ID_REG);

			/* check the read result*/
			if (readVal & 0x80000000)
			{
				return (FALSE);
			}
			else
			{
				/* check the Device ID field value*/
				readVal &= 0xFFFF;

				if ((((readVal & MARVELL_DEV_ID_MASK) >> MARVELL_DEV_ID_SHIFT) == MARVELL6052_DEV_ID_VALUE) ||
					(((readVal & MARVELL_DEV_ID_MASK) >> MARVELL_DEV_ID_SHIFT) == MARVELL6060_DEV_ID_VALUE))
				{
					return (TRUE);
				}
				return (FALSE);
			}
			return (FALSE);
		}
	}
	return (TRUE);
}

/**************************************************************************
 * FUNCTION NAME : marvellDeviceIdCheck
 **************************************************************************
 * DESCRIPTION  : This function checks the Marvell Device ID.
 *	
 * RETURNS	: TRUE	- successfull
 * 		  FALSE	- ERROR
 * 		
 ***************************************************************************/
static int marvellDeviceIdCheck (void) 
{
	u32 readVal;
	u32 base_address = AVALANCHE_LOW_EMAC_BASE;
  
#if 0
	/* read the Marvell device ID*/
	readVal = marvell_phy_read (base_address, CPU_PORT_ADDR, MARVELL_SWITCH_ID_REG);
  
	/* check the read result*/
	if (readVal & 0x80000000)
	{
		printk("\n ERROR: value read from portAddr 0x%X is 0x%X\n", CPU_PORT_ADDR, readVal);
		return (FALSE);
	}
	else
	{
    	/* check the Device ID field value*/
		readVal &= 0xFFFF;
	
		if ((((readVal & MARVELL_DEV_ID_MASK) >> MARVELL_DEV_ID_SHIFT) != MARVELL6052_DEV_ID_VALUE) &&
			(((readVal & MARVELL_DEV_ID_MASK) >> MARVELL_DEV_ID_SHIFT) != MARVELL6060_DEV_ID_VALUE))
    	{
			printk ("\n ERROR: Wrong Marvell DeviceId ,your id = %x\n",(readVal & MARVELL_DEV_ID_MASK) >> MARVELL_DEV_ID_SHIFT);

			/* read the Marvell device ID - again BUT assume a pull down resistor is NOT connected */
			/* to the marvell device EE_CLK pin */
			readVal = marvell_phy_read (base_address, CPU_PORT_ADDR + MARVELL_PULL_UP_RESISTOR_OFFSET, 
							MARVELL_SWITCH_ID_REG);

			/* check the read result*/
			if (readVal & 0x80000000)
			{
				printk ("\n ERROR: value read from portAddr 0x%X is 0x%X\n", 
					CPU_PORT_ADDR + MARVELL_PULL_UP_RESISTOR_OFFSET, readVal);
				return (FALSE);
			}
			else
			{
				/* check the Device ID field value*/
				readVal &= 0xFFFF;

				if ((((readVal & MARVELL_DEV_ID_MASK) >> MARVELL_DEV_ID_SHIFT) == MARVELL6052_DEV_ID_VALUE) ||
					(((readVal & MARVELL_DEV_ID_MASK) >> MARVELL_DEV_ID_SHIFT) == MARVELL6060_DEV_ID_VALUE))
				{
					printk	("\n ------- !!! HARDWARE CHANGE NEEDED !!! -----------");
					printk ("\n ---     PLEASE ADD PULL DOWN RESISTOR TO THE   ---");
					printk ("\n ---           MARVELL DEVICE EE_CLK PIN        ---");
					printk ("\n ---       MARVELL LOW LEVEL DRIVER DISABLED    ---");
					printk ("\n --------------------------------------------------");
				}
				return (FALSE);
			}
			return (FALSE);
		}
	}
#endif
	return (TRUE);
}

/**************************************************************************
 * FUNCTION NAME : marvellPortsModeCheck
 **************************************************************************
 * DESCRIPTION  : This function is used check if the LAN/WAN ports are 
 * 		  configured in 10/100Mbps mode and the CPU port is 
 * 		  configured to be a MII phy.
 *	
 * RETURNS	: TRUE	- successfull
 * 		  FALSE	- ERROR
 * 		
 ***************************************************************************/
static int marvellPortsModeCheck (void)
{
	MARVELL_PORT_ADDRESS portAddr;
	u32 readVal;
 	u32 base_address = AVALANCHE_LOW_EMAC_BASE; 
   	
	/* read the Marvell ports mode*/
	for (portAddr = LAN_PORT_1_ADDR; portAddr <= CPU_PORT_ADDR; portAddr++) 
	{
		/* read the register value*/
		readVal = marvell_phy_read (base_address, portAddr, MARVELL_PORT_STATUS_REG);

		/* check the read result*/
		if (readVal & 0x80000000)
 	   	{
			printk ("\n ERROR: value read from portAddr 0x%X, regAddr 0x%X, value is 0x%X\n", portAddr, 
								  MARVELL_PORT_STATUS_REG, readVal);
			return (FALSE);
		}
    		readVal &= 0xFFFF;

		/* check the port mode*/
		if (((readVal & MARVELL_PORT_MODE_MASK) >> MARVELL_PORT_MODE_SHIFT) != MARVELL_PORT_MODE_VALUE) 
		{
			printk ("\n ERROR: port mode is not MII 10/100 Mbps, check HW configuration !\n");
			printk ("\n ERROR: value read from portAddr 0x%X, regAddr 0x%X, value is 0x%X\n", portAddr,
							MARVELL_PORT_STATUS_REG, readVal);
			return (FALSE);
		}

		/* check only the CPU port PHYmode*/
		if (portAddr == CPU_PORT_ADDR) 
		{
			if (((readVal & MARVELL_PHY_MODE_MASK) >> MARVELL_PHY_MODE_SHIFT) != MARVELL_PHY_MODE_VALUE) 
			{
				printk ("\n ERROR: CPU port phy mode is not MII PHY Mode, check HW configuration !\n");
				printk ("\n ERROR: value read from portAddr 0x%X, regAddr 0x%X, value is 0x%X\n", 
						portAddr, MARVELL_PORT_STATUS_REG, readVal);
				return (FALSE);
			}
		}
	}
	return (TRUE);
}

/**************************************************************************
 * FUNCTION NAME : marvellPortsVlanDBnumSet
 **************************************************************************
 * DESCRIPTION  : This function sets the VLAN configuration and the database 
 * 		  number for WAN, LAN 1-4 and CPU port to isolate the WAN and 
 * 		  LAN ports. 
 * 		  	
 * RETURNS	: TRUE	- successfull
 * 		  FALSE	- ERROR
 * 		
 ***************************************************************************/
static int marvellPortsVlanDBnumSet (void)
{
	MARVELL_PORT_ADDRESS portAddr;
	u16 writeVal;
	u32 readVal;
	u32 base_address = AVALANCHE_LOW_EMAC_BASE; 
  
	/* Initialize each Port for it's VLAN and DBnum settings*/
	for (portAddr = LAN_PORT_1_ADDR; portAddr <= CPU_PORT_ADDR; portAddr++) 
	{
  		/* set the value to local argument*/
		writeVal = marvell_ports[portAddr-LAN_PORT_1_ADDR].portVlan | 
						(marvell_ports[portAddr-LAN_PORT_1_ADDR].portDbNum << DBNUM_FIELD_OFFSET);

		/* write the VLAN and DBnum setting to the port*/
		marvell_phy_write (base_address, portAddr, MARVELL_PORT_VLAN_MAP_REG, writeVal);
		
		/* read the register value*/
		readVal = marvell_phy_read (base_address, portAddr, MARVELL_PORT_VLAN_MAP_REG);

		/* check if what we wrote is really inside*/
		if ((readVal & 0x80000000) || ((readVal & 0xFFFF) != writeVal))
	   	{
			printk ("\n ERROR: value read from portAddr=0x%X, regAddr=0x%X readValue=0x%X, 
				while written value was 0x%X\n",portAddr, MARVELL_PORT_VLAN_MAP_REG, readVal, writeVal);
			return (FALSE);
		}
	}
	return (TRUE);
}


/**************************************************************************
 * FUNCTION NAME : marvellHeaderSet
 **************************************************************************
 * DESCRIPTION  : This function is used to set Ingress/Egress Header for 
 *                 the CPU port.
 * RETURNS	: TRUE	- successfull
 * 		  FALSE	- ERROR
 * 		
 ***************************************************************************/
static int marvellHeaderSet (void)
{
	u16 writeVal;
	u32 readVal;
	u32 base_address = AVALANCHE_LOW_EMAC_BASE; 
	
	/* set the CPU port to Egress/Ingress Header Mode*/
	readVal = marvell_phy_read (base_address, CPU_PORT_ADDR, MARVELL_PORT_CONTROL_REG);

	/* check the read result*/
	if (readVal & 0x80000000)
	{
		printk ("\n ERROR: value read from portAddr 0x%X is 0x%X\n", CPU_PORT_ADDR, readVal);
		return (FALSE);
	}
	else
	{
		readVal &= 0xFFFF;

		/* Set the Egress/Ingress Header mode for CPU port  and also enabling flow control */
		writeVal = readVal | MARVELL_HEADER_MODE_MASK | MARVELL_PORT_FLOW_CONTROL_MASK ;  
		
		/* write the settings to the port */
		marvell_phy_write (base_address, CPU_PORT_ADDR, MARVELL_PORT_CONTROL_REG, writeVal);

		/* read the register value*/
		readVal = marvell_phy_read (base_address, CPU_PORT_ADDR, MARVELL_PORT_CONTROL_REG);

		/* Check if write was successfull */
		if ((readVal & 0x80000000) || ((readVal & 0xFFFF) != writeVal))
    		{
			printk ("\n ERROR: value read from portAddr=0x%X, regAddr=0x%X readValue=0x%X, 
			while written value was 0x%X\n", CPU_PORT_ADDR, MARVELL_PORT_CONTROL_REG, readVal, writeVal);
			return (FALSE);
		}
	}
	return (TRUE);
}


#if 0
int marvell_print_cpuportregs(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{

	u32 readVal;
	u32 base_address = AVALANCHE_LOW_EMAC_BASE; 
	int len=0, index = 1;

	readVal = marvell_phy_read (base_address, CPU_PORT_ADDR, MARVELL_PORT_STATUS_REG);
	readVal = readVal & 0xffff; 
	len += sprintf(buf+len,"PORT STATUS = %x\n",readVal&0xffff);

	readVal = marvell_phy_read (base_address, CPU_PORT_ADDR, MARVELL_PORT_CONTROL_REG);
	readVal = readVal & 0xffff; 
	len += sprintf(buf+len,"PORT CONTROL = %x\n",readVal&0xffff);

	readVal = marvell_phy_read (base_address, CPU_PORT_ADDR, MARVELL_PORT_VLAN_MAP_REG);
	readVal = readVal & 0xffff; 
	len += sprintf(buf+len,"VLAN MAP = %x\n",readVal&0xffff);

	readVal = marvell_phy_read (base_address, CPU_PORT_ADDR, MARVELL_PORT_RX_COUNTER);
	readVal = readVal & 0xffff; 
	len += sprintf(buf+len,"RX COUNTER = %x\n",readVal&0xffff);


	readVal = marvell_phy_read (base_address, CPU_PORT_ADDR, MARVELL_PORT_TX_COUNTER);
	readVal = readVal & 0xffff; 
	len += sprintf(buf+len,"TX COUNTER = %x\n",readVal&0xffff);
	
	return len;
}
#endif


#if 0

/**************************************************************************
 * FUNCTION NAME : marvellEgressTrailerSet
 **************************************************************************
 * DESCRIPTION  : This function is used to set the CPU port in Egress Trailer 
 * 		  mode to track the source port of the incoming packet.
 * RETURNS	: TRUE	- successfull
 * 		  FALSE	- ERROR
 * 		
 ***************************************************************************/
static int marvellEgressTrailerSet (void)
{
	u16 writeVal;
	u32 readVal;
	u32 base_address = AVALANCHE_LOW_EMAC_BASE; 
	
	/* set the CPU port to Egress Tariler Mode*/
	readVal = marvell_phy_read (base_address, CPU_PORT_ADDR, MARVELL_PORT_CONTROL_REG);

	/* check the read result*/
	if (readVal & 0x80000000)
	{
		printk ("\n ERROR: value read from portAddr 0x%X is 0x%X\n", CPU_PORT_ADDR, readVal);
		return (FALSE);
	}
	else
	{
		readVal &= 0xFFFF;

		/* Set the Egress trailer mode and flow control for CPU port */
		writeVal = readVal | MARVELL_EGRESS_TRAILER_MODE_MASK | MARVELL_PORT_FLOW_CONTROL_MASK;
		
		/* write the settings to the port */
		marvell_phy_write (base_address, CPU_PORT_ADDR, MARVELL_PORT_CONTROL_REG, writeVal);

		/* read the register value*/
		readVal = marvell_phy_read (base_address, CPU_PORT_ADDR, MARVELL_PORT_CONTROL_REG);

		/* Check if write was successfull */
		if ((readVal & 0x80000000) || ((readVal & 0xFFFF) != writeVal))
    		{
			printk ("\n ERROR: value read from portAddr=0x%X, regAddr=0x%X readValue=0x%X, 
			while written value was 0x%X\n", CPU_PORT_ADDR, MARVELL_PORT_CONTROL_REG, readVal, writeVal);
			return (FALSE);
		}
	}
	return (TRUE);
}
#endif


#ifdef SET_MARVELL_PORT_MAC_ADDRESS
/**************************************************************************
 * FUNCTION NAME : marvell_set_mac_address
 **************************************************************************
 * DESCRIPTION  : This function sets the MAC address of the WAN, LAN 1-4 
 * 		  and CPU port. Note that when the bit 8 of Byte0 is set to 
 * 		  1, each port use a different MAC address. When set to zero 
 * 		  all ports have the same MAC address.
 *	
 * RETURNS	: TRUE	- successfull
 * 		  FALSE	- ERROR
 * 		
 ***************************************************************************/
static int marvell_set_mac_address (void)
{
	u32 base_address = AVALANCHE_LOW_EMAC_BASE; 
	u32 phy_address;
	volatile u32 readVal;
	u16 mac_addr_byte4_5 = 0x61ac;
	u16 mac_addr_byte2_3 = 0xa688;
	u16 mac_addr_byte0_1 = 0x01e0;

	phy_address = MARVELL_SWITCH_GLOBAL_REG_OFFSET;
	
	/* write MAC Address byte4 and byte5 */
	marvell_phy_write (base_address, phy_address, MARVELL_SWITCH_MAC_ADDR3_REG, mac_addr_byte4_5);
	
	/* Read back and check if write failed */
	readVal = marvell_phy_read (base_address, phy_address, MARVELL_SWITCH_MAC_ADDR3_REG);
  
	if ((readVal & 0x80000000) || ((readVal & 0xFFFF) != mac_addr_byte4_5))
	{
		printk ("\n ERROR: value read from phy_address=0x%X, regAddr=0x%X readValue=0x%X, 
			written value was 0x%X\n", phy_address, MARVELL_SWITCH_MAC_ADDR3_REG, readVal, mac_addr_byte4_5);
		return (FALSE);
	}

	/* write MAC Address byte2 and byte3 */
	marvell_phy_write (base_address, phy_address, MARVELL_SWITCH_MAC_ADDR2_REG, mac_addr_byte2_3);
	
	/* Read back and check if write failed */
	readVal = marvell_phy_read (base_address, phy_address, MARVELL_SWITCH_MAC_ADDR2_REG);
  
	if ((readVal & 0x80000000) || ((readVal & 0xFFFF) != mac_addr_byte2_3))
	{
		printk ("\n ERROR: value read from phy_address=0x%X, regAddr=0x%X readValue=0x%X, 
			written value was 0x%X\n", phy_address, MARVELL_SWITCH_MAC_ADDR2_REG, readVal, mac_addr_byte2_3);
		return (FALSE);
	}

	/* write MAC Address byte0 and byte1 */
	marvell_phy_write (base_address, phy_address, MARVELL_SWITCH_MAC_ADDR1_REG, mac_addr_byte0_1);
	
	/* Read back and check if write failed */
	readVal = marvell_phy_read (base_address, phy_address, MARVELL_SWITCH_MAC_ADDR1_REG);
  
	if ((readVal & 0x80000000) || ((readVal & 0xFFFF) != mac_addr_byte0_1))
	{
		printk ("\n ERROR: value read from phy_address=0x%X, regAddr=0x%X readValue=0x%X, 
			 written value was 0x%X\n", phy_address, MARVELL_SWITCH_MAC_ADDR1_REG, readVal, mac_addr_byte0_1);
		return (FALSE);
	}
	return (TRUE);
}
#endif

#define MARVELL_PORT_ADDRESS(phy_addr)	(phy_addr + 8)


#define MARVELL_PHY_CONFIG_REG		0
#define MARVELL_PHY_STATUS_REG		1

#define	MARVELL_PHY_AUTONEG_MODE	0
#define	MARVELL_PHY_10_HD_MODE		1
#define	MARVELL_PHY_10_FD_MODE		2
#define	MARVELL_PHY_100_HD_MODE		3
#define	MARVELL_PHY_100_FD_MODE		4
#define	MARVELL_PHY_LINK_DOWN_MODE	5

#define MARVELL_PHY_AUTONEG_MASK	(1 << 12)
#define	MARVELL_SPEED_MASK		(1 << 13)
#define	MARVELL_DUPLEX_MASK		(1 << 8)

#define	MARVELL_PHY_10_HD_MASK		0
#define	MARVELL_PHY_10_FD_MASK		(1 << 8)
#define	MARVELL_PHY_100_HD_MASK		(1 << 13)
#define	MARVELL_PHY_100_FD_MASK		((1 << 13) | (1 << 8)) 



/**************************************************************************
 * FUNCTION NAME : marvell_port_read_status
 **************************************************************************
 * DESCRIPTION  : This function reads the port status
 * 		  for WAN, LAN 1-4 .  
 *	
 * RETURNS	: TRUE	- successfull
 * 		  FALSE	- ERROR
 * 		
 ***************************************************************************/
int marvell_port_read_status(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
	int phy_addr = (int)data;
	u32 base_address = AVALANCHE_LOW_EMAC_BASE; 
	volatile u32 readVal;
	int len=0;
	int mode =0;
	
	readVal = marvell_phy_read(base_address, phy_addr, MARVELL_PHY_STATUS_REG);
	readVal &= 0xFFFF;

#define MARVELL_PHY_STATUS_LINK_MASK  	0x4

	if(!(readVal & MARVELL_PHY_STATUS_LINK_MASK))
		mode = MARVELL_PHY_LINK_DOWN_MODE;	

	else
	{	
		readVal = marvell_phy_read(base_address, phy_addr, MARVELL_PHY_CONFIG_REG);
		readVal &= 0xFFFF;
	
		
		if(readVal & MARVELL_PHY_AUTONEG_MASK)
		{
			int port_addr;
			port_addr = MARVELL_PORT_ADDRESS(phy_addr); 
			readVal = marvell_phy_read(base_address, port_addr, MARVELL_PORT_STATUS_REG);
			readVal &= 0xFFFF;

#define MARVELL_PORT_STATUS_AUTONEG_RESOLV_MASK  	(1 << 13)
#define MARVELL_PORT_STATUS_SPEED_MASK  		(1 << 8)
#define MARVELL_PORT_STATUS_DUPLEX_MASK  		(1 << 9)
#define MARVELL_PORT_STATUS_10_HD_MASK  		(0)
#define MARVELL_PORT_STATUS_100_HD_MASK  		(1 << 8)
#define MARVELL_PORT_STATUS_10_FD_MASK  		(1 << 9)
#define MARVELL_PORT_STATUS_100_FD_MASK  		((1 <<8) | (1 << 9))


			if(readVal & MARVELL_PORT_STATUS_AUTONEG_RESOLV_MASK)
			{
				
				readVal &= (MARVELL_PORT_STATUS_SPEED_MASK | MARVELL_PORT_STATUS_DUPLEX_MASK);

				switch(readVal)
				{
					case MARVELL_PORT_STATUS_10_HD_MASK:
						mode = MARVELL_PHY_10_HD_MODE;
					break;
			
					case MARVELL_PORT_STATUS_100_HD_MASK:
						mode = MARVELL_PHY_100_HD_MODE;
					break;

					case MARVELL_PORT_STATUS_10_FD_MASK:
						mode = MARVELL_PHY_10_FD_MODE;
					break;
			
					case MARVELL_PORT_STATUS_100_FD_MASK:
						mode = MARVELL_PHY_100_FD_MODE;
					break;
	
				}

			}
			else /* Auto Neg unresolved */
			{
				mode = MARVELL_PHY_LINK_DOWN_MODE;	
			}
			
		}
		else
		{
			readVal &= (MARVELL_SPEED_MASK | MARVELL_DUPLEX_MASK);

			switch(readVal)
			{
				case MARVELL_PHY_10_HD_MASK:
					mode = MARVELL_PHY_10_HD_MODE;
				break;
			
				case MARVELL_PHY_100_HD_MASK:
					mode = MARVELL_PHY_100_HD_MODE;
				break;


				case MARVELL_PHY_10_FD_MASK:
					mode = MARVELL_PHY_10_FD_MODE;
				break;
			
				case MARVELL_PHY_100_FD_MASK:
					mode = MARVELL_PHY_100_FD_MODE;
				break;
	
			}

		}	
	}	
	
	len += sprintf(buf,"%d\n", mode);	
	return len;
}

                      

/**************************************************************************
 * FUNCTION NAME : marvell_port_read_config
 **************************************************************************
 * DESCRIPTION  : This function reads the port configuration
 * 		  for WAN, LAN 1-4 .  
 *	
 * RETURNS	: TRUE	- successfull
 * 		  FALSE	- ERROR
 * 		
 ***************************************************************************/
int marvell_port_read_config(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
	int phy_addr = (int)data;
	u32 base_address = AVALANCHE_LOW_EMAC_BASE; 
	volatile u32 readVal;
	int len=0;
	int mode =0;
	
	readVal = marvell_phy_read(base_address, phy_addr, MARVELL_PHY_CONFIG_REG);
	readVal &= 0xFFFF;


	if(readVal & MARVELL_PHY_AUTONEG_MASK)
		mode = MARVELL_PHY_AUTONEG_MODE;	

	else
	{	
		readVal &= (MARVELL_SPEED_MASK | MARVELL_DUPLEX_MASK);

		switch(readVal)
		{
			case MARVELL_PHY_10_HD_MASK:
				mode = MARVELL_PHY_10_HD_MODE;
			break;
			
			case MARVELL_PHY_100_HD_MASK:
				mode = MARVELL_PHY_100_HD_MODE;
			break;


			case MARVELL_PHY_10_FD_MASK:
				mode = MARVELL_PHY_10_FD_MODE;
			break;
			
			case MARVELL_PHY_100_FD_MASK:
				mode = MARVELL_PHY_100_FD_MODE;
			break;
	
		}

	}	
	
	len += sprintf(buf,"%d\n", mode);	
	return len;
}

/**************************************************************************
 * FUNCTION NAME : marvell_port_write_config
 **************************************************************************
 * DESCRIPTION  : This function writes the port configuration
 * 		  for WAN, LAN 1-4 .  
 *	
 * RETURNS	: TRUE	- successfull
 * 		  FALSE	- ERROR
 * 		
 ***************************************************************************/
int marvell_port_write_config(struct file *file, const char* buffer, unsigned long count, void *data) 
{
	int phy_addr = (int)data;
	u32 base_address = AVALANCHE_LOW_EMAC_BASE; 
	volatile u32 readVal, writeVal;
	int mode =0;
	int len = count;
	char status[8];
	
	if(copy_from_user(status, buffer, len))
	{
		return -EFAULT;
	}
	
	while(len--)
	{
		if( *status >= '0' &&   *status <= '4')
		{
			mode = *status - '0';
			break;
		}
	}

	readVal = marvell_phy_read(base_address, phy_addr, MARVELL_PHY_CONFIG_REG);
	readVal &= 0xFFFF;

#define MARVELL_PHY_AUTO_RENEG_MASK	(1 << 9)

	if(mode ==MARVELL_PHY_AUTONEG_MODE)
	{
		writeVal =  readVal |  MARVELL_PHY_AUTO_RENEG_MASK | MARVELL_PHY_AUTONEG_MASK;
	}	

	else
	{	
		/* Disable autoneg */
		writeVal = (readVal &  ~MARVELL_PHY_AUTONEG_MASK) | MARVELL_PHY_AUTO_RENEG_MASK;

		writeVal &= ~(MARVELL_SPEED_MASK | MARVELL_DUPLEX_MASK); 

		switch(mode)
		{
			case MARVELL_PHY_10_HD_MODE:
				writeVal  |= MARVELL_PHY_10_HD_MASK;
			break;
			
			case MARVELL_PHY_100_HD_MODE:
				writeVal |= MARVELL_PHY_100_HD_MASK;
			break;


			case MARVELL_PHY_10_FD_MODE:
				writeVal |= MARVELL_PHY_10_FD_MASK;
			break;
			
			case MARVELL_PHY_100_FD_MODE:
				writeVal |= MARVELL_PHY_100_FD_MASK;
			break;
	
		}

	}	
	
		/* write the settings to the port */
		marvell_phy_write (base_address, phy_addr, MARVELL_PHY_CONFIG_REG, writeVal);

		/* read the register value*/
		readVal = marvell_phy_read (base_address, phy_addr, MARVELL_PHY_CONFIG_REG);

		/* Check if write was successfull */
		if ((readVal & 0x80000000) )
    		{
			printk ("\n ERROR: value read from portAddr=0x%X, regAddr=0x%X readValue=0x%X, 
			while written value was 0x%X\n", phy_addr, MARVELL_PHY_CONFIG_REG, readVal, writeVal);
		}
	return count;

}

/**************************************************************************
 * FUNCTION NAME : marvell_set_port_counters
 **************************************************************************
 * DESCRIPTION  : This function sets the counter mode fot TX and Rx counters
 * 		  for WAN, LAN 1-4 and CPU port. The control mode is defined 
 * 		  by MARVELL_PORT_COUNTER_VALUE. When the control bit is set
 * 		  to one counters track the Rx and TX errors. When set to 
 * 		  zero count the number of Tx and RX packets.
 *	
 * RETURNS	: TRUE	- successfull
 * 		  FALSE	- ERROR
 * 		
 ***************************************************************************/
static int marvell_set_port_counters(void)
{
	u32 base_address = AVALANCHE_LOW_EMAC_BASE; 
	u32 phy_address;
	volatile u32 readVal, writeVal;

	phy_address = MARVELL_SWITCH_GLOBAL_REG_OFFSET;

	readVal = marvell_phy_read (base_address, phy_address, MARVELL_SWITCH_GLOBAL_CONTROL_REG);
	readVal &= 0xFFFF;
	writeVal = readVal | MARVELL_PORT_COUNTER_VALUE;

	/* Enable the Rx and Tx counters */
	marvell_phy_write (base_address, phy_address, MARVELL_SWITCH_GLOBAL_CONTROL_REG, writeVal);
	
	/* Read back and check if write failed */
	readVal = marvell_phy_read (base_address, phy_address, MARVELL_SWITCH_GLOBAL_CONTROL_REG);
  
	if ((readVal & 0x80000000) || ((readVal & 0xFFFF) != writeVal))
	{
		printk ("\n ERROR: value read from phy_address=0x%X, regAddr=0x%X readValue=0x%X, 
			written value was 0x%X\n", phy_address, MARVELL_SWITCH_GLOBAL_CONTROL_REG, readVal, writeVal);
		return (FALSE);
	}

	return (TRUE);
}

/**************************************************************************
 * FUNCTION NAME : marvell_receive
 **************************************************************************
 * DESCRIPTION  : This function is called by the EMAC hardware receive function 
 * 		  to determine the source port from the Egress Trailer present 
 * 		  at the end of the packet. Note that the packet length 
 * 		  is decremented to remove the trailer.
 *	
 * RETURNS	: source_port
 * 
 ***************************************************************************/
int marvell_receive(unsigned char *data_ptr, unsigned int *packet_length)
{
#if 0
	int source_port = -1, spid_offset;
	unsigned char* spid_ptr;
	
	spid_offset = *packet_length - SPID_OFFSET; /* This is 3 bytes */
	
	spid_ptr = data_ptr + spid_offset;
	source_port = (*spid_ptr) & SPID_PORT_MASK;	/* Bits 0-2 */
	
	*packet_length -= MARVELL_EGRESS_TRAILER_LENGTH;
#endif
		
#if 0
	printk("source port = %d\n",data_ptr[1] &0xf);
#endif


	return (data_ptr[1] & 0xf);
}

/**************************************************************************
 * FUNCTION NAME : marvell_read_counters
 **************************************************************************
 * DESCRIPTION  : This function is called by the proc to read and display 
 * 		  the marvell Rx and TX counters for WAN, LAN 1-4 and CPU 
 * 		  port.
 *	
 * RETURNS	: TRUE	- successfull
 * 		  FALSE	- ERROR
 * 		
 ***************************************************************************/
int marvell_read_counters(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
	u32 base_address = AVALANCHE_LOW_EMAC_BASE; 
	MARVELL_PORT_ADDRESS portAddr;
	volatile u32 readVal;
	int len=0, index = 1;
  
	len += sprintf(buf+len, "\nRx COUNTER\n");

	/* read the Rx error counter for each port and print */
	for (portAddr = LAN_PORT_1_ADDR; portAddr < CPU_PORT_ADDR; portAddr++) 
	{
		readVal = marvell_phy_read (base_address, portAddr, MARVELL_PORT_RX_COUNTER);
		if (portAddr == WAN_PORT_ADDR)
			len += sprintf (buf+len, "\n %s  - %d\n", "WAN",
				(readVal & 0x0000FFFF));
		else
			len += sprintf (buf+len, "\n %s%d - %d\n", "LAN",
				index++, (readVal & 0x0000FFFF));

	}

	len += sprintf(buf+len, "\nTx COUNTER\n");
	index = 1;
	
	/* read the Tx error counter for each port and print */
	for (portAddr = LAN_PORT_1_ADDR; portAddr < CPU_PORT_ADDR; portAddr++) 
	{
		readVal = marvell_phy_read (base_address, portAddr, MARVELL_PORT_TX_COUNTER);
		if (portAddr == WAN_PORT_ADDR)
			len += sprintf (buf+len, "\n %s  - %d\n", "WAN",
				(readVal & 0x0000FFFF));
		else
			len += sprintf (buf+len, "\n %s%d - %d\n", "LAN",
				index++, (readVal & 0x0000FFFF));
	}
	return (len);
}


/**************************************************************************
 * FUNCTION NAME : marvell_read_ports
 **************************************************************************
 * DESCRIPTION  : This function is used to display the Marvell's port and 
 * 		  Phy status
 *	
 * RETURNS	: NONE
 *
 ***************************************************************************/

int marvell_read_ports(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
	u32 base_address = AVALANCHE_LOW_EMAC_BASE; 
	MARVELL_PORT_ADDRESS portAddr;
	volatile u32 readVal;
	int len=0, phy;
  
	len += sprintf(buf+len, "\nMARVELL PORTS\n");

	for (portAddr = LAN_PORT_1_ADDR; portAddr <= CPU_PORT_ADDR; portAddr++) 
	{
		/* read the register value*/
		readVal = marvell_phy_read (base_address, portAddr, MARVELL_PORT_STATUS_REG);
		len += sprintf (buf+len, "\n 0x%X - 0x%X\n", portAddr, (readVal & 0xFFFF));
		
	}
	
	len += sprintf(buf+len, "\nMARVELL PHYS\n");
	
	for (phy = 0; phy <= 4; phy++)
	{
		readVal = marvell_phy_read (base_address, phy, 0x11);
		len += sprintf (buf+len, "\n 0x%X - 0x%X\n", phy, (readVal & 0xFFFF));
	}

	return (len);
}

int marvell_read_database(char* buf, char **start, off_t offset, int count,int *eof, void *data)
{
	u32 base_address = AVALANCHE_LOW_EMAC_BASE; /* Make this the KSEG1 address instead of KSEG0 address */
	u32 phy_address, reg_address;
	volatile u32 readVal, writeVal, readVal1, readVal2, readVal3, portVec ;
	int len=0, database = LAN_DOMAIN;

	phy_address = MARVELL_SWITCH_GLOBAL_REG_OFFSET;
	reg_address = 0xB;
	
	
	while (database <= WAN_DOMAIN)
	{
		writeVal = 0xc000 | database;

		marvell_phy_write (base_address, phy_address, 0xD, 0);
		marvell_phy_write (base_address, phy_address, 0xE, 0);
		marvell_phy_write (base_address, phy_address, 0xF, 0);
	
		len += sprintf(buf+len, "\nMarvell Database : %x\n", database);
		while ((readVal1 != 0xFFFF) && (readVal2 != 0xFFFF) && (readVal3 != 0xFFFF))
		{
			readVal = marvell_phy_read (base_address, phy_address, reg_address);

			if ((readVal & 0x8000) != 0)
				return (len);
		
			marvell_phy_write (base_address, phy_address, reg_address, writeVal);
			readVal = marvell_phy_read (base_address, phy_address, reg_address);

			while ((readVal & 0x8000) != 0)
				readVal = marvell_phy_read (base_address, phy_address, reg_address);

			portVec = marvell_phy_read (base_address, phy_address, 0xC);
			portVec &= 0xFFFF;
		
			readVal1 = marvell_phy_read (base_address, phy_address, 0xD);
			readVal1 &= 0xFFFF;
			
			readVal2 = marvell_phy_read (base_address, phy_address, 0xE);
			readVal2 &= 0xFFFF;
		
			readVal3 = marvell_phy_read (base_address, phy_address, 0xF);
			readVal3 &= 0xFFFF;
		
			len += sprintf (buf+len, "0x%X - 0x%X%X%X\n", portVec, readVal1, readVal2, readVal3);
		}
		database++;
		readVal1 = readVal2 = readVal3 = 0;
	}
	return (len);
}

