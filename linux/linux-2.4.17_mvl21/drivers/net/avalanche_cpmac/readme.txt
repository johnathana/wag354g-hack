23 August 2004      CPMAC 1.7.8 (NSP Performance Team Release)

CC Labels: REL_20040823_HALdallas_cpmac_01.07.08

New features:  Key "MacAddr" can now be used to set the Mac Address after Open. 

 unsigned char MacAddr[6];

  // Set Mac Address to "00.B0.D0.10.80.C1"     
  MacAddr[0] = 0x00;
  MacAddr[1] = 0xB0;
  MacAddr[2] = 0xD0;
  MacAddr[3] = 0x10;
  MacAddr[4] = 0x80;
  MacAddr[5] = 0xC1;
      
  HalFunc->Control(HalDev, "MacAddr", hcSet, &MacAddr);      

Bug fixes: in Send(), Threshold is not checked if Tx Ints are re-enabled.

Modules affected:  hcpmac.c, hcpmac.h, cppi_cpmac.c

22 June 2004          CPMAC 1.7.6 (NSP Performance Team Release)

CC Labels: REL_20040622_HALdallas_cpmac_01.07.06

New features:  Key "TxIntDisable" used to disable Tx Interrupts. If it is set, then Tx Interrupts will be processed on Send() controlled by Tx ServiceMax Setting. 

 int On = 1;
 HalFunc->Control(HalDev, "TxIntDisable", "Set", &On);

Bug fixes: NTR

10 June 2004          CPMAC 1.7.5 (external release)

CC Labels: REL_20040610_HALdallas_cpmac_01.07.05

New features:  NTR

Bug fixes: Fixed an issue with calculation for the multicast hash.

27 May 2004          CPSAR 1.7.4, CPMAC 1.7.4 (external release)

CC Labels: REL_20040527_HALdallas_cpsar_01.07.04
           REL_20040527_HALdallas_cpmac_01.07.04

New features:  NTR

Bug fixes: A flaw was fixed in the critical sectioning of the CPPI file, affecting both
           the MAC and the SAR releases.  This flaw was detected on Titan PSP 4.7 BFT2.

05 May 2004          CPSAR 1.7.3, CPMAC 1.7.3 (external release)

CC Labels: REL_20040505_HALdallas_cpsar_01.07.03
                REL_20040505_HALdallas_cpmac_01.07.03

New features:  NTR

Bug fixes: 1) Firmware has been updated to fix a problem with Host OAM mode operation.  
               2) Cache macros have been fixed.

Notes:  This release contains all performance enhancements currently available for CPHAL 1.x.  

19 April 2004          CPSAR 1.7.2, CPMAC 1.7.2 (external release)

CC Labels: REL_20040419_HALdallas_cpsar_01.07.02
                REL_20040419_HALdallas_cpmac_01.07.02

New features:  NTR

Bug fixes: Fixes merge problem in 1.7.1.

Notes:  This is a branch release which contains only a subset of the performance improvements.
            The remaining performance improvements are stiill being qualified at this time.

1 April 2004          CPSAR 1.7.1, CPMAC 1.7.1 (external release)

NOTICE: DO NOT USE 1.7.1.  It has a known problem (see 1.7.2 notes)

CC Labels: REL_20040401_HALdallas_cpsar_01.07.01
                REL_20040401_HALdallas_cpmac_01.07.01

New features:  Performance improvement in CPPI layer, affecting both CPSAR and CPMAC.  

Bug fixes: NTR

17 Februrary 2004    CPSAR 1.7.0 (external release)

CC Labels: REL_20040217_HALdallas_cpsar_01.07.00

New features:  Added support for "TxFlush" feature.  This allows the upper
               layer to flush all or part of a given Tx queue for a given
               channel.  This is to be used during call setup for a voice
               connection.

30 January 2004      CPMAC 1.7.0 (external release)

CC Labels: REL_20040130_HALdallas_cpmac_01.07.00

Bug fixes:    CPMDIO - When in manual negotiate mode and linked, dropping link would move into NWAY state rather than manual state.
              CPMDIO - Extraneous debug message corrected
New features: CPMDIO - Support for AutoMdix usage added. 

25 September 2003      CPSAR 1.6.6 (external release)

CC Labels: REL_20030925_HALdallas_cpsar_01.06.06

Bug fixes:   PDSP firmware has been updated to fix the OAM padding problem.  It previously
             wrote pad bytes into a reserved field of the OAM cell.  There is a small
             change to the CPSAR configuration code which corresponds to the PDSP spec
             change.

New features: NTR

09 September 2003      CPMAC 1.6.6 (external release)

CC Labels: REL_20030909_HALdallas_cpmac_01.06.06

Bug fixes:   CPMAC : When _CPMDIO_NOPHY is set, Cpmac COntrol is set to Full Duplex
             Bridge loopback test does not show a problem using 1.6.5 if packet rate is
             below 50,000 pbs. Now testing with a 100% send from Ixia.

New features: NTR

05 August 2003         CPHAL 1.6.5 (external release)

CC Labels: REL_20030805_HALdallas_cpmac_01.06.05

Bug fixes: NTR

New features:  CPMAC : Added support for CPMAC modules that do not have a Phy connected.
               The CPMAC is informed of this by the MdioConnect option
               _CPMDIO_NOPHY. This is the only driver change needed to
               receive and transmit packets through the Marvel switch.
               Note  In this mode Link status will reported linked at 100/FD to
               PhyNum 0xFFFFFFFF.

               ALL:  Cleaned up some Vlynq support logic.

16 July 2003         CPSAR 1.6.3 (external release), no CPMAC release

CC Labels: REL_20030716_HALdallas_cpsar_01.06.03

Bug fixes: 1) Changed default value of CPCS_UU from 0x5aa5 to 0.  The old default value caused
              problems with Cisco routers.

New features:  NTR

Known issues not addressed in this release: NTR.

01 July 2003         CPHAL 1.6.2 (external release)

CC Labels: REL_20030701_HALdallas_cpmac_01.06.02
           REL_20030701_HALdallas_cpsar_01.06.02

Bug fixes: 1) A previous firmware upgrade caused firmware OAM loopback cells to only work on every other
              command.  This has been fixed in the new firmware version (0.47).
           2) Problem with PTI values changing on transparent mode packets has been resolved.
           3) Previously, successful firmware OAM loopback cells waited 5 seconds before notifying the
              OS of success, rather that notifying immediately.  This has been resolved in firmware.
           4) PITS #148 (MAC and SAR), #149 (MAC) have been fixed.

New features: 1) AAL5 HAL now capable of receiving unknown VCI/VPI cells on a single transparent channel.
                 See updated HAL document (AAL5 appendix) for implementation details.
              2) AAL5 HAL now allows OS to modify the OAM loopback timeout window.  Previously, failed
                 OAM loopback attempts timed out after a nominal 5 seconds (based on the SAR frequency
                 provided by the OS).  Now, the default is 5 seconds, but the OS may change the
                 value via halControl() to any integer number of milliseconds.  See updated HAL document
                 (AAL5 appendix) for implementation details.
              3) MAC (cpmdio): added loopback to Istate.  Used for debug.

Known issues not addressed in this release: NTR.

09 June 2003         CPSAR 1.6.1 (external release), CPMAC 1.6.1 (internal release - no functional change)

Note: This is the same set of fixes being applied to 1.6.0 that were applied to 1.5.3.  The only difference
      between 1.6.1 and 1.5.4 is that 1.6.1 has the TurboDSL fix.

CC Labels: REL_20030609_HALdallas_cpmac_01.06.01
           REL_20030609_HALdallas_cpsar_01.06.01

Bug fixes: 1) Bug in OamLoopbackConfig fixed.
           2) New firmware version (.43) to fix Westell issue of dropped downstream packets in
              presence of OAM traffic when operating at or near line rate.

New features: NTR.

09 June 2003         CPSAR 1.5.4 (external release), CPMAC 1.5.4 (internal release - no functional change)

Note:  This is a branch release from 1.5.3.  This does not contain anything from 1.6.0.  The CPMAC is
only being labeled to keep the release flow consistent.

CC Labels: REL_20030609_HALdallas_cpmac_01.05.04
           REL_20030609_HALdallas_cpsar_01.05.04

Bug fixes: 1) Bug in OamLoopbackConfig fixed.
           2) New firmware version (.43) to fix Westell issue of dropped downstream packets in
              presence of OAM traffic when operating at or near line rate.

New features: NTR.

30 May 2003         CPSAR 1.6.0 (external release), CPMAC 1.6.0 (internal release - no functional change)

CC Labels: REL_20030530_HALdallas_cpmac_01.06.00
           REL_20030530_HALdallas_cpsar_01.06.00

Bug fixes: 1) TurboDSL issue has been fixed with a software workaround in TxInt.  This workaround
              has been verified under Adam2 ONLY at this point.  Testing remains to be done on
              Linux and VxWorks.

New features: NTR.

Known issues not addressed in this release: NTR.

30 May 2003         CPSAR 1.5.3 (external release), CPMAC 1.5.3 (internal release - no functional change)

CC Labels: REL_20030530_HALdallas_cpmac_01.05.03
           REL_20030530_HALdallas_cpsar_01.05.03

Bug fixes: NTR.

New features: 1) AAL5 Send() has been modified to accept an ATM Header either in the first
                 fragment by itself, or in the first fragment directly in front of payload data.
                 The API() does not change.
              2) Documentation updates throughout, reflected in latest version of CPHAL user's
                 guide.
              3) AAL5 MaxFrags default value is now 46.  This is based upon the default AAL5
                 RxBufSize of 1518 (MaxFrags = (65568/1518) + 2).  IF THE OS CHOOSES A SMALLER
                 RxBufSize, IT MUST INCREASE THE VALUE OF MaxFrags ACCORDINGLY.  This is done
                 via halControl(), prior to Open().

Known issues not addressed in this release:
              1) The Linux SAR driver is seeing an issue in which it cannot
                 reliably send traffic simultaneously on both the high and
                 low priority queues of a single AAL5 channel. (TurboDSL)

23 May 2003         CPHAL 1.5.2 (external release)

CC Labels: REL_20030523_HALdallas_cpmac_01.05.02
           REL_20030523_HALdallas_cpsar_01.05.02

Bug fixes: 1) PITS #138: CPMAC flooding issue resolved.
           2) PITS #142: OS may now set "MaxFrags" via Control().  This controls the
              maximum number of fragments expected by the CPHAL.  The default value is 2 for
              CPMAC and 1028 for AAL5.  If the OS chooses a RxBufSize that will cause more
              fragments than the defaults, the OS must set "MaxFrags" to a correct value
              ((maximum packet length / RxBufSize) + 2).
           3) PITS #143: Fixed.
           4) Firmware OAM bug fixed. (new firmware release in this version)

New features: NTR.

Known issues not addressed in this release:
              1) The Linux SAR driver is seeing an issue in which it cannot
                 reliably send traffic simultaneously on both the high and
                 low priority queues of a single AAL5 channel. (TurboDSL)

14 May 2003         CPHAL 1.5.1 (external release)

CC Labels: REL_20030514_HALdallas_cpmac_01.05.01
           REL_20030514_HALdallas_cpsar_01.05.01

Bug fixes:    1) PITS 132 - (CPMAC) Frames < 60 bytes and split into
                             multi-fragments.
              2) BCIL MR PSP00000353 - (CPMAC) PhyDev not free'd on halClose()
              3) PITS 113 - OsSetup bug in ChannelSetup fixed.
              4) Fixed AAL5 to check return values of InitTcb/InitRcb.
              5) Fixed Shutdown to properly free resources in the case of a Close
                 mode 1 followed by Shutdown.  Previously, buffer and descriptor
                 resources were left unfreed in this case.

New features: 1) AAL5 Send() modified to be capable of accepting ATM header as first four
                 bytes of first fragment.  This allows the OS to "override" the
                 default ATM header which is constructed from preconfigured channel
                 parameters.
              2) AAL5 Receive() modified to be capable of passing the received ATM header (4 bytes, no HEC)
                 in the first fragment (by itself).  It also passes up the OS an indication
                 of what the received packet type was.  For Host OAM and transparent mode
                 packets, the ATM header is passed in this manner, and for other types of packets
                 (AAL5, NULL AAL) no ATM header is passed currently.

Known issues not addressed in this release:
              1) The Linux SAR driver is seeing an issue in which it cannot
                 reliably send traffic simultaneously on both the high and
                 low priority queues of a single AAL5 channel.

30 April 2003         CPHAL 1.5.0 (external release)

CC Labels: REL_20030430_HALdallas_cpmac_01.05.00
           REL_20030430_HALdallas_cpsar_01.05.00

Bug fixes:   1) Fixed AAL5 bug that rendered the low priority queue
                unusable.
             2) Fixed a bug in AAL5's Oam Rate calculations.
             3) Fixed use of "DeviceCPID" key in AAL5's halControl().
             4) Fixed RxReturn logic in HAL.  The HAL now can handle
                failing MallocRxBuffer calls when multiple fragments
                are being used.

New features: 1) AAL5 Stats now available on a per queue basis.
              2) AAL5 adds two new keys to halControl() for "Set" actions:
                 RxVc_OamCh and RxVp_OamCh.
              3) Shutdown() has been modified for both AAL5 and CPMAC to
                 call Close() if the module is still in the Open state.
              4) CPMAC adds the following access keys to halControl():
                 hcPhyAccess,hcPhyNum,hcCpmacBase,hcSize,and hcCpmacSize.
              5) CPHAL no longer requests an extra 15 bytes on data buffer
                 mallocs.

Known issues not addressed in this release:
              1) The Linux SAR driver is seeing an issue in which it cannot
                 reliably send traffic simultaneously on both the high and
                 low priority queues of a single AAL5 channel.

21 April 2003         CPHAL 1.4.1 (external release)

CC Labels: REL_20030421_HALdallas_cpmac_01.04.01
           REL_20030421_HALdallas_cpsar_01.04.01

Bug fixes:   1) Fixed OAM logic in SAR portion of CPHAL.

New features: 1) OAM loopback counters exposed through halControl.
              2) Host OAM Send() can now use a single channel to send
                 OAM cells on unlimited number of VP's/VC's.
              3) CPHAL now requests "SarFreq" through osControl.
              4) CPHAL now calculates all OAM function rates based on
                 "SarFreq"; function OamRateConfig removed for API.
              5) New OAM function OamLoopbackConfig, used for configuring
                 loopback functions in firmware OAM mode.

Known issues not addressed in this release:  Bug fix 1) in release 1.4
             (see below) does not work properly for multiple fragments.

10 April 2003         CPHAL 1.4 (external release)

CC Labels: REL_20030410_HALdallas_cpmac_01.04.00
           REL_20030410_HALdallas_cpsar_01.04.00

This release is for SAR and MAC.

  Bug fixes: 1) Implemented logic in HAL to re-request buffer mallocs
                in the case of MallocRxBuffer failing.  The HAL now maintains
                a NeedsBuffer queue of all RCB's that are without buffers.
                On interrupts, or on Send(), the HAL checks to see if any
                RCB's are on the queue, and if so, calls MallocRxBuffer
                to attempt to get a new buffer and return the RCB to
                circulation.
             2) SAR now properly returns all error codes from halOpen and
                halChannelSetup.

  New features: NTR

  Known issues not addressed in this release: NTR

08 April 2003         CPHAL 1.3.1 (internal release - SAR only)

  CC Labels: REL_20030408_HALdallas_cpsar_01.03.01

  This is a SAR only release.  The current CPMAC release is still 1.3.

  Bug fixes: 1) PDSP State RAM / Scratchpad RAM is now completely cleared after reset.
                This resolves a stability issue.

  New features: 1) OamMode is now a parameter in halControl().  Both "Set" and "Get"
                   actions are available.  The value may be "0" (Host OAM), or "1"
                   (Firmware OAM).

  Known issues not addressed in this release:
             1) Appropriate action for HAL in the case of MallocRxBuffer failing.  We
                are investigating whether the HAL should implement a needs buffer
                queue.

04 April 2003         CPHAL 1.3 (external release)

  CC Labels: REL_20030404_HALdallas_cpmac_01.03.00
             REL_20030404_HALdallas_cpsar_01.03.00
             REL_20030404_HALdallas_cpaal5_01.03.00
             REL_20030404_HALdallas_cpaal2_01.03.00

  This release requires no changes for the ethernet end driver.  The changes necessary
  for the sar driver (firmware file name changes) have already been implemented.

  Bug fixes: 1) RxReturn now returns an error if MallocRxBuffer fails.  On RxReturn error, the driver should
                call RxReturn again at a later time (when the malloc may succeed) in order for the CPHAL
                to maintain a full complement of Rx buffers.  We recommend holding off making this driver
                change until we verify that this condition occurs.

  New features: 1) Removed benign compiler warnings.
                2) PITS 122: http://www.nbu.sc.ti.com/cgi-bin/pits/redisplay_archive?product=cphal_dev&report=122
                3) Cpsar label (above) now is applied to everything
                   beneath /cpsar.
                4) PITS 14: http://www.nbu.sc.ti.com/cgi-bin/pits/redisplay_archive?product=cphal_dev&report=14
                   Transferred to MR PSP 00000089.
                5) PITS 120: http://www.nbu.sc.ti.com/cgi-bin/pits/redisplay_archive?product=cphal_dev&report=120

  Known issues not addressed in this release:
             1) PITS 102 (as relating to OamMode configuration):
                http://www.nbu.sc.ti.com/cgi-bin/pits/redisplay_archive?product=cphal_dev&report=102
                Future release will make OamMode configurable
                through halControl(), not on per channel basis.

20 March 2003         CPHAL 1.2.1 (internal release)

  CC Labels: REL_20030320_HALdallas_cpmac_01.02.01
             REL_20030320_HALdallas_cpsar_01.02.01
             REL_20030320_HALdallas_cpaal5_01.02.01
             REL_20030320_HALdallas_cpaal2_01.02.01

  Bug fixes: 1. Fixed modification of buffer pointer following
                MallocRxBuffer in cppi.c.
             2. Removed extra firmware files from /cpsar.

  New features: NTR.

  Known issues not addressed in this release: NTR.

07 March 2003         CPHAL 1.2 (external release)

  CPMAC/CPSAR feature complete release.  SAR added
  several features including full OAM support and various
  other features and bug fixes to address PITS 99-106, and
  114.  CPMAC cleaned up details raised by India PSP
  team.

29 January 2003       CPHAL RC 3.01a (external release)

  Corrects non-static functions to be static in cppi.c.

09 Janurary 2003      CPHAL RC 3.01 (external release)

  PITS 88: Fixed MDIO re-connection problem (hcpmac.c)
  PITS 90: Corrected Rx Buffer Pointer modification (cppi.c)

  Corrected error in cpremap.c

20 December 2002      CPHAL RC 3 (external release)

  Statistics support via halControl().  See Appendix A of guide.
  Fixed errors in ChannelTeardown/ChannelSetup CPHAL logic.
  Added multicast support as requested.
  Several new OS string functions added to OS_FUNCTIONS.
  "DebugLevel" configuration parameter changed to "Debug".
  "Stats0" changed to "StatsDump" for CPMAC.

13 December 2002      CPHAL RC 2.03 (internal release)

  Performance improvements.
  More debug statements implemented (esp AAL5).
  Updated makefile with "make debug" option.
  Hbridge performance: [debug library] 15774 tps (53% line rate)
                   [non-debug library] 13700 tps (46%)

10 December 2002      CPHAL Release Candidate 2.02 (internal release)

 Much of the configuration code internal to CPMAC and AAL5 has been made common.
 [os]Receive API had been modified to remove OsReceiveInfo.  This information is now
   available as third member of the FRAGLIST structure, on a per buffer basis.
 Successfully tested multi-fragment support on CPMAC, using 32 byte buffers.
 Code is now Emerald compliant - all buffer descriptors now aligned to cache-line
   boundaries.

2 December 2002      CPHAL Release Candidate 2.01

 Updates to comments in hcpmac.c, cpmdio.c, hcpmac.h
 Nested comment in hcpmac.c in RC2 can cause compile errors.

25 November 2002      CPHAL Release Candidate 2

Project Items not completed for RC2
#6  Ship as Library -  Once under CC. Moved to RC3
#8  Under Clearcase -  Moved to RC3
#25 Emerald compliant - Moved to RC3
#26 Statistics support - Moved to RC3 (some support in RC2)
#36 Debug scheme implemented - Moved to RC3 (some support in RC2)

8 November 2002       CPHAL Release Candidate 1

Notes:

Project Items not completed for RC1

#8  Under Clearcase -  Clearcase server failure this week. Moved to RC2
#6  Ship as Library -  Once under CC. Moved to RC2
#13 Verify Datatypes. Moved to RC2
#14 Review APIs.  Moved to RC2

APIs under review for RC2

halIsr()
hslRxReturn()
halSend()
osSendComplete()
osReceive()


CPMAC Build Instructions

Compile the file 'hcpmac.c'.


AAL5 Build Instructions

The AAL5 build is composed of the source files aal5sar.c and cpsar.c.
Refer to the provided makefile for an example of compiling these files
into a library.

Example CPHAL Code

CPMAC:

Example CPMAC code is provided in the file hbridge.c.
This program is provided simply as an example of using the CPHAL API.
It is not intended to be compiled and executed in your environment.

AAL5:

Example AAL5 code is provided in the file loopback.c.  This program
is provided simply as an example of using the CPHAL API.  It is not
intended to be compiled and executed in your environment.


Performance Baseline


Cpmac

RC1: hbridge.bin, running with IXIA cpahl_1.cfg.
     This sends 64-byte packets from each Ixia port, with mac destination the other Ixia port.
     MIPS core 4Kc.

RC2: hbridge.bin, running with IXIA cpahl_1.cfg.
     This sends 64-byte packets from each Ixia port, with mac destination the other Ixia port.
     MIPS core 4Ke.
     CPHAL now includes Emerald support, but this has been disabled by using 'cache -wt' to emulate 4Kc.

RC3: hbridge.bin, running with IXIA cpahl_1.cfg.
     This sends 64-byte packets from each Ixia port, with mac destination the other Ixia port.
     MIPS core 4Ke.
     Running as Emerald processor.

Release  Total Receive Rate    Throughput Setting

RC1             11300                 38%
RC2              9524                 32%
RC3             15190                 51%
