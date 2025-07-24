#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

int main()
{
int fd;
char p[]="Hiii buddy!";
char r[]="how are you?";
char q[100];

printf("str1:%s\n",p);
printf("str2:%s\n",r);

fd=open("/dev/cbuf-0",O_RDWR);
if(fd<0)
{
    perror("failed to open the device");
    return 0;
}

if(write(fd,p,strlen(p)+1)<0)
{
    perror("failed to  write");
    close(fd);
    return 1;
}

printf("message sent\n");

if(write(fd,r,strlen(r)+1)<0)
{
    perror("failed to  write");
    close(fd);
    return 1;
}

printf("message sent\n");

if(read(fd,q,sizeof(q))<0)
{
    perror("failed to  write");
    close(fd);
    return 1;
}

printf("Read from device:%s\n",q);

if(read(fd,q,sizeof(q))<0)
{
    perror("failed to  write");
    close(fd);
    return 1;
}
printf("Read from device:%s\n",q);
close(fd);

}
