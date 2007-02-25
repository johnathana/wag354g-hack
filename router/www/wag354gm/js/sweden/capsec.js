var sectopmenu = new Object();
sectopmenu.firewall="Brandvägg";

sectopmenu.vpn="VPN";

var secleftmenu = new Object();
//Firewall.asp
secleftmenu.firewall="Brandvägg";

secleftmenu.seclevel="Säkerhetsnivå";

secleftmenu.adfilters="Ytterligare filter";

secleftmenu.blowanreq="Blockera WAN-begäran";

//ipsec.asp
secleftmenu.vpnpass="VPN-genomströmning";

secleftmenu.ipsecvpntunnel="VPN-tunnel med IPSec";

secleftmenu.localsecgrp="Lokal säkerhetsgrupp";

secleftmenu.localsecgw="Lokal säkerhetsgateway";

secleftmenu.remotesecgrp="Fjärrsäkerhetsgrupp";

secleftmenu.remotesecgw="Fjärrsäkerhetsgateway";

secleftmenu.keymgnt="Hantering av nycklar";

secleftmenu.statu="Status";

//Below is content
//Firewall.asp
var secfirewall = new Object();
secfirewall.fwprotection="Brandväggsskydd";

secfirewall.high="Hög";

secfirewall.medium="Medelhög";

secfirewall.low="Låg";

secfirewall.custom="Anpassad";

secfirewall.filterproxy="Proxyfiltrering";

secfirewall.filtercookies="Filtrera cookies";

secfirewall.filterapplets="Filtrera Java-appletprogram";

secfirewall.filteractivex="Filtrera ActiveX";

secfirewall.blockinfo="Blockera anonyma Internetbegäran";

//ipsec.asp
var secvpnpass = new Object();
secvpnpass.ipsecpass="IPSec-genomströmning";

secvpnpass.pppoepass="PPPoE-genomströmning";

secvpnpass.pptppass="PPTP-genomströmning";

secvpnpass.l2tppass="L2TP-genomströmning";

var secipsectunnel = new Object();
secipsectunnel.selentry="Välj tunnel";

secipsectunnel.ipsectunnel="VPN-tunnel med IPSec";

secipsectunnel.tunnelname="Tunnelnamn";

secipsectunnel.encryption="Kryptering";

secipsectunnel.auth="Autentisering";

secipsectunnel.keytime="Nyckellivslängd";

secipsectunnel.presharedkey="För-delad nyckel";

//other files
var secother = new Object();
secother.encrypkey="Krypteringsnyckel";

secother.authkey="Autentiseringsnyckel";

secother.inboundspi="Inkommande SPI";

secother.outboundspi="Utgående SPI";

secother.thisgw="Den här gatewayen accepterar begäran från alla IP-adresser.";

secother.thesame="samma som inställningen för fjärrsäkerhetsgateway.";

secother.doname="Domännamn";

secother.vpnsum="VPN-inställningssammanfattning";

secother.tuname="Tunnelnamn";

secother.statu="Status";

secother.localgrp="Lokal grupp";

secother.remotegrp="Fjärrgrupp";

secother.remotegw="Fjärrgateway";

secother.secmethod="Säkerhetsmetod";

secother.vpnlog="VPN-logg";

secother.upnplog="UPNP-logg";

secother.syslog="Systemlogg";

secother.acclog="Åtkomstlogg";

secother.fwlog="Brandväggslogg";

secother.pagefresh="Siduppdatering";

secother.prepage="Föregående sida";

secother.nextpage="Nästa sida";

secother.outgoing="Utgående loggtabell";

secother.lanip="LAN-IP-adress";

secother.desturlip="URL/IP-adress för mål";

secother.srvportnum="Tjänst/Portnummer";

//ipsecadv.asp
var ipsecadv = new Object();
ipsecadv.advsetup="Avancerad konfiguration för VPN-tunnel med IPSec";

ipsecadv.phase="Fas";

ipsecadv.opmode="Driftläge";

ipsecadv.mainmode="Huvudläge";

ipsecadv.aggmode="Aggressivt läge";

ipsecadv.proposal="Förslag";

ipsecadv.encryption="Kryptering";

ipsecadv.auth="Autentisering";

ipsecadv.group="Grupp";

ipsecadv.keytime="Nyckellivslängd";

ipsecadv.note="Obs! Följande tre ytterligare förslag föreslås även i huvudläget";

ipsecadv.pfs="PFS";

ipsecadv.othersetting="Andra inställningar";

ipsecadv.nat="NAT Traversal";

ipsecadv.bios="NetBIOS-broadcast";

ipsecadv.keep="Behåll anslutning";

ipsecadv.ifthan="Om IKE misslyckades mer än ";

ipsecadv.mid="gånger ska den här obehöriga IP-adressen blockeras i";

ipsecadv.ipfor="IP i";

ipsecadv.seconds="sekunder";

ipsecadv.antireplay="Anti-replay";
