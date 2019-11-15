#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>

#define N 50
#define N_SRC 4*N+1 
#define N_DST 4*N+1 


#define pas_clear_string(dst_str)          (dst_str)[0] = 0
#define pas_copy_string(dst_str, src_str)  strcpy ((dst_str), (src_str))
#define pas_get_string_length(str)         strlen ((str))
#define pas_get_string_char(str, n)        (str)[(n)]
#define pas_append_char_to_string(str, c)  { int l = strlen((str)); (str)[l] = (c); (str)[l+1]=0; }


#include "info.h"

void perror_and_exit(char *s, int exit_code)
{
	perror(s);
	exit(exit_code);
}



static int
char_to_int (char c)
{
  int res;

  switch (c)
    {
      case '0': res = 0; break;
      case '1': res = 1; break;
      case '2': res = 2; break;
      case '3': res = 3; break;
      case '4': res = 4; break;
      case '5': res = 5; break;
      case '6': res = 6; break;
      case '7': res = 7; break;
      case '8': res = 8; break;
      case '9': res = 9; break;
      case 'A':
      case 'a': res = 10; break;
      case 'B':
      case 'b': res = 11; break;
      case 'C':
      case 'c': res = 12; break;
      case 'D':
      case 'd': res = 13; break;
      case 'E':
      case 'e': res = 14; break;
      case 'F':
      case 'f': res = 15; break;
      case '.': res = -1; break;
      default:
        abort();
    }

  return res;
}
static char
int_to_char (int n)
{
  char res;

  switch (n)
    {
      case 0: res = '0'; break;
      case 1: res = '1'; break;
      case 2: res = '2'; break;
      case 3: res = '3'; break;
      case 4: res = '4'; break;
      case 5: res = '5'; break;
      case 6: res = '6'; break;
      case 7: res = '7'; break;
      case 8: res = '8'; break;
      case 9: res = '9'; break;
      case 10: res = 'A'; break;
      case 11: res = 'B'; break;
      case 12: res = 'C'; break;
      case 13: res = 'D'; break;
      case 14: res = 'E'; break;
      case 15: res = 'F'; break;
      default:
        abort();
    }

  return res;
}

static void
divide_string (int radix,             
               char *divident_str,    
               int divisor,           
               char *res_quotient_str, 
               int *res_reminder_p)    
{
  int divident_len; 
  int divident_ind; 
  int cur_divident; 
  int cur_quotient; 
  int cur_reminder; 
  int int_val;   
  char char_val;   
  int flag_first;


  pas_clear_string (res_quotient_str);

  divident_len = pas_get_string_length (divident_str);
  divident_ind = 0;

  cur_divident = 0;

  do
    {
      flag_first = 0;
      while (cur_divident < divisor && divident_ind < divident_len)
        {
          char_val = pas_get_string_char (divident_str, divident_ind);
          divident_ind++;
          int_val = char_to_int (char_val);
          if (int_val == -1)
          {
            continue;
          }
          cur_divident = cur_divident * radix + int_val;
          if (flag_first == 1 && pas_get_string_length( res_quotient_str) > 0)
            {
              char_val = int_to_char (0);
              if (char_val = "")
              {
                continue;
              }
              pas_append_char_to_string (res_quotient_str, char_val);
            }

          flag_first = 1;
        }
      cur_quotient = cur_divident / divisor;
      cur_reminder = cur_divident % divisor;

      int_val = cur_quotient;
      char_val = int_to_char (int_val);
      pas_append_char_to_string (res_quotient_str, char_val);

      cur_divident = cur_reminder;
    }
  while (divident_ind < divident_len);

  *res_reminder_p = cur_reminder;

}

static int
is_number_zero (char *str)
{
  if (pas_get_string_length (str) == 1
      && pas_get_string_char (str, 0) == '0')
    return 1;

  return 0;
}

static void
conv_dec_to_bin (char *src_str,
                 int src_radix,
                 char *res_str,
                 int res_radix)
{
  char divident_str[N_SRC];
  char quotient_str[N_SRC];
  int reminder;
  int digits[N_DST];  
  int digit_ind;          
  int int_val;             
  char char_val;            
  int i;

  digit_ind = 0;

  pas_copy_string (divident_str, src_str);

  do
    {
      divide_string (src_radix, divident_str, res_radix, quotient_str, &reminder);

      digits[digit_ind] = reminder;
      digit_ind++;

      pas_copy_string (divident_str, quotient_str);
    }
  while (! is_number_zero (quotient_str));

  pas_clear_string (res_str);
  for (i = digit_ind - 1; i >=0 ; i--)
    {
      int_val = digits[i];
      char_val = int_to_char (int_val);
      pas_append_char_to_string (res_str, char_val);
    }
}

int main()
{
    int sock, listener;
    struct sockaddr_in addr, client_addr;
    char buf[MSG_LEN];
    char old_buf[MSG_LEN];
    char new_buf[MSG_LEN];
    int bytes_read;

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
        perror_and_exit("socket()", 1);
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SOCK_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        perror_and_exit("bind()", 2);

    listen(listener, 1);
    printf("Server is listening...\n");
    
    while(1)
    {
        socklen_t cli_addr_size = sizeof(client_addr);
        sock = accept(listener, (struct sockaddr*) &client_addr, &cli_addr_size);
        if(sock < 0)
            perror_and_exit("accept()", 3);


        printf("Received packet from %s:%d\n\n", 
                        inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        while(1)
        {
            bytes_read = recv(sock, buf, MSG_LEN, 0);
            if (strlen(buf) == 1 || strlen(buf) == 2)
            {
              if (strlen(buf) == 1)
                printf("Received index: %s\n", buf);
              else
              {
                FILE *fp;
                char name[] = "ip_adresses.txt";
                if ((fp = fopen(name, "r")) == NULL)
                {
                    return 0;
                }
                char cc[256] = {};
                char buff[1000] = {};
                while ((fgets(cc, 256, fp))!=NULL)
                {
                  strcat(buff, cc);
                }
                send(sock, buff, sizeof(buff), 0);
              }
            }
            if(bytes_read <= 0) break;

            FILE *fp;
            char name[] = "ip_adresses.txt";
            if ((fp = fopen(name, "r")) == NULL)
            {
                return 0;
            }
            int index = atoi(buf);
            int i = 0;
            while (1)
            {

                char *estr;
                estr = fgets (buf,sizeof(buf),fp);
                if (estr == NULL)
                {
                     if ( feof (fp) != 0)
                     {  
                        
                        break;
                     }
                     else
                     {
                        
                        break;
                     }
                }
                if (i == index)
                {
                    break;
                }

                strcpy(old_buf, buf);
                i++;
            }

            int main_num[3];
            int k = 0;
            int j = 0;
            int nums[3];
            printf("The main address: %s\n", old_buf);
            old_buf[strlen(old_buf)] = '.';
            for (int i = 0; i < strlen(old_buf); i++)
            {
                if (old_buf[i] != '.' && old_buf[i] != '\0')
                {
                    nums[j] = old_buf[i] - '0';
                    j++;
                }
                else
                {
                    if (j == 1)
                      main_num[k] = nums[0];
                    else if (j == 2)
                      main_num[k] = nums[0] * 10 + nums[1];
                    else if (j == 3)
                      main_num[k] = nums[0] * 100 + nums[1] * 10 + nums[2];

                    k++;
                    j = 0;
                }
            }
            char buf[50];
            char buf1[50];
            char buf2[50];
            char buf3[50];
            char buf4[50];
            char main_buf[100];
            char main_buf1[100];
            char main_buf2[100];
            char main_buf3[100];
            sprintf(buf,"%d", main_num[0]);
            conv_dec_to_bin(buf, 10, buf1, 16);
            sprintf(buf,"%d", main_num[1]);
            conv_dec_to_bin(buf, 10, buf2, 16);
            sprintf(buf,"%d", main_num[2]);
            conv_dec_to_bin(buf, 10, buf3, 16);
            sprintf(buf,"%d", main_num[3]);
            conv_dec_to_bin(buf, 10, buf4, 16);
            sprintf(main_buf1, "%s.%s.%s.%s", buf1, buf2, buf3, buf4);

            sprintf(buf,"%d", main_num[0]);
            conv_dec_to_bin(buf, 10, buf1, 8);
            sprintf(buf,"%d", main_num[1]);
            conv_dec_to_bin(buf, 10, buf2, 8);
            sprintf(buf,"%d", main_num[2]);
            conv_dec_to_bin(buf, 10, buf3, 8);
            sprintf(buf,"%d", main_num[3]);
            conv_dec_to_bin(buf, 10, buf4, 8);
            sprintf(main_buf2, "%s.%s.%s.%s", buf1, buf2, buf3, buf4);


            sprintf(buf,"%d", main_num[0]);
            conv_dec_to_bin(buf, 10, buf1, 2);
            sprintf(buf,"%d", main_num[1]);
            conv_dec_to_bin(buf, 10, buf2, 2);
            sprintf(buf,"%d", main_num[2]);
            conv_dec_to_bin(buf, 10, buf3, 2);
            sprintf(buf,"%d", main_num[3]);
            conv_dec_to_bin(buf, 10, buf4, 2);
            sprintf(main_buf3, "%s.%s.%s.%s", buf1, buf2, buf3, buf4);
            sprintf(main_buf, "%s\n%s\n%s\n", main_buf1, main_buf2, main_buf3);
            send(sock, main_buf, sizeof(main_buf), 0);
        }
    
        close(sock);
    }
    
    return 0;
}
