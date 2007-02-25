var settopmenu = new Object();

settopmenu.basicsetup="Grundlegende Einrichtung";

settopmenu.ddns="DDNS";

settopmenu.advrouting="Erweitertes Routing";

settopmenu.hotspot="Hot Spot";

settopmenu.voip="Voice";

var setleftmenu = new Object();

setleftmenu.inetsetup="Internet-Einrichtung";

setleftmenu.pvcconn="PVC-Verbindung";

setleftmenu.inetconntype="Internet-Verbindungstyp";

setleftmenu.vcsetting="VC-Einstellungen";

setleftmenu.ipsetting="IP-Einstellungen";

setleftmenu.optsetting="Optionale Einstellungen";

setleftmenu.info="f�r einige ISPs erforderlich";

setleftmenu.netsetup="Netzwerkeinrichtung";

setleftmenu.routerip="IP-Adresse des Gateways";

setleftmenu.cloudsetting="WOLKE Setzen";

setleftmenu.cloud="WOLKE";

setleftmenu.netaddress="Einstellungen des Netzwerk-Adressenservers";

setleftmenu.serversetting="Server-Einstellungen";

setleftmenu.timesetting="Zeiteinstellung";

setleftmenu.ddns="DDNS";

setleftmenu.advrouting="Erweitertes Routing";

setleftmenu.operamode="Betriebsmodus";

setleftmenu.dynrouting="Dynamisches Routing";

setleftmenu.staticrouting="Statisches Routing";

setleftmenu.pvcrout="PVC-Routing-Richtlinie";

var setinetsetup = new Object();

setinetsetup.please="W�hlen Sie eine Verbindung aus";

setinetsetup.enablenow="Jetzt aktivieren";

setinetsetup.encap="Kapselungsmethode";

setinetsetup.multiplexing="Multiplexing";

setinetsetup.qostype="QOS-Typ";

setinetsetup.pcrrate="PCR-Rate";

setinetsetup.scrrate="SCR-Rate";

setinetsetup.autodetect="Automatisch erkennen";

setinetsetup.vircir="Virtuelle Verbindung";

setinetsetup.obtain="IP-Adresse automatisch beziehen";

setinetsetup.usefollow="Folgende IP-Adresse verwenden";

setinetsetup.inetipaddr="Internet-IP-Adresse";

setinetsetup.subnetmask="Subnetzmaske";

setinetsetup.pridns="Prim�rer DNS";

setinetsetup.secdns="Sekund�rer DNS";

setinetsetup.pppoesession="PPPoE-Sitzung";

setinetsetup.hostname="Hostname";

setinetsetup.doname="Dom�nenname";

setinetsetup.mtu="MTU";

setinetsetup.size="Gr��e";

setinetsetup.symbol=",";

var setnetsetup = new Object();

setnetsetup.localip="Lokale IP-Adresse";

setnetsetup.subnet="Subnetzmaske";

setnetsetup.ldhcpserver="Lokaler DHCP-Server";

setnetsetup.dhcprelay="DHCP-Relay";

setnetsetup.dhcprelayserver="DHCP-Relay-Server";

setnetsetup.autodhcp="LAN-DHCP-Server automatisch erkennen";

setnetsetup.startip="Start-IP-Adresse";

setnetsetup.maxnum="Maximale Anzahl der DHCP-Benutzer";

setnetsetup.clientlease="Client-Leasedauer";

setnetsetup.minutes="Minuten";

setnetsetup.moneday="bedeutet einen Tag";

setnetsetup.staticdns="Statischer DNS";

setnetsetup.wins="WINS";

setnetsetup.timezone="Zeitzone";

setnetsetup.tinterval="Zeitintervall";

setnetsetup.seconds="Sekunden";

setnetsetup.lastinfo="Uhr automatisch an Zeitumstellung anpassen";

var setddns = new Object();

setddns.service="DDNS-Dienst";

setddns.org="DynDNS.org";

setddns.com="TZO.com";

setddns.username="Benutzername";

setddns.password="Passwort";

setddns.hostname="Hostname";

setddns.email="E-mail Address";

setddns.doname="Dom�nenname";

setddns.ipaddr="Internet-IP-Adresse";

setddns.statu="Status";

var ddnsm = new Object();

ddnsm.dyn_good="DDNS wurde erfolgreich aktualisiert.";

ddnsm.dyn_noupdate="DDNS wurde erfolgreich aktualisiert.";

ddnsm.dyn_nochg="DDNS wurde erfolgreich aktualisiert.";

ddnsm.dyn_badauth="Autorisierung fehlgeschlagen (Benutzername oder Kennw�rter)";

ddnsm.all_noip="Keine Internetverbindung";

ddnsm.dyn_yours="Benutzername ist nicht korrekt.";
 
ddnsm.dyn_nohost="Der Hostname ist nicht vorhanden.";

ddnsm.dyn_notfqdn="Dom�nenname ist nicht korrekt.";

ddnsm.dyn_abuse="Der Hostname ist durch den DDNS-Server blockiert.";

ddnsm.dyn_badsys="Die Systemparameter sind ung�ltig.";

ddnsm.dyn_badagent="Dieser Benutzeragent wurde blockiert.";

ddnsm.dyn_numhost="Zu viele bzw. zu wenige Hosts gefunden.";

ddnsm.dyn_dnserr="DNS-Fehler gefunden";

ddnsm.dyn_911="Unerwarteter Fehler (1)";

ddnsm.dyn_999="Unerwarteter Fehler (2)";

ddnsm.dyn_donator="Eine angeforderte Funktion ist nur f�r Spender verf�gbar. Melden Sie sich als Spender an.";

ddnsm.dyn_strange="Ungew�hnliche Serverantwort. Stellen Sie sicher, dass Sie ordnungsgem�� mit dem Server verbunden sind.";

ddnsm.dyn_uncode="Unbekannter R�ckgabecode";
 
ddnsm.tzo_good="Vorgang beendet";

ddnsm.tzo_noupdate="Vorgang beendet";

ddnsm.tzo_notfqdn="Ung�ltiger Dom�nenname";

ddnsm.tzo_notmail="Ung�ltige E-Mail-Adresse";

ddnsm.tzo_notact="Ung�ltige Aktion";

ddnsm.tzo_notkey="Ung�ltiger Schl�ssel";

ddnsm.tzo_notip="Ung�ltige IP-Adresse";

ddnsm.tzo_dupfqdn="Doppelter Dom�nenname";

ddnsm.tzo_fqdncre="Die Angabe \'Dom�nenname\' wurde f�r diesen Dom�nennamen bereits erstellt.";

ddnsm.tzo_expired="Das Konto ist abgelaufen.";

ddnsm.tzo_error="Unerwarteter Serverfehler";

ddnsm.all_closed="Verbindung zum DDNS-Server ist derzeit abgebrochen";

ddnsm.all_resolving="Dom�nenname wird aufgel�st";

ddnsm.all_errresolv="Aufl�sung des Dom�nennamens fehlgeschlagen";

ddnsm.all_connecting="Verbinden mit dem Server";

ddnsm.all_connectfail="Verbindung mit dem Server fehlgeschlagen";

ddnsm.all_disabled="DDNS-Funktion ist deaktiviert";

var setadvrouting = new Object();

setadvrouting.nat="NAT";

setadvrouting.rip="RIP";

setadvrouting.compatible="kompatibel";

setadvrouting.defaultroute="Abschicken Vorgegebener Wert Senden";

setadvrouting.multibroad="Multicast oder Rundfunk";

setadvrouting.broadcast="Rundfunk";

setadvrouting.multicast="Multicast";

setadvrouting.transver="RIP-Version �bertragen";

setadvrouting.receiver="RIP-Version empfangen";

setadvrouting.selnum="Set-Nummer ausw�hlen";

setadvrouting.dstipaddr="Ziel-IP-Adresse";

setadvrouting.subnet="Subnetzmaske";

setadvrouting.hop="Abschnitte";

setadvrouting.deletes="Diesen Eintrag l�schen";

setadvrouting.showtable="Routing-Tabelle anzeigen";

setadvrouting.routingtable="Routing-Tabelle";

setadvrouting.pvcsetting="PVC-Routing-Richtlinie";

setadvrouting.pvcpolicy="PVC-Routing-Einstellung";

var setother = new Object();

setother.pppoasetting="PPPoA-Einstellungen";

setother.condemand="Bei Bedarf verbinden";

setother.maxidle="Max. Leerlaufzeit";

setother.keepalive="Verbindung aufrechterhalten";

setother.radial="Wahlwiederholung";

setother.pppoesetting="PPPoE-Einstellungen";

setother.servicename="Dienstname";

setother.secondpppoe="Zweites PPPoE";

setother.matchs="Dom�nenname anpassen";

setother.macclone="MAC kopieren";

setother.userdefine="Benutzerdefinierter Eintrag";

setother.clone="MAC-Adresse des PCs kopieren";

setother.hotspot="Hot Spot";

setother.macaddrclone="MAC-Adresse kopieren";


