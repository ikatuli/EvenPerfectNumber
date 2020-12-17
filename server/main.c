#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h> 
#include <netdb.h>
#include <memory.h>
#include <errno.h>
#include <math.h>
#include <gmp.h>

char * luk (unsigned int number) //Тест Люка-Лемера
{
	if (number==2) return "6"; //Тест Люка-Лемера не работает с двойкой.ф
	mpz_t L,tmp,mersenne;
	mpz_init_set_ui(L,4);
	mpz_init (tmp);	

	//mersenne pow(2, q) - 1;
	mpz_init_set_ui(mersenne,2);
	mpz_pow_ui(mersenne,mersenne,number);//pow(2, q)
	mpz_sub_ui(mersenne,mersenne,1);//pow(2, q) - 1

	for (unsigned long long  i = 1; i <= number-2; i++) //Проверка
	{
		mpz_mul (tmp,L,L);//L*L
		mpz_sub_ui(tmp,tmp,2);//L*L-2
		mpz_mod(L,tmp,mersenne);//(L*L-2) %mersenne
	}

	//char * mpz_get_str(char *str, intbase, mpztop)

	//gmp_printf("Test: %Zd \n",L);

	if (mpz_cmp_ui(L,0)!=0) return "0";
	else 
	{
		mpz_t resolte;// 1ull << (number-1))*mersenne
		mpz_init_set_ui(resolte,2);
		mpz_pow_ui(resolte,resolte,number-1);
		mpz_mul(resolte,resolte,mersenne);
		//gmp_printf("Число: %Zd \n",resolte);
		//printf("str: %s",mpz_get_str(NULL,10,resolte));
		return mpz_get_str(NULL,10,resolte) ; //Если тест прошёл, то вычисляем положительное совершеное число.
	}
}

int prime(unsigned int n) //Проверка на простоту
{
	for(unsigned int i=2;i<=sqrt(n);i++) if(n%i==0)	return 0;
	return 1;
}

int main(int argc , char **argv[])
{
	int port=18666;
	if (argc == 2) port=atoi(argv[1]); // Получаем параметр строки 
	
	// Создание сокета
	int socket_desc;
	socket_desc = socket(AF_INET , SOCK_STREAM , 0); //ipv4 
	
	if(socket_desc < 0)
      {
              perror("Сокет не создан");
              return 0;
      }

	//Определяем прослушиваемый порт и адрес
	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
    addr.sin_port = htons(port); // Порт 18666
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if( bind(socket_desc, (struct sockaddr *)&addr, sizeof(addr)) < 0 )
      {
              perror("Ошибка при cвязке");
              return 0;
      }
	
	/*помечаем сокет, как пассивный*/

	if( listen(socket_desc, 5) )
      {
              perror("Ошибка в прослушивание порта");
              return 0;
      }

	for(;;) //Этот цикл делает цикл получения данных по сети бесконечным. По карйней мере до нажатия ctrl+c
	{

	//сокет на чтение
	int s1 = accept(socket_desc, NULL, NULL);
      if( s1 < 0 )
      {
              perror("Ошибка при вызове accept");
              return 0;
      }
	
	//*читаем данные из сокет
	
	unsigned int buffer[0];
    int counter = 0;
	memset(buffer, 0, sizeof(buffer));
    
	for(;;)
    {
            
			int rc = recv(s1, buffer, sizeof(buffer)-1, 0);
            if( rc < 0 )
            {
                    if( errno == EINTR )
                            continue;
                    perror("Не могу получить данные");
                    return 0;
            }
            if( rc == 0 )
                    break;
            printf("%d\n", buffer[0]);
    }

	char * response = "0";
    if (prime(buffer[0]) == 1)  response = luk (buffer[0]);//отправляем данные на тест

	printf("size: %d", strlen(response));

    if( sendto( s1, response, strlen(response), 0, (struct sockaddr *)&addr, sizeof(addr) ) < 0 )
            perror("Error sending response");
    printf("Response send: %s\n",response);
	}
	
	return 0;
}

