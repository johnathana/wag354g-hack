var sectopmenu = new Object();
sectopmenu.firewall="Firewall";

sectopmenu.vpn="VPN";

var secleftmenu = new Object();
//Firewall.asp
secleftmenu.firewall="Firewall";

secleftmenu.seclevel="Security Level";

secleftmenu.adfilters="Addtional Filters";

secleftmenu.blowanreq="Block WAN Requests";

//ipsec.asp
secleftmenu.vpnpass="VPN Passthrough";

secleftmenu.ipsecvpntunnel="IPSec VPN Tunnel";

secleftmenu.localsecgrp="Local Secure Group";

secleftmenu.localsecgw="Local Security Gateway";

secleftmenu.remotesecgrp="Remote Secure Group";

secleftmenu.remotesecgw="Remote Security Gateway";

secleftmenu.keymgnt="Key Management";

secleftmenu.statu="Status";

//Below is content
//Firewall.asp
var secfirewall = new Object();
secfirewall.fwprotection="Firewall Protection";

secfirewall.high="High";

secfirewall.medium="Medium";

secfirewall.low="Low";

secfirewall.custom="Custom";

secfirewall.filterproxy="Filter Proxy";

secfirewall.filtercookies="Filter Cookies";

secfirewall.filterapplets="Filter Java Applets";

secfirewall.filteractivex="Filter ActiveX";

secfirewall.blockinfo="Block Anonymous Internet Requests";

//ipsec.asp
var secvpnpass = new Object();
secvpnpass.ipsecpass="IPSec Passthrough";

secvpnpass.pppoepass="PPPoE Passthrough";

secvpnpass.pptppass="PPTP Passthrough";

secvpnpass.l2tppass="L2TP Passthrough";

var secipsectunnel = new Object();
secipsectunnel.selentry="Select Tunnel Entry";

secipsectunnel.ipsectunnel="IPSec VPN Tunnel";

secipsectunnel.tunnelname="Tunnel Name";

secipsectunnel.encryption="Encryption";

secipsectunnel.auth="Authentication";

secipsectunnel.keytime="Key Lifetime";

secipsectunnel.presharedkey="Pre-shared Key";

//other files
var secother = new Object();
secother.encrypkey="Encryption Key";

secother.authkey="Authentication Key";

secother.inboundspi="Inbound SPI";

secother.outboundspi="Outbound SPI";

secother.thisgw="This Gateway accepts requests from any IP address!";

secother.thesame="the same as Remote Security Gateway setting!";

secother.doname="Domain Name";

secother.vpnsum="VPN Settings Summary";

secother.tuname="Tunnel Name";

secother.statu="Status";

secother.localgrp="Local Group";

secother.remotegrp="Remote Group";

secother.remotegw="Remote Gateway";

secother.secmethod="Security Method";

secother.vpnlog="VPN Log";

secother.upnplog="UPNP Log";

secother.syslog="System Log";

secother.acclog="Access Log";

secother.fwlog="Firewall Log";

secother.pagefresh="pageRefresh";

secother.prepage="Previous Page";

secother.nextpage="Next Page";

secother.outgoing="Outgoing Log Table";

secother.lanip="LAN IP";

secother.desturlip="Destination URL/IP";

secother.srvportnum="Service/Port Number";

//ipsecadv.asp
var ipsecadv = new Object();
ipsecadv.advsetup="Advanced IPSec VPN Tunnel Setup";

ipsecadv.phase="Phase";

ipsecadv.opmode="Operation mode";

ipsecadv.mainmode="Main mode";

ipsecadv.aggmode="Aggressive mode";

ipsecadv.proposal="Proposal";

ipsecadv.encryption="Encryption";

ipsecadv.auth="Authentication";

ipsecadv.group="Group";

ipsecadv.keytime="Key Lifetime";

ipsecadv.note="Note:Following three additional proposals are also proposed in Main mode";

ipsecadv.pfs="PFS";

ipsecadv.othersetting="Other Settings";

ipsecadv.nat="NAT Traversal";

ipsecadv.bios="NetBIOS broadcast";

ipsecadv.keep="Keep-Alive";

ipsecadv.ifthan="If IKE failed more than ";

ipsecadv.mid="times, block this unauthorized IP for";

ipsecadv.ipfor="IP for";

ipsecadv.seconds="seconds";

ipsecadv.antireplay="Anti-replay";
