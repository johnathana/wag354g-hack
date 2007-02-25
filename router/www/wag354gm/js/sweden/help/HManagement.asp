<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">
<!-- saved from url=(0039)http://192.168.1.1/help/HManagement.asp -->
<HTML><HEAD><TITLE>Hjälp: Säkerhet</TITLE>
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
<font face="Arial" size="3">På skärmen <i>Hantering</i> kan du ändra säkerhetsinställningarna
för gatewayen. Du rekommenderas att ändra gatewayens fabriksinställda lösenord,
<b>admin</b> . Alla användare som försöker gå till gatewayens webbaserade
verktyg eller installationsguide måste ange lösenordet för gatewayen.</font><br>
<font size="3">&nbsp;</font></P>
<table width="100%" border="4">
  <tbody>
    <tr vAlign="top">
      <td width="13%"><font face="Arial" size="3"><b>Användarnamn för
        gatewayen</b></font>
      <td width="70%"><font face="Arial" size="3">Det nya användarnamnet får
        inte vara längre än 63 tecken.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>Lösenord för gatewayen</b></font>
      <td><font face="Arial" size="3">Det nya lösenordet får inte vara längre
        än 30 tecken och får inte innehålla några blanksteg. Bekräfta det
        nya lösenordet genom att ange det en gång till.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>Fjärrhantering</b></font>
      <td><font face="Arial" size="3">Med den här funktionen kan du hantera
        gatewayen från en </font><font face="Arial" color="black" size="3">fjärran
        plats</font><font size="3"> </font><font face="Arial" size="3">via
        Internet. Om du vill inaktivera den här funktionen behåller du
        standardinställningen <b>Inaktivera</b>. Du aktiverar den här
        funktionen genom att välja<b> Aktivera</b> . Du fjärrhanterar
        gatewayen genom att använda den angivna porten (standardinställningen
        är <b><% remote_management_config("http_wanport", 0); %></b> ) på datorn. Du måste också ändra gatewayens
        standardlösenord till ett eget om du inte redan har gjort det. Med ett
        unikt lösenord ökas säkerheten.</font><font face="Times New Roman" size="2"><br>
        </font><font face="Arial" size="3"><br>
        Om du vill fjärrhantera gatewayen anger du </font><a href="http://xxx.xxx.xxx.xxx:<% remote_management_config("http_wanport", 0); %>/"><font color="blue" size="3"><b><u>http://xxx.xxx.xxx.xxx:<% remote_management_config("http_wanport", 0); %>
        </u></b></font></a><font size="3">(varje x motsvarar ett tecken i
        gatewayens Internet-IP-adress, och <% remote_management_config("http_wanport", 0); %> motsvarar den angivna porten) i
        webbläsarens <i>adressfält</i> . Du blir ombedd att ange gatewayens lösenord.
        När du har angett korrekt lösenord kan du gå till gatewayens
        webbaserade verktyg.</font><font face="Times New Roman" size="2"><br>
        </font><font face="Arial" size="3"><br>
        Obs! Om funktionen för fjärrhantering har aktiverats kan vem som helst
        som kan gatewayens Internet-IP-adress och lösenord ändra gatewayens
        inställningar.</font>
    </tr>
  </tbody>
</table>
<p><font size="3">&nbsp;</font><br>
<font face="Arial" size="3"><b>SNMP</b></font>
<p><font size="3">&nbsp;</font><br>
<font face="Arial" size="3">SNMP är ett populärt protokoll för nätverksövervakning
och hantering. Om du vill aktivera SNMP-identifiering klickar du på <b>Aktivera.</b>
Om du vill inaktivera SNMP klickar du på <b>Inaktivera</b></font><br>
<font size="3">&nbsp;</font>
<table width="100%" border="4">
  <tbody>
    <tr vAlign="top">
      <td width="19%"><font face="Arial" size="3"><b>Enhetsnamn</b></font>
      <td width="80%"><font face="Arial" size="3">Du kan ange ett nytt
        enhetsnamn för att förenkla nätverkshanteringen. Det nya enhetsnamnet
        får inte vara längre än 39 tecken.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>Hämta mötesplats</b></font>
      <td><font face="Arial" size="3">Ange det lösenord som medger läsåtkomst
        till gatewayens SNMP-information.</font>
    <tr vAlign="top">
      <td><font face="Arial" size="3"><b>Ange mötesplats</b></font>
      <td><font face="Arial" size="3">Ange det lösenord som medger läs-/skrivåtkomst
        till gatewayens SNMP-information.</font>
    </tr>
  </tbody>
</table>
<p><font size="3">&nbsp;</font><br>
<font face="Arial" size="3"><b>UPnP</b></font>
<p><font size="3">&nbsp;</font><br>
<font face="Arial" size="3">UPnP gör att Windows XP kan konfigurera gatewayen
automatiskt för olika </font><font face="Arial" color="black" size="3">Internettillämpningar
</font><font face="Arial" size="3">, till exempel spel och videokonferenser. Om
du vill aktivera UPnP klickar du på <b>Aktivera.</b> Om du vill inaktivera
UPnP klickar du på <b>Inaktivera.</b></font><br>
<font size="3">&nbsp;</font><br>
<font face="Arial" size="3">Kontrollera alla värden och klicka på <b>Spara
inställningar</b> om du vill spara inställningarna. Klicka på <b>Avbryt </b>
om du vill avbryta ändringar som du inte vill spara.</font></p>
<p>

<CENTER><INPUT style="FONT-SIZE: 10pt; FONT-FAMILY: Arial" onclick=self.close() type=button value=Stäng name=B3></CENTER>
</p>
<P></P></FORM></BODY></HTML>
