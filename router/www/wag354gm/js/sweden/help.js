//Basic Setup
var hsetup = new Object();
hsetup.phase1="Skärmen <i>Konfiguration</i> \
		är den första skärm du ser när du går till routern. De flesta användare  \
		kan konfigurera routern och få den att fungera ordentligt enbart med hjälp av \
		inställningarna på den här skärmen. Vissa Internet-leverantörer kräver  \
		att du anger viss information, till exempel användarnamn, lösenord, IP-adress,  \
		standard-gateway-adress eller DNS-IP-adress. Du kan få den informationen  \
		från din Internet-leverantör om den behövs.";
hsetup.phase2="Obs! När \
		du har konfigurerat inställningarna bör du ange ett nytt lösenord för routern  \
		på skärmen <i>Säkerhet</i>. På så vis förbättras säkerheten och routern  \
		skyddas från obehöriga ändringar. Alla användare som försöker använda routerns \
		webbaserade verktyg eller installationsguide måste ange lösenordet för routern. ";
hsetup.phase3="Välj korrekt \
		tidszon för ditt land eller område. Om du bor i ett land med sommartid \
    		behåller du markeringen i rutan bredvid <i>Justera klockan automatiskt för \
    		sommartid</i>.";
hsetup.phase4="MTU står för  \
    		Maximum Transmission Unit. Det anger största tillåtna paketstorlek \
    		för Internet-överföring. Behåll standardinställningen <b>Auto</b> om du vill att \
    		routern ska välja bästa MTU för din Internet-anslutning. Om du vill ange en \
    		MTU-storlek väljer du <b>Manuellt</b> och anger sedan önskat värde (standardvärdet är <b> \
    		1400</b>). Du bör ange ett värde på mellan 1200 och 1500.";
hsetup.phase5="Den här posten är nödvändig för vissa Internet-leverantörer. Du kan få uppgifterna av dem.";
hsetup.phase6="I routern finns funktioner för fyra anslutningstyper:";
hsetup.phase7="Automatisk konfiguration - DHCP";
hsetup.phase8="(Point-to-Point Protocol over Ethernet)";
hsetup.phase9="(Point-to-Point Tunneling Protocol)";
hsetup.phase10="Du kan välja de här typerna i listrutan bredvid  Internet-anslutning. \
    		Den information som behövs och de funktioner som finns tillgängliga beror på \
    		den anslutningstyp som du har valt. En del beskrivningar av den här \
    		informationen anges här:";		
hsetup.phase11="Internet-IP-Adress och nätmask";
hsetup.phase12="Det här är routerns IP-adress \
    		och nätmask som visas för externa användare på Internet (även din \
    		Internet-leverantör). Om du behöver en statisk IP-adress för din Internet-anslutning kommer \
    		Internet-leverantören att ge dig en statisk IP-adress och nätmask.";
hsetup.phase13="Internet-leverantören förser dig med gateway-IP-adressen."
hsetup.phase14="(Domain Name Server)";
hsetup.phase15="Internet-leverantören förser dig med minst en DNS-IP-adress.";
hsetup.phase16="Användarnamn och Lösenord";
hsetup.phase17="Ange det <b>Användarnamn</b> och \
		<b>Lösenord</b> som du använder när du loggar in hos din Internet-leverantör via en PPPoE- eller PPTP- \
   		anslutning.";
hsetup.phase18="Anslut på begäran";
hsetup.phase19="Du kan konfigurera routern så att \
    		Internet-anslutningen bryts efter en viss tids inaktivitet \
    		(Maximal vilotid). Om Internet-anslutningen har brutits på grund av \
    		inaktivitet kan routern med hjälp av funktionen Anslut på begäran automatiskt \
   		återupprätta anslutningen så fort du försöker använda Internet \
   		igen. Om du vill aktivera Anslut på begäran klickar du på alternativknappen. Om \
    		du vill att Internet-anslutningen ska vara aktiv hela tiden anger du <b>0</b> \
    		i fältet <i>Maximal vilotid</i>. Annars anger du efter hur många minuters \
    		inaktiv tid du vill att Internet-anslutningen ska brytas.";
hsetup.phase20="Alternativet Behåll anslutning ";
hsetup.phase21="Med det här alternativet behålls anslutningen \
    		till Internet även när anslutningen inte är aktiv. Om du vill använda \
    		det här alternativet klickar du på alternativknappen bredvid <i>Behåll anslutning</i>. Den förinställda \
    		återuppringningsperioden är 30 sekunder (med andra ord kontrolleras  \
    		Internet-anslutningen var 30:e sekund).";
hsetup.phase22="Obs! Vissa \
    		kabelleverantörer kräver att du anger en specifik MAC-adress för att ansluta till \
    		Internet. Klicka på fliken <b>System</b> om du vill veta mer. Klicka sedan på \
    		knappen <b>Hjälp</b> och läs om funktionen MAC-kloning.";
hsetup.phase23="LAN";
hsetup.phase24="IP-adress och nätmask";
hsetup.phase25="Det här är\
    		routerns IP-adress och nätmask som visas på det interna nätverket. Standardvärdet \
    		är 192.168.1.1 för IP-adressen och 255.255.255.0 för \
    		nätmasken.";
hsetup.phase26="DHCP";
hsetup.phase27="Behåll \
    		standardinställningen <b>Aktivera</b> om du vill aktivera routerns DHCP-serveralternativ. Om du \
    		redan har en DHCP-server på ditt nätverk eller om du inte vill ha en DHCP-server \
    		väljer du <b>Avaktivera</b>.";
hsetup.phase28="Ange ett \
    		numeriskt värde som DHCP-servern ska börja med vid tilldelning av IP-adresser. \
    		Börja inte med 192.168.1.1 (routerns IP-adress).";
hsetup.phase29="Maximalt antal DHCP-användare";
hsetup.phase30="Ange \
    		högsta antal datorer som du vill att DHCP-servern ska tilldela IP-adresser \
    		till. Högsta möjliga antal är 253, vilket är möjligt om 192.168.1.2 är din \
    		start-IP-adress.";
hsetup.phase31="Klientlånetid \
    		är den tid som en nätverksanvändare får vara ansluten \
    		till routern med aktuell dynamisk IP-adress. Ange hur lång \
    		tid i minuter som användaren får \"låna\" den här dynamiska IP-adressen.";
hsetup.phase32="Statisk DNS 1-3 ";
hsetup.phase33="DNS \
    		(Domain Name System) är den metod som Internet använder för att översätta domän- och webbplatsnamn \
    		till Internet-adresser eller URL-adresser. Internet-leverantören förser dig med minst en \
    		DNS-serveradress. Om du vill använda en annan anger du den IP-adressen \
    		i ett av de här fälten. Du kan ange upp till tre DNS-serveradresser  \
    		här. Routern använder dem för att snabbare komma åt fungerande  \
    		DNS-servrar.";
hsetup.phase34="WINS (Windows Internet Naming Service) hanterar varje dators samspel med \
    		Internet. Om du använder en WINS-server anger du den serverns IP-adress här. \
    		I annat fall lämnar du det tomt.";
hsetup.phase35="Kontrollera alla \
		värden och klicka på <b>Spara inställningar</b> om du vill spara inställningarna. Klicka på <b>Avbryt \
		ändringar</b> om du vill \
		avbryta ändringar som inte sparats. Du kan testa inställningarna genom att ansluta till  \
		Internet. ";    		    		    		

//DDNS
var helpddns = new Object();
helpddns.phase1="Routern har en DDNS-funktion (Dynamic Domain Name System). Med DDNS kan du tilldela ett fast \
		värd- och domännamn till en dynamisk Internet-IP-adress. Funktionen är användbar om du \
		har en egen webbplats, FTP-server eller en annan server bakom routern. Innan \
		du använder den här funktionen måste du registrera dig för DDNS-tjänsten på <i>www.dyndns.org</i>, \
		en DDNS-leverantör.";
helpddns.phase2="DDNS-tjänst";
helpddns.phase3="Om du vill avaktivera DDNS-tjänsten behåller du standardinställningen <b>Avaktivera</b>. Du aktiverar DDNS-tjänsten \
		så här:";
helpddns.phase4="Registrera dig för en DDNS-tjänst på<i>www.dyndns.org</i> och skriv ner \
		ditt användarnamn,<i> </i>lösenord och<i> </i>värdnamn.";
helpddns.phase5="På skärmen <i>DDNS</i> väljer du <b>Aktivera.</b>";
helpddns.phase6="Fyll i fälten <i> Användarnamn</i>,<i> Lösenord</i> och<i> Värdnamn</i>.";
helpddns.phase7="Spara ändringarna genom att klicka på <b>Spara inställningar</b>. Klicka på <b>Avbryt ändringar</b> om du vill \
		avbryta ändringar som inte har sparats.";
helpddns.phase8="Här visas routerns aktuella Internet-IP-adress.";
helpddns.phase9="Här visas status för anslutningen till DDNS-tjänsten.";
		
//MAC Address Clone
var helpmac =  new Object();
helpmac.phase1="MAC-kloning";
helpmac.phase2="Routerns MAC-adress består av en kod på 12 siffror som tilldelas unik \
    		maskinvara för identifiering. Vissa Internet-leverantörer kräver att du registrerar MAC-adressen \
    		för det nätverkskort/den nätverksadapter som var ansluten till ditt kabel- eller \
    		DSL-modem under installationen. Om Internet-leverantören kräver att du registrerar din MAC-adress \
    		tar du reda på adapterns MAC-adress genom att följa \
    		anvisningarna för datorns operativsystem.";
helpmac.phase3="Windows 98 och Millennium:";
helpmac.phase4="1.  Klicka på <b>Start</b> och välj <b>Kör</b>.";
helpmac.phase5="2.  Skriv <b>winipcfg </b>i fältet och tryck på <b>OK </b>.";
helpmac.phase6="3.  Välj den Ethernet-adapter du använder.";
helpmac.phase7="4.  Klicka på <b>Mer information</b>.";
helpmac.phase8="5.  Skriv ner adapterns MAC-adress.";
helpmac.phase9="1.  Klicka på <b>Start</b> och välj <b>Kör</b>.";
helpmac.phase10="2.  Skriv <b>cmd </b>i fältet och tryck på <b>OK</b>.";
helpmac.phase11="3.  Vid kommandotolken kör du <b>ipconfig /all</b> och noterar adapterns fysiska adress.";
helpmac.phase12="4.  Skriv ner adapterns MAC-adress.";
helpmac.phase13="Om du vill klona nätverksadapterns MAC-adress till routern och slippa ringa till din \
    		Internet-leverantör för att ändra den registrerade MAC-adressen gör du så här:";
helpmac.phase14="Windows 2000 och XP:";
helpmac.phase15="1.  Välj <b>Aktivera</b>.";
helpmac.phase16="2.  Ange adapterns MAC-adress i fältet <i>MAC-adress</i>.";
helpmac.phase17="3.  Klicka på <b>Spara inställningar</b>.";
helpmac.phase18="Du kan avaktivera kloning av MAC-adress genom att behålla standardinställningen <b>Avaktivera</b>.";

//Advance Routing
var hrouting = new Object();
hrouting.phase1="Routing";
hrouting.phase2="På skärmen <i>Routing</i> kan du ställa in routerns routingläge och routinginställningar. \
		 Gatewayläget rekommenderas för de flesta användare.";
hrouting.phase3="Välj korrekt funktionsläge. Behåll standardinställningen <b> \
    		 Gateway</b> om du använder routern som värd för nätverkets Internet-anslutning (gatewayläget rekommenderas för de flesta användare). Select <b> \
    		 Router </b>om routern ingår i ett nätverk med andra routrar.";
hrouting.phase4="Dynamisk routing (RIP)";
hrouting.phase5="Obs! Den här funktionen fungerar inte i gatewayläge.";
hrouting.phase6="Med dynamisk routing kan routern automatiskt justeras för fysiska ändringar i \
    		 nätverkets layout och utbyta routingtabeller med andra routrar. Med \
    		 routern fastställs nätverkspaketens väg utifrån lägst antal \
    		 hopp mellan källan och målet. ";
hrouting.phase7="Om du vill aktivera funktionen för dynamisk routing för WAN-sidan väljer du <b>WAN</b>. \
    		 Du kan aktivera den här funktionen för LAN-sidan och den trådlösa sidan genom att välja <b>LAN och trådlöst</b>. \
    		 Du kan aktivera funktionen för både WAN och LAN genom att välja <b> \
    		 Båda</b>. Om du vill avaktivera funktionen för dynamisk routing för alla dataöverföringar behåller du \
    		 standardinställningen <b>Avaktivera</b>. ";
hrouting.phase8="Statisk routing, mål-IP-adress, nätmask, gateway och gränssnitt";
hrouting.phase9="Om du vill konfigurera statisk routing mellan routern och ett annat nätverk \
    		 väljer du ett nummer i listrutan <i>Statisk routing</i>. (Statisk \
    		 routing är en förbestämd väg som nätverksinformation måste färdas för att \
    		 nå en viss värd eller ett visst nätverk.)";
hrouting.phase10="Ange följande uppgifter:";
hrouting.phase11="Mål-IP-adress </b>- \
		  Mål-IP-adressen är adressen till det nätverk eller den värd som du vill tilldela statisk routing.";
hrouting.phase12="Nätmask </b>- \
		  Nätmasken fastställer den del av en IP-adress som är nätverksdelen och den  \
    		  del som är värddelen. ";
hrouting.phase13="Gateway </b>- \
		  Det här är IP-adressen för den gatewayenhet som medger kontakt mellan routern och nätverket eller värden.";
hrouting.phase14=Beroende på var mål-IP-adressen finns väljer du \
    		  <b>LAN och trådlöst </b>eller <b>WAN </b>i listrutan <i>Gränssnitt</i>.";
hrouting.phase15="Du kan spara ändringarna genom att klicka på <b>Verkställ</b>. Du kan avbryta ändringar som inte har sparats genom att klicka på <b> \
    		  Avbryt</b>.";
hrouting.phase16="Upprepa steg 1-4 för fler statiska routrar vid behov.";
hrouting.phase17="Ta bort den här posten";
hrouting.phase18="Ta bort en statisk routingpost:";
hrouting.phase19="I listrutan <i>Statisk routing</i> väljer du postnumret för den statiska routingen.";
hrouting.phase20="Klicka på <b>Ta bort den här posten</b>.";
hrouting.phase21="Spara genom att klicka på <b>Verkställ</b>. Avbryt genom att klicka på <b> \
    		  Avbryt</b>. ";
hrouting.phase22="Visa routingtabell";
hrouting.phase23="Klicka på \
    		  <b>Visa routingtabell </b>om du vill visa alla giltiga routingposter som används. Mål-IP-adress, nätmask, \
    		  gateway och gränssnitt visas för varje post. Du kan uppdatera visade uppgifter genom att klicka på <b>Uppdatera</b>. Klicka på<b>\
    		  Stäng</b> om du vill gå tillbaka till skärmen <i>Routing</i>.";
hrouting.phase24="Mål-IP-adress</b> - \
		  Mål-IP-adressen är adressen till det nätverk eller den värd som den statiska routingen är tilldelad. ";
hrouting.phase25="Nätmask</b> - \
		  Nätmasken fastställer vilken del av en IP-adress som är nätverksdelen och vilken del som är värddelen.";
hrouting.phase26="Gateway</b> - Det här är IP-adressen för den gatewayenhet som medger kontakt mellan routern och nätverket eller värden.";
hrouting.phase27="Gränssnitt</b> - Det här gränssnittet anger om mål-IP-adressen är på \
    		  <b> LAN och trådlöst </b>(interna trådbundna eller trådlösa nätverk), <b>WAN</b> (Internet) eller <b> \
    		  Loopback</b> (ett dummy-nätverk där en dator fungerar som ett nätverk och som behövs för vissa program). ";

//Firewall
var hfirewall = new Object();
hfirewall.phase1="Blockera WAN-begäran";
hfirewall.phase2="Genom att aktivera funktionen Blockera WAN-begäran kan du förhindra att nätverket \
    		  \"pingas\", eller upptäcks, av andra Internet-användare. Funktionen Blockera WAN-begäran \
    		 stärker även nätverkets säkerhet genom att dölja nätverksportarna. \
    		 Båda funktionerna gör det svårare för \
    		 externa användare att ta sig in i ditt nätverk. Den här funktionen är aktiverad \
    		 som standard. Välj <b>Avaktivera</b> om du vill avaktivera funktionen.";
hfirewall.right="Aktivera eller avaktivera SPI.";

//VPN
var helpvpn = new Object();
helpvpn.phase1="VPN-genomströmning";
helpvpn.phase2="Virtual Private Networking (VPN) används vanligen för arbetsrelaterade nätverk. För \
    		VPN-tunnlar finns i routern funktioner för IPSec- och PPTP-protokoll.";
helpvpn.phase3="<b>IPSec</b> - IPSec (Internet Protocol Security) är en<b> </b>svit med protokoll som används för att implementera ett \
      		säkert utbyte av paket på IP-skiktet. För att möjliggöra IPSec-tunnlar i  \
      		routern är IPSec-genomströmning aktiverat som standard. Om du vill avaktivera \
      		IPSec-genomströmning avmarkerar du rutan bredvid <i>IPSec</i>.";
helpvpn.phase4="<b>PPTP </b>- PPTP (Point-to-Point Tunneling Protocol) är den metod som används för att aktivera VPN-sessioner \
      		på en Windows NT 4.0- eller 2000-server. För att möjliggöra PPTP-tunnlar i  \
      		routern är PPTP-genomströmning aktiverat som standard. Om du vill avaktivera \
      		PPTP-genomströmning avmarkerar du rutan bredvid <i>PPTP</i>.";

helpvpn.right="Du kan välja att aktivera PPTP-, L2TP- eller IPSec-genomströmning om du vill att dina nätverksenheter ska \
		kunna kommunicera via VPN.";
//Internet Access
var hfilter = new Object();
hfilter.phase1="Filter";
hfilter.phase2="På skärmen <i>Filter</i> kan du blockera eller tillåta vissa typer av Internet-användning. \
		 Du kan ange regler för Internetåtkomst för vissa datorer och konfigurera \
		filter med hjälp av numren på nätverksportarna.";
hfilter.phase3="Med den här funktionen kan du anpassa upp till tio olika regler för Internetåtkomst \
    		för olika datorer, som identifieras med hjälp av deras IP- eller MAC-adresser. För \
    		varje tilldelad dator, under angivna dagar och tidsperioder.";
hfilter.phase4="Skapa eller redigera regler:";
hfilter.phase5="Välj ett regelnummer \(1-10\) i listrutan.";
hfilter.phase6="Ange ett namn i fältet <i>Ange profilnamn</i>.";
hfilter.phase7="Klicka på <b>Redigera lista med datorer</b>.";
hfilter.phase8="Spara ändringarna genom att klicka på <b>Verkställ</b>. Klicka på <b>Avbryt</b> om du vill \
    		avbryta ändringar som inte sparats. Klicka på <b>Stäng</b> om du vill återgå till \
    		skärmen <i>Filter</i>.";
hfilter.phase9="Om du vill blockera de angivna datorerna från Internet under de dagar och den \
    		tid du angett behåller du standardinställningen <b>Avaktivera Internetåtkomst för angivna \
    		datorer</b>. Om du vilI att de angivna datorerna ska kunna få åtkomst till Internet under \
    		de dagar och den tid du angett klickar du på alternativknappen bredvid<i> Aktivera  \
    		Internetåtkomst för angivna datorer</i>.";
hfilter.phase10="Ange datorer med IP-adress eller MAC-adress på skärmen <i>Lista med datorer</i>. Ange  \
    		IP-adresser i <i>IP</i>-fälten. Om du vill filtrera ett intervall med \
    		IP-adresser fyller du i fälten för <i>IP-intervalI</i>. \
    		Ange MAC-adresser i <i>MAC</i>-fälten.";
hfilter.phase11="Ange vilken tid åtkomsten ska filtreras. Välj <b>24 timmar</b><b> </b>eller markera rutan bredvid <i>Från</i>\
    		och ange tidsperiod med hjälp av listrutorna. ";
hfilter.phase12="Ange vilken tid åtkomsten ska filtreras. Välj <b>Varje dag</b> eller önskade veckodagar. ";
hfilter.phase13="Spara ändringarna och aktivera regeln genom att klicka på <b>Lägg till regel</b>.";
hfilter.phase14="Upprepa steg 1-9 om du vill skapa eller redigera fler regler.";
hfilter.phase15="Om du vill ta bort en regel för Internetåtkomst kan du markera regelnumret och klicka på <b>Ta bort</b>.";
hfilter.phase16="Om du vill se en sammanfattning av alla regler klickar du på <b>Sammanfattning</b>. På skärmen <i>\
    		Sammanfattning av Internet-regler</i> visas varje regelnummer liksom regelns \
    		namn, dagar och tid på dagen. Om du vill ta bort en regel klickar du på motsvarande ruta och  \
    		sedan på <b>Ta bort</b>. Klicka på <b>Stäng</b> om du vill återgå till \
    		skärmen <i>Filter</i>.";
hfilter.phase17="Filtrerat portintervall för Internet";
hfilter.phase18="Du kan filtrera datorer via nätverksportnummer genom att välja <b>Båda</b>, <b>TCP</b> eller <b>UDP</b>, \
    		beroende på vilka protokoll du vill filtrera. Ange sedan<b> </b>vilka portnummer  \
    		du vill filtrera i portnummerfälten. Datorer som är anslutna till  \
    		routern kommer inte att få åtkomst till de portnummer som anges här längre. Om du vill  \
    		avaktivera ett filter väljer du <b>Avaktivera</b>.";

//share of help string
var hshare = new Object();
hshare.phase1="Kontrollera alla värden och klicka på <b>Spara inställningar</b> om du vill spara inställningarna. Klicka på <b>Avbryt \
		ändringar</b> om du vill avbryta ändringar som inte har sparats.";


//DMZ
var helpdmz = new Object();
helpdmz.phase1="Med DMZ-värdfunktionen kan en lokal användare bli synlig mot Internet för användning \
		av en särskild tjänst, till exempel Internet-spel eller videokonferenser. \
		Med DMZ-värdfunktionen aktiverad vidarebefordras alla portar på samma gång till en dator. Funktionen \
    		Vidarebefordran av en port är säkrare eftersom den bara öppnar den port du vill \
    		öppna, medan DMZ-värdfunktionen öppnar alla portar på en dator \
    		så att hela Internet kan se datorn. ";    		
helpdmz.phase2="Alla datorer vars port vidarebefordras måste ha sin DHCP-klientfunktion avaktiverad  \
    		och bör tilldelas en ny statisk IP-adress eftersom dess IP-adress  \
    		kan ändras när DHCP-funktionen används.";
/***To expose one PC, select enable.***/
helpdmz.phase3="Om du vill exponera en dator väljer du ";
helpdmz.phase4="Ange datorns IP-adress i fältet <i>IP-adress för DMZ-värd</i>.";



//help number string
var hnum = new Object();
hnum.one="1."
hnum.two="2."
hnum.three="3."
hnum.four="4."
hnum.five="5."
hnum.six="5."
hnum.seven="6."
hnum.eight="7."
hnum.night="8."
