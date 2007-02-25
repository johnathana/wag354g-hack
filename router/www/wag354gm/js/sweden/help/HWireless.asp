<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<!-- saved from url=(0037)http://192.168.1.1/help/HWireless.asp -->
<!-- saved from url=(0022)http://internet.e-mail --><HTML><HEAD><TITLE>Hjälp: Trådlöst</TITLE>
<META http-equiv=Content-Type content="text/html; charset=windows-1252">
<STYLE>P.MsoNormal { FONT-SIZE: 12pt; FONT-FAMILY: "Times New Roman"; mso-style-parent: ""; margin-left:0cm; margin-right:0cm; margin-top:0cm; margin-bottom:0pt } TABLE.MsoNormalTable { FONT-SIZE: 10pt; FONT-FAMILY: "Times New Roman"; mso-style-parent: "" }
</STYLE>

<META content="Microsoft FrontPage 4.0" name=GENERATOR></HEAD>
<BODY bgColor=white>
<FORM>
<table border="0" width="100%">
  <tr>
    <td width="75%"><font face="Arial" color="blue" size="5"><b>Trådlöst </b></font></td>
    <td width="25%">
      <p align="right"><FONT color=#3333ff><B><SPAN
style="FONT-SIZE: 16pt; FONT-FAMILY: Arial"><A href="javascript:print();"><IMG height=44
src="../image/Printer.gif" width=82 align=center
border=0></A></SPAN></B></FONT></p>
    </td>
  </tr>
</table>
<P class=MsoNormal><br>
<font face="Arial" size="3">På skärmen <i>Trådlöst</i> kan du anpassa inställningarna
för dataöverföring. I de flesta fall bör du behålla standardinställningarna
för de avancerade inställningarna på den här skärmen.</font><br>
<font size="3">&nbsp;</font></P>
<table width="100%" border="4">
  <tbody>
    <tr vAlign="top">
      <td width="29%"><font face="Arial" size="3"><b>Autentiseringstyp</b></font>
      <td width="70%"><font face="Arial" size="3">Standardvärdet är <b>Auto</b>
        , vilket innebär att autentisering av typen Öppet system eller Delad
        nyckel används. Vid autentisering med Öppet system delar INTE avsändare
        och mottagare WEP-nyckel för autentisering. Vid autentisering med Delad
        nyckel delar avsändaren och mottagaren en WEP-nyckel för autentisering.
        Om du bara vill använda autentisering med delad nyckel väljer du <b>Delad
        nyckel</b>.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>Kontrollera överföringshastighet</b></font>
      <td><font face="Arial" size="3">Standardinställningen är <b>Auto</b>.
        Intervallet är mellan 1 och 54 Mbit/s.<br>
        Hastigheten för dataöverföring bör anges utifrån hastigheten i det
        trådlösa nätverket. Du kan välja mellan olika överföringshastigheter
        eller behålla standardinställningen <b>Auto</b> om du vill att
        gatewayen automatiskt ska använda den snabbast tillgängliga
        datahastigheten och aktivera funktionen Automatisk tillbakagång. Med
        funktionen Automatisk tillbakagång används den bästa möjliga
        anslutningshastighet mellan gatewayen och en trådlös klient.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>Signalintervall</b></font>
      <td><font face="Arial" size="3">Standardvärdet är <b>100</b> . Ange ett
        värde mellan 1 och 65 535 millisekunder. Värdet för signalintervall
        anger signalens frekvensintervall. En signal är ett paket som sänds av
        gatewayen för att synkronisera det trådlösa nätverket.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>DTIM-intervall</b></font>
      <td><font face="Arial" size="3">Standardvärdet är <b>1</b> . Det här värdet,
        som ligger på mellan 1 och 255 millisekunder, anger intervallet för
        DTIM-meddelandet (Delivery Traffic Indication Message). Ett DTIM-fält
        är ett nedräkningsfält som meddelar klienter om nästa fönster för
        lyssning av broadcast- och multicast-meddelanden. När gatewayen har
        buffrat broadcast- eller multicast-meddelanden för associerade klienter
        sänder den nästa DTIM med ett DTIM-intervallvärde. Klienterna
        identifierar signalpaketen och aktiveras för att ta emot broadcast- och
        multicast-meddelandena.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>Fragmenteringsgränsvärde</b></font>
      <td><font face="Arial" size="3">Det här värdet ska lämnas på
        standardinställningen <b>2346</b> . Intervallet är 256-2346 byte. Det
        anger den maximala storleken för ett paket innan data fragmenteras till
        flera paket. Om du får en hög paketfelfrekvens kan du prova med att öka
        värdet något. Om du anger ett för lågt värde kan nätverksprestandan
        försämras. Det rekommenderas att du bara gör mindre ändringar av
        detta värde.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>RTS-gränsvärde</b></font>
      <td><font face="Arial" size="3">Det här värdet ska lämnas på
        standardinställningen <b>2347</b> . Intervallet är 0-2347 byte.<br>
        Om du får problem med inkonsekventa dataflöden rekommenderas endast
        mindre ändringar. Om ett nätverkspaket är mindre än den förinställda
        RTS-gränsvärdesstorleken aktiveras inte RTS/CTS-mekanismen. Gatewayen
        skickar RTS-ramar (Request to Send) till en viss mottagarstation och förhandlar
        sedan om sändningen av en dataram. Efter att ha tagit emot en RTS
        svarar den trådlösa stationen med en CTS-ram (Clear to Send) som bekräftar
        rätten att påbörja överföringen.</font>
    </tr>
  </tbody>
</table>
&nbsp;
<P class=MsoNormal>&nbsp;</P>
<table width="100%" border="4">
  <tbody>
    <tr vAlign="top">
      <td width="29%"><font face="Arial" size="3"><b>Trådlös MAC-filtrering</b></font>
      <td width="70%"><font face="Arial" size="3">Med funktionen Trådlös MAC-filtrering
        kan du styra vilka trådlöst anslutna datorer som får kommunicera med
        gatewayen utifrån deras MAC-adress. Om du vill avaktivera funktionen Trådlös
        MAC-filtrering behåller du standardinställningen <b>Tillåt alla</b>.
        Om du vill konfigurera ett filter klickar du på <b>Begränsa åtkomst</b>
        och gör så här:</font><font size="3"><br>
        </font><font face="Arial" size="3"><br>
        1.</font><font face="Times New Roman" size="1"> </font><font face="Arial" size="3">Om
        du vill blockera vissa trådlöst anslutna datorer från att kommunicera
        med gatewayen behåller du standardinställningen <b>Hindra datorer som
        anges nedan att få åtkomst till det trådlösa nätverket </b>. Om du
        vill tillåta vissa trådlöst anslutna datorer att kommunicera med
        gatewayen klickar du på alternativknappen bredvid </font><font face="Arial" color="red" size="3"><b>Tillåt
        endast datorer som anges nedan att få åtkomst till det trådlösa nätverket
        </b></font><font face="Arial" size="3">.</font><font size="3"><br>
        </font><font face="Arial" size="3"><br>
        2.</font><font face="Times New Roman" size="1"> </font><font face="Arial" size="3">Klicka
        på <b>Redigera åtkomstlistan för MAC-adresser</b>. Ange MAC-adresser
        i <i>MAC</i> -fälten.<br>
        Obs! I varje MAC-fält ska MAC-adressen anges i följande format:
        xxxxxxxxxxxx (varje x motsvarar ett tecken i MAC-adressen).</font><font size="3"><br>
        </font><font face="Arial" size="3"><br>
        3.</font><font face="Times New Roman" size="1"> </font><font face="Arial" size="3">Spara
        dina val genom att klicka på Spara inställningar. Klicka på <b>Avbryt
        ändringar</b> om du vill avbryta ändringar som inte sparats. Klicka på
        <b>Stäng</b> om du vill återgå till skärmen <i>Avancerat trådlöst</i>
        utan att spara ändringarna.</font>
  </tbody>
</table>
<p><font face="Arial" size="3"><b>Trådlöst</b></font></p>
<table width="100%" border="4">
  <tbody>
    <tr vAlign="top">
      <td width="29%"><font face="Arial" size="3"><b>Läge</b></font>
      <td width="70%"><font face="Arial" size="3">Behåll standardinställningen
        <b>Blandat läge</b> om det ingår </font><font face="Arial" color="red" size="3">Wireless-B</font><font size="3">
        </font><font face="Arial" size="3">enheter i nätverket. Välj <b>Endast
        G-läge</b> om alla enheter är av typen Wireless-G. Välj <b>Avaktivera</b>
        om du vill avaktivera det trådlösa nätverket. Om du vill begränsa nätverket
        till enbart 802.11b-enheter väljer du <b>Endast B-läge</b>.</font><font face="Times New Roman" size="3">
        </font><font size="3">&nbsp;</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>SSID</b></font>
      <td><font face="Arial" size="3">SSID är det nätverksnamn som delas av
        alla enheter i ett trådlöst nätverk. SSID måste vara identiskt för
        alla enheter i det trådlösa nätverket. Namnet är skiftlägeskänsligt
        och får inte överstiga 32 alfanumeriska tecken (alla
        tangentbordstecken kan användas). Se till att inställningen är
        identisk för alla enheter i det trådlösa nätverket. Av säkerhetsskäl
        rekommenderas du att ändra det förinställda SSID-värdet ( <b>linksys</b>)
        till ett unikt, valfritt värde.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>SSID Broadcast</b></font>
      <td><font face="Arial" size="3">När trådlösa klienter söker i det
        lokala nätet efter trådlösa nätverk att ansluta till identifierar de
        gatewayens SSID Broadcast. Om du vill sända gatewayens SSID behåller
        du standardinställningen <b>Aktivera</b>. Om du inte vill sända
        gatewayens SSID väljer du <b>Inaktivera</b>.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>Kanal</b></font>
      <td><font face="Arial" size="3">I listan väljer du lämplig kanal som
        motsvarar dina nätverksinställningar.Alla enheter i det trådlösa nätverket måste använda samma kanal om
        de ska fungera ordentligt.</font>
  </tbody>
</table>
<p><font size="3">&nbsp;</font>
<br>
<font face="Arial" size="3">Kontrollera alla värden och klicka på <b>Spara
inställningar</b> om du vill spara inställningarna. Klicka på <b>Avbryt </b>
om du vill avbryta ändringar som inte sparats.</font>
<P
class=MsoNormal>
<CENTER><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" onclick=self.close() type=button value=Stäng name=B3></CENTER>
<P></P></FORM></BODY></HTML>
