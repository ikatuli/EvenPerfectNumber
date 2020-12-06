#include <sys/types.h>
#include <sys/socket.h>
#include <memory.h>
#include <stdio.h>
#include <netdb.h>

#include <gtk/gtk.h>

//Глобальные переменные для GUI
GtkWidget *window;

long long data_up (char *ip,int port, unsigned int number)
{
	// Создаём сокет
	int s = socket( AF_INET, SOCK_STREAM, 0 );
	if(s < 0) perror( "Не удалось создать socket" );

	//соединяемся
    struct sockaddr_in peer;
    peer.sin_family = AF_INET;
	peer.sin_port = htons( port ); //Порт
    peer.sin_addr.s_addr = inet_addr( ip ); //шз

    int result = connect( s, ( struct sockaddr * )&peer, sizeof( peer ) );
    if( result ) perror( "Не удалось подключиться connect" );

	//посылаем данные
	
	unsigned int buf[] = {number};
    result = send( s,buf, sizeof(buf), 0);
    if( result <= 0 ) perror( "Ошибка при отправке данных" );

	// закрываем соединения
    if( shutdown(s, 1) < 0) perror("Ошибка при закрытие");

    // читаем ответ
    fd_set readmask;
    fd_set allreads;
    FD_ZERO( &allreads );
    FD_SET( 0, &allreads );
    FD_SET( s, &allreads );
    for(;;)
	{
		readmask = allreads;
        if( select(s + 1, &readmask, NULL, NULL, NULL ) <= 0 ) perror("Error calling select");
        if( FD_ISSET( s, &readmask ) )
              {
                  unsigned long long rezultat[0];
                      memset(rezultat, 0, 20*sizeof(rezultat));
                      int result = recv( s, rezultat, sizeof(rezultat) - 1, 0 );
                      if( result < 0 ) perror("Error calling recv");
                      if( result == 0 ) perror("Server disconnected");
                     
					  return rezultat[0]; //Ответ
              }
              if( FD_ISSET( 0, &readmask ) ) printf( "No server response" );
	}
}

void closeApp(GtkWidget *window, gpointer data) //Завершаем программу
{
    gtk_main_quit();

}


int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);


    //Параметр окна
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Laba");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 100);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	//Отправляем чило и ждём ответа.
	long long answer;
    answer = data_up ("127.0.0.1",18666,13);

	//Если ответ равен нулю, значит ответ не найден.
	if (answer!=0) printf ("Ответ: %lu \n",answer);

	gtk_widget_show_all(window);

	gtk_main();
	
	return 0;
}
