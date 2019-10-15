#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <unistd.h> 
#include <string.h> 
#include<stdlib.h>
#include<fcntl.h>
#include<pthread.h>
#include<netinet/in.h>
#define maxcon 100
void *trackerfun(void *t);
void* serverfun(void *t);
char checkgrpvalidity(char *,char *);
char chkactive(char *ipad,char *portno);

struct tracker
{
char *fname;
char *trackerno;
};





int main(int argc,char**argv)
{
char *str=(char*)malloc(sizeof(char)*100);
pthread_t pid;
int len;
struct tracker *temp=(struct tracker*)malloc(sizeof(struct tracker));
temp->fname=argv[1];
temp->trackerno=argv[2];
pthread_create(&pid,NULL,serverfun,temp);
while(fgets(str,100,stdin))
{
len=strlen(str);
str[len-1]='\0';
if(strcmp(str,"quit")==0)
{
fclose(fopen("activeusers.txt","w"));
exit(0);
}
}
return 0;
}



void*serverfun(void *t)
{
struct tracker*temp=(struct tracker*)t;
char *fname=temp->fname;
char *trackerno=temp->trackerno;
FILE*fp=fopen(fname,"r");
if(!fp)
{
	printf("unable to open the file\n");
	pthread_exit(NULL);
}
char *chck=(char*)malloc(sizeof(char)*100);
size_t size=100;int flag=0,i;
char *tokens[3];
while(getline(&chck,&size,fp)!=-1)
{
	if(chck[0]=='\n')
break;
tokens[0]=strtok(chck," ");
if(strcmp(tokens[0],trackerno)==0)
{
	i=0;
while(tokens[i]!=NULL)
{
	i++;
	if(i==2)
		tokens[i]=strtok(NULL,"\n");
	else
	tokens[i]=strtok(NULL," ");
}
flag=1;
break;
}
memset(chck,'\0',100);
}
fclose(fp);
if(flag==0)
{
printf("there is no such tracker with d specified id");
pthread_exit(NULL);
}
int pno,q,sfd,sfd1;
pno=atoi(tokens[2]);
char *ipaddress=(char*)malloc(sizeof(char)*100);
strcpy(ipaddress,tokens[1]);
sfd=socket(AF_INET,SOCK_STREAM,0);
if(sfd<0)
{
printf("unable to create a socket");
exit(1);
}
struct sockaddr_in a;
int adrlength=sizeof(a);
a.sin_family=AF_INET;
a.sin_addr.s_addr=inet_addr(ipaddress);
a.sin_port=htons(pno);
q=bind(sfd,(struct sockaddr*)&a,adrlength);
if(q<0)
{
printf("unable to bind to the specified port");
exit(1);
}
q=listen(sfd,maxcon);
if(q<0)
{
printf("unable to listen");
exit(1);
}
pthread_t threads[maxcon];
while((sfd1=accept(sfd,(struct sockaddr*)&a,(socklen_t*)&adrlength))>=0)
{
	pthread_create(&threads[i],NULL,trackerfun,&sfd1);
}
if(sfd1<0)
{
printf("pblm");
}
}
















void *trackerfun(void *t)
{
	int len;
int sfd1=*(int *)t;
char buff[100];
char *arr=(char *)malloc(sizeof(char)*200);
memset(arr,'\0',200);
char *req=(char *)malloc(sizeof(char)*30);
int uid;
char *unme=(char *)malloc(sizeof(char)*20);
char *passwd=(char*)malloc(sizeof(char)*20);
char *ipaddress=(char*)malloc(sizeof(char)*20);
char *portno=(char*)malloc(sizeof(char)*20);

char *ownername=(char*)malloc(sizeof(char)*20);
char *ownerip=(char*)malloc(sizeof(char)*20);
char *ownerport=(char*)malloc(sizeof(char)*20);

while(recv(sfd1,&len,sizeof(len),0))
{
	recv(sfd1,req,len+1,0);
if(strcmp(req,"create_user")==0)
{
memset(arr,'\0',200);memset(unme,'\0',20);memset(passwd,'\0',20);
memset(req,'\0',30);
FILE *fp=fopen("usersauthnlist.txt","a");
if (!fp)
{
printf("unable to open\n");
continue;
}
recv(sfd1,&len,sizeof(len),0);
recv(sfd1,unme,len+1,0);
strcat(arr,unme);
recv(sfd1,&len,sizeof(len),0);
recv(sfd1,passwd,len+1,0);
strcat(arr," ");
strcat(arr,passwd);
len=strlen(arr);
arr[len]='\n';
char *chck=(char *)malloc(sizeof(char)*100);
size_t size=100;int flag=0;
FILE *fp1=fopen("usersauthnlist.txt","r");
if(fp1)
{
while(getline(&chck,&size,fp1)!=-1)
{
if(chck[0]=='\n')
break;
char *tokens[2];
tokens[0]=strtok(chck," ");
tokens[1]=strtok(NULL,"\n");
//printf("%s %s %s %s\n",unme,passwd,tokens[0],tokens[1]);
int z=0;
if(strcmp(tokens[0],unme)==0)
{
	flag=1;
	break;
}
}
free(chck);
fclose(fp1);
}
char c;
if(flag==1)
{
	c='n';
	send(sfd1,&c,sizeof(c),0);
	fclose(fp);
}

if(flag==0)
{
	c='y';
send(sfd1,&c,sizeof(c),0);
//printf("arr:%s\n",arr);
fprintf(fp,"%s",arr);
fclose(fp);
}
}

if(strcmp(req,"login")==0)
{
memset(unme,'\0',20);memset(passwd,'\0',20);
memset(req,'\0',30);memset(ipaddress,'\0',20);memset(portno,'\0',20);

FILE *fp=fopen("usersauthnlist.txt","r");
if (fp==NULL)
{
printf("unable to open\n");
}
recv(sfd1,&len,sizeof(len),0);
recv(sfd1,unme,len+1,0);
recv(sfd1,&len,sizeof(len),0);
recv(sfd1,passwd,len+1,0);
recv(sfd1,&len,sizeof(len),0);
recv(sfd1,ipaddress,len+1,0);
recv(sfd1,&len,sizeof(len),0);
recv(sfd1,portno,len+1,0);

char *chck=(char *)malloc(sizeof(char)*100);
size_t size=100;int flag=0;
while(getline(&chck,&size,fp)!=-1)
{
if(chck[0]=='\n')
break;
char *tokens[2];
tokens[0]=strtok(chck," ");
tokens[1]=strtok(NULL,"\n");
int z=0;
if(strcmp(tokens[0],unme)==0 && strcmp(tokens[1],passwd)==0)
{
	flag=1;
	break;
}
}
free(chck);
fclose(fp);

char c;
if(flag==1)//user is authorized one
{
	memset(ownername,'\0',20);
	memset(ownerip,'\0',20);
	memset(ownerport,'\0',20);
	c='y';
send(sfd1,&c,sizeof(c),0);
strcpy(ownername,unme);
strcpy(ownerip,ipaddress);
strcpy(ownerport,portno);
fp=fopen("activeusers.txt","a");
fprintf(fp,"%s %s %s\n",ownername,ownerip,ownerport);
fclose(fp);
}
else//unauthorized
{
	c='n';
send(sfd1,&c,sizeof(c),0);
}
}

if(strcmp(req,"create_group")==0)
{
memset(arr,'\0',200);
FILE *fp=fopen("grouplist.txt","a");
if(!fp)
{
	printf("unable to open\n");
	continue;
}
char *gname=(char*)malloc(sizeof(char)*20);
int len;
recv(sfd1,&len,sizeof(len),0);
recv(sfd1,gname,len+1,0);
strcat(arr,gname);
strcat(arr," ");
strcat(arr,ownername);
len=strlen(arr);
arr[len]='\n';
char *chck=(char *)malloc(sizeof(char)*100);
size_t size=100;int flag=0;
FILE *fp1=fopen("grouplist.txt","r");
if(fp1)
{
	//printf("hello\n");
while(getline(&chck,&size,fp1)!=-1)
{
if(chck[0]=='\n')
break;
char *tokens[2];
tokens[0]=strtok(chck," ");
tokens[1]=strtok(NULL,"\n");
//printf("%s %s %s %s\n",unme,passwd,tokens[0],tokens[1]);
int z=0;
if(strcmp(tokens[0],gname)==0)
{
	flag=1;
	break;
}
}
free(chck);
fclose(fp1);
}
char c;
if(flag==1)
{
	c='n';
	send(sfd1,&c,sizeof(c),0);
	fclose(fp);
}

if(flag==0)
{
	c='y';
send(sfd1,&c,sizeof(c),0);
//printf("arr:%s\n",arr);
fprintf(fp,"%s",arr);
fclose(fp);
//.....newlyadded
char *str=(char*)malloc(sizeof(char)*25);
memset(str,'\0',25);
strcat(str,"grpmemof_");
strcat(str,gname);
strcat(str,".txt");
fp=fopen(str,"a");
fprintf(fp,"%s\n",ownername);
free(str);fclose(fp);
//......
}
}



if(strcmp(req,"list_groups")==0)
{
	int len;
	char *token=(char*)malloc(sizeof(char)*20);
FILE *fp=fopen("grouplist.txt","r");
char *chk=(char*)malloc(sizeof(char)*50);
size_t size=50;int flag;
while(getline(&chk,&size,fp)!=-1)
{
if(chk[0]=='\n')
{
break;
}
flag=1;
strcpy(token,strtok(chk," "));
send(sfd1,&flag,sizeof(flag),0);
len=strlen(token);
send(sfd1,&len,sizeof(len),0);
send(sfd1,token,len+1,0);
memset(chk,'\0',50);
memset(token,'\0',20);
}
fclose(fp);
free(chk);
flag=0;
send(sfd1,&flag,sizeof(flag),0);
}

if(strcmp(req,"logout")==0)
{
	//printf("%s\n",ownername);
	//int i=0;
	char *chck=(char *)malloc(sizeof(char)*100);
	size_t size=100;
	char *fname="activeusers.txt";
FILE *fp=fopen("activeusers.txt","r");
FILE *fp1=fopen("tempo.txt","w");
char *tokens[3];
while(getline(&chck,&size,fp)!=-1)
{
if(chck[0]=='\n')
	break;
tokens[0]=strtok(chck," ");
tokens[1]=strtok(NULL," ");
tokens[2]=strtok(NULL,"\n");
if(strcmp(tokens[0],ownername)!=0)
{
fprintf(fp1,"%s %s %s\n",tokens[0],tokens[1],tokens[2]);
}
}
memset(ownername,'\0',20);
fclose(fp);
fclose(fp1);
remove("activeusers.txt");
rename("tempo.txt",fname);
free(chck);
}








if(strcmp(req,"join_group")==0)
{
memset(req,'\0',30);int len;
int flag=0;
char*grpid=(char*)malloc(sizeof(char)*20);
recv(sfd1,&len,sizeof(len),0);
recv(sfd1,grpid,len+1,0);
FILE *fp=fopen("grouplist.txt","r");
char *chk=(char *)malloc(sizeof(char)*50);
size_t size=50;
char *chunk;
while(getline(&chk,&size,fp)!=-1)
{
if(chk[0]=='\n')
break;
chunk=strtok(chk," ");
if(strcmp(chunk,grpid)==0)
{
	flag=1;
	break;
}
}
fclose(fp);
memset(chk,'\0',50);
if(flag==0)
{
	send(sfd1,&flag,sizeof(flag),0);
	continue;
}
if(flag==1)
{
send(sfd1,&flag,sizeof(flag),0);
fp=fopen("pendingreq.txt","a");
fprintf(fp,"%s %s\n",grpid,ownername);
fclose(fp);
}
free(chk);
}

if(strcmp(req,"list_requests")==0)
{
FILE *fp=fopen("pendingreq.txt","r");
char *gid=(char*)malloc(sizeof(char)*15);
char *chck=(char*)malloc(sizeof(char)*30);
size_t size=30;
int len,flag;
recv(sfd1,&len,sizeof(len),0);
recv(sfd1,gid,len+1,0);
char *tokens[2];
char *temp=(char*)malloc(sizeof(char)*10);
while(getline(&chck,&size,fp)!=-1)
{
if(chck[0]=='\n')
{
	flag=0;
	send(sfd1,&flag,sizeof(flag),0);
	break;
}
tokens[0]=strtok(chck," ");
if(strcmp(tokens[0],gid)==0)
{
	flag=1;
	send(sfd1,&flag,sizeof(flag),0);
	tokens[1]=strtok(NULL,"\n");
	strcpy(temp,tokens[1]);
	len=strlen(temp);
	send(sfd1,&len,sizeof(len),0);
	send(sfd1,temp,len+1,0);
}
}
if(chck[0]!='\n')
{
flag=0;
send(sfd1,&flag,sizeof(flag),0);
}
fclose(fp);free(chck);
}

if(strcmp(req,"accept_request")==0)//check once...
{
FILE *fp=fopen("grouplist.txt","r");	
int len,flag=0;
char *gid=(char*)malloc(sizeof(char)*20);
char *uid=(char*)malloc(sizeof(char)*20);
recv(sfd1,&len,sizeof(len),0);
recv(sfd1,gid,len+1,0);
recv(sfd1,&len,sizeof(len),0);
recv(sfd1,uid,len+1,0);
char *chck=(char*)malloc(sizeof(char)*50);
size_t size=50;
char *chnks[2];
while(getline(&chck,&size,fp)!=-1)
{
if(chck[0]=='\n')
break;
//printf("chk:%s",chck);
chnks[0]=strtok(chck," ");
if(strcmp(chnks[0],gid)==0)
{
	flag=1;
	chnks[1]=strtok(NULL,"\n");
	if(strcmp(chnks[1],ownername)==0)
	{
		flag=2;
		break;
	}
	break;
}
memset(chck,'\0',50);
}
if(flag==0 || flag==1)
send(sfd1,&flag,sizeof(flag),0);
else
{
char *str=(char*)malloc(sizeof(char)*25);
strcat(str,"grpmemof_");
strcat(str,gid);
strcat(str,".txt");
//printf("fnme:%s\n",str);
FILE *fp=fopen(str,"a");
fprintf(fp,"%s\n",uid);
send(sfd1,&flag,sizeof(flag),0);
fclose(fp);
fp=fopen("pendingreq.txt","r");
FILE* fp2=fopen("temp.txt","w");
char *chk=(char *)malloc(sizeof(char)*50);
char *chunks[2];
size_t size=50;
while(getline(&chck,&size,fp)!=-1)
{
if(chck[0]=='\n')
break;
chunks[0]=strtok(chck," ");
chunks[1]=strtok(NULL,"\n");
if(strcmp(chunks[0],gid)==0 && strcmp(chunks[1],uid)==0)
continue;
else
fprintf(fp2,"%s %s\n",chunks[0],chunks[1]);
}
fclose(fp);fclose(fp2);
remove("pendingreq.txt");
rename("temp.txt","pendingreq.txt");
}
}

if(strcmp(req,"leave_group")==0)
{
char *grpid=(char*)malloc(sizeof(char)*20);
char *str=(char*)malloc(sizeof(char)*40);
char *chk=(char*)malloc(sizeof(char)*40);
size_t size=50;
memset(str,'\0',0);
int len,flag=0;
recv(sfd1,&len,sizeof(len),0);
recv(sfd1,grpid,len+1,0);
strcpy(str,"grpmemof_");
strcat(str,grpid);
strcat(str,".txt");
FILE *fp3=fopen("grouplist.txt","r");
memset(chk,'\0',40);
char *grpown=(char*)malloc(sizeof(char)*15);
char *chunks[2];
while(getline(&chk,&size,fp3)!=-1)
{
	if(chk[0]=='\n')
		break;
chunks[0]=strtok(chk," ");
if(strcmp(chunks[0],grpid)==0)
{
	flag=1;
	chunks[1]=strtok(NULL,"\n");
	strcpy(grpown,chunks[1]);
	break;
}
}
fclose(fp3);
if(flag==0)
{
send(sfd1,&flag,sizeof(flag),0);
continue;	
}
char o;
if(strcmp(ownername,grpown)==0)
o='y';
else
o='n';

if(o=='y')
{
	memset(str,'\0',40);
strcpy(str,"grpmemof_");
strcat(str,grpid);
strcat(str,".txt");	
FILE *fp1=fopen(str,"r");
memset(chk,'\0',40);
int count=0;
while(getline(&chk,&size,fp1)!=-1)
{
	//printf("chk[0]:%c\n",chk[0]);
	if(chk[0]=='\n')
		break;
count++;
memset(chk,'\0',40);
}
fclose(fp1);
if(count<=1)//only owner is present in grp...so deleting the group..
{
remove(str);
fp1=fopen("grouplist.txt","r");
FILE *fp2=fopen("temporary.txt","w");
memset(chk,'\0',40);
while(getline(&chk,&size,fp1)!=-1)
{
if(chk[0]=='\n')
break;
chunks[0]=strtok(chk," ");
chunks[1]=strtok(NULL,"\n");
if(strcmp(chunks[0],grpid)!=0)
fprintf(fp2,"%s %s\n",chunks[0],chunks[1]);
}
fclose(fp1);
fclose(fp2);
remove("grouplist.txt");
rename("temporary.txt","grouplist.txt");
flag=1;
send(sfd1,&flag,sizeof(flag),0);
continue;
}
if(count>1)//removing ownername from groupmemberslist..and updaing owner of grp
{

fp1=fopen(str,"r");
FILE *fp2=fopen("temporary.txt","w");
memset(chk,'\0',40);
char *newown=(char*)malloc(sizeof(char)*15);
int done=0;
while(getline(&chk,&size,fp1)!=-1)
{
if(chk[0]=='\n')
break;
chunks[0]=strtok(chk,"\n");
if(strcmp(chunks[0],grpown)!=0)//except grp own adding others..
{
if(done==0)
{
	done=1;
	strcpy(newown,chunks[0]);
	
}
fprintf(fp2,"%s\n",chunks[0]);
}
}
fclose(fp1);
fclose(fp2);
remove(str);
rename("temporary.txt",str);

fp1=fopen("grouplist.txt","r");
memset(chk,'\0',40);
fp2=fopen("temporary.txt","w");
while(getline(&chk,&size,fp1)!=-1)
{
if(chk[0]=='\n')
break;
chunks[0]=strtok(chk," ");
chunks[1]=strtok(NULL,"\n");
if(strcmp(chunks[0],grpid)!=0)
{
	fprintf(fp2,"%s %s\n",chunks[0],chunks[1]);
}
}
fprintf(fp2,"%s %s\n",grpid,newown);//adding new owner of group
flag=1;
send(sfd1,&flag,sizeof(flag),0);
fclose(fp1);fclose(fp2);
remove("grouplist.txt");
rename("temporary.txt","grouplist.txt");
continue;
}
}

if(o=='n')
{
	int pres=0;
FILE *fp6=fopen(str,"r");
memset(chk,'\0',0);
char *temp6;
while(getline(&chk,&size,fp6)!=-1)
{
	if(chk[0]=='\n')
	break;
temp6=strtok(chk,"\n");

if(strcmp(temp6,ownername)==0)
{
	pres=1;
	break;
}
}
if(pres==0)
{
fclose(fp6);
flag=-1;
send(sfd1,&flag,sizeof(flag),0);
continue;
}
//user is part of grp..
FILE *fp1=fopen(str,"r");
FILE *fp2=fopen("temporary.txt","w");
memset(chk,'\0',40);
while(getline(&chk,&size,fp1)!=-1)
{
if(chk[0]=='\n')
break;
temp6=strtok(chk,"\n");
if(strcmp(temp6,ownername)!=0)//except user who want to leave grp,adding others..
{
fprintf(fp2,"%s\n",chunks[0]);
}
}
fclose(fp1);
fclose(fp2);
remove(str);
rename("temporary.txt",str);

send(sfd1,&flag,sizeof(flag),0);
fclose(fp6);
memset(chk,'\0',40);
continue;
}
}

if(strcmp("upload_file",req)==0)
{
char *grpid=(char*)malloc(sizeof(char)*15);
char *filepath=(char*)malloc(sizeof(char)*60);
int len,flag=0;
recv(sfd1,&len,sizeof(len),0);
recv(sfd1,filepath,len+1,0);
recv(sfd1,&len,sizeof(len),0);
recv(sfd1,grpid,len+1,0);
char c=checkgrpvalidity(grpid,ownername);
if(c=='n')
{
flag=-1;
send(sfd1,&flag,sizeof(flag),0);
continue;
}
if(c=='e')
{
	flag=0;
	send(sfd1,&flag,sizeof(flag),0);
	continue;
}
FILE *fp=fopen("file_list.txt","a");
fprintf(fp,"%s %s %s %s %s\n",filepath,grpid,ownername,ownerip,ownerport);
fclose(fp);
free(filepath);free(grpid);
flag=1;
send(sfd1,&flag,sizeof(flag),0);
continue;
}


if(strcmp("download_file",req)==0)
{
int len,flag;
char *grpid=(char*)malloc(sizeof(char)*15);
char *fname=(char*)malloc(sizeof(char)*60);
recv(sfd1,&len,sizeof(len),0);
recv(sfd1,grpid,len+1,0);
recv(sfd1,&len,sizeof(len),0);
recv(sfd1,fname,len+1,0);
char c=checkgrpvalidity(grpid,ownername);
//printf("c:%c\n",c);
if(c=='n')
{
flag=-1;
free(grpid);free(fname);
send(sfd1,&flag,sizeof(flag),0);
continue;
}
if(c=='e')//error case..no such grpid....
{
	flag=0;
	free(grpid);free(fname);
	send(sfd1,&flag,sizeof(flag),0);
	continue;
}
flag=1;
send(sfd1,&flag,sizeof(flag),0);
//printf("c:%c\n",c);
FILE *fp=fopen("file_list.txt","r");
char *chk=(char*)malloc(sizeof(char)*100);
size_t size=100;
char *chunks[5];
char *ipad=(char*)malloc(sizeof(char)*30);
char *port=(char*)malloc(sizeof(char)*10);
int i;
int fpres=0,more;
while(getline(&chk,&size,fp)!=-1)
{
if(chk[0]=='\n')
break;
//printf("chk:%s:\n",chk);
chunks[0]=strtok(chk," ");
i=0;
while(chunks[i]!=NULL)
{
i++;
if(i==4)
chunks[i]=strtok(NULL,"\n");
else
chunks[i]=strtok(NULL," ");
}
//printf("chk0:%s chk1:%s chk2:%s chk3:%s fname:%s grpid:%s\n",chunks[0],chunks[1],chunks[2],chunks[3],fname,grpid);
if(strcmp(chunks[0],fname)==0 && strcmp(chunks[1],grpid)==0)
{
fpres=1;
strcpy(ipad,chunks[3]);strcpy(port,chunks[4]);
c=chkactive(ipad,port);

if(c=='p')
{
	//printf("there is an active port..\n");
more=1;
send(sfd1,&more,sizeof(more),0);
len=strlen(ipad);
//printf("ipad:%s\n",ipad);
send(sfd1,&len,sizeof(len),0);
send(sfd1,ipad,len+1,0);
len=strlen(port);
//printf("port:%s\n",port);
send(sfd1,&len,sizeof(len),0);
send(sfd1,port,len+1,0);
}
}
}
fclose(fp);
more=0;
send(sfd1,&more,sizeof(more),0);
send(sfd1,&fpres,sizeof(fpres),0);
free(ipad);free(port);free(chk);free(grpid);free(fname);
continue;
}

if(strcmp(req,"list_files")==0)
{
int len,flag,more;
char *grpid=(char*)malloc(sizeof(char)*15);
recv(sfd1,&len,sizeof(len),0);
recv(sfd1,grpid,len+1,0);
FILE *fp=fopen("file_list.txt","r");
if(fp==NULL)
{
free(grpid);
flag=0;
send(sfd1,&flag,sizeof(flag),0);
continue;
}
flag=1;
send(sfd1,&flag,sizeof(flag),0);
char *chk=(char*)malloc(sizeof(char)*200);
size_t size=200;
char *chunks[2];
char *temp=(char*)malloc(sizeof(char)*60);
while(getline(&chk,&size,fp)!=-1)
{
if(chk[0]=='\n')
break;
//printf("chk:%s\n",chk);
chunks[0]=strtok(chk," ");
chunks[1]=strtok(NULL," ");
if(strcmp(chunks[1],grpid)==0)
{
more=1;
send(sfd1,&more,sizeof(more),0);
strcpy(temp,chunks[0]);
len=strlen(temp);
send(sfd1,&len,sizeof(len),0);
send(sfd1,temp,len+1,0);
memset(temp,'\0',60);
}
}
more=0;
send(sfd1,&more,sizeof(more),0);
fclose(fp);free(chk);free(grpid);
}

if(strcmp(req,"stop_share")==0)
{
int len;int flag;
char *grpid=(char*)malloc(sizeof(char)*15);
char *fname=(char*)malloc(sizeof(char)*50);
recv(sfd1,&len,sizeof(len),0);
recv(sfd1,grpid,len+1,0);
recv(sfd1,&len,sizeof(len),0);
recv(sfd1,fname,len+1,0);

char c=checkgrpvalidity(grpid,ownername);
//printf("c:%c\n",c);
if(c=='n')
{
flag=-1;
free(grpid);free(fname);
send(sfd1,&flag,sizeof(flag),0);
continue;
}
if(c=='e')//error case..no such grpid....
{
	flag=0;
	free(grpid);free(fname);
	send(sfd1,&flag,sizeof(flag),0);
	continue;
}

FILE*fp=fopen("file_list.txt","r");
if(fp==NULL)
{
	flag=1;
	free(grpid);free(fname);
	send(sfd1,&flag,sizeof(flag),0);
	continue;
}
flag=2;
send(sfd1,&flag,sizeof(flag),0);
FILE *fp2=fopen("tempo.txt","w");
char *chk=(char*)malloc(sizeof(char)*200);
size_t size=200;
char *chunks[5];
while(getline(&chk,&size,fp)!=-1)
{
if(chk[0]=='\n')
break;
chunks[0]=strtok(chk," ");
chunks[1]=strtok(NULL," ");
chunks[2]=strtok(NULL," ");
chunks[3]=strtok(NULL," ");
chunks[4]=strtok(NULL,"\n");
if(strcmp(chunks[0],fname)==0 && strcmp(chunks[1],grpid)==0 && strcmp(chunks[2],ownername)==0)
continue;
fprintf(fp2,"%s %s %s %s %s\n",chunks[0],chunks[1],chunks[2],chunks[3],chunks[4]);
}
fclose(fp);fclose(fp2);
remove("file_list.txt");
rename("tempo.txt","file_list.txt");
}



}
}





char chkactive(char *ipad,char *portno)
{
char c='a';//assuming inactive
FILE *fp=fopen("activeusers.txt","r");
char *chk=(char*)malloc(sizeof(char)*100);
size_t size=100;char *chunks[3];
while(getline(&chk,&size,fp)!=-1)
{
//printf("chk:%s\n",chk);
if(chk[0]=='\n')
break;
chunks[0]=strtok(chk," ");
chunks[1]=strtok(NULL," ");
chunks[2]=strtok(NULL,"\n");
//printf("chunk1:%s chunk2:%s ipad:%s portno:%s\n",chunks[1],chunks[2],ipad,portno);
if(strcmp(chunks[1],ipad)==0 && strcmp(chunks[2],portno)==0)
{
	//printf("hello\n");
c='p';//active
free(chk);fclose(fp);
return c;
}
}
free(chk);fclose(fp);
//printf("inactive\n");
return c;
}








char checkgrpvalidity(char *grpid,char *ownername)
{
	//printf("grpid:%s ownername:%s\n",grpid,ownername);
char c='n';
char *str=(char*)malloc(sizeof(char)*30);
strcpy(str,"grpmemof_");
strcat(str,grpid);
strcat(str,".txt");	
//printf("str:%s\n",str);
FILE *fp=fopen(str,"r");
if(fp==NULL)
{
	//printf("no such file\n");
	c='e';
return c;
}
char *chk=(char*)malloc(sizeof(char)*30);
size_t size=30;
char *token;
while(getline(&chk,&size,fp)!=-1)
{
if(chk[0]=='\n')
break;
token=strtok(chk,"\n");
//printf("token:%s ownername:%s",token,ownername);
if(strcmp(token,ownername)==0)
{
	c='y';
	break;
}
}
fclose(fp);
free(chk);
free(str);
return c;
}
