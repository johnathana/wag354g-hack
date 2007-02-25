<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//de">
<!-- saved from url=(0036)http://192.168.1.1/HRangeForward.htm -->
<!-- saved from url=(0036)http://192.168.1.1/HRangeForward.htm -->
<HTML><HEAD><TITLE>Port-Bereich-Weiterleitung - Hilfe</TITLE>
<META http-equiv=Content-Type content="text/html; charset=windows-1252">
<META content="Microsoft FrontPage 4.0" name=GENERATOR></HEAD>
<BODY vLink=#800080 link=#0000ff onload=window.focus()>
<table border="0" width="100%">
  <tr>
    <td width="75%"> <FONT face=Arial style="font-size: 16pt" color="#3333FF"><b>Port-Bereich-Weiterleitung
</b></FONT></td>
    <td width="25%">
      <p align="right"><font color="#3333ff"><b> <span style="FONT-SIZE: 16pt; FONT-FAMILY: Arial"><a href="javascript:print();"> <img src="../image/Printer.gif" align="center" border="0" width="82" height="44"></a></span></b></font></td>
  </tr>
</table>
 <FONT face=Arial size=2>
<P>Die Port-Bereich-Weiterleitung kann verwendet werden, um &ouml;ffentliche Dienste im Netzwerk einzurichten. Wenn Benutzer Ihr Netzwerk &uuml;ber das Internet abfragen, kann das Gateway diese Abfragen an Computer weiterleiten, die zur Verarbeitung dieser Abfragen ausger&uuml;stet sind. Wenn Sie die Port-Nummer 80 (HTTP) beispielsweise so einstellen, dass alle Abfragen an die IP-Adresse 192.168.1.2 weitergeleitet werden, erfolgt eine Weiterleitung aller HTTP-Abfragen von externen Benutzern an 192.168.1.2. <B>Es wird empfohlen, dass der Computer eine statische IP-Adresse verwendet.</B></P>
<P>Sie k&ouml;nnen diese Funktion verwenden, um einen Webserver oder FTP-Server &uuml;ber ein IP-Gateway einzurichten. Geben Sie unbedingt eine g&uuml;ltige IP-Adresse ein. (Sie m&uuml;ssen unter Umst&auml;nden eine statische IP-Adresse bei Ihrem ISP einrichten, um einen Internetserver ordnungsgem&auml;&szlig; ausf&uuml;hren zu k&ouml;nnen.) F&uuml;r zus&auml;tzliche Sicherheit k&ouml;nnen die Internetbenutzer zwar mit dem Server kommunizieren, es wird jedoch keine tats&auml;chliche Verbindung hergestellt. Die Pakete werden einfach &uuml;ber das Gateway weitergeleitet.</P>
<P>So f&uuml;gen Sie einen Server zur Weiterleitung hinzu:</P>
<OL>
  <LI>Geben Sie einen Namen der <STRONG>Anwendung</STRONG> f&uuml;r den Dienst ein, der weitergeleitet werden soll.
  <LI>Geben Sie f&uuml;r den weiterzuleitenden Dienst den <STRONG>Start</STRONG> und das <STRONG>Ende</STRONG> des Anschlussbereichs
    ein.
  <LI>W&auml;hlen Sie das von den Diensten verwendete <STRONG>Protokoll</STRONG> aus.
  <LI>Geben Sie die <STRONG>IP-Adresse</STRONG> des Servers ein, auf den die Internetbenutzer zugreifen sollen.
  <LI>W&auml;hlen Sie f&uuml;r diesen Eintrag <STRONG>Aktiviert</STRONG>&nbsp;aus.
  <LI>Klicken Sie auf die Schaltfl&auml;che <STRONG>Einstellungen speichern</STRONG>, um die Einstellungen zu speichern.</LI></OL><BR>
<P>So l&ouml;schen Sie Diensteintr&auml;ge:</P>
<OL>
  <LI>Geben Sie eine 0 (Null) in die Felder f&uuml;r den Port-Nummernbereich und die IP-Adresse ein.
  <LI>Deaktivieren Sie die Kontrollk&auml;stchen <STRONG>TCP</STRONG> und/oder <STRONG>UDP</STRONG> sowie <STRONG>Aktivieren</STRONG>.
  <LI>L&ouml;schen Sie alle Angaben aus dem Feld <STRONG>Anwendung</STRONG>.
  <LI>Klicken Sie auf die Schaltfl&auml;che <STRONG>Einstellungen speichern</STRONG>, um die &Auml;nderungen zu
  speichern.</LI></OL>
<P><CENTER><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" onclick=self.close() type=button value=Schlie&szlig;en name=B3></CENTER></P></FONT></BODY></HTML>
