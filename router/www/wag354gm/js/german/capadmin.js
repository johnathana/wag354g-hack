var admtopmenu = new Object();
admtopmenu.credval="Credentials Validate";

admtopmenu.manage="Verwaltungsfunktionen";

admtopmenu.report="Berichtaufzeichnung";

admtopmenu.diag="Diagnose";

admtopmenu.facdef="Werkseinstellungen";

admtopmenu.bakrest="Sichern & Wiederherstellen";

admtopmenu.upgrade="Firmware aktualisieren";

admtopmenu.reboot="Neustart";

var admleftmenu = new Object();
admleftmenu.gwaccess="Gateway-Zugriff";

admleftmenu.localgwaccess="Lokaler Gateway-Zugriff";

admleftmenu.remotegwaccess="Entfernter Gateway-Zugriff";

admleftmenu.remoteupgrade="Remote-Aktualisierung";

admleftmenu.snmp="SNMP";

admleftmenu.upnp="UPnP";

admleftmenu.tr064="TR064";

admleftmenu.tr069="TR-069";

admleftmenu.igmpproxy="IGMP-Proxy";

admleftmenu.wlan="WLAN";

admleftmenu.report="Berichtaufzeichnung";

admleftmenu.emailalt="E-Mail-Warnungen";

admleftmenu.ping="Ping-Test";

admleftmenu.pingparam="Ping-Test-Parameter";

admleftmenu.bakconfig="Konfiguration sichern";

admleftmenu.restconfig="Konfiguration wiederherstellen";

admleftmenu.facdef="Werkseinstellungen";

admleftmenu.upgradewan="Aktualisieren aus dem WAN";

admleftmenu.upgradelan="Aktualisieren aus dem LAN";

admleftmenu.reboot="Neustart";

var admgwaccess = new Object();

admgwaccess.gwusername="Gateway-Benutzerauflisten";

admgwaccess.gwusername="Gateway-Benutzername";

admgwaccess.gwpassword="Gateway-Passwort";

admgwaccess.reconfirm="Zur Best&auml;tigung erneut eingeben";

admgwaccess.remotemgt="Remote-Verwaltung";

admgwaccess.mgtport="Verwaltung-Port";

admgwaccess.allowip="Zugelassene IP";

admgwaccess.rmtupgrade="Remote-Aktualisierung";

var admsnmp = new Object();
admsnmp.snmp="SNMP";

admsnmp.devname="Ger&auml;tename";

admsnmp.snmp_v1v2="SNMP, Vers. 1/2";

admsnmp.getcomm="Get Community";

admsnmp.setcomm="Set Community";

admsnmp.snmp_v3="Snmp V3";

admsnmp.rwuser="Benutzer mit Schreib-/Lesezugriff";

admsnmp.authprotocol="Beurkundung Protokoll";

admsnmp.md5="MD5";

admsnmp.sha="SHA";

admsnmp.authpassword="Beurkundung Passwort";

admsnmp.privpassword="Datenschutz Passwort";

admsnmp.md5passwd="MD5-Passwort";

admsnmp.trapmgt="Trap-Verwaltung";

admsnmp.trapto="Trap herstellen mit";

var admupnp = new Object();
admupnp.upnp="UPnP";

admupnp.selectpvc="W&auml;hlen Sie eine PVC-Verbindung aus";

var admigmpproxy = new Object();
admigmpproxy.pvcavail="PVC verf&uuml;gbar";

admigmpproxy.igmpproxy="IGMP-Proxy";

var admtr064 = new Object();
admtr064.tr064="TR-064";

var admtr069 = new Object();
admtr069.tr069="TR-069";

var admwlan = new Object();
admwlan.mgtwlan="Verwaltung &uuml;ber WLAN";

var admreport = new Object();
admreport.logs="Protokoll";

admreport.enable="Aktivieren";

admreport.disable="Deaktivieren";

admreport.logviewerip="Logviewer-IP-Adresse";

admreport.emailalerts="E-Mail-Warnungen";

admreport.denservice="DoS-Schwellenwerte";

admreport.events="Ereignisse";

admreport.eventsrange="(20 - 100)";

admreport.smtpserver="SMTP Mail-Server";

admreport.emailaddr="E-Mail-Adresse f&uuml;r &nbsp;Warnprotokolle";

admreport.returnaddr="E-Mail-Antwortadresse";

admreport.alls="ALLE";

admreport.systemlog="Systemprotokoll";

admreport.accesslog="Zugriffsprotokoll";

admreport.firewalllog="Firewall-Protokoll";

admreport.vpnlog="VPN-Protokoll";

admreport.upnplog="UPNP-ProtokollP";

var admping = new Object();
admping.targetip="Ping-Ziel-IP";

admping.size="Ping-Größe";

admping.bytes="Byte";

admping.numbers="Anzahl der Pings";

admping.range="(Bereich 1 - 100)";

admping.interval="Ping-Intervall";

admping.timeout="Ping-Wartezeit";

admping.milliseconds="Millisekunden";

admping.result="Ping-Ergebnis";

admping.pktsend="Vers Pkt";

admping.pktrecv="Empf Pkt";

admping.avgrtt="Mwert Laufz";

var admrestore = new Object();
admrestore.selectfile="&nbsp;W&auml;hlen Sie die Datei aus, die wiederhergestellt werden soll.";

var admfacdef = new Object();
admfacdef.refacdefault="&nbsp;Werkseinstellungen wiederherstellen";

admfacdef.yes="Ja";

admfacdef.no="Nein";

var admupgradewan = new Object();
admupgradewan.upgradeweb="Firmware-WEB-Pfad";

admupgradewan.warning="<B>Warnung: </B>Die Aktualisierung aus dem WAN <BR>kann einige Minuten dauern. <BR>Schalten Sie das Ger&auml;t nicht aus, <BR>und dr&uuml;cken Sie nicht die Reset-Taste.";

admupgradewan.url="URL";

admupgradewan.notinterrupted="Der Aktualisierungsvorgang darf nicht unterbrochen werden.";

var admupgradelan = new Object();
admupgradelan.upgradefw="Firmware aktualisieren";

admupgradelan.filepath="Dateipfad";

admupgradelan.warning="<B>Warnung: </B>Die Aktualisierung aus dem LAN <BR>kann einige Minuten dauern.<BR> Schalten Sie das Ger&auml;t nicht aus, <BR>und dr&uuml;cken Sie nicht die Reset-Taste.";

admupgradelan.notinterrupted="Der Aktualisierungsvorgang darf nicht unterbrochen werden.";

var admpostupgrade = new Object();
admpostupgrade.upgradesuccess="Aktualisierung war erfolgreich.";

admpostupgrade.reboot="Neustart......";

var admreboot = new Object();
admreboot.mode="Neustart-Modus";

admreboot.hard="Kaltstart";

admreboot.soft="Warmstart";

admreboot.gwusername="Benutzername";

admreboot.gwpassword="Passwort";

var admbutton =new Object();
admbutton.viewlogs="Protokolle anzeigen";

admbutton.starttest="Test ausf&uuml;hren";

admbutton.backup="Sichern";

admbutton.restore="Wiederherstellen";

admbutton.browse="Durchsuchen...";

admbutton.upgrade="Aktualisieren";

admbutton.clears="L&ouml;schen";

admbutton.pagerefresh="Seite aktualisieren";

admbutton.prevpage="Vorherige Seite";

admbutton.nextpage="Nächste Seite";

admbutton.cancelupgrade="Aktualisierung abbrechen";

