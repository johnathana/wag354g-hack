var admtopmenu = new Object();
admtopmenu.credval="Bekräfta behörighet";

admtopmenu.manage="Hantering";

admtopmenu.report="Rapportering";

admtopmenu.diag="Diagnostik";

admtopmenu.facdef="Fabriksinställningar";

admtopmenu.bakrest="Säkerhetskopiering och återställning";

//admtopmenu.upgrade="Uppgradera fast programvara";
admtopmenu.upgrade="Uppgradera programvara";

admtopmenu.reboot="Omstart";

var admleftmenu = new Object();
admleftmenu.gwaccess="Gatewayåtkomst";

admleftmenu.localgwaccess="Lokal gatewayåtkomst";

admleftmenu.remotegwaccess="Fjärråtkomst av gatewayen";

admleftmenu.remoteupgrade="Fjärruppgradering";

admleftmenu.snmp="SNMP";

admleftmenu.upnp="UPnP";

admleftmenu.tr064="TR064";

admleftmenu.tr069="TR-069";

admleftmenu.igmpproxy="IGMP-proxy";

admleftmenu.wlan="WLAN";

admleftmenu.report="Rapportering";

admleftmenu.emailalt="Varningsmeddelanden via e-post";

admleftmenu.ping="Pingtest";

admleftmenu.pingparam="Parametrar för pingtest";

admleftmenu.bakconfig="Konfiguration av säkerhetskopiering";

admleftmenu.restconfig="Återställ konfiguration";

admleftmenu.facdef="Fabriksinställningar";

admleftmenu.upgradewan="Uppgradera från WAN";

admleftmenu.upgradelan="Uppgradera från LAN";

admleftmenu.reboot="Omstart";

var admgwaccess = new Object();
admgwaccess.gwusername="Användarnamn för gatewayen";

admgwaccess.gwpassword="Lösenord för gatewayen";

//admgwaccess.reconfirm="Bekräfta genom att ange en gång till";
admgwaccess.reconfirm="Bekräfta losenordet genom att ange det en gång till";

admgwaccess.remotemgt="Fjärrhantering";

admgwaccess.mgtport="Hanteringsport";

admgwaccess.allowip="Tillåtna IP";

admgwaccess.all="Alla";

admgwaccess.ipaddr="IP-adress";

admgwaccess.iprange="IP-intervall";

admgwaccess.rmtupgrade="Fjärruppgradering";

admgwaccess.usehttps="Använd https";

var admsnmp = new Object();
admsnmp.snmp="SNMP";

admsnmp.devname="Enhetsnamn";

admsnmp.snmp_v1v2="Snmp V1/V2";

admsnmp.getcomm="Hämta mötesplats";

admsnmp.setcomm="Ange mötesplats";

admsnmp.snmp_v3="Snmp V3";

admsnmp.rwuser="Användare med behörighet att läsa/skriva";

admsnmp.authprotocol="Autentiseringsprotokoll";

admsnmp.md5="MD5";

admsnmp.sha="SHA";

admsnmp.authpassword="Autentiseringslösenord";

admsnmp.privpassword="Sekretesslösenord";

admsnmp.md5passwd="MD5-lösenord";

admsnmp.trapmgt="Felhantering";

admsnmp.trapto="Fel till";

admsnmp.ipaddr="IP-adress";

admsnmp.iprange="IP-intervall";

var admupnp = new Object();
admupnp.upnp="UPnP";

admupnp.selectpvc="Välj en PVC-anslutning att binda";

var admigmpproxy = new Object();
admigmpproxy.pvcavail="PVC tillgänglig";

admigmpproxy.igmpproxy="IGMP-proxy";

var admtr064 = new Object();
admtr064.tr064="TR-064";

var admtr069 = new Object();
admtr069.tr069="TR-069";

admtr069.cpeusername="CPE-användarnamn";

admtr069.acsurl="ACS-URL";

admtr069.acspassword="ACS-lösenord";

var admwlan = new Object();
admwlan.mgtwlan="Hantering via WLAN";

var admreport = new Object();
admreport.logs="Logg";

admreport.enable="Aktivera";

//admreport.disable="Avaktivera";
admreport.disable="Inaktivera";

admreport.logviewerip="IP-adress för Logviewer";

admreport.emailalerts="Varningsmeddelanden via e-post";

admreport.denservice="DoS-gränsvärden";

admreport.events="händelser";

admreport.eventsrange="(20-100)";

admreport.smtpserver="SMTP-postserver";

admreport.emailaddr="E-postadress för varningsloggar";

admreport.returnaddr="E-postadress för svar";

admreport.alls="ALLA";

admreport.systemlog="Systemlogg";

admreport.accesslog="Åtkomstlogg";

admreport.firewalllog="Brandväggslogg";

admreport.vpnlog="VPN-logg";

admreport.upnplog="UPnP-logg";

var admping = new Object();
admping.targetip="Mål-IP för ping";

admping.size="Pingstorlek";

admping.bytes="Byte";

admping.numbers="Antal ping";

admping.range="(Mellan 1 och 100)";

admping.interval="Pingintervall";

admping.timeout="Tidsgräns för ping";

admping.milliseconds="millisekunder";

admping.result="Resultat för ping";

admping.pktsend="Skicka pkt";

admping.pktrecv="Ta emot pkt";

admping.avgrtt="Med. rtt";

admping.pingtest="Pingtest";

admping.ipaddr_domainname="IP-adress eller domännamn";

admping.unlimited="Obegränsad";

var admrestore = new Object();
admrestore.selectfile="Välj den fil du vill återställa";

var admfacdef = new Object();
admfacdef.refacdefault="Återställ fabriksinställningar";

admfacdef.yes="Ja";

admfacdef.no="Nej";

var admupgradewan = new Object();
admupgradewan.upgradeweb="Uppgradering av fast programvara från Internet";

admupgradewan.url="URL";

admupgradewan.warning="<B>Varning!</B>Uppgraderingen av fast programvara från WAN kan ta några minuter. <BR>Stäng inte av strömmen och tryck inte på återställningsknappen."

admupgradewan.notinterrupted="Du får inte avbryta uppgraderingen.";

var admupgradelan = new Object();
admupgradelan.upgradefw="Uppgradering programvara för brandvägg";

admupgradelan.filepath="Filsökväg";

admupgradelan.warning="<B>Varning! </B>Uppgraderingen av fast programvara från det lokala <BR>nätverket kan ta några minuter. Stäng inte av strömmen <BR>och tryck inte på återställningsknappen.";

admupgradelan.notinterrupted="Du får inte avbryta uppgraderingen.";

var admpostupgrade = new Object();
admpostupgrade.upgradesuccess="Uppgraderingen slutfördes.";

admpostupgrade.reboot="Startar om...";

var admreboot = new Object();
admreboot.mode="Omstartsläge";

admreboot.hard="Hård";

admreboot.soft="Mjuk";

admreboot.gwusername="Användarlistan för gatewayen";

admreboot.gwusername="Användarnamn för gatewayen";

admreboot.gwpassword="Lösenord för gatewayen";

var admbutton =new Object();
admbutton.viewlogs="Visa loggar";

admbutton.starttest="Starta test";

admbutton.backup="Säkerhetskopiera";

admbutton.restore="Återställ";

admbutton.browse="Bläddra...";

admbutton.upgrade="Uppgradera";

admbutton.clears="Rensa";

admbutton.pagerefresh="Siduppdatering";

admbutton.prevpage="Föregående sida";

admbutton.nextpage="Nästa sida";

admbutton.cancelupgrade="Avbryt uppgraderingar";

admbutton.stop="Stopp";

admbutton.clearlog="Rensa logg";

admbutton.close="Stäng";
