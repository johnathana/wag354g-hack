//Basic Setup
var hsetup = new Object();
hsetup.phase1="Sk�rmen <i>Konfiguration</i> \
		�r den f�rsta sk�rm du ser n�r du g�r till routern. De flesta anv�ndare  \
		kan konfigurera routern och f� den att fungera ordentligt enbart med hj�lp av \
		inst�llningarna p� den h�r sk�rmen. Vissa Internet-leverant�rer kr�ver  \
		att du anger viss information, till exempel anv�ndarnamn, l�senord, IP-adress,  \
		standard-gateway-adress eller DNS-IP-adress. Du kan f� den informationen  \
		fr�n din Internet-leverant�r om den beh�vs.";
hsetup.phase2="Obs! N�r \
		du har konfigurerat inst�llningarna b�r du ange ett nytt l�senord f�r routern  \
		p� sk�rmen <i>S�kerhet</i>. P� s� vis f�rb�ttras s�kerheten och routern  \
		skyddas fr�n obeh�riga �ndringar. Alla anv�ndare som f�rs�ker anv�nda routerns \
		webbaserade verktyg eller installationsguide m�ste ange l�senordet f�r routern. ";
hsetup.phase3="V�lj korrekt \
		tidszon f�r ditt land eller omr�de. Om du bor i ett land med sommartid \
    		beh�ller du markeringen i rutan bredvid <i>Justera klockan automatiskt f�r \
    		sommartid</i>.";
hsetup.phase4="MTU st�r f�r  \
    		Maximum Transmission Unit. Det anger st�rsta till�tna paketstorlek \
    		f�r Internet-�verf�ring. Beh�ll standardinst�llningen <b>Auto</b> om du vill att \
    		routern ska v�lja b�sta MTU f�r din Internet-anslutning. Om du vill ange en \
    		MTU-storlek v�ljer du <b>Manuellt</b> och anger sedan �nskat v�rde (standardv�rdet �r <b> \
    		1400</b>). Du b�r ange ett v�rde p� mellan 1200 och 1500.";
hsetup.phase5="Den h�r posten �r n�dv�ndig f�r vissa Internet-leverant�rer. Du kan f� uppgifterna av dem.";
hsetup.phase6="I routern finns funktioner f�r fyra anslutningstyper:";
hsetup.phase7="Automatisk konfiguration - DHCP";
hsetup.phase8="(Point-to-Point Protocol over Ethernet)";
hsetup.phase9="(Point-to-Point Tunneling Protocol)";
hsetup.phase10="Du kan v�lja de h�r typerna i listrutan bredvid  Internet-anslutning. \
    		Den information som beh�vs och de funktioner som finns tillg�ngliga beror p� \
    		den anslutningstyp som du har valt. En del beskrivningar av den h�r \
    		informationen anges h�r:";		
hsetup.phase11="Internet-IP-Adress och n�tmask";
hsetup.phase12="Det h�r �r routerns IP-adress \
    		och n�tmask som visas f�r externa anv�ndare p� Internet (�ven din \
    		Internet-leverant�r). Om du beh�ver en statisk IP-adress f�r din Internet-anslutning kommer \
    		Internet-leverant�ren att ge dig en statisk IP-adress och n�tmask.";
hsetup.phase13="Internet-leverant�ren f�rser dig med gateway-IP-adressen."
hsetup.phase14="(Domain Name Server)";
hsetup.phase15="Internet-leverant�ren f�rser dig med minst en DNS-IP-adress.";
hsetup.phase16="Anv�ndarnamn och L�senord";
hsetup.phase17="Ange det <b>Anv�ndarnamn</b> och \
		<b>L�senord</b> som du anv�nder n�r du loggar in hos din Internet-leverant�r via en PPPoE- eller PPTP- \
   		anslutning.";
hsetup.phase18="Anslut p� beg�ran";
hsetup.phase19="Du kan konfigurera routern s� att \
    		Internet-anslutningen bryts efter en viss tids inaktivitet \
    		(Maximal vilotid). Om Internet-anslutningen har brutits p� grund av \
    		inaktivitet kan routern med hj�lp av funktionen Anslut p� beg�ran automatiskt \
   		�teruppr�tta anslutningen s� fort du f�rs�ker anv�nda Internet \
   		igen. Om du vill aktivera Anslut p� beg�ran klickar du p� alternativknappen. Om \
    		du vill att Internet-anslutningen ska vara aktiv hela tiden anger du <b>0</b> \
    		i f�ltet <i>Maximal vilotid</i>. Annars anger du efter hur m�nga minuters \
    		inaktiv tid du vill att Internet-anslutningen ska brytas.";
hsetup.phase20="Alternativet Beh�ll anslutning ";
hsetup.phase21="Med det h�r alternativet beh�lls anslutningen \
    		till Internet �ven n�r anslutningen inte �r aktiv. Om du vill anv�nda \
    		det h�r alternativet klickar du p� alternativknappen bredvid <i>Beh�ll anslutning</i>. Den f�rinst�llda \
    		�teruppringningsperioden �r 30 sekunder (med andra ord kontrolleras  \
    		Internet-anslutningen var 30:e sekund).";
hsetup.phase22="Obs! Vissa \
    		kabelleverant�rer kr�ver att du anger en specifik MAC-adress f�r att ansluta till \
    		Internet. Klicka p� fliken <b>System</b> om du vill veta mer. Klicka sedan p� \
    		knappen <b>Hj�lp</b> och l�s om funktionen MAC-kloning.";
hsetup.phase23="LAN";
hsetup.phase24="IP-adress och n�tmask";
hsetup.phase25="Det h�r �r\
    		routerns IP-adress och n�tmask som visas p� det interna n�tverket. Standardv�rdet \
    		�r 192.168.1.1 f�r IP-adressen och 255.255.255.0 f�r \
    		n�tmasken.";
hsetup.phase26="DHCP";
hsetup.phase27="Beh�ll \
    		standardinst�llningen <b>Aktivera</b> om du vill aktivera routerns DHCP-serveralternativ. Om du \
    		redan har en DHCP-server p� ditt n�tverk eller om du inte vill ha en DHCP-server \
    		v�ljer du <b>Avaktivera</b>.";
hsetup.phase28="Ange ett \
    		numeriskt v�rde som DHCP-servern ska b�rja med vid tilldelning av IP-adresser. \
    		B�rja inte med 192.168.1.1 (routerns IP-adress).";
hsetup.phase29="Maximalt antal DHCP-anv�ndare";
hsetup.phase30="Ange \
    		h�gsta antal datorer som du vill att DHCP-servern ska tilldela IP-adresser \
    		till. H�gsta m�jliga antal �r 253, vilket �r m�jligt om 192.168.1.2 �r din \
    		start-IP-adress.";
hsetup.phase31="Klientl�netid \
    		�r den tid som en n�tverksanv�ndare f�r vara ansluten \
    		till routern med aktuell dynamisk IP-adress. Ange hur l�ng \
    		tid i minuter som anv�ndaren f�r \"l�na\" den h�r dynamiska IP-adressen.";
hsetup.phase32="Statisk DNS 1-3 ";
hsetup.phase33="DNS \
    		(Domain Name System) �r den metod som Internet anv�nder f�r att �vers�tta dom�n- och webbplatsnamn \
    		till Internet-adresser eller URL-adresser. Internet-leverant�ren f�rser dig med minst en \
    		DNS-serveradress. Om du vill anv�nda en annan anger du den IP-adressen \
    		i ett av de h�r f�lten. Du kan ange upp till tre DNS-serveradresser  \
    		h�r. Routern anv�nder dem f�r att snabbare komma �t fungerande  \
    		DNS-servrar.";
hsetup.phase34="WINS (Windows Internet Naming Service) hanterar varje dators samspel med \
    		Internet. Om du anv�nder en WINS-server anger du den serverns IP-adress h�r. \
    		I annat fall l�mnar du det tomt.";
hsetup.phase35="Kontrollera alla \
		v�rden och klicka p� <b>Spara inst�llningar</b> om du vill spara inst�llningarna. Klicka p� <b>Avbryt \
		�ndringar</b> om du vill \
		avbryta �ndringar som inte sparats. Du kan testa inst�llningarna genom att ansluta till  \
		Internet. ";    		    		    		

//DDNS
var helpddns = new Object();
helpddns.phase1="Routern har en DDNS-funktion (Dynamic Domain Name System). Med DDNS kan du tilldela ett fast \
		v�rd- och dom�nnamn till en dynamisk Internet-IP-adress. Funktionen �r anv�ndbar om du \
		har en egen webbplats, FTP-server eller en annan server bakom routern. Innan \
		du anv�nder den h�r funktionen m�ste du registrera dig f�r DDNS-tj�nsten p� <i>www.dyndns.org</i>, \
		en DDNS-leverant�r.";
helpddns.phase2="DDNS-tj�nst";
helpddns.phase3="Om du vill avaktivera DDNS-tj�nsten beh�ller du standardinst�llningen <b>Avaktivera</b>. Du aktiverar DDNS-tj�nsten \
		s� h�r:";
helpddns.phase4="Registrera dig f�r en DDNS-tj�nst p�<i>www.dyndns.org</i> och skriv ner \
		ditt anv�ndarnamn,<i> </i>l�senord och<i> </i>v�rdnamn.";
helpddns.phase5="P� sk�rmen <i>DDNS</i> v�ljer du <b>Aktivera.</b>";
helpddns.phase6="Fyll i f�lten <i> Anv�ndarnamn</i>,<i> L�senord</i> och<i> V�rdnamn</i>.";
helpddns.phase7="Spara �ndringarna genom att klicka p� <b>Spara inst�llningar</b>. Klicka p� <b>Avbryt �ndringar</b> om du vill \
		avbryta �ndringar som inte har sparats.";
helpddns.phase8="H�r visas routerns aktuella Internet-IP-adress.";
helpddns.phase9="H�r visas status f�r anslutningen till DDNS-tj�nsten.";
		
//MAC Address Clone
var helpmac =  new Object();
helpmac.phase1="MAC-kloning";
helpmac.phase2="Routerns MAC-adress best�r av en kod p� 12 siffror som tilldelas unik \
    		maskinvara f�r identifiering. Vissa Internet-leverant�rer kr�ver att du registrerar MAC-adressen \
    		f�r det n�tverkskort/den n�tverksadapter som var ansluten till ditt kabel- eller \
    		DSL-modem under installationen. Om Internet-leverant�ren kr�ver att du registrerar din MAC-adress \
    		tar du reda p� adapterns MAC-adress genom att f�lja \
    		anvisningarna f�r datorns operativsystem.";
helpmac.phase3="Windows 98 och Millennium:";
helpmac.phase4="1.  Klicka p� <b>Start</b> och v�lj <b>K�r</b>.";
helpmac.phase5="2.  Skriv <b>winipcfg </b>i f�ltet och tryck p� <b>OK </b>.";
helpmac.phase6="3.  V�lj den Ethernet-adapter du anv�nder.";
helpmac.phase7="4.  Klicka p� <b>Mer information</b>.";
helpmac.phase8="5.  Skriv ner adapterns MAC-adress.";
helpmac.phase9="1.  Klicka p� <b>Start</b> och v�lj <b>K�r</b>.";
helpmac.phase10="2.  Skriv <b>cmd </b>i f�ltet och tryck p� <b>OK</b>.";
helpmac.phase11="3.  Vid kommandotolken k�r du <b>ipconfig /all</b> och noterar adapterns fysiska adress.";
helpmac.phase12="4.  Skriv ner adapterns MAC-adress.";
helpmac.phase13="Om du vill klona n�tverksadapterns MAC-adress till routern och slippa ringa till din \
    		Internet-leverant�r f�r att �ndra den registrerade MAC-adressen g�r du s� h�r:";
helpmac.phase14="Windows 2000 och XP:";
helpmac.phase15="1.  V�lj <b>Aktivera</b>.";
helpmac.phase16="2.  Ange adapterns MAC-adress i f�ltet <i>MAC-adress</i>.";
helpmac.phase17="3.  Klicka p� <b>Spara inst�llningar</b>.";
helpmac.phase18="Du kan avaktivera kloning av MAC-adress genom att beh�lla standardinst�llningen <b>Avaktivera</b>.";

//Advance Routing
var hrouting = new Object();
hrouting.phase1="Routing";
hrouting.phase2="P� sk�rmen <i>Routing</i> kan du st�lla in routerns routingl�ge och routinginst�llningar. \
		 Gatewayl�get rekommenderas f�r de flesta anv�ndare.";
hrouting.phase3="V�lj korrekt funktionsl�ge. Beh�ll standardinst�llningen <b> \
    		 Gateway</b> om du anv�nder routern som v�rd f�r n�tverkets Internet-anslutning (gatewayl�get rekommenderas f�r de flesta anv�ndare). Select <b> \
    		 Router </b>om routern ing�r i ett n�tverk med andra routrar.";
hrouting.phase4="Dynamisk routing (RIP)";
hrouting.phase5="Obs! Den h�r funktionen fungerar inte i gatewayl�ge.";
hrouting.phase6="Med dynamisk routing kan routern automatiskt justeras f�r fysiska �ndringar i \
    		 n�tverkets layout och utbyta routingtabeller med andra routrar. Med \
    		 routern fastst�lls n�tverkspaketens v�g utifr�n l�gst antal \
    		 hopp mellan k�llan och m�let. ";
hrouting.phase7="Om du vill aktivera funktionen f�r dynamisk routing f�r WAN-sidan v�ljer du <b>WAN</b>. \
    		 Du kan aktivera den h�r funktionen f�r LAN-sidan och den tr�dl�sa sidan genom att v�lja <b>LAN och tr�dl�st</b>. \
    		 Du kan aktivera funktionen f�r b�de WAN och LAN genom att v�lja <b> \
    		 B�da</b>. Om du vill avaktivera funktionen f�r dynamisk routing f�r alla data�verf�ringar beh�ller du \
    		 standardinst�llningen <b>Avaktivera</b>. ";
hrouting.phase8="Statisk routing, m�l-IP-adress, n�tmask, gateway och gr�nssnitt";
hrouting.phase9="Om du vill konfigurera statisk routing mellan routern och ett annat n�tverk \
    		 v�ljer du ett nummer i listrutan <i>Statisk routing</i>. (Statisk \
    		 routing �r en f�rbest�md v�g som n�tverksinformation m�ste f�rdas f�r att \
    		 n� en viss v�rd eller ett visst n�tverk.)";
hrouting.phase10="Ange f�ljande uppgifter:";
hrouting.phase11="M�l-IP-adress </b>- \
		  M�l-IP-adressen �r adressen till det n�tverk eller den v�rd som du vill tilldela statisk routing.";
hrouting.phase12="N�tmask </b>- \
		  N�tmasken fastst�ller den del av en IP-adress som �r n�tverksdelen och den  \
    		  del som �r v�rddelen. ";
hrouting.phase13="Gateway </b>- \
		  Det h�r �r IP-adressen f�r den gatewayenhet som medger kontakt mellan routern och n�tverket eller v�rden.";
hrouting.phase14=Beroende p� var m�l-IP-adressen finns v�ljer du \
    		  <b>LAN och tr�dl�st </b>eller <b>WAN </b>i listrutan <i>Gr�nssnitt</i>.";
hrouting.phase15="Du kan spara �ndringarna genom att klicka p� <b>Verkst�ll</b>. Du kan avbryta �ndringar som inte har sparats genom att klicka p� <b> \
    		  Avbryt</b>.";
hrouting.phase16="Upprepa steg 1-4 f�r fler statiska routrar vid behov.";
hrouting.phase17="Ta bort den h�r posten";
hrouting.phase18="Ta bort en statisk routingpost:";
hrouting.phase19="I listrutan <i>Statisk routing</i> v�ljer du postnumret f�r den statiska routingen.";
hrouting.phase20="Klicka p� <b>Ta bort den h�r posten</b>.";
hrouting.phase21="Spara genom att klicka p� <b>Verkst�ll</b>. Avbryt genom att klicka p� <b> \
    		  Avbryt</b>. ";
hrouting.phase22="Visa routingtabell";
hrouting.phase23="Klicka p� \
    		  <b>Visa routingtabell </b>om du vill visa alla giltiga routingposter som anv�nds. M�l-IP-adress, n�tmask, \
    		  gateway och gr�nssnitt visas f�r varje post. Du kan uppdatera visade uppgifter genom att klicka p� <b>Uppdatera</b>. Klicka p�<b>\
    		  St�ng</b> om du vill g� tillbaka till sk�rmen <i>Routing</i>.";
hrouting.phase24="M�l-IP-adress</b> - \
		  M�l-IP-adressen �r adressen till det n�tverk eller den v�rd som den statiska routingen �r tilldelad. ";
hrouting.phase25="N�tmask</b> - \
		  N�tmasken fastst�ller vilken del av en IP-adress som �r n�tverksdelen och vilken del som �r v�rddelen.";
hrouting.phase26="Gateway</b> - Det h�r �r IP-adressen f�r den gatewayenhet som medger kontakt mellan routern och n�tverket eller v�rden.";
hrouting.phase27="Gr�nssnitt</b> - Det h�r gr�nssnittet anger om m�l-IP-adressen �r p� \
    		  <b> LAN och tr�dl�st </b>(interna tr�dbundna eller tr�dl�sa n�tverk), <b>WAN</b> (Internet) eller <b> \
    		  Loopback</b> (ett dummy-n�tverk d�r en dator fungerar som ett n�tverk och som beh�vs f�r vissa program). ";

//Firewall
var hfirewall = new Object();
hfirewall.phase1="Blockera WAN-beg�ran";
hfirewall.phase2="Genom att aktivera funktionen Blockera WAN-beg�ran kan du f�rhindra att n�tverket \
    		  \"pingas\", eller uppt�cks, av andra Internet-anv�ndare. Funktionen Blockera WAN-beg�ran \
    		 st�rker �ven n�tverkets s�kerhet genom att d�lja n�tverksportarna. \
    		 B�da funktionerna g�r det sv�rare f�r \
    		 externa anv�ndare att ta sig in i ditt n�tverk. Den h�r funktionen �r aktiverad \
    		 som standard. V�lj <b>Avaktivera</b> om du vill avaktivera funktionen.";
hfirewall.right="Aktivera eller avaktivera SPI.";

//VPN
var helpvpn = new Object();
helpvpn.phase1="VPN-genomstr�mning";
helpvpn.phase2="Virtual Private Networking (VPN) anv�nds vanligen f�r arbetsrelaterade n�tverk. F�r \
    		VPN-tunnlar finns i routern funktioner f�r IPSec- och PPTP-protokoll.";
helpvpn.phase3="<b>IPSec</b> - IPSec (Internet Protocol Security) �r en<b> </b>svit med protokoll som anv�nds f�r att implementera ett \
      		s�kert utbyte av paket p� IP-skiktet. F�r att m�jligg�ra IPSec-tunnlar i  \
      		routern �r IPSec-genomstr�mning aktiverat som standard. Om du vill avaktivera \
      		IPSec-genomstr�mning avmarkerar du rutan bredvid <i>IPSec</i>.";
helpvpn.phase4="<b>PPTP </b>- PPTP (Point-to-Point Tunneling Protocol) �r den metod som anv�nds f�r att aktivera VPN-sessioner \
      		p� en Windows NT 4.0- eller 2000-server. F�r att m�jligg�ra PPTP-tunnlar i  \
      		routern �r PPTP-genomstr�mning aktiverat som standard. Om du vill avaktivera \
      		PPTP-genomstr�mning avmarkerar du rutan bredvid <i>PPTP</i>.";

helpvpn.right="Du kan v�lja att aktivera PPTP-, L2TP- eller IPSec-genomstr�mning om du vill att dina n�tverksenheter ska \
		kunna kommunicera via VPN.";
//Internet Access
var hfilter = new Object();
hfilter.phase1="Filter";
hfilter.phase2="P� sk�rmen <i>Filter</i> kan du blockera eller till�ta vissa typer av Internet-anv�ndning. \
		 Du kan ange regler f�r Internet�tkomst f�r vissa datorer och konfigurera \
		filter med hj�lp av numren p� n�tverksportarna.";
hfilter.phase3="Med den h�r funktionen kan du anpassa upp till tio olika regler f�r Internet�tkomst \
    		f�r olika datorer, som identifieras med hj�lp av deras IP- eller MAC-adresser. F�r \
    		varje tilldelad dator, under angivna dagar och tidsperioder.";
hfilter.phase4="Skapa eller redigera regler:";
hfilter.phase5="V�lj ett regelnummer \(1-10\) i listrutan.";
hfilter.phase6="Ange ett namn i f�ltet <i>Ange profilnamn</i>.";
hfilter.phase7="Klicka p� <b>Redigera lista med datorer</b>.";
hfilter.phase8="Spara �ndringarna genom att klicka p� <b>Verkst�ll</b>. Klicka p� <b>Avbryt</b> om du vill \
    		avbryta �ndringar som inte sparats. Klicka p� <b>St�ng</b> om du vill �terg� till \
    		sk�rmen <i>Filter</i>.";
hfilter.phase9="Om du vill blockera de angivna datorerna fr�n Internet under de dagar och den \
    		tid du angett beh�ller du standardinst�llningen <b>Avaktivera Internet�tkomst f�r angivna \
    		datorer</b>. Om du vilI att de angivna datorerna ska kunna f� �tkomst till Internet under \
    		de dagar och den tid du angett klickar du p� alternativknappen bredvid<i> Aktivera  \
    		Internet�tkomst f�r angivna datorer</i>.";
hfilter.phase10="Ange datorer med IP-adress eller MAC-adress p� sk�rmen <i>Lista med datorer</i>. Ange  \
    		IP-adresser i <i>IP</i>-f�lten. Om du vill filtrera ett intervall med \
    		IP-adresser fyller du i f�lten f�r <i>IP-intervalI</i>. \
    		Ange MAC-adresser i <i>MAC</i>-f�lten.";
hfilter.phase11="Ange vilken tid �tkomsten ska filtreras. V�lj <b>24 timmar</b><b> </b>eller markera rutan bredvid <i>Fr�n</i>\
    		och ange tidsperiod med hj�lp av listrutorna. ";
hfilter.phase12="Ange vilken tid �tkomsten ska filtreras. V�lj <b>Varje dag</b> eller �nskade veckodagar. ";
hfilter.phase13="Spara �ndringarna och aktivera regeln genom att klicka p� <b>L�gg till regel</b>.";
hfilter.phase14="Upprepa steg 1-9 om du vill skapa eller redigera fler regler.";
hfilter.phase15="Om du vill ta bort en regel f�r Internet�tkomst kan du markera regelnumret och klicka p� <b>Ta bort</b>.";
hfilter.phase16="Om du vill se en sammanfattning av alla regler klickar du p� <b>Sammanfattning</b>. P� sk�rmen <i>\
    		Sammanfattning av Internet-regler</i> visas varje regelnummer liksom regelns \
    		namn, dagar och tid p� dagen. Om du vill ta bort en regel klickar du p� motsvarande ruta och  \
    		sedan p� <b>Ta bort</b>. Klicka p� <b>St�ng</b> om du vill �terg� till \
    		sk�rmen <i>Filter</i>.";
hfilter.phase17="Filtrerat portintervall f�r Internet";
hfilter.phase18="Du kan filtrera datorer via n�tverksportnummer genom att v�lja <b>B�da</b>, <b>TCP</b> eller <b>UDP</b>, \
    		beroende p� vilka protokoll du vill filtrera. Ange sedan<b> </b>vilka portnummer  \
    		du vill filtrera i portnummerf�lten. Datorer som �r anslutna till  \
    		routern kommer inte att f� �tkomst till de portnummer som anges h�r l�ngre. Om du vill  \
    		avaktivera ett filter v�ljer du <b>Avaktivera</b>.";

//share of help string
var hshare = new Object();
hshare.phase1="Kontrollera alla v�rden och klicka p� <b>Spara inst�llningar</b> om du vill spara inst�llningarna. Klicka p� <b>Avbryt \
		�ndringar</b> om du vill avbryta �ndringar som inte har sparats.";


//DMZ
var helpdmz = new Object();
helpdmz.phase1="Med DMZ-v�rdfunktionen kan en lokal anv�ndare bli synlig mot Internet f�r anv�ndning \
		av en s�rskild tj�nst, till exempel Internet-spel eller videokonferenser. \
		Med DMZ-v�rdfunktionen aktiverad vidarebefordras alla portar p� samma g�ng till en dator. Funktionen \
    		Vidarebefordran av en port �r s�krare eftersom den bara �ppnar den port du vill \
    		�ppna, medan DMZ-v�rdfunktionen �ppnar alla portar p� en dator \
    		s� att hela Internet kan se datorn. ";    		
helpdmz.phase2="Alla datorer vars port vidarebefordras m�ste ha sin DHCP-klientfunktion avaktiverad  \
    		och b�r tilldelas en ny statisk IP-adress eftersom dess IP-adress  \
    		kan �ndras n�r DHCP-funktionen anv�nds.";
/***To expose one PC, select enable.***/
helpdmz.phase3="Om du vill exponera en dator v�ljer du ";
helpdmz.phase4="Ange datorns IP-adress i f�ltet <i>IP-adress f�r DMZ-v�rd</i>.";



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
