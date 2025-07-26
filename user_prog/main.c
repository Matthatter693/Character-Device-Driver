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
            break;

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
                break;
            case 'h':
                printf("not planned yet\n");
                close(fd);
                break;
            case '?':
                printf("write correct arguments :(\n");
                close(fd);
                break;
            default:
                printf("Please provide some options\n");
                close(fd);
                exit(0);
        }
    }
}
