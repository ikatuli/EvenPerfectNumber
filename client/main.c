#include <sys/types.h>
#include <sys/socket.h>
#include <memory.h>
#include <stdio.h>
#include <netdb.h>

int main(int argc, char * argv[])
{

      // Создаём сокет
      int s = socket( AF_INET, SOCK_STREAM, 0 );
      if(s < 0)
      {
              perror( "Не удалось создать socket" );
              return 0;
      }

      //соединяемся
      struct sockaddr_in peer;
      peer.sin_family = AF_INET;
      peer.sin_port = htons( 18666 ); //Порт
      peer.sin_addr.s_addr = inet_addr( "127.0.0.1" ); //шз
      int result = connect( s, ( struct sockaddr * )&peer, sizeof( peer ) );
      if( result )
      {
              perror( "Не удалось подключиться connect" );
              return 0;
      }

	  //посылаем данные
	  
	  char buf[] = "Hello, world!";
      result = send( s, "Hello, world!", 13, 0);
      if( result <= 0 )
      {
              perror( "Ошибка при отправке данных" );
              return 0;
      }

	  // закрываем соединения
      if( shutdown(s, 1) < 0)
      {
              perror("Ошибка при закрытие");
              return 0;
      }

	  // читаем ответ
      fd_set readmask;
      fd_set allreads;
      FD_ZERO( &allreads );
      FD_SET( 0, &allreads );
      FD_SET( s, &allreads );
      for(;;)
      {
              readmask = allreads;
              if( select(s + 1, &readmask, NULL, NULL, NULL ) <= 0 )
              {
                      perror("Error calling select");
                      return 0;
              }
              if( FD_ISSET( s, &readmask ) )
              {
                      char buffer[20];
                      memset(buffer, 0, 20*sizeof(char));
                      int result = recv( s, buffer, sizeof(buffer) - 1, 0 );
                      if( result < 0 )
                      {
                              perror("Error calling recv");
                              return 0;
                      }
                      if( result == 0 )
                      {
                              perror("Server disconnected");
                              return 0;
                      }
					  printf("%s\n", buffer);
              }
              if( FD_ISSET( 0, &readmask ) )
              {
                      printf( "No server response" );
                      return 0;
              }
      }
      return 0;
}
