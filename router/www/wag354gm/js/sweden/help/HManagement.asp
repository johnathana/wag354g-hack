<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<!-- saved from url=(0039)http://192.168.1.1/help/HManagement.asp -->
<HTML><HEAD><TITLE>Hj�lp: S�kerhet</TITLE>
<META http-equiv=Content-Type content="text/html; charset=windows-1252">
<STYLE>P.MsoNormal { FONT-SIZE: 12pt; FONT-FAMILY: "Times New Roman"; mso-style-parent: ""; margin-left:0cm; margin-right:0cm; margin-top:0cm; margin-bottom:0pt } TABLE.MsoNormalTable { FONT-SIZE: 10pt; FONT-FAMILY: "Times New Roman"; mso-style-parent: "" } LI.MsoNormal { FONT-SIZE: 12pt; FONT-FAMILY: "Times New Roman"; mso-style-parent: ""; margin-left:0cm; margin-right:0cm; margin-top:0cm; margin-bottom:0pt }
</STYLE>

<META content="Microsoft FrontPage 4.0" name=GENERATOR></HEAD>
<BODY bgColor=white>
<FORM>
<table border="0" width="100%">
  <tr>
    <td width="75%"><font face="Arial" color="blue" size="5"><b>Hantering </b></font></td>
    <td width="25%">
      <p align="right"><FONT color=#3333ff><B><SPAN
style="FONT-SIZE: 16pt; FONT-FAMILY: Arial"><A href="javascript:print();"><IMG height=44
src="../image/Printer.gif" width=82 align=center
border=0></A></SPAN></B></FONT></td>
  </tr>
</table>
<P class=MsoNormal><br>
<font face="Arial" size="3">P� sk�rmen <i>Hantering</i> kan du �ndra s�kerhetsinst�llningarna
f�r gatewayen. Du rekommenderas att �ndra gatewayens fabriksinst�llda l�senord,
<b>admin</b> . Alla anv�ndare som f�rs�ker g� till gatewayens webbaserade
verktyg eller installationsguide m�ste ange l�senordet f�r gatewayen.</font><br>
<font size="3">&nbsp;</font></P>
<table width="100%" border="4">
  <tbody>
    <tr vAlign="top">
      <td width="13%"><font face="Arial" size="3"><b>Anv�ndarnamn f�r
        gatewayen</b></font>
      <td width="70%"><font face="Arial" size="3">Det nya anv�ndarnamnet f�r
        inte vara l�ngre �n 63 tecken.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>L�senord f�r gatewayen</b></font>
      <td><font face="Arial" size="3">Det nya l�senordet f�r inte vara l�ngre
        �n 30 tecken och f�r inte inneh�lla n�gra blanksteg. Bekr�fta det
        nya l�senordet genom att ange det en g�ng till.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>Fj�rrhantering</b></font>
      <td><font face="Arial" size="3">Med den h�r funktionen kan du hantera
        gatewayen fr�n en </font><font face="Arial" color="black" size="3">fj�rran
        plats</font><font size="3"> </font><font face="Arial" size="3">via
        Internet. Om du vill inaktivera den h�r funktionen beh�ller du
        standardinst�llningen <b>Inaktivera</b>. Du aktiverar den h�r
        funktionen genom att v�lja<b> Aktivera</b> . Du fj�rrhanterar
        gatewayen genom att anv�nda den angivna porten (standardinst�llningen
        �r <b><% remote_management_config("http_wanport", 0); %></b> ) p� datorn. Du m�ste ocks� �ndra gatewayens
        standardl�senord till ett eget om du inte redan har gjort det. Med ett
        unikt l�senord �kas s�kerheten.</font><font face="Times New Roman" size="2"><br>
        </font><font face="Arial" size="3"><br>
        Om du vill fj�rrhantera gatewayen anger du </font><a href="http://xxx.xxx.xxx.xxx:<% remote_management_config("http_wanport", 0); %>/"><font color="blue" size="3"><b><u>http://xxx.xxx.xxx.xxx:<% remote_management_config("http_wanport", 0); %>
        </u></b></font></a><font size="3">(varje x motsvarar ett tecken i
        gatewayens Internet-IP-adress, och <% remote_management_config("http_wanport", 0); %> motsvarar den angivna porten) i
        webbl�sarens <i>adressf�lt</i> . Du blir ombedd att ange gatewayens l�senord.
        N�r du har angett korrekt l�senord kan du g� till gatewayens
        webbaserade verktyg.</font><font face="Times New Roman" size="2"><br>
        </font><font face="Arial" size="3"><br>
        Obs! Om funktionen f�r fj�rrhantering har aktiverats kan vem som helst
        som kan gatewayens Internet-IP-adress och l�senord �ndra gatewayens
        inst�llningar.</font>
    </tr>
  </tbody>
</table>
<p><font size="3">&nbsp;</font><br>
<font face="Arial" size="3"><b>SNMP</b></font>
<p><font size="3">&nbsp;</font><br>
<font face="Arial" size="3">SNMP �r ett popul�rt protokoll f�r n�tverks�vervakning
och hantering. Om du vill aktivera SNMP-identifiering klickar du p� <b>Aktivera.</b>
Om du vill inaktivera SNMP klickar du p� <b>Inaktivera</b></font><br>
<font size="3">&nbsp;</font>
<table width="100%" border="4">
  <tbody>
    <tr vAlign="top">
      <td width="19%"><font face="Arial" size="3"><b>Enhetsnamn</b></font>
      <td width="80%"><font face="Arial" size="3">Du kan ange ett nytt
        enhetsnamn f�r att f�renkla n�tverkshanteringen. Det nya enhetsnamnet
        f�r inte vara l�ngre �n 39 tecken.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>H�mta m�tesplats</b></font>
      <td><font face="Arial" size="3">Ange det l�senord som medger l�s�tkomst
        till gatewayens SNMP-information.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>Ange m�tesplats</b></font>
      <td><font face="Arial" size="3">Ange det l�senord som medger l�s-/skriv�tkomst
        till gatewayens SNMP-information.</font>
    </tr>
  </tbody>
</table>
<p><font size="3">&nbsp;</font><br>
<font face="Arial" size="3"><b>UPnP</b></font>
<p><font size="3">&nbsp;</font><br>
<font face="Arial" size="3">UPnP g�r att Windows XP kan konfigurera gatewayen
automatiskt f�r olika </font><font face="Arial" color="black" size="3">Internettill�mpningar
</font><font face="Arial" size="3">, till exempel spel och videokonferenser. Om
du vill aktivera UPnP klickar du p� <b>Aktivera.</b> Om du vill inaktivera
UPnP klickar du p� <b>Inaktivera.</b></font><br>
<font size="3">&nbsp;</font><br>
<font face="Arial" size="3">Kontrollera alla v�rden och klicka p� <b>Spara
inst�llningar</b> om du vill spara inst�llningarna. Klicka p� <b>Avbryt </b>
om du vill avbryta �ndringar som du inte vill spara.</font></p>
<p>

<CENTER><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" onclick=self.close() type=button value=St�ng name=B3></CENTER>
</p>
<P></P></FORM></BODY></HTML>
