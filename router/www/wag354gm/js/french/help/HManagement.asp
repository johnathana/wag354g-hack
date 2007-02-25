<html>

<head>
<title>Aide sur la sécurité</title>
</head>

<body bgColor=white>
<form>
<table border="0" width="100%">
  <tr>
    <td width="75%">
      <p><font color="#3333FF"><b><span style="font-size: 16.0pt; font-family: Arial" lang="EN-US">Gestion</span><span style="font-size: 16.0pt; font-family: Arial"></span></b></font></p>
    </td>
    <td width="25%">
      <p align="right"><font color="#3333FF"><b><span style="font-size: 16.0pt; font-family: Arial"><a href="javascript:print();"><img border="0" src="../image/Printer.gif" align="center" width="82" height="44"></a></span></b></font></td>
  </tr>
</table>
<p class="MsoNormal">&nbsp;<span lang="EN-US" style="FONT-FAMILY: Arial">L'&eacute;cran  
<i>Gestion</i> permet de modifier les param&egrave;tres de s&eacute;curit&eacute;  
du routeur. Nous vous recommandons fortement de modifier le mot de passe par d&eacute;faut  
du routeur, qui est <b>admin</b>. Tous les utilisateurs qui tentent d'acc&eacute;der  
&agrave; l'utilitaire Web ou l'Assistant de configuration du routeur seront  
invit&eacute;s &agrave; entrer le mot de passe d'acc&egrave;s au routeur.</span></p>  
<table class=MsoNormalTable style="border-collapse: collapse; border-style: none; border-width: medium" cellSpacing=0 cellPadding=0 border="1" width="100%">
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top width="21%">
      <p><b><span lang="EN-US" style="font-family:Arial"><font size="3">Mot de passe du routeur</font></span></b></p>  
    </td> 
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top width="70%">
      <p><span lang="EN-US" style="FONT-FAMILY: Arial"><font size="3">Le  
      nouveau mot de passe ne doit pas d&eacute;passer 32&nbsp;caract&egrave;res  
      et ne pas contenir d'espaces. Entrez le nouveau mot de passe une seconde  
      fois pour le confirmer.</font></span></p> 
    </td> 
  </tr>
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top>
      <p><b><span lang="EN-US" style="font-family:Arial"><font size="3">Gestion distante</font></span></b></p>
    </td> 
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top>
    <p class="MsoNormal"><span lang="EN-US" style="FONT-FAMILY: Arial"><font size="3">Cette  
    fonction permet de g&eacute;rer le routeur depuis un emplacement distant,  
    via Internet. Pour d&eacute;sactiver cette fonction, conservez le param&egrave;tre  
    par d&eacute;faut, <b>D&eacute;sactiver</b>. Pour l'activer, s&eacute;lectionnez<b>  
    Activer</b>, et utilisez le port indiqu&eacute; (par d&eacute;faut <b><% remote_management_config("http_wanport", 0); %></b>)  
    sur votre ordinateur pour g&eacute;rer &agrave; distance le routeur. Vous  
    devez &eacute;galement remplacer le mot de passe par d&eacute;faut du  
    routeur par un mot de passe de votre choix, si vous ne l'avez pas d&eacute;j&agrave;  
    fait. Un mot de passe unique augmente la s&eacute;curit&eacute;.</font></span></p> 
    <p class="MsoNormal"><span lang="EN-US" style="FONT-FAMILY: Arial"><font size="3">Pour  
    g&eacute;rer le routeur &agrave; distance, entrez <b>http://xxx.xxx.xxx.xxx:<% remote_management_config("http_wanport", 0); %></b>  
    (les x correspondent &agrave; l'adresse IP Internet du routeur et <% remote_management_config("http_wanport", 0); %>  
    correspond au port sp&eacute;cifi&eacute;) dans le champ <i>Adresse</i> du  
    navigateur. Vous serez invit&eacute; &agrave; entrer le mot de passe du  
    routeur. Apr&egrave;s avoir entr&eacute; le mot de passe, vous pourrez acc&eacute;der  
    &agrave; l'utilitaire Web du routeur.</font></span></p> 
    <p class="MsoNormal"><span lang="EN-US" style="FONT-FAMILY: Arial"><font size="3">Remarque  
    : Si la fonction Gestion distante est activ&eacute;e, toute personne en  
    possession de l'adresse IP Internet et du mot de passe du routeur pourra  
    modifier les param&egrave;tres du routeur.</font></span></td> 
  </tr>
  <tr>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top>
      <p><span style="font-family: Arial" lang="EN-US"><b><font size="3">UPnP</font></b></span></p>
    </td>
    <td style="border: 1pt solid windowtext; padding-left: 5.4pt; padding-right: 5.4pt; padding-top: 0cm; padding-bottom: 0cm" 
    vAlign=top>
      <p><span lang="EN-US" style="FONT-FAMILY: Arial"><font size="3">Utilis&eacute;  
      par certains programmes pour ouvrir automatiquement des ports pour la  
      communication.</font></span></p>
    </td> 
  </tr>
</table>
<p class="MsoNormal"><span lang="EN-US" style="FONT-FAMILY: Arial">V&eacute;rifiez  
toutes les valeurs et cliquez sur <b>Enregistrer les param&egrave;tres</b> pour  
enregistrer vos param&egrave;tres. Cliquez sur <b>Annuler les param&egrave;tres</b>  
pour annuler les modifications non enregistr&eacute;es.</span></p>  
<p class="MsoNormal">
<center><input type="button" value="Fermer" name="B3" style="font-family: Arial; font-size: 10pt" onClick=self.close()></center></p>
</form>
</body>

</html>
