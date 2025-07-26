#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<getopt.h>


//Buffer sizes for Tx and Rx
#define RBUF 100
#define TBUF 100

//flag for verbose args
static int verbose_flag;

int main(int argc,char *argv[])
{
    int opt;
    int fd;
    char tx[TBUF];
    char rx[RBUF];

    //opening file descriptor
    fd=open("/dev/cbuf-0",O_RDWR);
    if(fd<0)
    {
        perror("failed to open the device");
        return 0;
    }
    perror("open");
    

    while(1)
    {
        static struct option long_options[]=
        {
            {"verbose",no_argument,&verbose_flag,1},
            {"brief",no_argument,&verbose_flag,0},
            {"write",required_argument,0,'w'},
            {"read",no_argument,0,'r'},
            {"help",no_argument,0,'h'},
            {0,0,0,0}
        };

        int option_index=0;
        opt=getopt_long(argc,argv,"rhw:",long_options,&option_index);
        if(opt==-1)
        {
            printf("Wrong usage! run \"%s --help\" for more info\n",argv[0]);
            break;
        }

        switch(opt)
        {
            case 0:
                if(long_options[option_index].flag!=0)
                    break;
                break;
            case 'r': 
                if(read(fd,rx,sizeof(rx))<0)
                {
                    perror("failed to read");
                    close(fd);
                    return 1;
                }
                printf("Read from device:%s\n",rx);
                close(fd);
                return 0;
                break;
            case 'w':
                strcpy(tx,optarg);
                if(write(fd,tx,strlen(tx)+1)<0)
                {
                    perror("failed to write");
                    close(fd);
                    return 1;
                }
                if(verbose_flag)
                    printf("This works dawg\n");
                printf("Message written successfully\n");
                close(fd);
                return 0;
                break;
            case 'h':
                printf("Kernel Ring buffer CLI\n");
                printf("*******************************************\n");
                printf("To write message:%s --write message (or) %s -w message\n",argv[0],argv[0]);
                printf("To read message:%s --read (or) %s -r\n",argv[0],argv[0]);
                printf("To help:%s --help (or) %s -h\n",argv[0],argv[0]);
                printf("*******************************************\n");
                close(fd);
                return 0;
                break;
            case '?':
                printf("write correct arguments :(\n");
                close(fd);
                return 0;
                break;
            default:
                printf("does this work?\n");
                close(fd);
                return 0;
                exit(0);
        }
    }
}
