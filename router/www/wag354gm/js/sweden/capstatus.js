var statopmenu = new Object();
statopmenu.gateway="Gateway";

statopmenu.localnet="Lokalt n�tverk";

statopmenu.wireless="Tr�dl�st";

statopmenu.dslconn="DSL-anslutning";

var staleftmenu = new Object();
//Gateway
staleftmenu.gwinfo="Gatewayinformation";

staleftmenu.inetconn="Internetanslutning";

//Local Network
staleftmenu.localnet="Lokalt n�tverk";

//Wireless
staleftmenu.wireless="Tr�dl�st";

//DSL Connection
staleftmenu.dslstatus="DSL-status";

staleftmenu.pvcconn="PVC-anslutning";

//Gateway Information
var stagwinfo = new Object();
//stagwinfo.firmwarever="Version av fast programvara";
stagwinfo.firmwarever="Version av programvara";

stagwinfo.macaddr="MAC-adress";

stagwinfo.curtime="Aktuell tid";

var stainetconn = new Object();
stainetconn.connection="Anslutning";

stainetconn.logintype="Inloggningstyp";

stainetconn.iface="Gr�nssnitt";

stainetconn.ipaddr="IP-adress";

stainetconn.subnetmask="N�tmask";

stainetconn.defaultgw="Standard-gateway";

stainetconn.dns1="DNS 1";

stainetconn.dns2="DNS 2";

stainetconn.dns3="DNS 3";

stainetconn.wins="WINS";

stainetconn.svcname="Tj�nstenamn";

stainetconn.acname="AC-namn";

//Local Network
var stalocalnet = new Object();
stalocalnet.macaddr="MAC-adress";

stalocalnet.ipaddr="IP-adress";

stalocalnet.subnetmask="N�tmask";

stalocalnet.dhcpsvr="DHCP-server";

stalocalnet.startipaddr="Start-IP-adress";

stalocalnet.endipaddr="Slut-IP-adress";

//Wireless
var stawireless = new Object();
//stawireless.firmwarever="Version av fast programvara";
stawireless.firmwarever="Version av programvara";

stawireless.macaddr="MAC-adress";

stawireless.mode="L�ge";

stawireless.mixed="Blandat l�ge";

stawireless.gonly="Endast G-l�ge";

stawireless.bonly="Endast B-l�ge";

stawireless.ssid="SSID";

stawireless.dhcpsvr="DHCP-server";

stawireless.channel="Kanal";

stawireless.encryptfunc="Krypteringsfunktion";

stawireless.enabled="Aktiverad";

//stawireless.disabled="Avaktiverad"
stawireless.disabled="Inaktiverad"

//DSL Status
var stadslstatus = new Object();
stadslstatus.dslstatus="DSL-status";

stadslstatus.dslmodulmode="DSL-moduleringsl�ge";

stadslstatus.dslpathmode="DSL-s�kv�g";

stadslstatus.downrate="Mottagningshastighet";

stadslstatus.uprate="S�ndningshastighet";

stadslstatus.downmargin="Mottagningsmarginal";

stadslstatus.upmargin="S�ndningsmarginal";

stadslstatus.downlineatn="Linjed�mpning vid mottagning";

stadslstatus.uplineatn="Linjed�mpning vid s�ndning";

stadslstatus.downtranspower="�verf�ringseffekt vid mottagning";

stadslstatus.uptranspower="�verf�ringseffekt vid s�ndning";

//PVC Connection
var stapvcconn = new Object();
stapvcconn.connect="Anslutning";

stapvcconn.encaps="Inkapsling";

stapvcconn.multiplex="Multiplex";

stapvcconn.qos="QoS";

stapvcconn.pcrrate="PCR-hastighet";

stapvcconn.scrrate="SCR-hastighet";

stapvcconn.autodetect="Automatisk avk�nning";

stapvcconn.vpi="VPI";

stapvcconn.vci="VCI";

stapvcconn.enable="Aktivera";

stapvcconn.pvcstatus="PVC-status";

//DHCP Client Table
var stadhcptbl = new Object();
stadhcptbl.dhcpactiveiptbl="Tabellen Aktiv DHCP-IP-adress";

stadhcptbl.dhcpsrvipaddr="DHCP-serverns IP-adress";

stadhcptbl.clienthostname="Klientens v�rdnamn";

stadhcptbl.ipaddr="IP-adress";

stadhcptbl.macaddr="MAC-adress";

stadhcptbl.expires="F�rfaller";

stadhcptbl.del="Ta bort";

stadhcptbl.closes="St�ng";

//DNS Entries Table
var stadnsentrytbl = new Object();
stadnsentrytbl.dnsentrytbl="Tabell f�r DNS-poster";

stadnsentrytbl.domainname="Dom�nnamn";

stadnsentrytbl.dnsipaddr="DNS-IP-adress";

stadnsentrytbl.ipaddr="IP-adress";

stadnsentrytbl.selects="V�lj";

stadnsentrytbl.dsldevice="DSL-enhet";

//Wireless Clients Connected
var stawltbl = new Object();
stawltbl.networkedcomputers="N�tverksanslutna datorer";

stawltbl.computername="Datornamn";

stawltbl.ipaddr="IP-adress";

stawltbl.macaddr="MAC-adress";

//ARP/RARP Table
var staarptbl = new Object();
staarptbl.arptbl="ART-/RARP-tabell";

//Status' buttons
var stabutton = new Object();
stabutton.connect="Anslut";

stabutton.disconnect="Koppla bort";

stabutton.dhcptbl="Tabell f�r DHCP-klient";

stabutton.dnsentrytbl="Tabell f�r DNS-poster";

stabutton.arptbl="ARP-/RARP-tabell";

stabutton.wlclntconn="Anslutna tr�dl�sa klienter";

stabutton.refresh="Uppdatera";

stabutton.closes="St�ng";

stabutton.dhcprelease="DHCP Release";

stabutton.dhcprenew="DHCP Renew";
