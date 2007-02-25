<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<!-- saved from url=(0039)http://192.168.1.1/help/HManagement.asp -->
<HTML><HEAD><TITLE>Verwaltungsfunktionen - Hilfe</TITLE>
<META http-equiv=Content-Type content="text/html; charset=windows-1252">
<STYLE>P.MsoNormal {
	FONT-SIZE: 12pt; FONT-FAMILY: "Times New Roman"; mso-style-parent: ""; margin-left:0cm; margin-right:0cm; margin-top:0cm; margin-bottom:0pt
}
TABLE.MsoNormalTable {
	FONT-SIZE: 10pt; FONT-FAMILY: "Times New Roman"; mso-style-parent: ""
}
LI.MsoNormal {
	FONT-SIZE: 12pt; FONT-FAMILY: "Times New Roman"; mso-style-parent: ""; margin-left:0cm; margin-right:0cm; margin-top:0cm; margin-bottom:0pt
}
</STYLE>

<META content="Microsoft FrontPage 4.0" name=GENERATOR></HEAD>
<BODY bgColor=white>
<FORM>
<table border="0" width="100%">
  <tr>
    <td width="75%"><FONT color=#3333ff><B><SPAN lang=EN-US
style="FONT-SIZE: 16pt; FONT-FAMILY: Arial">Verwaltungsfunktionen</SPAN></B></FONT></td>
    <td width="25%">
      <p align="right"><FONT color=#3333ff><B><SPAN
style="FONT-SIZE: 16pt; FONT-FAMILY: Arial"><A href="javascript:print();"><IMG height=44
src="../image/Printer.gif" width=82 align=center
border=0></A></SPAN></B></FONT></td>
  </tr>
</table>
<P class=MsoNormal>&#12288;</P> 
<P class=MsoNormal><span lang="EN-US" style="FONT-FAMILY: Arial"><font size="3">Im
Fenster <i>Verwaltungsfunktionen</i> können Sie die Sicherheitseinstellungen
des Gateways ändern. Es wird dringend empfohlen, dass Sie das werkseitig
eingestellte Gateway-Passwort <b>admin</b> ändern. Alle Benutzer, die auf das
webbasierte Dienstprogramm des Gateways oder den Setup-Assistenten zugreifen,
werden zur Eingabe des Gateway-Passworts aufgefordert.</font></span></P> 
<P class=MsoNormal><font size="3">&#12288;</font></P>
<table class=MsoNormalTable style="border-collapse: collapse; border-style: none; border-width: medium" cellSpacing=0 cellPadding=0 border="1" width="100%">
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top width="21%"><b><span style="font-family: Arial"><font size="3">Gateway-Benutzername</font></span></b></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top width="70%"><span lang="EN-US" style="FONT-FAMILY: Arial"><font size="3">Der
      neue Benutzername darf 63 Zeichen nicht überschreiten.</font></span></td>
  </tr>
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><b><font size="3"><span style="font-family: Arial">Gateway</span><SPAN lang=EN-US style="FONT-FAMILY: Arial">-Passwort</SPAN></font></b></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><span lang="EN-US" style="FONT-FAMILY: Arial"><font size="3">Das
      neue Passwort darf 30 Zeichen nicht überschreiten und keine Leerzeichen
      enthalten. Geben Sie das neue Passwort zur Bestätigung ein zweites Mal
      ein.</font></span></td>
  </tr>
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><B><SPAN lang=EN-US style="FONT-FAMILY: Arial"><font size="3">Remote-Verwaltung</font></SPAN></B></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top>
      <p class="MsoNormal"><span lang="EN-US" style="FONT-FAMILY: Arial"><font size="3">Mit
      dieser Funktion können Sie das Gateway von einem entfernten Standort über
      das Internet verwalten. Um diese Funktion zu deaktivieren, behalten Sie
      die Standardeinstellung <b>Deaktivieren</b> bei. Zur Aktivierung dieser
      Funktion wählen Sie<b> Aktivieren</b> aus, und verwenden Sie den
      festgelegten Port (Standardeinstellung ist <b><% remote_management_config("http_wanport", 0); %></b>) an Ihrem PC, um
      Ihr Gateway von einem entfernten Standort aus zu verwalten. Sie müssen außerdem
      das Standardpasswort des Gateways durch ein eigenes ersetzen, wenn dies
      noch nicht geschehen ist. Ein individuelles Passwort erhöht die
      Sicherheit.</font></span></p>
      <p class="MsoNormal"><font size="3">&nbsp;</font></p>
      <p class="MsoNormal"><span lang="EN-US" style="FONT-FAMILY: Arial"><font size="3">Um
      Ihr Gateway von einem entfernten Standort aus zu verwalten, geben Sie in
      das Feld <i>Adresse</i> Ihres Web-Browsers Folgendes ein: <b>http://xxx.xxx.xxx.xxx:<% remote_management_config("http_wanport", 0); %></b>
      (die x-Zeichenfolge entspricht der Internet-IP-Adresse Ihres Gateways und
      <% remote_management_config("http_wanport", 0); %> dem festgelegten Port). Sie werden nach dem Gateway-Passwort gefragt.
      Nach erfolgreicher Eingabe des Passworts können Sie auf das webbasierte
      Dienstprogramm des Gateways zugreifen.</font></span></p>
      <p class="MsoNormal"><font size="3">&nbsp;</font></p>
      <p class="MsoNormal"><span lang="EN-US" style="FONT-FAMILY: Arial"><font size="3">Hinweis:
      Wenn die Funktion <b>Remote-Verwaltung</b> aktiviert ist, kann jeder, der
      die Internet-IP-Adresse und das Passwort des Gateways kennt, die Gateway-Einstellungen
      ändern.</font></span></td>
  </tr>
</table>
<P class=MsoNormal><font size="3">&#12288;</font></P>
<P class=MsoNormal><b><font face="Arial" size="3">SNMP</font></b></P>
<P class=MsoNormal><font size="3">&#12288;</font></P>
<P class=MsoNormal><font face="Arial" size="3">SNMP ist ein häufig verwendetes
Protokoll zur Netzwerküberwachung und -verwaltung.&nbsp;&nbsp; Zur Aktivierung<b>
</b>von SNMP klicken Sie auf <b>Aktiviert</b>. Um SNMP zu deaktivieren, klicken
Sie auf <b>Deaktiviert</b>.</font></P> 
<P class=MsoNormal><font size="3">&#12288;</font></P>
<table class=MsoNormalTable style="border-collapse: collapse; border-style: none; border-width: medium" cellSpacing=0 cellPadding=0 border="1" width="100%">
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top width="21%"><b><span style="FONT-FAMILY: Arial"><font size="3">Gerätename</font></span></b></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top width="70%"><span lang="EN-US" style="FONT-FAMILY: Arial"><font size="3">Sie
      können zur Vereinfachung der Netzwerkverwaltung einen neuen Gerätenamen
      eingeben. Der neue Gerätename darf 39 Zeichen nicht überschreiten.</font></span></td>
  </tr>
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><b><span style="font-family: Arial"><font size="3">Get Community</font></span></b></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><span style="FONT-FAMILY: Arial"><font size="3">Geben Sie das
      Passwort ein, mit dem ein Lesezugriff auf die SNMP-Informationen des
      Gateways gewährt wird.</font></span></td>
  </tr>
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><b><span style="font-family: Arial"><font size="3">Set Community</font></span></b></td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top><span style="FONT-FAMILY: Arial"><font size="3">Geben Sie das
      Passwort ein, mit dem ein Schreib-/Lesezugriff auf die SNMP-Informationen
      des Gateways gewährt wird.</font></span></td>
  </tr>
</table>
<P class=MsoNormal><font size="3">&#12288;</font></P>
<P class=MsoNormal><b><font face="Arial" size="3">UPnP</font></b></P>
<P class=MsoNormal><font size="3">&#12288;</font></P>
<p class="MsoNormal"><font size="3" face="Arial">Mit UPnP kann unter Windows&nbsp;XP das
Gateway automatisch für verschiedene Internetanwendungen wie Internetspiele
oder Videokonferenzen konfiguriert werden. Zur Aktivierung von UPnP klicken Sie
auf <b>Aktiviert</b>. Um UPnP zu deaktivieren, klicken Sie auf <b>Deaktiviert</b>.</font></p>
<p class="MsoNormal"><font size="3" face="Arial">&nbsp;</font></p>
<p class="MsoNormal"><font size="3" face="Arial"><span lang="EN-US">Überprüfen
Sie alle Werte, und klicken Sie auf <b>Einstellungen speichern</b>, um Ihre
Einstellungen zu speichern. Klicken Sie auf <b>Änderungen verwerfen</b>, um
Ihre nicht gespeicherten Änderungen zu verwerfen.</font></span></p>
<P class=MsoNormal>&#12288;</P>
<P
class=MsoNormal>

<CENTER><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" onclick=self.close() type=button value=Schlie&szlig;en name=B3></CENTER>
<P></P></FORM></BODY></HTML>
