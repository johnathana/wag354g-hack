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
<p><font face="Arial" size="2">NAT st�r f�r Network Address Translation. </font><font face="Arial" color="red" size="2">Den
h�r funktionen</font><font size="3"> </font><font face="Arial" size="2">g�r
att flera datorer kan dela en Internet-anslutning. Du kan st�nga av NAT-funktionen
genom att v�lja alternativet <b>Inaktivera</b>. Som standard �r NAT-funktionen
inst�lld p� <b>Aktivera </b>.</font>
<p><font face="Arial" size="2"><b>Dynamisk routing</b></font><br>
<font face="Arial" size="2">Funktionen Dynamisk routing kan anv�ndas f�r
automatisk justering enligt fysiska �ndringar i n�tverkets layout. Gatewayen
anv�nder det dynamiska RIP-protokollet. Det fastst�ller n�tverkspaketens v�g
utifr�n l�gsta antalet hopp mellan k�llan och m�let. RIP-protokollet s�nder
regelbundet ut routinginformation till andra routrar i n�tverket.</font>
<p><font face="Arial" size="2"><b>Konfigurera Dynamisk routing:</b></font><br>
<font face="sans-serif" size="2">1. &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;</font><font face="Arial" size="2">V�lj
<b>Aktivera </b>f�r RIP-l�get.</font><br>
<font face="sans-serif" size="2">2. &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;</font><font face="Arial" size="2">Vid
��verf�r RIP-version� v�ljer du det protokoll som ska anv�ndas att �verf�ra
data p� det n�tverksprotokoll som anv�nds till att �verf�ra data p� n�tverket.</font><br>
<font face="sans-serif" size="2">3. &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;</font><font face="Arial" size="2">Vid
�Ta emot RIP-version� v�ljer du vilket protokoll du vill anv�nda f�r att
ta emot data fr�n n�tverket.</font><br>
<font face="sans-serif" size="2">4. &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;</font><font face="Arial" size="2">Spara
�ndringarna genom att klicka p� <b>Spara inst�llningar</b>.</font><br>
<font face="Arial" size="2"><b>Statisk routing</b></font><br>
<font face="Arial" size="2">N�r du installerar flera routrar p� n�tverket m�ste
du konfigurera statisk routing. Med funktionen statisk routing fastst�lls �verf�ringsv�gen
f�r data p� n�tverket innan och efter de passerar gatewayen. Du kan anv�nda
statisk routing om du vill till�ta olika IP-dom�nanv�ndare att anv�nda
Internet via gatewayen. <b>Det h�r �r en avancerad funktion. Var f�rsiktig n�r
du anv�nder den </b>.</font><br>
<font face="Arial" size="2">Om du vill konfigurera statisk routing ska du l�gga
till routingposter i gatewayens tabell som anger vart enheten ska skicka alla
inkommande paket. Alla </font><font face="Arial" color="red" size="2">routrar</font><font size="3">
</font><font face="Arial" size="2">ska dirigera standard-routingposten till den
h�r gatewayen.</font><br>
<font face="Arial" size="2"><b>Skapa en statisk routingpost:</b></font><br>
<font face="sans-serif" size="2">1. &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;</font><font face="Arial" size="2">V�lj
en post i </font><font face="Arial" color="red" size="2">listan </font><font face="Arial" size="2">.
I gatewayen hanteras upp till 20 statiska routingposter.</font><br>
<font face="sans-serif" size="2">2. &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;</font><font face="Arial" size="2">Ange
f�ljande </font><font face="Arial" color="red" size="2">information</font><font size="3">
</font><font face="Arial" size="2">f�r den statiska routingen.</font>
<table width="100%" border="4">
  <tbody>
    <tr vAlign="top">
      <td width="13%"><font face="Arial" size="3"><b>M�l-IP-adress</b></font>
      <td width="70%"><font face="Arial" size="3">Ange n�tverksadressen f�r
        det fj�rrstyrda lokala LAN-segmentet. F�r en vanlig klass C IP-dom�n
        utg�rs n�tverksadressen av de tre f�rsta f�lten i LAN-IP-adressen f�r
        m�let. Det sista f�ltet ska vara noll.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>N�tmask</b></font>
      <td><font face="Arial" size="3">Ange vilken n�tmask som ska anv�ndas p�
        m�l-IP-dom�nen. F�r en vanlig klass C IP-dom�n �r n�tmasken
        255.255.255.0.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>Gateway</b></font>
      <td><font face="Arial" size="3">Om den h�r gatewayen anv�nds f�r att
        ansluta n�tverket till Internet �r gateway-IP-adressen samma som
        gatewayens IP-adress. Om du har en annan router som hanterar n�tverkets
        Internet-anslutning anger du den routerns <b>IP-adress</b> i st�llet.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>Antal hopp</b></font>
      <td><font face="Arial" size="3">Ange antal hopp. Detta �r antalet hopp
        till varje nod fram till dess att m�let n�s.</font>
    </tr>
  </tbody>
</table>
<br>
<font face="sans-serif" size="2">3. &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;</font><font face="Times New Roman" size="3">Spara
�ndringarna genom att klicka p� <b>Sparainst�llningar</b> . Klicka p� <b>Avbryt </b> om du vill avbryta �ndringar
som inte sparats.</font>
<p><font face="Arial" size="2">Du kan visa aktuell routingtabell genom att
klicka p� <b>Visa routingtabell </b>.</font><br>
<font face="Arial" size="2"><b>Ta bort en statisk routingpost:</b></font><br>
<font face="sans-serif" size="2">1. &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;</font><font face="Arial" size="2">V�lj
en post i </font><font face="Arial" color="red" size="2">listan</font><font size="3">
</font><font face="Arial" size="2">.</font><br>
<font face="sans-serif" size="2">2. &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;</font><font face="Arial" size="2">Klicka
p� <b>Ta bort den h�r posten</b>.</font><br>
<font face="sans-serif" size="2">3. &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; &nbsp;</font><font face="Arial" size="2">Spara
�ndringarna genom att klicka p� <b>Spara inst�llningar</b>.</font></p>
<FONT size=2>
<HR>
<CENTER><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" onclick=self.close() type=button value=St�ng name=B3></CENTER>
</FONT></BODY></HTML>
