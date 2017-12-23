#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>   // for sleep()
//#include <stdlib.h>   // for atoi()


#define DEV_DRIVER_NAME "/dev/myfsdriver"
//#include "../common/fdriver_ioctl.h"
#include "fdriver_ioctl.h"

#define atoi fatox64

#define BUFFER_LENGTH 1024
int main(int argc, char ** argv);
uint64_t fatox64(char * string);

#define ZERO(x) {x.function=0xDEADBEEFF00DCAFE;x.address=0xCAB0CAB1CAB2CAB3;x.value=0xFECDBA9876543210;}

FILE * g_kFile = NULL;

void print_usage(void)
{
   printf("Main - Test Eye \n");
   printf("-p <serialport>   EG: /dev/ttyS0  (default)\n");
   printf("-i                Interactive (Terminal Server)  \n");
   printf("-v                Verbose Flag  \n");
   printf("-d <0xDebugFlags> Debug Flags  \n");
   printf("-m                CLI menu  \n");

   printf("example: \n");
   printf("./fmain  -p /dev/ttyS0 -t test  -p9090 -v \n");
   printf("./fmain  -m \n");
   printf("\n");
}

int DoRead(void)
{
    int RdCount= 0;
    int BuffLen= BUFFER_LENGTH;
    char Buff[BUFFER_LENGTH];
    int i;

           RdCount = fread( Buff,1,BuffLen,g_kFile);

           printf(" read count %d\n",RdCount);
           for (i = 0 ; i < RdCount ; i++)
           {
              if( i%0x10 == 0)
              {
                 printf("\n%3d ",i);
              }
//            printf(" %02x",*(Buff+i));
              if( ((*(Buff+i)) >= 0x20) && ((*(Buff+i)) < 0x7f) )
                   printf("%c",*(Buff+i));
              else
                   printf(".");
           }
           printf("\n");
    return 0;

}

int DoWrite(void)
{
    char String[]= "fdriver test code - example";
    int WrCount = 0;

    printf(" execute the fwrite function\n");
    WrCount = fwrite(String,1,strlen(String),g_kFile);
    printf(" Bytes Written:  count= %d\n",WrCount);

    return 0;
}



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
void doMenuMenu(void)
{
printf("\n");
printf("q                 - quit program \n");
printf("ic                - ioctl -clear \n");
printf("ig                - ioctl -get  \n");
printf("is                - ioctl -set  \n");
printf("r                 - read file \n");
printf("rc <addr>         - read csr \n");
printf("s <filename>      - run a script file \n");
printf("w                 - write file \n");
printf("wc <addr> <value> - read csr \n");
printf("T                 - Toggle GPIO 0 pin \n");
printf("\n");
}

uint64_t  csr_read( uint64_t address)
{
    fdrive_arg_t  d;
    int result; 
    d.address  = address;
    result = ioctl( fileno( g_kFile) , FDRIVER_CSR_64_RD,&d);
    if(result != 0)
    {
         printf("IOCTL Error cannot READ CSR:%d \n",result);
    }
//  printf(" RD  %lx %016lx\n",data.address, d.value);
//              printf(" data.function 0x%lx\n",data.function);
//              printf(" data.address  0x%lx\n",data.address);
//              printf(" data.value    0x%lx\n",data.value);
 
   return d.value;
}

uint64_t  csr_write(uint64_t address,uint64_t data)
{
     fdrive_arg_t  d;
     int result = 0; 
     d.address = address;
     d.value   = data;

//     printf("%d:%s-%s address 0x%lx  data 0x%lx\n",__LINE__,__FILE__,__FUNCTION__,address,data);

     result = ioctl( fileno( g_kFile) , FDRIVER_CSR_64_WR,&d);
     if(result != 0)
     {
          printf("IOCTL Error cannot write CSR %d\n",result);
     }
//         printf(" WR  %lx %016lx\n",data.address, d.value);
//              printf(" data.function 0x%lx\n",data.function);
//              printf(" data.address  0x%lx\n",data.address);
//              printf(" data.value    0x%lx\n",data.value);
     return result;
}
 

typedef struct reg_string_struct {
   uint64_t reg;
   char     string[80];
 } reg_string_t;

reg_string_t  g_RegSt[]= {
   {0x803000000000, "GPIO_RX_DAT"},
   {0x803000000008, "GPIO_TX_SET"},
   {0x803000000010, "GPIO_TX_CLEAR"},
   {0x803000000018, "GPIO_MULTI_CAST"},
   {0x803000000020, "GPIO_OCLA_EXTEN_TRIG"},
   {0x803000000028, "GPIO_STRAP"},
   {0x803000000040, "GPIO_CLK_GEN0"},
   {0x803000000048, "GPIO_CLK_GEN1"},
   {0x803000000050, "GPIO_CLK_GEN2"},
   {0x803000000058, "GPIO_CLK_GEN3"},
   {0x803000000060, "GPIO_CLK_SYNC0"},
   {0x803000000068, "GPIO_CLK_SYNC1"},
   {0x803000000080, "GPIO_COMP"},
   {0x803000000090, "GPIO_CONST"},
   { 0x0            , ""},  };

  



#define ARG_SIZE   64    // max size of an Individual ARGC
#define MAX_ARGS   5    // max number of Command Line Arguments.

int g_argc = 0;
char g_arg0[ARG_SIZE];
char g_arg1[ARG_SIZE];
char g_arg2[ARG_SIZE];
char g_arg3[ARG_SIZE];
char g_arg4[ARG_SIZE];

char g_MenuPrompt[] ={"Yes?> "};

#define CMD_BUFFER_SIZE 256
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
void DoMenu(void)
{
int d = 0;
char CmdLine[CMD_BUFFER_SIZE] = { 0 };
//int  GetLineResults ;
int Cmd;
int result;
char *ptr;
fdrive_arg_t  data;
int i;
int flag;
uint64_t reg;

   data.function  = 10;
   data.address = 34;
   data.value     = 20;

#define FILE_STRING_LENGTH 64


    doMenuMenu();

    while (d == 0)
    {
       printf("%s",g_MenuPrompt);
       ptr = fgets(CmdLine,CMD_BUFFER_SIZE,stdin);
       if (ptr == NULL)
       {
          printf("fgets is having a problem\n");
       }
       g_arg0[0] = 0x00;  // if g_argc == 0 we will run last cmd. - stop this behavior
       g_argc = sscanf(CmdLine,"%s%s%s%s%s",g_arg0,g_arg1,g_arg2,g_arg3,g_arg4);

       Cmd = g_arg0[0];
       switch(Cmd) {

       case 'c':
       case 'C':
           printf("Canned Test \n");
//           doStatistics();

           // do script file
           break;

       case 'i':
       case 'I':
           switch(g_arg0[1]){
           default:
           case 'g':
           case 'G':

               printf("ioctl Get Cmd \n");

               result = ioctl( fileno( g_kFile) , FDRIVER_GET_VARIABLES,&data);
               if(result != 0)
               {
                  printf("IOCTL Error cannot get variables\n");
               }
               else
               {
                   printf(" data.function 0x%lx\n",data.function);
                   printf(" data.address  0x%lx\n",data.address);
                   printf(" data.value    0x%lx\n",data.value);
               }
              break;
           case 'c':
           case 'C':

              printf("ioctl Clear Cmd \n");
//            ioctl( file, function, void * data);
              result = ioctl( fileno( g_kFile) , FDRIVER_CLR_VARIABLES,NULL);
              if(result != 0)
              {
                  printf("IOCTL Error cannot clear variables\n");
              }
              break;
           case 's':
           case 'S':
              printf("ioctl Set Cmd \n");
              if(g_argc > 1) data.function  = atoi (g_arg1);
              if(g_argc > 2) data.address = atoi (g_arg2);
              if(g_argc > 3) data.value     = atoi (g_arg3);

              result = ioctl( fileno( g_kFile) , FDRIVER_SET_VARIABLES,&data);
              if(result != 0)
              {
                  printf("IOCTL Error cannot set variables\n");
              }
              break;
#if 0
           case 'x':
           case 'X':
              printf("ioctl xxx Cmd \n");
              if(g_argc > 1) data.function  = atoi (g_arg1);
              if(g_argc > 2) data.address   = atoi (g_arg2);
              if(g_argc > 3) data.value     = atoi (g_arg3);

              result = ioctl( fileno( g_kFile) , FDRIVER_READ_IO,&data);
              if(result != 0)
              {
                  printf("IOCTL Error cannot set variables\n");
              }
              break;
#endif
           } // end switch on second char
           break;

       case 'q':
       case 'Q':
           d = 1;
           break;

       case 'r':
       case 'R':
           switch(g_arg0[1]){
           case 'c':
           case 'C':
              printf("ioctl Read CSR <hex-address>\n");
              ZERO(data);
              if(g_argc > 1)
              {
                  data.address  = fatox64 (g_arg1);
              }   // end address is present
              if( data.address != 0)
              {
                  result = ioctl( fileno( g_kFile) , FDRIVER_CSR_64_RD,&data);
                  if(result != 0)
                  {
                      printf("IOCTL Error cannot read CSR\n");
                  }
                  else
                  {
                      printf(" data.function 0x%lx\n",data.function);
                      printf(" data.address  0x%lx\n",data.address);
                      printf(" data.value    0x%lx\n",data.value);
                  }
              } // end address not zero
              else
              {
                  printf(" bad format  rc <hex-address>  \n");
              }
              break;
           default:
           case 'm':
           case 'M':
               printf("Read File  \n");
               DoRead();
               break;

           } // end switch on second char
           break;
       case 's':
       case 'S':
           printf("Read GPIO Registers \n");
  
           i = 0;
           while ( g_RegSt[i].reg != 0)
           {
               printf("   %20s  %lx %016lx\n",g_RegSt[i].string
                             ,g_RegSt[i].reg, 
                             csr_read( g_RegSt[i].reg ));
               i++;
           }

#if 0
//         check if more than 64 IO signal 
           if ( (reg & 0xff) > 64)
           { 
               data.address  = 0x803000001418;
              printf("   GPIO_STRAP1  %lx %016lx\n",data.address, csr_read(data.address ));
           }
#endif
 
           break;

// GPIO BIT:  9 8 7 6 5 4 3 2 1 0
// ------------------------------
// cfg addr   4 4 4 4 4 4 4 4 4 4  
//            4 4 3 3 2 2 1 1 0 0  
//            8 0 8 0 8 0 8 0 8 0
//
// biti       2 1 8 4 2 1 8 4 2 1           
//            2   1       0
#define GPIO_RX       0x803000000000
#define GPIO_TX_SET   0x803000000008
#define GPIO_TX_CLR   0x803000000010
#define GPIO_CFG(x)  (0x803000000400 + (8 * x))
#define LED   5
       case 't':
       case 'T':
           printf("Do Script File : ToggleLED \n");
           flag = 20;
           i = 0;
// set GPIO to output
           csr_write(GPIO_CFG(LED),1);

     // read it back
           reg  = GPIO_CFG(LED);
           if( flag > 0) {flag--; printf("%d RD %lx %016lx\n",i,reg,csr_read( reg ));}
// loop
           for( i = 0 ; i < 1000; i++)
           {
//    Set the Pin High
               csr_write( GPIO_TX_SET , (0x01 << LED));
               // read it back
               reg  = GPIO_TX_SET;
               if( flag > 0) {flag--; printf("%d RD %lx %016lx\n",i,reg,csr_read( reg ));}
               reg  = GPIO_RX;
               if( flag > 0) {flag--; printf("%d RD %lx %016lx\n",i,reg,csr_read( reg ));}
// SLEEP
               sleep(1);
//    Set the Pin Low
               csr_write( GPIO_TX_CLR , (0x01 << LED));
               // read it back
               reg  = GPIO_TX_CLR;
               if( flag > 0) {flag--; printf("%d RD %lx %016lx\n",i,reg,csr_read( reg ));}
               reg  = GPIO_RX;
               if( flag > 0) {flag--; printf("%d RD %lx %016lx\n",i,reg,csr_read( reg ));}
// SLEEP
               sleep(1);
//               printf("\r1000 ->%d",i);
           }

           break;

       case 'W':
       case 'w':
           switch(g_arg0[1]){
           case 'c':
           case 'C':
              printf("ioctl Write CSR <hex-address> <hex-value>\n");
              ZERO(data);
              if(g_argc > 2)
              {
                  data.address  = fatox64 (g_arg1);
                  data.value    = fatox64 (g_arg2);
              }   // end address is present
              if( data.address != 0)
              {
                  result = ioctl( fileno( g_kFile) , FDRIVER_CSR_64_WR,&data);
                  if(result != 0)
                  {
                      printf("IOCTL Error cannot read CSR\n");
                  }
                  else
                  {
                      printf(" data.function 0x%lx\n",data.function);
                      printf(" data.address  0x%lx\n",data.address);
                      printf(" data.value    0x%lx\n",data.value);
                  }
              } // end address not zero
              else
              {
                  printf(" bad format  wc <hex-address> <hex-value>  \n");
              }
              break;
           default:
           case 'm':
           case 'M':
               printf("Write File:\n");
               DoWrite();
               break;

           } // end switch on second char

          break;

       default:
           doMenuMenu();
           break;
       } // end switch
    }  // end while

}





int main(int argc, char ** argv)
{
    char DeviceName[]={ DEV_DRIVER_NAME };

    printf("Test attaching to the %s\n",DeviceName);

    g_kFile = fopen( DeviceName,"w+");

    if(g_kFile == NULL)
    {
        printf("Failed to open: %s  %p \n",DeviceName,g_kFile);
       return -1;
    }

    DoMenu();

    if (g_kFile != NULL)
    {
       fclose(g_kFile);
    }
    return argc;
}


#if 0
//  http://www.linuxforu.com/2011/08/io-control-in-linux/


#include "query_ioctl.h"

void get_vars(int fd)
{
    query_arg_t q;

    if (ioctl(fd, QUERY_GET_VARIABLES, &q) == -1)
    {
        perror("query_apps ioctl get");
    }
    else
    {
        printf("Status : %d\n", q.status);
        printf("Dignity: %d\n", q.dignity);
        printf("Ego    : %d\n", q.ego);
    }
}
void clr_vars(int fd)
{
    if (ioctl(fd, QUERY_CLR_VARIABLES) == -1)
    {
        perror("query_apps ioctl clr");
    }
}
void set_vars(int fd)
{
    int v;
    query_arg_t q;

    printf("Enter Status: ");
    scanf("%d", &v);
    getchar();
    q.status = v;
    printf("Enter Dignity: ");
    scanf("%d", &v);
    getchar();
    q.dignity = v;
    printf("Enter Ego: ");
    scanf("%d", &v);
    getchar();
    q.ego = v;

    if (ioctl(fd, QUERY_SET_VARIABLES, &q) == -1)
    {
        perror("query_apps ioctl set");
    }
}

int main(int argc, char *argv[])
{
    char *file_name = "/dev/query";
    int fd;
    enum
    {
        e_get,
        e_clr,
        e_set
    } option;

    if (argc == 1)
    {
        option = e_get;
    }
    else if (argc == 2)
    {
        if (strcmp(argv[1], "-g") == 0)
        {
            option = e_get;
        }
        else if (strcmp(argv[1], "-c") == 0)
        {
            option = e_clr;
        }
        else if (strcmp(argv[1], "-s") == 0)
        {
            option = e_set;
        }
        else
        {
            fprintf(stderr, "Usage: %s [-g | -c | -s]\n", argv[0]);
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Usage: %s [-g | -c | -s]\n", argv[0]);
        return 1;
    }
    fd = open(file_name, O_RDWR);
    if (fd == -1)
    {
        perror("query_apps open");
        return 2;
    }

    switch (option)
    {
        case e_get:
            get_vars(fd);
            break;
        case e_clr:
            clr_vars(fd);
            break;
        case e_set:
            set_vars(fd);
            break;
        default:
            break;
    }

    close (fd);

    return 0;
}
#endif

uint64_t fatox64(char * string)
{
    uint64_t result = 0;
    int64_t i = 0;
    int64_t done = 0;
    uint8_t in;
    int64_t length = (int32_t)strlen(string);

    while( (done == 0) && (i < length))
    {
        in = *(string+i);
        if( in == 0x00)
        {
            done = 1;
        }
        else
        {
            if (( in >= '0') &&( in <= '9'))
            {
                result <<= 4;
                result += in & 0x0f;
            }
            else if( (( in >= 'a') &&( in <= 'f')) ||
                (( in >= 'A') &&( in <= 'F')) )
            {
                result <<= 4;
                result += ((in & 0x0f) + 9);
            }
            else if( ( in == 'x') || ( in == 'X'))
            {

            }
            else
            {
                done = 1; // unrecognized character
            }
            i++;
        }
    }
    return result;
}

