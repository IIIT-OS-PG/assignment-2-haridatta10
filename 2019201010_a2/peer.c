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
void* trackercall(void *t);
void *serverfun(void *);
void *clientfun(void *);
void *filetransfer(void *);

struct clnode
{
char *ipad;
int portno;
char *fname;
char *destiny;
};


struct senode
{
char *ipad;
int portno;
};
int verified=0;

struct fdet
{
char *ip;
char *port;
};


int thdcount=0;
int main(int argc,char **argv)
{
char *str=(char*)malloc(sizeof(char)*100);
pthread_t pid;
struct senode *temp=(struct senode*)malloc(sizeof(struct senode));
temp->ipad=argv[1];
temp->portno=atoi(argv[2]);
pthread_create(&pid,NULL,serverfun,temp);
char *fname=(char *)malloc(sizeof(char)*30);
strcpy(fname,argv[3]);

//connecting to a tracker..
FILE *fp=fopen(fname,"r");
if(!fp)
{
	printf("unable to open the file");
	exit(0);
}
int id=rand()%2;
char *chck=(char*)malloc(sizeof(char)*100);
size_t size=100;
char *tokens[3];
while(getline(&chck,&size,fp)!=-1)
{
if(chck[0]=='\n')
break;
tokens[0]=strtok(chck," ");
if(atoi(tokens[0])==id)
{
int i=0;
while(tokens[i]!=NULL)
{
	i++;
	if(i==2)
		tokens[i]=strtok(NULL,"\n");
	else
	tokens[i]=strtok(NULL," ");
}
}
}

fclose(fp);
int pno=atoi(tokens[2]);
char *ipaddress=(char*)malloc(sizeof(char)*30);
strcpy(ipaddress,tokens[1]);
int sfd=socket(AF_INET,SOCK_STREAM,0);
if(sfd==-1)
{
printf("unable to create a socket..");
}
struct sockaddr_in add;
add.sin_family=AF_INET;
add.sin_port=htons(pno);
add.sin_addr.s_addr=inet_addr(ipaddress);
if (connect(sfd,(struct sockaddr *)&add, sizeof(add)) < 0) 
{ 
        perror("Connection Failed with tracker.."); 
}

while(fgets(str,100,stdin))
{
str[strlen(str)-1]='\0';
char *token[4];
token[0]=strtok(str," ");
int z=0;
while(token[z] != NULL)
{
	++z;
	token[z]=strtok(NULL," ");
}


if(strcmp(token[0],"create_user")==0)
{
int len;
char *pass=(char*)malloc(sizeof(char)*20);
char *req=(char*)malloc(sizeof(char)*20);
char *uid=(char*)malloc(sizeof(char)*20);

strcpy(uid,token[1]);
strcpy(pass,token[2]);
strcpy(req,token[0]);
len=strlen(req);
	send(sfd,&len,sizeof(len),0);
	send(sfd,req,strlen(req)+1,0);
	len=strlen(uid);
	send(sfd,&len,sizeof(len),0);
	send(sfd,uid,strlen(uid)+1,0);
	len=strlen(pass);
	send(sfd,&len,sizeof(len),0);
	send(sfd,pass,strlen(pass)+1,0);
char flag;
recv(sfd,&flag,sizeof(flag),0);
	if(flag=='y')
	{
		printf("successfully created the user with given credentials..\n");
	}
	else
	{
		printf("choose dfrnt username...\n");
	}
	memset(str,'\0',100);
	continue;
}

if(strcmp(token[0],"login")==0)
{
if(verified==1)
{
printf("logged in as some user already..\n");
continue;
}
int len;
char *pass=(char*)malloc(sizeof(char)*20);
char *req=(char*)malloc(sizeof(char)*20);
char *uid=(char*)malloc(sizeof(char)*20);
char *uipad=(char*)malloc(sizeof(char)*50);
char *uport=(char*)malloc(sizeof(char)*20);
strcpy(uid,token[1]);
strcpy(pass,token[2]);
strcpy(req,token[0]);
strcpy(uipad,argv[1]);
strcpy(uport,argv[2]);
char flag;
	len=strlen(req);
	send(sfd,&len,sizeof(len),0);
	send(sfd,req,strlen(req)+1,0);
	len=strlen(uid);
	send(sfd,&len,sizeof(len),0);
	send(sfd,uid,strlen(uid)+1,0);
	len=strlen(pass);
	send(sfd,&len,sizeof(len),0);
	send(sfd,pass,strlen(pass)+1,0);
	len=strlen(uipad);
	send(sfd,&len,sizeof(len),0);
	send(sfd,uipad,len+1,0);
	len=strlen(uport);
	send(sfd,&len,sizeof(len),0);
	send(sfd,uport,len+1,0);




	recv(sfd,&flag,sizeof(flag),0);
	if(flag=='y')// user typed his correct credentials,so..adding to a file..
	{
		printf("logged in successfully...\n");
		verified=1;
	}
	else
	{
		printf("entered credentials are wrong\n");
	}
	memset(str,'\0',100);
	continue;
}

if(strcmp(token[0],"create_group")==0)
{
	int len;
if(verified==0)
{
	printf("login to use the resources\n");
	continue;
}
char *req=(char*)malloc(sizeof(char)*20);
char *gnme=(char *)malloc(sizeof(char)*20);
strcpy(gnme,token[1]);
strcpy(req,token[0]);
len=strlen(req);
send(sfd,&len,sizeof(len),0);
send(sfd,req,strlen(req)+1,0);
len=strlen(gnme);
send(sfd,&len,sizeof(len),0);
send(sfd,gnme,len+1,0);
char flag;
recv(sfd,&flag,sizeof(flag),0);
	if(flag=='y')
	{
		printf("successfully created the group....\n");
	}
	else
	{
		printf("choose dfrnt groupname...\n");
	}
memset(str,'\0',100);
	continue;

}

if(strcmp(token[0],"list_groups")==0)
{
	//printf("hiidude..\n");
int len;
if(verified==0)
{
	printf("login to use the resources\n");
	memset(str,'\0',100);
	continue;
}
char *req=(char*)malloc(sizeof(char)*20);
char *gnme=(char *)malloc(sizeof(char)*20);
strcpy(req,token[0]);
len=strlen(req);
send(sfd,&len,sizeof(len),0);
send(sfd,req,len+1,0);
int flag;
recv(sfd,&flag,sizeof(flag),0);
if(flag==0)
{
	printf("there are no groups created...\n");
	memset(str,'\0',100);
	continue;
}
else
{
	printf("groups list...\n");
}
while(flag==1)
{
recv(sfd,&len,sizeof(len),0);
recv(sfd,gnme,len+1,0);
printf("%s\n",gnme);
memset(gnme,'\0',20);
recv(sfd,&flag,sizeof(flag),0);
}
printf("end..\n");
memset(str,'\0',100);
continue;
}

if(strcmp(token[0],"logout")==0)
{
	int len;
if(verified==0)
{
	printf("there is no user who logged in...\n");
	memset(str,'\0',100);
	continue;
}
else
{
	char *req=(char*)malloc(sizeof(char)*20);
	strcpy(req,token[0]);
	len=strlen(req);
	send(sfd,&len,sizeof(len),0);
	send(sfd,req,len+1,0);
	verified=0;
	printf("logged out successfully...\n");
	memset(str,'\0',100);
	continue;
}
}

if(strcmp(token[0],"join_group")==0)
{
	if(verified==0)
{
	printf("login to use the resources\n");
	memset(str,'\0',100);
	continue;
}
	//printf("hii");
char *req=(char *)malloc(sizeof(char)*30);
char *grpid=(char*)malloc(sizeof(char)*20);
strcpy(grpid,token[1]);
int len;
int flag;
strcpy(req,token[0]);
len=strlen(req);
send(sfd,&len,sizeof(len),0);
send(sfd,req,len+1,0);
len=strlen(grpid);
send(sfd,&len,sizeof(len),0);
send(sfd,grpid,len+1,0);
recv(sfd,&flag,sizeof(flag),0);
//printf("%d\n",flag);
if(flag==0)
{
	printf("There is no such group...\n");
	continue;
}
else
{
printf("The request has been kept in waiting queue...\n");
continue;
}
}

if(strcmp(token[0],"list_requests")==0)
{
	if(verified==0)
{
	printf("login to use the resources\n");
	memset(str,'\0',100);
	continue;
}
int len,flag=0,pres=0;
char *req=(char *)malloc(sizeof(char)*30);
char *grpid=(char*)malloc(sizeof(char)*20);
char *uid=(char*)malloc(sizeof(char)*20);
strcpy(req,token[0]);
strcpy(grpid,token[1]);
len=strlen(req);
send(sfd,&len,sizeof(len),0);
send(sfd,req,len+1,0);
len=strlen(grpid);
send(sfd,&len,sizeof(len),0);
send(sfd,grpid,len+1,0);
recv(sfd,&flag,sizeof(flag),0);
while(flag==1)
{
pres=1;
recv(sfd,&len,sizeof(len),0);
recv(sfd,uid,len+1,0);
recv(sfd,&flag,sizeof(flag),0);
printf("%s\n",uid);
memset(uid,'\0',20);
}
if(pres==0)
{
printf("There are no req for this grpid\n");
memset(str,'\0',100);
continue;
}
memset(str,'\0',100);
	continue;
}

if(strcmp(token[0],"accept_request")==0)//working wrong... 
{
	if(verified==0)
{
	printf("login to use the resources\n");
	memset(str,'\0',100);
	continue;
}
	int len;int flag;
char *gid=(char*)malloc(sizeof(char)*20);
char *uid=(char*)malloc(sizeof(char)*20);
char *req=(char*)malloc(sizeof(char)*20);
strcpy(gid,token[1]);
strcpy(uid,token[2]);
strcpy(req,token[0]);
len=strlen(req);
send(sfd,&len,sizeof(len),0);
send(sfd,req,len+1,0);
len=strlen(gid);
send(sfd,&len,sizeof(len),0);
send(sfd,gid,len+1,0);
len=strlen(uid);
send(sfd,&len,sizeof(len),0);
send(sfd,uid,len+1,0);
recv(sfd,&flag,sizeof(flag),0);
if(flag==0)
{
printf("There is no such grpid ..\n");
memset(str,'\0',100);
	continue;
}
if(flag==1)
{
	printf("u r not authorized to do this..(can be done nly by grp owner..\n)");
}
else
printf("done..\n");
free(gid);free(uid);free(req);
memset(str,'\0',100);
continue;
}

if(strcmp(token[0],"leave_group")==0)
{
	if(verified==0)
{
	printf("login to use the resources\n");
	memset(str,'\0',100);
	continue;
}
int len,flag;
char *req=(char*)malloc(sizeof(char)*20);
char *grpid=(char*)malloc(sizeof(char)*20);
strcpy(req,token[0]);
strcpy(grpid,token[1]);
len=strlen(req);
send(sfd,&len,sizeof(len),0);
send(sfd,req,len+1,0);
len=strlen(grpid);
send(sfd,&len,sizeof(len),0);
send(sfd,grpid,len+1,0);
recv(sfd,&flag,sizeof(flag),0);
if(flag==0)
{
	printf("There is no such group\n");
}
if(flag==1)
{
    printf("done..\n");
}
if(flag==-1)
{
	printf("u r not part of grp..\n");
}

printf("done..will stop sharing if that file exists..\n");

free(grpid);
memset(str,'\0',100);
	continue;
}


if(strcmp(token[0],"upload_file")==0)
{
if(verified==0)
{
	printf("login to use the resources\n");
	memset(str,'\0',100);
	continue;
}
int len,flag;
char *req=(char*)malloc(sizeof(char)*20);
char *grpid=(char*)malloc(sizeof(char)*15);
char *filepath=(char*)malloc(sizeof(char)*60);
strcpy(req,token[0]);
strcpy(grpid,token[2]);
strcpy(filepath,token[1]);
len=strlen(req);
send(sfd,&len,sizeof(len),0);
send(sfd,req,len+1,0);
len=strlen(filepath);
send(sfd,&len,sizeof(len),0);
send(sfd,filepath,len+1,0);
len=strlen(grpid);
send(sfd,&len,sizeof(len),0);
send(sfd,grpid,len+1,0);
recv(sfd,&flag,sizeof(flag),0);
if(flag==-1)
{
printf("u are not part of group...u cant do this..\n");
}
else if(flag==0)
{
printf("There is no such grp..\n");
}
else
{
	printf("uploaded successfully..\n");
}
free(req);free(grpid);free(filepath);
continue;
}

if(strcmp(token[0],"download_file")==0)
{
	if(verified==0)
{
	printf("login to use the resources\n");
	memset(str,'\0',100);
	continue;
}
int len;int flag;
char *req=(char*)malloc(sizeof(char)*20);
char *grpid=(char*)malloc(sizeof(char)*15);
char *fname=(char*)malloc(sizeof(char)*60);
char *dpath=(char*)malloc(sizeof(char)*60);//this field need not be sent bcuse tracker sends ip and port to us and so we need to connect to that server and so this field is passed to our local file transer fun 
strcpy(req,token[0]);strcpy(grpid,token[1]);strcpy(fname,token[2]);
strcpy(dpath,token[3]);
len=strlen(req);
send(sfd,&len,sizeof(len),0);
send(sfd,req,len+1,0);
len=strlen(grpid);
send(sfd,&len,sizeof(len),0);
send(sfd,grpid,len+1,0);
len=strlen(fname);
send(sfd,&len,sizeof(len),0);
send(sfd,fname,len+1,0);
recv(sfd,&flag,sizeof(flag),0);
if(flag==-1)
{
printf("u are not part of group...u cant download the file..\n");
free(req);free(grpid);free(fname);free(dpath);
continue;
}
else if(flag==0)
{
printf("There is no such grp..\n");
free(req);free(grpid);free(fname);free(dpath);
continue;
}
struct fdet a[20];
int count=-1,more,fpres;
while(1)
{
recv(sfd,&more,sizeof(more),0);
if(more==0)
break;
count++;
char *ipaddress=(char*)malloc(sizeof(char)*20);
char *port=(char*)malloc(sizeof(char)*10);
recv(sfd,&len,sizeof(len),0);
recv(sfd,ipaddress,len+1,0);
recv(sfd,&len,sizeof(len),0);
recv(sfd,port,len+1,0);
a[count].ip=ipaddress;a[count].port=port;
}

recv(sfd,&fpres,sizeof(fpres),0);

if(count==-1)
{
	if(fpres==0)
	{
		printf("there is no such file in the group u specified\n");
		free(req);free(grpid);free(fname);free(dpath);
		continue;
	}
	else
	{
		printf("the peers which have d file are inactive..\n");
		free(req);free(grpid);free(fname);free(dpath);
		continue;	
	}
}

if(count>=0)
{
	printf("The info given by tracker...\n");
	for(int i=0;i<=count;i++)
	{
		printf("ip:%s port:%s\n",a[i].ip,a[i].port);
	}
	struct clnode *temp=(struct clnode *)malloc(sizeof(struct clnode)); 
	temp->ipad=a[0].ip;
	temp->portno=atoi(a[0].port);
	temp->fname=fname;
	temp->destiny=dpath;
	pthread_t p[100];
	pthread_create(&p[thdcount],NULL,clientfun,temp);
	thdcount++;
	int len,flag;
	strcpy(req,"upload_file");
	len=strlen(req);
	send(sfd,&len,sizeof(len),0);
	send(sfd,req,len+1,0);
	len=strlen(dpath);
	send(sfd,&len,sizeof(len),0);
	send(sfd,dpath,len+1,0);
	len=strlen(grpid);
	send(sfd,&len,sizeof(len),0);
	send(sfd,grpid,len+1,0);
	recv(sfd,&flag,sizeof(flag),0);
	printf("details uploaded successfully..\n");
}
free(req);free(grpid);
continue;
}

if(strcmp(token[0],"list_files")==0)
{
	if(verified==0)
{
	printf("login to use the resources\n");
	memset(str,'\0',100);
	continue;
}
int len;int flag,more,count=0;
char *req=(char*)malloc(sizeof(char)*20);
char *grpid=(char*)malloc(sizeof(char)*15);
strcpy(req,token[0]);strcpy(grpid,token[1]);
len=strlen(req);
send(sfd,&len,sizeof(len),0);
send(sfd,req,len+1,0);
len=strlen(grpid);
send(sfd,&len,sizeof(len),0);
send(sfd,grpid,len+1,0);
recv(sfd,&flag,sizeof(flag),0);
if(flag==0)
{
	free(req);free(grpid);
	printf("no files are uploaded ..\n");
	continue;
}
char *filepath=(char*)malloc(sizeof(char)*60);
if(flag==1)
{
while(1)
{
recv(sfd,&more,sizeof(more),0);
if(more==0)
	break;
count++;
recv(sfd,&len,sizeof(len),0);
recv(sfd,filepath,len+1,0);
//printf("%s\n",filepath);
memset(filepath,'\0',60);
}
}
if(flag==1 && count==0)
printf("no files uploaded under d specified grpid..\n");
else
printf("done\n");


free(req);free(grpid);free(filepath);
}

if(strcmp(token[0],"stop_share")==0)
{
	if(verified==0)
{
	printf("login to use the resources\n");
	memset(str,'\0',100);
	continue;
}
int len;int flag;
char *req=(char*)malloc(sizeof(char)*20);
char *grpid=(char*)malloc(sizeof(char)*15);
char *fname=(char*)malloc(sizeof(char)*50);
strcpy(req,token[0]);strcpy(grpid,token[1]);strcpy(fname,token[2]);
len=strlen(req);
send(sfd,&len,sizeof(len),0);
send(sfd,req,len+1,0);
len=strlen(grpid);
send(sfd,&len,sizeof(len),0);
send(sfd,grpid,len+1,0);
len=strlen(fname);
send(sfd,&len,sizeof(len),0);
send(sfd,fname,len+1,0);
recv(sfd,&flag,sizeof(flag),0);
if(flag==-1)
{
	free(grpid);free(fname);free(req);
	printf("u r not part of d grp...\n");
	continue;
}

if(flag==0)
{
	free(grpid);free(fname);free(req);
	printf("there is no such group..\n");
	continue;
}

if(flag==1)
{
	free(grpid);free(fname);free(req);
	printf("there are no file uploads at all..\n");
	continue;
}

printf("will stop sharing if file is uploade by u.....\n");
continue;
}
}
return 0;
}





void *clientfun(void *t)
{
	//printf("hii\n");
struct clnode *u=(struct clnode*)t;
char buff[512000];
memset(buff,'\0',512000);
int pno=u->portno;
char *ipaddress=u->ipad;
char *flname=u->fname;
char *destiny=(char *)malloc(sizeof(char)*30);
strcpy(destiny,u->destiny);

//printf("%s %d %s %s\n",ipaddress,pno,flname,destiny);
int o,y;
int sfd=socket(AF_INET,SOCK_STREAM,0);
if(sfd==-1)
{
printf("unable to create a socket\n");
pthread_exit(NULL);
}
struct sockaddr_in add;
add.sin_family=AF_INET;
add.sin_port=htons(pno);
add.sin_addr.s_addr=inet_addr(ipaddress);
    if (connect(sfd,(struct sockaddr *)&add, sizeof(add)) < 0) 
    { 
        perror("Connection Failed\n"); 
        pthread_exit(NULL);
    } 
    char name[100];
    strcpy(name,flname);
	send(sfd,name,strlen(name)+1,0);
	
    char flag;
recv(sfd,&flag,sizeof(flag),0);

if(flag=='a')
{
	printf("no such file\n");
pthread_exit(NULL);
}
else
{
int nchunks;
recv(sfd,&nchunks,sizeof(nchunks),0);//receiving no of chunks
//printf("chunks:%d\n",nchunks);
for(int i=0;i<nchunks;i++)
{
//printf("%d\n",i);
int fsize;
recv(sfd,&fsize,sizeof(fsize),0);
//printf("fsize:%d\n",fsize);
int h=open(destiny,O_WRONLY|O_CREAT|O_APPEND,00777);
int k;
while( (k=read(sfd,buff,fsize)) && fsize>0)
{ 
fsize=fsize-k;
//printf("k:%d,fsize:%d\n",k,fsize);
write(h,buff,k);
memset(buff,'\0',512000);
if(fsize==0)
break;
}
close(h);
//printf("received\n");
}
}
close(sfd);
}




void *serverfun(void*t)
{
struct senode *temp=(struct senode *)t;

int pno=temp->portno,q,sfd,sfd1;
//size=524288;
sfd=socket(AF_INET,SOCK_STREAM,0);
if(sfd<0)
{
printf("unable to create a socket\n");
exit(1);
}
struct sockaddr_in a;
int adrlength=sizeof(a);
a.sin_family=AF_INET;
a.sin_addr.s_addr=inet_addr(temp->ipad);
a.sin_port=htons(pno);
q=bind(sfd,(struct sockaddr*)&a,adrlength);
if(q<0)
{
printf("unable to bind to the specified port\n");
exit(1);
}
q=listen(sfd,maxcon);
if(q<0)
{
printf("unable to listen\n");
exit(1);
}
pthread_t threads[maxcon];
int i=1;
char flag;
while((sfd1=accept(sfd,(struct sockaddr*)&a,(socklen_t*)&adrlength))>=0)
{
i++;
pthread_create(&threads[i],NULL,filetransfer,&sfd1);
}
if(sfd1<0)
{
printf("pblm\n");
}
}











void *filetransfer(void *t)
{
	int *z=(int*)t;
	int sfd1=*z;
	int size=512000;
char *arr=(char *)malloc(sizeof(char)*2000);
read(sfd1,arr,50);
int fd=open(arr,O_RDONLY);
char flag='p';
if(fd<0)
{
flag='a';
send(sfd1,&flag,sizeof(flag),0);
close(sfd1);
pthread_exit(NULL);
}
else
{
send(sfd1,&flag,sizeof(flag),0);
char buff[512000];
int k,a;
int q=lseek(fd,0,SEEK_END);
lseek(fd,0,SEEK_SET);
int tot=q/size;
if(q%size!=0)
tot+=1;
int s2=q%size;
char msg[size];
//printf("hurray\n");
send(sfd1,&tot,sizeof(tot),0);//sending chunks info
for(int i=0;i<tot;i++)
{
memset(msg,'\0',512000);
a=size;
if(i==tot-1)
a=q%size;
int p=0;
while(a>0 && (k=read(fd,msg+p,a)))
{
//printf("%d bytes read\n",k);
a=a-k;
p=p+k;
}
//printf("%d bytes sending\n",p);
if(i==tot-1)
{
send(sfd1,&s2,sizeof(s2),0);
send(sfd1,msg,s2,0);
}
else
{
send(sfd1,&size,sizeof(size),0);
send(sfd1,msg,size,0);
}
//printf("sent %d chunk\n",i);
}
close(sfd1);
pthread_exit(NULL);
}
}







