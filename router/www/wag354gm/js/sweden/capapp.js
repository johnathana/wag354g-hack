//Applications & Gaming top menu strings
var apptopmenu = new Object();
//apptopmenu.singleport="Vidarebefordran av en port";
apptopmenu.singleport="Vidarebefordran av portar";

apptopmenu.forward="Vidarebefordra";

apptopmenu.portrange="Vidarebefordran av portintervall";

apptopmenu.porttrigger="Portutlösare";

apptopmenu.dmz="DMZ";

apptopmenu.qos="QoS";

//Application & Gaming left menu strings
var appleftmenu = new Object();
//appleftmenu.singleport="Vidarebefordran av en port";
appleftmenu.singleport="Vidarebefordran av portar";

appleftmenu.pvcselect="Välj PVC-anslutning";

appleftmenu.portmaplist="Portkartlista";

appleftmenu.portrange="Vidarebefordran av portintervall";

appleftmenu.porttrigger="Portutlösare";

appleftmenu.dmz="DMZ";

appleftmenu.qos="QoS";

appleftmenu.appbaseqos="Tillämpningsbaserad QoS";

appleftmenu.advqos="Avancerad QoS";

//Single Port Forwarding
var appsingleport = new Object();
appsingleport.pvcselect="Välj en PVC-anslutning";

appsingleport.app="Tillämpning";

appsingleport.extport="Extern port";

appsingleport.intport="Intern port";

appsingleport.proto="Protokoll";

appsingleport.ipaddr="IP-adress";

appsingleport.enabled="Aktiverad";

appsingleport.tcp="TCP";

appsingleport.udp="UDP";

//Port Range Forwarding
var appportrange = new Object();
appportrange.portrange="Portintervall";

appportrange.app="Tillämpning";

appportrange.start="Start";

appportrange.end="Slut";

appportrange.proto="Protokoll";

appportrange.ipaddr="IP-adress";

appportrange.enabled="Aktiverad";

appportrange.to="till";

appportrange.tcp="TCP";

appportrange.udp="UDP";

appportrange.both="Båda";

appportrange.advanced="Avancerad";

//Advanced Port Forwarding Table
var appadvport = new Object();
appadvport.srcs="Källa";

appadvport.dst="Mål";

appadvport.ipaddr="IP-adress";

appadvport.startport="Startport";

appadvport.endport="Slutport";

appadvport.proto="Protokoll";

appadvport.ipdomain="IP-adress eller domän";

appadvport.domainname="Domännamn";

appadvport.enable="Aktivera";

//Port Triggering
var appporttrigger = new Object();
appporttrigger.triggerrange="Utlöst intervall";

appporttrigger.forwardrange="Vidarebefordrat intervall";

appporttrigger.app="Tillämpning";

appporttrigger.startport="Startport";

appporttrigger.endport="Slutport";

appporttrigger.enable="Aktivera";

appporttrigger.to="till";

//DMZ
var appdmz = new Object();
appdmz.hosting="DMZ-värdfunktion";

appdmz.dmzhostip="IP-adress för DMZ-värd";

appdmz.validrange="(giltigt intervall är 1-254)";

//QoS
//Application-based QoS
var appbaseqos = new Object();
appbaseqos.hpriority="Hög prioritet";

appbaseqos.mpriority="Medelhög prioritet";

appbaseqos.lpriority="Låg prioritet";

appbaseqos.ftp="FTP";

appbaseqos.http="HTTP";

appbaseqos.telnet="Telnet";

appbaseqos.smtp="SMTP";

appbaseqos.pop3="POP3";

appbaseqos.specport="Specifik port#";

var appadvqos = new Object();
appadvqos.fragsize="Fragmentpaketen för AF- och BE-trafik måste vara lika stora som för EF-trafik";

appadvqos.bytes="byte.";

appadvqos.enable8021p="Aktivera schemaläggning med P bitar för 802.1p.";

appadvqos.vlanvid="VLAN-ID";
