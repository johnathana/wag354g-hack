var admtopmenu = new Object();
admtopmenu.credval="Bekr�fta beh�righet";

admtopmenu.manage="Hantering";

admtopmenu.report="Rapportering";

admtopmenu.diag="Diagnostik";

admtopmenu.facdef="Fabriksinst�llningar";

admtopmenu.bakrest="S�kerhetskopiering och �terst�llning";

//admtopmenu.upgrade="Uppgradera fast programvara";
admtopmenu.upgrade="Uppgradera programvara";

admtopmenu.reboot="Omstart";

var admleftmenu = new Object();
admleftmenu.gwaccess="Gateway�tkomst";

admleftmenu.localgwaccess="Lokal gateway�tkomst";

admleftmenu.remotegwaccess="Fj�rr�tkomst av gatewayen";

admleftmenu.remoteupgrade="Fj�rruppgradering";

admleftmenu.snmp="SNMP";

admleftmenu.upnp="UPnP";

admleftmenu.tr064="TR064";

admleftmenu.tr069="TR-069";

admleftmenu.igmpproxy="IGMP-proxy";

admleftmenu.wlan="WLAN";

admleftmenu.report="Rapportering";

admleftmenu.emailalt="Varningsmeddelanden via e-post";

admleftmenu.ping="Pingtest";

admleftmenu.pingparam="Parametrar f�r pingtest";

admleftmenu.bakconfig="Konfiguration av s�kerhetskopiering";

admleftmenu.restconfig="�terst�ll konfiguration";

admleftmenu.facdef="Fabriksinst�llningar";

admleftmenu.upgradewan="Uppgradera fr�n WAN";

admleftmenu.upgradelan="Uppgradera fr�n LAN";

admleftmenu.reboot="Omstart";

var admgwaccess = new Object();
admgwaccess.gwusername="Anv�ndarnamn f�r gatewayen";

admgwaccess.gwpassword="L�senord f�r gatewayen";

//admgwaccess.reconfirm="Bekr�fta genom att ange en g�ng till";
admgwaccess.reconfirm="Bekr�fta losenordet genom att ange det en g�ng till";

admgwaccess.remotemgt="Fj�rrhantering";

admgwaccess.mgtport="Hanteringsport";

admgwaccess.allowip="Till�tna IP";

admgwaccess.all="Alla";

admgwaccess.ipaddr="IP-adress";

admgwaccess.iprange="IP-intervall";

admgwaccess.rmtupgrade="Fj�rruppgradering";

admgwaccess.usehttps="Anv�nd https";

var admsnmp = new Object();
admsnmp.snmp="SNMP";

admsnmp.devname="Enhetsnamn";

admsnmp.snmp_v1v2="Snmp V1/V2";

admsnmp.getcomm="H�mta m�tesplats";

admsnmp.setcomm="Ange m�tesplats";

admsnmp.snmp_v3="Snmp V3";

admsnmp.rwuser="Anv�ndare med beh�righet att l�sa/skriva";

admsnmp.authprotocol="Autentiseringsprotokoll";

admsnmp.md5="MD5";

admsnmp.sha="SHA";

admsnmp.authpassword="Autentiseringsl�senord";

admsnmp.privpassword="Sekretessl�senord";

admsnmp.md5passwd="MD5-l�senord";

admsnmp.trapmgt="Felhantering";

admsnmp.trapto="Fel till";

admsnmp.ipaddr="IP-adress";

admsnmp.iprange="IP-intervall";

var admupnp = new Object();
admupnp.upnp="UPnP";

admupnp.selectpvc="V�lj en PVC-anslutning att binda";

var admigmpproxy = new Object();
admigmpproxy.pvcavail="PVC tillg�nglig";

admigmpproxy.igmpproxy="IGMP-proxy";

var admtr064 = new Object();
admtr064.tr064="TR-064";

var admtr069 = new Object();
admtr069.tr069="TR-069";

admtr069.cpeusername="CPE-anv�ndarnamn";

admtr069.acsurl="ACS-URL";

admtr069.acspassword="ACS-l�senord";

var admwlan = new Object();
admwlan.mgtwlan="Hantering via WLAN";

var admreport = new Object();
admreport.logs="Logg";

admreport.enable="Aktivera";

//admreport.disable="Avaktivera";
admreport.disable="Inaktivera";

admreport.logviewerip="IP-adress f�r Logviewer";

admreport.emailalerts="Varningsmeddelanden via e-post";

admreport.denservice="DoS-gr�nsv�rden";

admreport.events="h�ndelser";

admreport.eventsrange="(20-100)";

admreport.smtpserver="SMTP-postserver";

admreport.emailaddr="E-postadress f�r varningsloggar";

admreport.returnaddr="E-postadress f�r svar";

admreport.alls="ALLA";

admreport.systemlog="Systemlogg";

admreport.accesslog="�tkomstlogg";

admreport.firewalllog="Brandv�ggslogg";

admreport.vpnlog="VPN-logg";

admreport.upnplog="UPnP-logg";

var admping = new Object();
admping.targetip="M�l-IP f�r ping";

admping.size="Pingstorlek";

admping.bytes="Byte";

admping.numbers="Antal ping";

admping.range="(Mellan 1 och 100)";

admping.interval="Pingintervall";

admping.timeout="Tidsgr�ns f�r ping";

admping.milliseconds="millisekunder";

admping.result="Resultat f�r ping";

admping.pktsend="Skicka pkt";

admping.pktrecv="Ta emot pkt";

admping.avgrtt="Med. rtt";

admping.pingtest="Pingtest";

admping.ipaddr_domainname="IP-adress eller dom�nnamn";

admping.unlimited="Obegr�nsad";

var admrestore = new Object();
admrestore.selectfile="V�lj den fil du vill �terst�lla";

var admfacdef = new Object();
admfacdef.refacdefault="�terst�ll fabriksinst�llningar";

admfacdef.yes="Ja";

admfacdef.no="Nej";

var admupgradewan = new Object();
admupgradewan.upgradeweb="Uppgradering av fast programvara fr�n Internet";

admupgradewan.url="URL";

admupgradewan.warning="<B>Varning!</B>Uppgraderingen av fast programvara fr�n WAN kan ta n�gra minuter. <BR>St�ng inte av str�mmen och tryck inte p� �terst�llningsknappen."

admupgradewan.notinterrupted="Du f�r inte avbryta uppgraderingen.";

var admupgradelan = new Object();
admupgradelan.upgradefw="Uppgradering programvara f�r brandv�gg";

admupgradelan.filepath="Fils�kv�g";

admupgradelan.warning="<B>Varning! </B>Uppgraderingen av fast programvara fr�n det lokala <BR>n�tverket kan ta n�gra minuter. St�ng inte av str�mmen <BR>och tryck inte p� �terst�llningsknappen.";

admupgradelan.notinterrupted="Du f�r inte avbryta uppgraderingen.";

var admpostupgrade = new Object();
admpostupgrade.upgradesuccess="Uppgraderingen slutf�rdes.";

admpostupgrade.reboot="Startar om...";

var admreboot = new Object();
admreboot.mode="Omstartsl�ge";

admreboot.hard="H�rd";

admreboot.soft="Mjuk";

admreboot.gwusername="Anv�ndarlistan f�r gatewayen";

admreboot.gwusername="Anv�ndarnamn f�r gatewayen";

admreboot.gwpassword="L�senord f�r gatewayen";

var admbutton =new Object();
admbutton.viewlogs="Visa loggar";

admbutton.starttest="Starta test";

admbutton.backup="S�kerhetskopiera";

admbutton.restore="�terst�ll";

admbutton.browse="Bl�ddra...";

admbutton.upgrade="Uppgradera";

admbutton.clears="Rensa";

admbutton.pagerefresh="Siduppdatering";

admbutton.prevpage="F�reg�ende sida";

admbutton.nextpage="N�sta sida";

admbutton.cancelupgrade="Avbryt uppgraderingar";

admbutton.stop="Stopp";

admbutton.clearlog="Rensa logg";

admbutton.close="St�ng";
