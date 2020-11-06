#ifdef _WIN32 //Windows пока не работает
#include <WinSock2.h>

#else // linux

#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h> 
#include <netdb.h>
#include <memory.h>
#include <errno.h>

#endif

int main(int argc , char *argv[])
{
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
    addr.sin_port = htons(18666); // Порт
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

	//сокет на чтение
	int s1 = accept(socket_desc, NULL, NULL);
      if( s1 < 0 )
      {
              perror("Ошибка при вызове accept");
              return 0;
      }
	
	//*читаем данные из сокета
	
	char buffer[31];
    int counter = 0;
    for(;;)
    {
            memset(buffer, 0, sizeof(char)*31);
			int rc = recv(s1, buffer, 30, 0);
            if( rc < 0 )
            {
                    if( errno == EINTR )
                            continue;
                    perror("Не могу получить данные");
                    return 0;
            }
            if( rc == 0 )
                    break;
            printf("%s\n", buffer);
    }
    
    //Не забыть написать нормальный возврат
    char response[] = "0";
    if( sendto( s1, response, sizeof(response), 0, (struct sockaddr *)&addr, sizeof(addr) ) < 0 )
            perror("Error sending response");
    printf("Response send\n");
    
	return 0;
}

