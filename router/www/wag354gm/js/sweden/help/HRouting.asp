<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<!-- saved from url=(0032)http://192.168.1.1/HelpRoute.htm -->
<!-- saved from url=(0032)http://192.168.1.1/HelpRoute.htm --><HTML><HEAD><TITLE>Routing</TITLE>
<META http-equiv=Content-Type content="text/html; charset=windows-1252">
<META content="Microsoft FrontPage 4.0" name=GENERATOR>
<style>
<!--
LI.MsoNormal {
	FONT-SIZE: 12pt; FONT-FAMILY: "Times New Roman"; mso-style-parent: ""; margin-left:0cm; margin-right:0cm; margin-top:0cm; margin-bottom:0pt
}
-->
</style>
</HEAD>
<BODY onload=window.focus()>
<table border="0" width="100%">
  <tr>
    <td width="75%"><font face="Arial" color="blue" size="5"><b>Avancerad routing</b></font></td>
    <td width="25%">
      <p align="right"><font color="#3333ff"><b><span style="FONT-SIZE: 16pt; FONT-FAMILY: Arial"><a href="javascript:print();"><img src="../image/Printer.gif" align="center" border="0" width="82" height="44"></a></span></b></font></td>
  </tr>
</table>
<p><font face="Arial" size="2"><b>NAT</b></font>
<p><font face="Arial" size="2">NAT står för Network Address Translation. </font><font face="Arial" color="red" size="2">Den
här funktionen</font><font size="3"> </font><font face="Arial" size="2">gör
att flera datorer kan dela en Internet-anslutning. Du kan stänga av NAT-funktionen
genom att välja alternativet <b>Inaktivera</b>. Som standard är NAT-funktionen
inställd på <b>Aktivera </b>.</font>
<p><font face="Arial" size="2"><b>Dynamisk routing</b></font><br>
<font face="Arial" size="2">Funktionen Dynamisk routing kan användas för
automatisk justering enligt fysiska ändringar i nätverkets layout. Gatewayen
använder det dynamiska RIP-protokollet. Det fastställer nätverkspaketens väg
utifrån lägsta antalet hopp mellan källan och målet. RIP-protokollet sänder
regelbundet ut routinginformation till andra routrar i nätverket.</font>
<p><font face="Arial" size="2"><b>Konfigurera Dynamisk routing:</b></font><br>
<font face="sans-serif" size="2">1. &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;</font><font face="Arial" size="2">Välj
<b>Aktivera </b>för RIP-läget.</font><br>
<font face="sans-serif" size="2">2. &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;</font><font face="Arial" size="2">Vid
”Överför RIP-version” väljer du det protokoll som ska användas att överföra
data på det nätverksprotokoll som används till att överföra data på nätverket.</font><br>
<font face="sans-serif" size="2">3. &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;</font><font face="Arial" size="2">Vid
”Ta emot RIP-version” väljer du vilket protokoll du vill använda för att
ta emot data från nätverket.</font><br>
<font face="sans-serif" size="2">4. &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;</font><font face="Arial" size="2">Spara
ändringarna genom att klicka på <b>Spara inställningar</b>.</font><br>
<font face="Arial" size="2"><b>Statisk routing</b></font><br>
<font face="Arial" size="2">När du installerar flera routrar på nätverket måste
du konfigurera statisk routing. Med funktionen statisk routing fastställs överföringsvägen
för data på nätverket innan och efter de passerar gatewayen. Du kan använda
statisk routing om du vill tillåta olika IP-domänanvändare att använda
Internet via gatewayen. <b>Det här är en avancerad funktion. Var försiktig när
du använder den </b>.</font><br>
<font face="Arial" size="2">Om du vill konfigurera statisk routing ska du lägga
till routingposter i gatewayens tabell som anger vart enheten ska skicka alla
inkommande paket. Alla </font><font face="Arial" color="red" size="2">routrar</font><font size="3">
</font><font face="Arial" size="2">ska dirigera standard-routingposten till den
här gatewayen.</font><br>
<font face="Arial" size="2"><b>Skapa en statisk routingpost:</b></font><br>
<font face="sans-serif" size="2">1. &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;</font><font face="Arial" size="2">Välj
en post i </font><font face="Arial" color="red" size="2">listan </font><font face="Arial" size="2">.
I gatewayen hanteras upp till 20 statiska routingposter.</font><br>
<font face="sans-serif" size="2">2. &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;</font><font face="Arial" size="2">Ange
följande </font><font face="Arial" color="red" size="2">information</font><font size="3">
</font><font face="Arial" size="2">för den statiska routingen.</font>
<table width="100%" border="4">
  <tbody>
    <tr vAlign="top">
      <td width="13%"><font face="Arial" size="3"><b>Mål-IP-adress</b></font>
      <td width="70%"><font face="Arial" size="3">Ange nätverksadressen för
        det fjärrstyrda lokala LAN-segmentet. För en vanlig klass C IP-domän
        utgörs nätverksadressen av de tre första fälten i LAN-IP-adressen för
        målet. Det sista fältet ska vara noll.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>Nätmask</b></font>
      <td><font face="Arial" size="3">Ange vilken nätmask som ska användas på
        mål-IP-domänen. För en vanlig klass C IP-domän är nätmasken
        255.255.255.0.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>Gateway</b></font>
      <td><font face="Arial" size="3">Om den här gatewayen används för att
        ansluta nätverket till Internet är gateway-IP-adressen samma som
        gatewayens IP-adress. Om du har en annan router som hanterar nätverkets
        Internet-anslutning anger du den routerns <b>IP-adress</b> i stället.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>Antal hopp</b></font>
      <td><font face="Arial" size="3">Ange antal hopp. Detta är antalet hopp
        till varje nod fram till dess att målet nås.</font>
    </tr>
  </tbody>
</table>
<br>
<font face="sans-serif" size="2">3. &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;</font><font face="Times New Roman" size="3">Spara
ändringarna genom att klicka på <b>Sparainställningar</b> . Klicka på <b>Avbryt </b> om du vill avbryta ändringar
som inte sparats.</font>
<p><font face="Arial" size="2">Du kan visa aktuell routingtabell genom att
klicka på <b>Visa routingtabell </b>.</font><br>
<font face="Arial" size="2"><b>Ta bort en statisk routingpost:</b></font><br>
<font face="sans-serif" size="2">1. &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;</font><font face="Arial" size="2">Välj
en post i </font><font face="Arial" color="red" size="2">listan</font><font size="3">
</font><font face="Arial" size="2">.</font><br>
<font face="sans-serif" size="2">2. &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;</font><font face="Arial" size="2">Klicka
på <b>Ta bort den här posten</b>.</font><br>
<font face="sans-serif" size="2">3. &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;</font><font face="Arial" size="2">Spara
ändringarna genom att klicka på <b>Spara inställningar</b>.</font></p>
<FONT size=2>
<HR>
<CENTER><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" onclick=self.close() type=button value=Stäng name=B3></CENTER>
</FONT></BODY></HTML>
