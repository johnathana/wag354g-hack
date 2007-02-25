var apptopmenu = new Object();
apptopmenu.singleport="Port-Weiterleitung";

apptopmenu.forward="Forward";

apptopmenu.portrange="Port-Bereich-Weiterleitung";

apptopmenu.porttrigger="Port-Triggering";

apptopmenu.dmz="DMZ";

apptopmenu.qos="QoS";

var appleftmenu = new Object();
appleftmenu.singleport="Port-Weiterleitung";

appleftmenu.pvcselect="PVC-Verbindungsauswahl";

appleftmenu.portmaplist="Anschlussliste";

appleftmenu.portrange="Port-Bereich-Weiterleitung";

appleftmenu.porttrigger="Port-Triggering";

appleftmenu.dmz="DMZ";

appleftmenu.qos="QoS";

appleftmenu.appbaseqos="Anwendungsbasierte QoS";

appleftmenu.advqos="Erweiterte QoS";


var appsingleport = new Object();
appsingleport.pvcselect="W&auml;hlen Sie eine <BR>&nbsp;PVC-Verbindung aus";

appsingleport.app="Anwendung";

appsingleport.extport="Externer Port";

appsingleport.intport="Interner Port";

appsingleport.proto="Protokoll";

appsingleport.ipaddr="IP-Adresse";

appsingleport.enabled="Aktivieren";

appsingleport.tcp="TCP";

appsingleport.udp="UDP";


var appportrange = new Object();
appportrange.portrange="Port-Bereich";

appportrange.app="Anwendung";

appportrange.start="Start";

appportrange.end="Ende";

appportrange.proto="Protokoll";

appportrange.ipaddr="IP-Adresse";

appportrange.enabled="Aktivieren";

appportrange.to="bis";

appportrange.tcp="TCP";

appportrange.udp="UDP";

appportrange.both="Beide";

appportrange.advanced="Erweitert";



var appadvport = new Object();
appadvport.srcs="Quelle";

appadvport.dst="Ziel";

appadvport.ipaddr="IP-Adresse";

appadvport.startport="Start-Port";

appadvport.endport="End-Port";

appadvport.proto="Protokoll";

appadvport.ipdomain="IP oder Domäne";

appadvport.domainname="Domänenname";

appadvport.enable="Aktivieren";


var appporttrigger = new Object();
appporttrigger.triggerrange="Triggering-Bereich";

appporttrigger.forwardrange="Weiterleitungsbereich";

appporttrigger.app="Anwendung";

appporttrigger.startport="Start-Port";

appporttrigger.endport="End-Port";

appporttrigger.enable="Aktivieren";

appporttrigger.to="bis";

var appdmz = new Object();
appdmz.hosting="DMZ-Hosting";

appdmz.dmzhostip="IP-Adresse des DMZ-Hosts";

appdmz.validrange="(g&uuml;ltiger Bereich: 1-254)";

var appbaseqos = new Object()
appbaseqos.hpriority="Hohe <br>Priorit&auml;t";

appbaseqos.mpriority="Mittlere <br>Priorit&auml;t";

appbaseqos.lpriority="Niedrige <br>Priorit&auml;t";

appbaseqos.ftp="FTP";

appbaseqos.http="HTTP";

appbaseqos.telnet="Telnet";

appbaseqos.smtp="SMTP";

appbaseqos.pop3="POP3";

appbaseqos.specport="Spez. Port-Nummer";

var appadvqos = new Object();

appadvqos.fragsize="Gr&ouml;&szlig;e des Pakets von AF- und BE-Datenverkehr <br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fragmentieren, so dass diese der Gr&ouml;&szlig;e des EF-<br>Datenverkehrs&nbsp;entspricht";

appadvqos.bytes="Byte";

appadvqos.enable8021p="Planen von 802.1p P-Bits aktivieren.";

appadvqos.vlanvid="ID des VLAN";

