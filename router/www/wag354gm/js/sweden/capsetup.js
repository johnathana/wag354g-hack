var settopmenu = new Object();
settopmenu.basicsetup="Grundl�ggande inst�llningar";

settopmenu.ddns="DDNS";

settopmenu.advrouting="Avancerad routing";

settopmenu.hotspot="Anknytningspunkt";

settopmenu.voip="Voice";

var setleftmenu = new Object();
setleftmenu.inetsetup="Internet-inst�llningar";

setleftmenu.pvcconn="PVC-anslutning";

setleftmenu.inetconntype="Internet-anslutningstyp";

setleftmenu.vcsetting="VC-inst�llningar";

setleftmenu.ipsetting="IP-inst�llningar";

setleftmenu.optsetting="Valfria inst�llningar";

setleftmenu.info="kr�vs av vissa Internet-leverant�rer";

setleftmenu.netsetup="N�tverksinst�llningar";

setleftmenu.routerip="Router-IP";

setleftmenu.cloudsetting="CLOUD-inst�llning";

setleftmenu.cloud="CLOUD";

setleftmenu.netaddress="N�tverksadress";

setleftmenu.serversetting="Serverinst�llningar";

setleftmenu.timesetting="Tidsinst�llning";

setleftmenu.ddns="DDNS";

setleftmenu.advrouting="Avancerad routing";

setleftmenu.operamode="Driftsl�ge";

setleftmenu.dynrouting="Dynamisk routing";

setleftmenu.staticrouting="Statisk routing";

setleftmenu.pvcrout="Regler f�r PVC-dirigering";

//index.asp
var setinetsetup = new Object();
setinetsetup.please="V�lj en anslutning";

setinetsetup.enablenow="Aktivera nu";

setinetsetup.encap="Inkapsling";

setinetsetup.multiplexing="Multiplex";

setinetsetup.qostype="QoS-typ";

setinetsetup.pcrrate="PCR-hastighet";

setinetsetup.scrrate="SCR-hastighet";

setinetsetup.autodetect="Automatisk avk�nning";

setinetsetup.vircir="Virtuell krets";

setinetsetup.obtain="Erh�ll en IP-adress automatiskt";

setinetsetup.usefollow="Anv�nd f�ljande IP-adress";

setinetsetup.inetipaddr="Internet-IP-adress";

setinetsetup.subnetmask="N�tmask";

setinetsetup.pridns="Prim�r DNS";

setinetsetup.secdns="Sekund�r DNS";

setinetsetup.pppoesession="PPPoE-session";

setinetsetup.hostname="V�rdnamn";

setinetsetup.doname="Dom�nnamn";

setinetsetup.mtu="MTU";

setinetsetup.size="Storlek";

setinetsetup.symbol=".";

var setnetsetup = new Object();
setnetsetup.localip="Lokal IP-adress";

setnetsetup.subnet="N�tmask";

setnetsetup.ldhcpserver="Lokal DHCP-server";

setnetsetup.dhcprelay="DHCP-rel�";

setnetsetup.dhcprelayserver="DHCP-rel�server";

setnetsetup.autodhcp="Identifiera n�tverks-DHCP-server automatiskt";

setnetsetup.startip="Start-IP-adress";

setnetsetup.maxnum="Maximalt antal DHCP-anv�ndare";

setnetsetup.clientlease="Klientl�netid";

setnetsetup.minutes="minuter";

setnetsetup.moneday="inneb�r en dag";

setnetsetup.staticdns="Statisk DNS";

setnetsetup.wins="WINS";

setnetsetup.timezone="Tidszon";

setnetsetup.tinterval="Tidsintervall";

setnetsetup.seconds="sekunder";

setnetsetup.lastinfo="Justera klockan automatiskt f�r sommartid";

//DDNS.asp
var setddns = new Object();
setddns.service="DDNS-tj�nst";

setddns.org="DynDNS.org";

setddns.com="TZO.com";

setddns.username="Anv�ndarnamn";

setddns.password="L�senord";

setddns.hostname="V�rdnamn";

setddns.email="E-postadress";

setddns.doname="Dom�nnamn";

setddns.ipaddr="Internet-IP-adress";

setddns.statu="Status";

var ddnsm = new Object();

ddnsm.dyn_good="DDNS-uppdateringen genomf�rdes";

ddnsm.dyn_noupdate="DDNS-uppdateringen genomf�rdes";

ddnsm.dyn_nochg="DDNS-uppdateringen genomf�rdes";

ddnsm.dyn_badauth="Beh�righetskontrollen misslyckades (anv�ndarnamn eller l�senord)";

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

setadvrouting.defaultroute="S�nd standardrouting";

setadvrouting.multibroad="Multicast eller Broadcast";

setadvrouting.broadcast="Broadcast";

setadvrouting.multicast="Multicast";

setadvrouting.transver="�verf�r RIP-version";

setadvrouting.receiver="Ta emot RIP-version";

setadvrouting.selnum="V�lj upps�ttningsnummer";

setadvrouting.dstipaddr="M�l-IP-adress";

setadvrouting.subnet="N�tmask";

setadvrouting.hop="Antal hopp";

setadvrouting.deletes="Ta bort den h�r posten";

setadvrouting.showtable="Visa routingtabell";

setadvrouting.routingtable="Routingtabell";

setadvrouting.pvcsetting="Inst�llning f�r PVC-dirigering";

setadvrouting.pvcpolicy="Regler f�r PVC-dirigering";

//index_1483bridged.asp
var setother = new Object();
setother.pppoasetting="PPPoA-inst�llningar";

setother.condemand="Anslut p� beg�ran";

setother.maxidle="Maximal vilotid";

setother.keepalive="Beh�ll anslutning";

setother.radial="�teruppringningsperiod";

setother.pppoesetting="PPPoE-inst�llningar";

setother.servicename="Tj�nstenamn";

setother.secondpppoe="Sekund�r PPPoE";

setother.matchs="Matcha dom�nnamn";

setother.macclone="MAC-klon";

setother.userdefine="Anv�ndardefinierad post";

setother.clone="Klona datorns MAC-adress";

setother.hotspot="Anknytningspunkt";

setother.macaddrclone="MAC-adressklon";
