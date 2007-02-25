<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<!-- saved from url=(0032)http://192.168.1.1/HelpRoute.htm -->
<!-- saved from url=(0032)http://192.168.1.1/HelpRoute.htm -->
<HTML><HEAD><TITLE>Erweitertes Routing - Hilfe</TITLE>
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
    <td width="75%"><FONT face=Arial style="font-size: 16pt" color="#3333FF"><b>Erweitertes Routing  </b></FONT>
    </td>
    <td width="25%">
      <p align="right"><font color="#3333ff"><b><span style="FONT-SIZE: 16pt; FONT-FAMILY: Arial"><a href="javascript:print();">
<img src="../image/Printer.gif" align="center" border="0" width="82" height="44"></a></span></b></font></td>
  </tr>
</table>
<P><STRONG>
<FONT face=Arial size=3>
NAT</FONT></STRONG></P>
<P>
<FONT face=Arial size=3>
Mithilfe von NAT (<i>Network Address Translation</i>) k�nnen mehrere Computer eine Internetverbindung gemeinsam nutzen. Falls Sie NAT nicht verwenden m�chten, w�hlen Sie die Option&nbsp;<STRONG>Deaktivieren </STRONG> aus. Standardm��ig ist NAT auf <STRONG>Aktivieren</STRONG> 
 eingestellt.</FONT></P>
<P><STRONG>
<FONT face=Arial size=3>
Dynamisches Routing</FONT></STRONG></P>
<P>
<FONT face=Arial size=3>
Mit der Funktion <b>Dynamisches Routing</b> kann das Gateway automatisch an physische �nderungen in der Netzwerkanordnung angepasst werden. Das Gateway legt unter Verwendung des RIP-Protokolls die Route der Netzwerkpakete auf der Grundlage der geringsten Anzahl an Spr�ngen zwischen Quelle und Ziel fest. �ber das RIP-Protokoll werden in regelm��igen Abst�nden Routing-Informationen an andere Router im Netzwerk
gesendet.</FONT> </P><B>
<P>
<FONT face=Arial size=3>
So richten Sie das dynamische Routing ein:</FONT></P>
<OL></B>
  <LI>
<FONT face=Arial size=3>
W�hlen Sie f�r den RIP-Modus die Option <b>Aktiviert </b> aus.</FONT><LI>
<FONT face=Arial size=3>
W�hlen Sie aus dem Dropdown-Men� <b>RIP-Version �bertragen</b> das Protokoll aus, das Sie zum Versenden von Daten �ber das Netzwerk verwenden
m�chten.</FONT> 
  <LI>
<FONT face=Arial size=3>
W�hlen Sie aus dem Dropdown-Men� <b>RIP-Version empfangen</b> das Protokoll aus, das Sie zum Empfangen von Daten �ber das Netzwerk verwenden
m�chten.</FONT> 
  <LI>
<FONT face=Arial size=3>
Klicken Sie auf die Schaltfl�che <B>Einstellungen speichern</B>, um die �nderungen zu
speichern.</FONT> 
</LI></OL>
<P><STRONG>
<FONT face=Arial size=3>
Statisches Routing</FONT></STRONG></P>
<P>
<FONT face=Arial size=3>
Wenn in Ihrem Netzwerk mehrere Router installiert sind, m�ssen Sie das statische Routing konfigurieren. Mit der Funktion <b>Statisches Routing</b> bestimmten Sie die Route der Daten durch das Netzwerk, bevor und nachdem sie das Gateway passiert haben. 
Mithilfe des statischen Routings k�nnen Sie verschiedenen IP-Dom�nenbenutzern den Zugriff auf das Internet �ber das Gateway erlauben.<B> Es handelt sich hierbei um eine erweiterte Funktion. Bitte gehen Sie entsprechend vorsichtig vor</B>.</FONT> </P>
<P>
<FONT face=Arial size=3>
Um das statische Routing einzurichten, f�gen Sie Routing-Eintr�ge in die Tabelle des Gateways ein. Diese Eintr�ge geben dem Ger�t an, wohin alle eingehenden Pakete zu senden sind. Alle Ihre Netzwerk-Router sollten den Standardrouteneintrag an dieses Gateway
leiten.</FONT></P><B>
<P><FONT face=Arial size=3>So erstellen Sie einen Eintrag f�r eine statische Route:
 
  </FONT>
</P>
<OL></B>
  <LI><FONT face=Arial size=3>W�hlen Sie einen Eintrag aus der Dropdown-Liste aus. Das Gateway unterst�tzt bis zu 20 Eintr�ge f�r statische
    Routen.
 
  </FONT>
  <LI><FONT face=Arial size=3>Geben Sie folgende Daten f�r die statische Route
    ein.
  </FONT>
<table class=MsoNormalTable style="border-collapse: collapse; border-style: none; border-width: medium" cellSpacing=0 cellPadding=0 border="1" width="95%">
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top width="20%"><b><font face="Arial" size="3">Ziel-IP-Adresse</font></b></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top width="70%"><font face="Arial" size="3">Geben Sie die Netzwerkadresse des entfernten LAN-Segments an. Bei einer standardm��igen IP-Dom�ne der Klasse C setzt sich die Netzwerkadresse aus den ersten drei Feldern der Ziel-LAN-IP-Adresse zusammen. Das letzte Feld muss Null
      sein.</font></td>
  </tr>
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><b><font face="Arial" size="3">Subnetzmaske</font></b></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><font face="Arial" size="3">Geben Sie die Subnetzmaske ein, die f�r die Ziel-IP-Dom�ne verwendet wird. Bei einer standardm��igen IP-Dom�ne der Klasse C lautet die Subnetzmaske 255.255.255.0.</font></td>
  </tr>
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><b><font face="Arial" size="3">Gateway</font></b></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><font face="Arial" size="3">Falls Ihr Netzwerk �ber dieses Gateway eine Verbindung zum Internet herstellt, dann ist Ihre Gateway-IP die IP-Adresse des Gateways. Falls ein anderer Router f�r die Internetverbindung Ihres Netzwerks zust�ndig ist, geben Sie stattdessen die <b>IP-Adresse</b> dieses Routers
      ein.</font></td>
  </tr>
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><b><font face="Arial" size="3">Abschnitte</font></b></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><font face="Arial" size="3">Geben Sie die Abschnitte ein. 
 Hierbei handelt es sich um die Anzahl der Spr�nge zu den einzelnen Knoten, bis das Ziel erreicht
      wird.</font></td>
  </tr>
</table>
  <LI><font face="Arial" size="3">Klicken Sie auf die Schaltfl�che <STRONG>Einstellungen speichern</STRONG>, um die �nderungen zu speichern. 
 Klicken Sie auf <STRONG>�nderungen verwerfen</STRONG>, um Ihre �nderungen zu
    verwerfen.</font></LI></OL>
<P><FONT 
  face=Arial size=3>Um die aktuelle Routing-Tabelle einzusehen, klicken Sie auf die Schaltfl�che <STRONG>Routing-Tabelle anzeigen</STRONG>.</FONT></P>
<P><STRONG><FONT 
  face=Arial size=3>So l�schen Sie einen Eintrag f�r eine statische Route:</FONT></STRONG></P>
<OL>
  <LI><FONT 
  face=Arial size=3>W�hlen Sie einen Eintrag aus der Dropdown-Liste aus.</FONT> 
  <LI><FONT 
  face=Arial size=3>Klicken Sie auf die Schaltfl�che <B>Eintrag l�schen</B>.</FONT> 
  <LI><FONT 
  face=Arial size=3>Klicken Sie auf die Schaltfl�che <B>Einstellungen speichern</B>, um die �nderungen zu
    speichern.</FONT></LI></OL><FONT size=2>
<P>
<HR>
<CENTER><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" onclick=self.close() type=button value=Schlie&szlig;en name=B3></CENTER>
</FONT></BODY></HTML>
