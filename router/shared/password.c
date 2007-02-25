int
ej_passwd_match(int eid, webs_t wp, int argc, char_t **argv)
{
	int count;
	char http_remote_passwd[50],http_def_passwd[50];

	memset(http_remote_passwd, 0, sizeof(http_remote_passwd));
	memset(http_def_passwd, 0, sizeof(http_def_passwd));
	
	if(ejArgs(argc, argv, "%d", &count) < 1)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}

	strcpy(http_remote_passwd,nvram_safe_get("http_remote_passwd"));	
	strcpy(http_def_passwd,nvram_safe_get("http_def_passwd"));

	if(!strcmp(http_remote_passwd,http_def_passwd))
	//if(http_remote_passwd == nvram_get("http_def_passwd"))
		return websWrite(wp, "1");
	else 
		return websWrite(wp, "0");
}

int
ej_l_passwd_match(int eid, webs_t wp, int argc, char_t **argv)
{
	int count;
	char http_passwd[50],http_def_passwd[50];

	memset(http_passwd, 0, sizeof(http_passwd));
	memset(http_def_passwd, 0, sizeof(http_def_passwd));
	
	if(ejArgs(argc, argv, "%d", &count) < 1)
	{
		websError(wp, 400, "Insufficient args\n");
		return -1;
	}
	
	strcpy(http_passwd,nvram_safe_get("http_passwd"));	
	strcpy(http_def_passwd,nvram_safe_get("http_def_passwd"));	
	
	if(!strcmp(http_passwd,http_def_passwd))
		return websWrite(wp, "%s", "1");
	else 
		return websWrite(wp, "%s", "0");
}
