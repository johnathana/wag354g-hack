var sectopmenu = new Object();
sectopmenu.firewall="Brandv�gg";

sectopmenu.vpn="VPN";

var secleftmenu = new Object();
//Firewall.asp
secleftmenu.firewall="Brandv�gg";

secleftmenu.seclevel="S�kerhetsniv�";

secleftmenu.adfilters="Ytterligare filter";

secleftmenu.blowanreq="Blockera WAN-beg�ran";

//ipsec.asp
secleftmenu.vpnpass="VPN-genomstr�mning";

secleftmenu.ipsecvpntunnel="VPN-tunnel med IPSec";

secleftmenu.localsecgrp="Lokal s�kerhetsgrupp";

secleftmenu.localsecgw="Lokal s�kerhetsgateway";

secleftmenu.remotesecgrp="Fj�rrs�kerhetsgrupp";

secleftmenu.remotesecgw="Fj�rrs�kerhetsgateway";

secleftmenu.keymgnt="Hantering av nycklar";

secleftmenu.statu="Status";

//Below is content
//Firewall.asp
var secfirewall = new Object();
secfirewall.fwprotection="Brandv�ggsskydd";

secfirewall.high="H�g";

secfirewall.medium="Medelh�g";

secfirewall.low="L�g";

secfirewall.custom="Anpassad";

secfirewall.filterproxy="Proxyfiltrering";

secfirewall.filtercookies="Filtrera cookies";

secfirewall.filterapplets="Filtrera Java-appletprogram";

secfirewall.filteractivex="Filtrera ActiveX";

secfirewall.blockinfo="Blockera anonyma Internetbeg�ran";

//ipsec.asp
var secvpnpass = new Object();
secvpnpass.ipsecpass="IPSec-genomstr�mning";

secvpnpass.pppoepass="PPPoE-genomstr�mning";

secvpnpass.pptppass="PPTP-genomstr�mning";

secvpnpass.l2tppass="L2TP-genomstr�mning";

var secipsectunnel = new Object();
secipsectunnel.selentry="V�lj tunnel";

secipsectunnel.ipsectunnel="VPN-tunnel med IPSec";

secipsectunnel.tunnelname="Tunnelnamn";

secipsectunnel.encryption="Kryptering";

secipsectunnel.auth="Autentisering";

secipsectunnel.keytime="Nyckellivsl�ngd";

secipsectunnel.presharedkey="F�r-delad nyckel";

//other files
var secother = new Object();
secother.encrypkey="Krypteringsnyckel";

secother.authkey="Autentiseringsnyckel";

secother.inboundspi="Inkommande SPI";

secother.outboundspi="Utg�ende SPI";

secother.thisgw="Den h�r gatewayen accepterar beg�ran fr�n alla IP-adresser.";

secother.thesame="samma som inst�llningen f�r fj�rrs�kerhetsgateway.";

secother.doname="Dom�nnamn";

secother.vpnsum="VPN-inst�llningssammanfattning";

secother.tuname="Tunnelnamn";

secother.statu="Status";

secother.localgrp="Lokal grupp";

secother.remotegrp="Fj�rrgrupp";

secother.remotegw="Fj�rrgateway";

secother.secmethod="S�kerhetsmetod";

secother.vpnlog="VPN-logg";

secother.upnplog="UPNP-logg";

secother.syslog="Systemlogg";

secother.acclog="�tkomstlogg";

secother.fwlog="Brandv�ggslogg";

secother.pagefresh="Siduppdatering";

secother.prepage="F�reg�ende sida";

secother.nextpage="N�sta sida";

secother.outgoing="Utg�ende loggtabell";

secother.lanip="LAN-IP-adress";

secother.desturlip="URL/IP-adress f�r m�l";

secother.srvportnum="Tj�nst/Portnummer";

//ipsecadv.asp
var ipsecadv = new Object();
ipsecadv.advsetup="Avancerad konfiguration f�r VPN-tunnel med IPSec";

ipsecadv.phase="Fas";

ipsecadv.opmode="Driftl�ge";

ipsecadv.mainmode="Huvudl�ge";

ipsecadv.aggmode="Aggressivt l�ge";

ipsecadv.proposal="F�rslag";

ipsecadv.encryption="Kryptering";

ipsecadv.auth="Autentisering";

ipsecadv.group="Grupp";

ipsecadv.keytime="Nyckellivsl�ngd";

ipsecadv.note="Obs! F�ljande tre ytterligare f�rslag f�resl�s �ven i huvudl�get";

ipsecadv.pfs="PFS";

ipsecadv.othersetting="Andra inst�llningar";

ipsecadv.nat="NAT Traversal";

ipsecadv.bios="NetBIOS-broadcast";

ipsecadv.keep="Beh�ll anslutning";

ipsecadv.ifthan="Om IKE misslyckades mer �n ";

ipsecadv.mid="g�nger ska den h�r obeh�riga IP-adressen blockeras i";

ipsecadv.ipfor="IP i";

ipsecadv.seconds="sekunder";

ipsecadv.antireplay="Anti-replay";
