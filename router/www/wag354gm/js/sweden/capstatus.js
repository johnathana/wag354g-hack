var statopmenu = new Object();
statopmenu.gateway="Gateway";

statopmenu.localnet="Lokalt nätverk";

statopmenu.wireless="Trådlöst";

statopmenu.dslconn="DSL-anslutning";

var staleftmenu = new Object();
//Gateway
staleftmenu.gwinfo="Gatewayinformation";

staleftmenu.inetconn="Internetanslutning";

//Local Network
staleftmenu.localnet="Lokalt nätverk";

//Wireless
staleftmenu.wireless="Trådlöst";

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

stainetconn.iface="Gränssnitt";

stainetconn.ipaddr="IP-adress";

stainetconn.subnetmask="Nätmask";

stainetconn.defaultgw="Standard-gateway";

stainetconn.dns1="DNS 1";

stainetconn.dns2="DNS 2";

stainetconn.dns3="DNS 3";

stainetconn.wins="WINS";

stainetconn.svcname="Tjänstenamn";

stainetconn.acname="AC-namn";

//Local Network
var stalocalnet = new Object();
stalocalnet.macaddr="MAC-adress";

stalocalnet.ipaddr="IP-adress";

stalocalnet.subnetmask="Nätmask";

stalocalnet.dhcpsvr="DHCP-server";

stalocalnet.startipaddr="Start-IP-adress";

stalocalnet.endipaddr="Slut-IP-adress";

//Wireless
var stawireless = new Object();
//stawireless.firmwarever="Version av fast programvara";
stawireless.firmwarever="Version av programvara";

stawireless.macaddr="MAC-adress";

stawireless.mode="Läge";

stawireless.mixed="Blandat läge";

stawireless.gonly="Endast G-läge";

stawireless.bonly="Endast B-läge";

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

stadslstatus.dslmodulmode="DSL-moduleringsläge";

stadslstatus.dslpathmode="DSL-sökväg";

stadslstatus.downrate="Mottagningshastighet";

stadslstatus.uprate="Sändningshastighet";

stadslstatus.downmargin="Mottagningsmarginal";

stadslstatus.upmargin="Sändningsmarginal";

stadslstatus.downlineatn="Linjedämpning vid mottagning";

stadslstatus.uplineatn="Linjedämpning vid sändning";

stadslstatus.downtranspower="Överföringseffekt vid mottagning";

stadslstatus.uptranspower="Överföringseffekt vid sändning";

//PVC Connection
var stapvcconn = new Object();
stapvcconn.connect="Anslutning";

stapvcconn.encaps="Inkapsling";

stapvcconn.multiplex="Multiplex";

stapvcconn.qos="QoS";

stapvcconn.pcrrate="PCR-hastighet";

stapvcconn.scrrate="SCR-hastighet";

stapvcconn.autodetect="Automatisk avkänning";

stapvcconn.vpi="VPI";

stapvcconn.vci="VCI";

stapvcconn.enable="Aktivera";

stapvcconn.pvcstatus="PVC-status";

//DHCP Client Table
var stadhcptbl = new Object();
stadhcptbl.dhcpactiveiptbl="Tabellen Aktiv DHCP-IP-adress";

stadhcptbl.dhcpsrvipaddr="DHCP-serverns IP-adress";

stadhcptbl.clienthostname="Klientens värdnamn";

stadhcptbl.ipaddr="IP-adress";

stadhcptbl.macaddr="MAC-adress";

stadhcptbl.expires="Förfaller";

stadhcptbl.del="Ta bort";

stadhcptbl.closes="Stäng";

//DNS Entries Table
var stadnsentrytbl = new Object();
stadnsentrytbl.dnsentrytbl="Tabell för DNS-poster";

stadnsentrytbl.domainname="Domännamn";

stadnsentrytbl.dnsipaddr="DNS-IP-adress";

stadnsentrytbl.ipaddr="IP-adress";

stadnsentrytbl.selects="Välj";

stadnsentrytbl.dsldevice="DSL-enhet";

//Wireless Clients Connected
var stawltbl = new Object();
stawltbl.networkedcomputers="Nätverksanslutna datorer";

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

stabutton.dhcptbl="Tabell för DHCP-klient";

stabutton.dnsentrytbl="Tabell för DNS-poster";

stabutton.arptbl="ARP-/RARP-tabell";

stabutton.wlclntconn="Anslutna trådlösa klienter";

stabutton.refresh="Uppdatera";

stabutton.closes="Stäng";

stabutton.dhcprelease="DHCP Release";

stabutton.dhcprenew="DHCP Renew";
