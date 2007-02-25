#ifndef EMACMDIO_INC
#define EMACMDIO_INC

#define NWAY_FD100          (1<<8)
#define NWAY_HD100          (1<<7)
#define NWAY_FD10           (1<<6)
#define NWAY_HD10           (1<<5)
#define NWAY_AUTO           (1<<0)

#include <asm/types.h>

/*Called once at the begining of time                                        */

void EmacMdioInit(u32 macbase, u32 *PhyState, u32 cpufreq,int verbose);

/*Called every 10 mili Seconds, returns TRUE if there has been a mode change */

int EmacMdioTic(u32 macbase,u32 *PhyState);

/*Called to set Phy mode                                                     */

void EmacMdioSetPhyMode(u32 macbase,u32 *PhyState,u32 PhyMode);

/*Calls to retreive info after a mode change!                                */

int  EmacMdioGetDuplex(u32 macbase,u32 *PhyState);   /*Get The Duplex info   */
int  EmacMdioGetSpeed(u32 macbase,u32 *PhyState);    /*Get the Speed Info    */
int  EmacMdioGetPhyNum(u32 macbase,u32 *PhyState);   /*Get The Phy Number    */
int  EmacMdioGetLinked(u32 macbase,u32 *PhyState);   /*Get The Phy Linked State*/
void EmacMdioLinkChange(u32 macbase,u32 *PhyState);  /*Used to report a link change event!*/
#endif
