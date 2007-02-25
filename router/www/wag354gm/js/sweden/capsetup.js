var settopmenu = new Object();
settopmenu.basicsetup="Grundläggande inställningar";

settopmenu.ddns="DDNS";

settopmenu.advrouting="Avancerad routing";

settopmenu.hotspot="Anknytningspunkt";

settopmenu.voip="Voice";

var setleftmenu = new Object();
setleftmenu.inetsetup="Internet-inställningar";

setleftmenu.pvcconn="PVC-anslutning";

setleftmenu.inetconntype="Internet-anslutningstyp";

setleftmenu.vcsetting="VC-inställningar";

setleftmenu.ipsetting="IP-inställningar";

setleftmenu.optsetting="Valfria inställningar";

setleftmenu.info="krävs av vissa Internet-leverantörer";

setleftmenu.netsetup="Nätverksinställningar";

setleftmenu.routerip="Router-IP";

setleftmenu.cloudsetting="CLOUD-inställning";

setleftmenu.cloud="CLOUD";

setleftmenu.netaddress="Nätverksadress";

setleftmenu.serversetting="Serverinställningar";

setleftmenu.timesetting="Tidsinställning";

setleftmenu.ddns="DDNS";

setleftmenu.advrouting="Avancerad routing";

setleftmenu.operamode="Driftsläge";

setleftmenu.dynrouting="Dynamisk routing";

setleftmenu.staticrouting="Statisk routing";

setleftmenu.pvcrout="Regler för PVC-dirigering";

//index.asp
var setinetsetup = new Object();
setinetsetup.please="Välj en anslutning";

setinetsetup.enablenow="Aktivera nu";

setinetsetup.encap="Inkapsling";

setinetsetup.multiplexing="Multiplex";

setinetsetup.qostype="QoS-typ";

setinetsetup.pcrrate="PCR-hastighet";

setinetsetup.scrrate="SCR-hastighet";

setinetsetup.autodetect="Automatisk avkänning";

setinetsetup.vircir="Virtuell krets";

setinetsetup.obtain="Erhåll en IP-adress automatiskt";

setinetsetup.usefollow="Använd följande IP-adress";

setinetsetup.inetipaddr="Internet-IP-adress";

setinetsetup.subnetmask="Nätmask";

setinetsetup.pridns="Primär DNS";

setinetsetup.secdns="Sekundär DNS";

setinetsetup.pppoesession="PPPoE-session";

setinetsetup.hostname="Värdnamn";

setinetsetup.doname="Domännamn";

setinetsetup.mtu="MTU";

setinetsetup.size="Storlek";

setinetsetup.symbol=".";

var setnetsetup = new Object();
setnetsetup.localip="Lokal IP-adress";

setnetsetup.subnet="Nätmask";

setnetsetup.ldhcpserver="Lokal DHCP-server";

setnetsetup.dhcprelay="DHCP-relä";

setnetsetup.dhcprelayserver="DHCP-reläserver";

setnetsetup.autodhcp="Identifiera nätverks-DHCP-server automatiskt";

setnetsetup.startip="Start-IP-adress";

setnetsetup.maxnum="Maximalt antal DHCP-användare";

setnetsetup.clientlease="Klientlånetid";

setnetsetup.minutes="minuter";

setnetsetup.moneday="innebär en dag";

setnetsetup.staticdns="Statisk DNS";

setnetsetup.wins="WINS";

setnetsetup.timezone="Tidszon";

setnetsetup.tinterval="Tidsintervall";

setnetsetup.seconds="sekunder";

setnetsetup.lastinfo="Justera klockan automatiskt för sommartid";

//DDNS.asp
var setddns = new Object();
setddns.service="DDNS-tjänst";

setddns.org="DynDNS.org";

setddns.com="TZO.com";

setddns.username="Användarnamn";

setddns.password="Lösenord";

setddns.hostname="Värdnamn";

setddns.email="E-postadress";

setddns.doname="Domännamn";

setddns.ipaddr="Internet-IP-adress";

setddns.statu="Status";

var ddnsm = new Object();

ddnsm.dyn_good="DDNS-uppdateringen genomfördes";

ddnsm.dyn_noupdate="DDNS-uppdateringen genomfördes";

ddnsm.dyn_nochg="DDNS-uppdateringen genomfördes";

ddnsm.dyn_badauth="Behörighetskontrollen misslyckades (användarnamn eller lösenord)";

ddnsm.all_noip="Ingen Internet-anslutning";

ddnsm.dyn_yours="Username is not correct";
 
ddnsm.dyn_nohost="The hostname does not exist";

ddnsm.dyn_notfqdn="Domain Name is not correct";

ddnsm.dyn_abuse="The hostname is blocked by the DDNS server";

ddnsm.dyn_badsys="The system parameters are invalid";

ddnsm.dyn_badagent="This useragent has been blocked";

ddnsm.dyn_numhost="Too many or too few hosts found";

ddnsm.dyn_dnserr= "DNS error encountered";

ddnsm.dyn_911= "An unexpected error (1)";

ddnsm.dyn_999= "An unexpected error (2)";

ddnsm.dyn_donator="A feature requested is only available to donators, please donate";

ddnsm.dyn_strange= "Strange server response, are you connecting to the right server?";

ddnsm.dyn_uncode= "Unknown return code";
 
ddnsm.tzo_good= "Operation Complete";

ddnsm.tzo_noupdate="Operation Complete";

ddnsm.tzo_notfqdn= "Invalid Domain Name";

ddnsm.tzo_notmail= "Invalis Email";

ddnsm.tzo_notact= "Invalid Action";

ddnsm.tzo_notkey= "Invalid Key";

ddnsm.tzo_notip= "Invalid IP address";

ddnsm.tzo_dupfqdn= "Duplicate Domain Name";

ddnsm.tzo_fqdncre= "Domain Name has already been created for this domain name";

ddnsm.tzo_expired= "The account has expired";

ddnsm.tzo_error= "An unexpected server error";

ddnsm.all_closed= "DDNS server is currently closed";

ddnsm.all_resolving= "Domain name resolve fail";

ddnsm.all_errresolv= "Domain name resolve fail";

ddnsm.all_connecting= "Connect to server fail";

ddnsm.all_connectfail= "Connect to server fail";

ddnsm.all_disabled= "DDNS function is disabled";

//Routing.asp
var setadvrouting = new Object();
setadvrouting.nat="NAT";

setadvrouting.rip="RIP";

setadvrouting.compatible="Kompatibel";

setadvrouting.defaultroute="Sänd standardrouting";

setadvrouting.multibroad="Multicast eller Broadcast";

setadvrouting.broadcast="Broadcast";

setadvrouting.multicast="Multicast";

setadvrouting.transver="Överför RIP-version";

setadvrouting.receiver="Ta emot RIP-version";

setadvrouting.selnum="Välj uppsättningsnummer";

setadvrouting.dstipaddr="Mål-IP-adress";

setadvrouting.subnet="Nätmask";

setadvrouting.hop="Antal hopp";

setadvrouting.deletes="Ta bort den här posten";

setadvrouting.showtable="Visa routingtabell";

setadvrouting.routingtable="Routingtabell";

setadvrouting.pvcsetting="Inställning för PVC-dirigering";

setadvrouting.pvcpolicy="Regler för PVC-dirigering";

//index_1483bridged.asp
var setother = new Object();
setother.pppoasetting="PPPoA-inställningar";

setother.condemand="Anslut på begäran";

setother.maxidle="Maximal vilotid";

setother.keepalive="Behåll anslutning";

setother.radial="Återuppringningsperiod";

setother.pppoesetting="PPPoE-inställningar";

setother.servicename="Tjänstenamn";

setother.secondpppoe="Sekundär PPPoE";

setother.matchs="Matcha domännamn";

setother.macclone="MAC-klon";

setother.userdefine="Användardefinierad post";

setother.clone="Klona datorns MAC-adress";

setother.hotspot="Anknytningspunkt";

setother.macaddrclone="MAC-adressklon";
