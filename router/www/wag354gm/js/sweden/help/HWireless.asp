<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<!-- saved from url=(0037)http://192.168.1.1/help/HWireless.asp -->
<!-- saved from url=(0022)http://internet.e-mail --><HTML><HEAD><TITLE>Hj�lp: Tr�dl�st</TITLE>
<META http-equiv=Content-Type content="text/html; charset=windows-1252">
<STYLE>P.MsoNormal { FONT-SIZE: 12pt; FONT-FAMILY: "Times New Roman"; mso-style-parent: ""; margin-left:0cm; margin-right:0cm; margin-top:0cm; margin-bottom:0pt } TABLE.MsoNormalTable { FONT-SIZE: 10pt; FONT-FAMILY: "Times New Roman"; mso-style-parent: "" }
</STYLE>

<META content="Microsoft FrontPage 4.0" name=GENERATOR></HEAD>
<BODY bgColor=white>
<FORM>
<table border="0" width="100%">
  <tr>
    <td width="75%"><font face="Arial" color="blue" size="5"><b>Tr�dl�st </b></font></td>
    <td width="25%">
      <p align="right"><FONT color=#3333ff><B><SPAN
style="FONT-SIZE: 16pt; FONT-FAMILY: Arial"><A href="javascript:print();"><IMG height=44
src="../image/Printer.gif" width=82 align=center
border=0></A></SPAN></B></FONT></p>
    </td>
  </tr>
</table>
<P class=MsoNormal><br>
<font face="Arial" size="3">P� sk�rmen <i>Tr�dl�st</i> kan du anpassa inst�llningarna
f�r data�verf�ring. I de flesta fall b�r du beh�lla standardinst�llningarna
f�r de avancerade inst�llningarna p� den h�r sk�rmen.</font><br>
<font size="3">&nbsp;</font></P>
<table width="100%" border="4">
  <tbody>
    <tr vAlign="top">
      <td width="29%"><font face="Arial" size="3"><b>Autentiseringstyp</b></font>
      <td width="70%"><font face="Arial" size="3">Standardv�rdet �r <b>Auto</b>
        , vilket inneb�r att autentisering av typen �ppet system eller Delad
        nyckel anv�nds. Vid autentisering med �ppet system delar INTE avs�ndare
        och mottagare WEP-nyckel f�r autentisering. Vid autentisering med Delad
        nyckel delar avs�ndaren och mottagaren en WEP-nyckel f�r autentisering.
        Om du bara vill anv�nda autentisering med delad nyckel v�ljer du <b>Delad
        nyckel</b>.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>Kontrollera �verf�ringshastighet</b></font>
      <td><font face="Arial" size="3">Standardinst�llningen �r <b>Auto</b>.
        Intervallet �r mellan 1 och 54 Mbit/s.<br>
        Hastigheten f�r data�verf�ring b�r anges utifr�n hastigheten i det
        tr�dl�sa n�tverket. Du kan v�lja mellan olika �verf�ringshastigheter
        eller beh�lla standardinst�llningen <b>Auto</b> om du vill att
        gatewayen automatiskt ska anv�nda den snabbast tillg�ngliga
        datahastigheten och aktivera funktionen Automatisk tillbakag�ng. Med
        funktionen Automatisk tillbakag�ng anv�nds den b�sta m�jliga
        anslutningshastighet mellan gatewayen och en tr�dl�s klient.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>Signalintervall</b></font>
      <td><font face="Arial" size="3">Standardv�rdet �r <b>100</b> . Ange ett
        v�rde mellan 1 och 65 535 millisekunder. V�rdet f�r signalintervall
        anger signalens frekvensintervall. En signal �r ett paket som s�nds av
        gatewayen f�r att synkronisera det tr�dl�sa n�tverket.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>DTIM-intervall</b></font>
      <td><font face="Arial" size="3">Standardv�rdet �r <b>1</b> . Det h�r v�rdet,
        som ligger p� mellan 1 och 255 millisekunder, anger intervallet f�r
        DTIM-meddelandet (Delivery Traffic Indication Message). Ett DTIM-f�lt
        �r ett nedr�kningsf�lt som meddelar klienter om n�sta f�nster f�r
        lyssning av broadcast- och multicast-meddelanden. N�r gatewayen har
        buffrat broadcast- eller multicast-meddelanden f�r associerade klienter
        s�nder den n�sta DTIM med ett DTIM-intervallv�rde. Klienterna
        identifierar signalpaketen och aktiveras f�r att ta emot broadcast- och
        multicast-meddelandena.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>Fragmenteringsgr�nsv�rde</b></font>
      <td><font face="Arial" size="3">Det h�r v�rdet ska l�mnas p�
        standardinst�llningen <b>2346</b> . Intervallet �r 256-2346 byte. Det
        anger den maximala storleken f�r ett paket innan data fragmenteras till
        flera paket. Om du f�r en h�g paketfelfrekvens kan du prova med att �ka
        v�rdet n�got. Om du anger ett f�r l�gt v�rde kan n�tverksprestandan
        f�rs�mras. Det rekommenderas att du bara g�r mindre �ndringar av
        detta v�rde.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>RTS-gr�nsv�rde</b></font>
      <td><font face="Arial" size="3">Det h�r v�rdet ska l�mnas p�
        standardinst�llningen <b>2347</b> . Intervallet �r 0-2347 byte.<br>
        Om du f�r problem med inkonsekventa datafl�den rekommenderas endast
        mindre �ndringar. Om ett n�tverkspaket �r mindre �n den f�rinst�llda
        RTS-gr�nsv�rdesstorleken aktiveras inte RTS/CTS-mekanismen. Gatewayen
        skickar RTS-ramar (Request to Send) till en viss mottagarstation och f�rhandlar
        sedan om s�ndningen av en dataram. Efter att ha tagit emot en RTS
        svarar den tr�dl�sa stationen med en CTS-ram (Clear to Send) som bekr�ftar
        r�tten att p�b�rja �verf�ringen.</font>
    </tr>
  </tbody>
</table>
&nbsp;
<P class=MsoNormal>&nbsp;</P>
<table width="100%" border="4">
  <tbody>
    <tr vAlign="top">
      <td width="29%"><font face="Arial" size="3"><b>Tr�dl�s MAC-filtrering</b></font>
      <td width="70%"><font face="Arial" size="3">Med funktionen Tr�dl�s MAC-filtrering
        kan du styra vilka tr�dl�st anslutna datorer som f�r kommunicera med
        gatewayen utifr�n deras MAC-adress. Om du vill avaktivera funktionen Tr�dl�s
        MAC-filtrering beh�ller du standardinst�llningen <b>Till�t alla</b>.
        Om du vill konfigurera ett filter klickar du p� <b>Begr�nsa �tkomst</b>
        och g�r s� h�r:</font><font size="3"><br>
        </font><font face="Arial" size="3"><br>
        1.</font><font face="Times New Roman" size="1"> </font><font face="Arial" size="3">Om
        du vill blockera vissa tr�dl�st anslutna datorer fr�n att kommunicera
        med gatewayen beh�ller du standardinst�llningen <b>Hindra datorer som
        anges nedan att f� �tkomst till det tr�dl�sa n�tverket </b>. Om du
        vill till�ta vissa tr�dl�st anslutna datorer att kommunicera med
        gatewayen klickar du p� alternativknappen bredvid </font><font face="Arial" color="red" size="3"><b>Till�t
        endast datorer som anges nedan att f� �tkomst till det tr�dl�sa n�tverket
        </b></font><font face="Arial" size="3">.</font><font size="3"><br>
        </font><font face="Arial" size="3"><br>
        2.</font><font face="Times New Roman" size="1"> </font><font face="Arial" size="3">Klicka
        p� <b>Redigera �tkomstlistan f�r MAC-adresser</b>. Ange MAC-adresser
        i <i>MAC</i> -f�lten.<br>
        Obs! I varje MAC-f�lt ska MAC-adressen anges i f�ljande format:
        xxxxxxxxxxxx (varje x motsvarar ett tecken i MAC-adressen).</font><font size="3"><br>
        </font><font face="Arial" size="3"><br>
        3.</font><font face="Times New Roman" size="1"> </font><font face="Arial" size="3">Spara
        dina val genom att klicka p� Spara inst�llningar. Klicka p� <b>Avbryt
        �ndringar</b> om du vill avbryta �ndringar som inte sparats. Klicka p�
        <b>St�ng</b> om du vill �terg� till sk�rmen <i>Avancerat tr�dl�st</i>
        utan att spara �ndringarna.</font>
  </tbody>
</table>
<p><font face="Arial" size="3"><b>Tr�dl�st</b></font></p>
<table width="100%" border="4">
  <tbody>
    <tr vAlign="top">
      <td width="29%"><font face="Arial" size="3"><b>L�ge</b></font>
      <td width="70%"><font face="Arial" size="3">Beh�ll standardinst�llningen
        <b>Blandat l�ge</b> om det ing�r </font><font face="Arial" color="red" size="3">Wireless-B</font><font size="3">
        </font><font face="Arial" size="3">enheter i n�tverket. V�lj <b>Endast
        G-l�ge</b> om alla enheter �r av typen Wireless-G. V�lj <b>Avaktivera</b>
        om du vill avaktivera det tr�dl�sa n�tverket. Om du vill begr�nsa n�tverket
        till enbart 802.11b-enheter v�ljer du <b>Endast B-l�ge</b>.</font><font face="Times New Roman" size="3">
        </font><font size="3">&nbsp;</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>SSID</b></font>
      <td><font face="Arial" size="3">SSID �r det n�tverksnamn som delas av
        alla enheter i ett tr�dl�st n�tverk. SSID m�ste vara identiskt f�r
        alla enheter i det tr�dl�sa n�tverket. Namnet �r skiftl�gesk�nsligt
        och f�r inte �verstiga 32 alfanumeriska tecken (alla
        tangentbordstecken kan anv�ndas). Se till att inst�llningen �r
        identisk f�r alla enheter i det tr�dl�sa n�tverket. Av s�kerhetssk�l
        rekommenderas du att �ndra det f�rinst�llda SSID-v�rdet ( <b>linksys</b>)
        till ett unikt, valfritt v�rde.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>SSID Broadcast</b></font>
      <td><font face="Arial" size="3">N�r tr�dl�sa klienter s�ker i det
        lokala n�tet efter tr�dl�sa n�tverk att ansluta till identifierar de
        gatewayens SSID Broadcast. Om du vill s�nda gatewayens SSID beh�ller
        du standardinst�llningen <b>Aktivera</b>. Om du inte vill s�nda
        gatewayens SSID v�ljer du <b>Inaktivera</b>.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>Kanal</b></font>
      <td><font face="Arial" size="3">I listan v�ljer du l�mplig kanal som
        motsvarar dina n�tverksinst�llningar.Alla enheter i det tr�dl�sa n�tverket m�ste anv�nda samma kanal om
        de ska fungera ordentligt.</font>
  </tbody>
</table>
<p><font size="3">&nbsp;</font>
<br>
<font face="Arial" size="3">Kontrollera alla v�rden och klicka p� <b>Spara
inst�llningar</b> om du vill spara inst�llningarna. Klicka p� <b>Avbryt </b>
om du vill avbryta �ndringar som inte sparats.</font>
<P
class=MsoNormal>
<CENTER><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" onclick=self.close() type=button value=St�ng name=B3></CENTER>
<P></P></FORM></BODY></HTML>
