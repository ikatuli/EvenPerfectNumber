#include <sys/types.h>
#include <sys/socket.h>
#include <memory.h>
#include <stdio.h>
#include <netdb.h>

int data_up (char *ip,int port, int number)
{
	// Создаём сокет
	int s = socket( AF_INET, SOCK_STREAM, 0 );
	if(s < 0) return -1;

	//соединяемся
    struct sockaddr_in peer;
    peer.sin_family = AF_INET;
	peer.sin_port = htons( port ); //Порт
    peer.sin_addr.s_addr = inet_addr( ip ); //шз

    int result = connect( s, ( struct sockaddr * )&peer, sizeof( peer ) );
    if( result ) return -2;

	//посылаем данные
	
	int buf[] = {number};
    result = send( s,buf, sizeof(buf), 0);
    if( result <= 0 ) return -3;

	// закрываем соединения
    if( shutdown(s, 1) < 0) return -4;

    // читаем ответ
    fd_set readmask;
    fd_set allreads;
    FD_ZERO( &allreads );
    FD_SET( 0, &allreads );
    FD_SET( s, &allreads );
    for(;;)
	{
		readmask = allreads;
        if( select(s + 1, &readmask, NULL, NULL, NULL ) <= 0 ) return -5;
        if( FD_ISSET( s, &readmask ) )
              {
                  int rezultat[0];
                      memset(rezultat, 0, 20*sizeof(rezultat));
                      int result = recv( s, rezultat, sizeof(rezultat) - 1, 0 );
                      if( result < 0 ) return -6;
                      if( result == 0 ) return -7;
                     
					  return rezultat[0]; //Ответ
              }
              if( FD_ISSET( 0, &readmask ) ) return -8;
	}
}

int main()
{
	//Отправляем чило и ждём ответа.
	int answer;
    answer = data_up ("127.0.0.1",18666,5);

	//Если ответ меньше нуля, значит произошла обика.
	if (answer<0){
		switch (answer){
			case -1: perror( "Не удалось создать socket" );break;
			case -2: perror( "Не удалось подключиться connect" );break;
			case -3: perror( "Ошибка при отправке данных" );break;
			case -4: perror("Ошибка при закрытие");break;
			case -5: perror("Error calling select");break;
			case -6: perror("Error calling recv");break;
			case -7: perror("Server disconnected");break;
			case -8: printf( "No server response" );break;
		}       
      }
	else printf ("Ответ: %d",answer);

	return 0;
}
