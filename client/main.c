#include <sys/types.h>
#include <sys/socket.h>
#include <memory.h>
#include <stdio.h>
#include <netdb.h>

#include <gtk/gtk.h>

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

//Данные для таблицы серверов
typedef struct 
{
  gint   number;
  gchar *ip;
  gint   port;
}
Item;

enum
{
  COLUMN_ITEM_NUMBER,
  COLUMN_ITEM_IP,
  COLUMN_ITEM_PORT,
  NUM_ITEM_COLUMNS
};

enum
{
  COLUMN_NUMBER_TEXT,
  NUM_NUMBER_COLUMNS
};

static GArray *articles = NULL;

//Элименты списка серверов по умолчанию.
static void add_items (void)
{
  Item foo;

  g_return_if_fail (articles != NULL);

  foo.number = 0;
  foo.ip = "127.0.0.1";
  foo.port = 18666;
  g_array_append_vals (articles, &foo, 1);
}

static GtkTreeModel * create_items_model (void)
{
  gint i = 0;
  GtkListStore *model;
  GtkTreeIter iter;

  /* create array */
  articles = g_array_sized_new (FALSE, FALSE, sizeof (Item), 1);

  add_items ();

  /* create list store */
  model = gtk_list_store_new (NUM_ITEM_COLUMNS,G_TYPE_INT,G_TYPE_STRING, G_TYPE_INT, G_TYPE_BOOLEAN); //Номер колонны, ip, port
  /* add items */
  for (i = 0; i < articles->len; i++)
    {
      gtk_list_store_append (model, &iter);

      gtk_list_store_set (model, &iter,
			              COLUMN_ITEM_NUMBER,
			              g_array_index (articles, Item, i).number,
                          COLUMN_ITEM_IP,
                          g_array_index (articles, Item, i).ip,
                          COLUMN_ITEM_PORT,
                          g_array_index (articles, Item, i).port,
						  -1);
    }

  return GTK_TREE_MODEL (model);
}

static GtkTreeModel * create_numbers_model (void)
{
#define N_NUMBERS 10
  gint i = 0;
  GtkListStore *model;
  GtkTreeIter iter;

  /* create list store */
  model = gtk_list_store_new (NUM_NUMBER_COLUMNS, G_TYPE_STRING, G_TYPE_INT);

  /* add numbers */
  for (i = 0; i < N_NUMBERS; i++)
    {
      char str[2];

      str[0] = '0' + i;
      str[1] = '\0';

      gtk_list_store_append (model, &iter);

      gtk_list_store_set (model, &iter,
                          COLUMN_NUMBER_TEXT, str,
                          -1);
    }

  return GTK_TREE_MODEL (model);

#undef N_NUMBERS
}

static gboolean separator_row (GtkTreeModel *model, GtkTreeIter  *iter, gpointer data)
{
  GtkTreePath *path;
  gint idx;

  path = gtk_tree_model_get_path (model, iter);
  idx = gtk_tree_path_get_indices (path)[0];

  gtk_tree_path_free (path);

  return idx == 5;
}

static void editing_started (GtkCellRenderer *cell, GtkCellEditable *editable, const gchar *path, gpointer data)
{
  gtk_combo_box_set_row_separator_func (GTK_COMBO_BOX (editable), separator_row, NULL, NULL);
}

//Функция редактирует ячейку колонки
static void cell_edited (GtkCellRendererText *cell, const gchar *path_string, const gchar *new_text, gpointer data)
{
  GtkTreeModel *model = (GtkTreeModel *)data;
  GtkTreePath *path = gtk_tree_path_new_from_string (path_string);
  GtkTreeIter iter;

  gint column = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (cell), "column"));

  gtk_tree_model_get_iter (model, &iter, path);

  switch (column)
    {
    case COLUMN_ITEM_NUMBER:
      {
        gint i;

        i = gtk_tree_path_get_indices (path)[0];
        g_array_index (articles, Item, i).number = atoi (new_text);

        gtk_list_store_set (GTK_LIST_STORE (model), &iter, column,
                            g_array_index (articles, Item, i).number, -1);
      }
      break;

	case COLUMN_ITEM_PORT:
	  {
		  gint i,val;
		  val = atoi (new_text);
		  i = gtk_tree_path_get_indices (path)[0];
		  g_array_index (articles, Item, i).port = val;
		  gtk_list_store_set (GTK_LIST_STORE (model), &iter, column, g_array_index (articles, Item, i).port, -1);
	  }
	  break;

    case COLUMN_ITEM_IP:
      {
		gint i;
        
        g_array_index (articles, Item, i).ip = new_text;

        gtk_list_store_set (GTK_LIST_STORE (model), &iter, column,g_array_index (articles, Item, i).ip, -1);
      }
      break;
        
    }

  gtk_tree_path_free (path);
}


static void add_columns (GtkTreeView  *treeview, GtkTreeModel *items_model, GtkTreeModel *numbers_model)//Добавляем колонны определённых типов и выбираем функции для их редактирования
{
  GtkCellRenderer *renderer;

  /* number column */
  renderer = gtk_cell_renderer_combo_new ();
  g_object_set (renderer, "model", numbers_model, "text-column", COLUMN_NUMBER_TEXT, "has-entry", FALSE, "editable", TRUE, NULL);
  g_signal_connect (renderer, "edited", G_CALLBACK (cell_edited), items_model);
  g_signal_connect (renderer, "editing-started",  G_CALLBACK (editing_started), NULL);
  g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (COLUMN_ITEM_NUMBER));

  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview), -1, "Number", renderer,"text", COLUMN_ITEM_NUMBER, NULL);

  /* ip column */
  renderer = gtk_cell_renderer_text_new ();
  g_object_set (renderer, "editable", TRUE, NULL);
  g_signal_connect (renderer, "edited", G_CALLBACK (cell_edited), items_model);
  g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (COLUMN_ITEM_IP));

  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview), -1, "IP", renderer, "text", COLUMN_ITEM_IP, NULL);

  /* port column */

  renderer = gtk_cell_renderer_text_new ();
  g_object_set (renderer, "editable", TRUE, NULL);
  g_signal_connect (renderer, "edited", G_CALLBACK (cell_edited), items_model);
  g_object_set_data (G_OBJECT (renderer), "column", GINT_TO_POINTER (COLUMN_ITEM_PORT));

  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeview), -1, "Port", renderer, "text", COLUMN_ITEM_PORT, NULL);

}

//Добавить сервер в список
static void add_item (GtkWidget *button, gpointer data)
{
  Item foo;
  GtkTreeIter current, iter;
  GtkTreePath *path;
  GtkTreeModel *model;
  GtkTreeViewColumn *column;
  GtkTreeView *treeview = (GtkTreeView *)data;

  g_return_if_fail (articles != NULL);

  foo.number = -1;
  foo.ip = g_strdup ("127.0.0.1");
  foo.port = 18666;
  g_array_append_vals (articles, &foo, 1);

  /* Insert a new row below the current one */
  gtk_tree_view_get_cursor (treeview, &path, NULL);
  model = gtk_tree_view_get_model (treeview);
  if (path)
    {
      gtk_tree_model_get_iter (model, &current, path);
      gtk_tree_path_free (path);
      gtk_list_store_insert_after (GTK_LIST_STORE (model), &iter, &current);
    }
  else
    {
      gtk_list_store_insert (GTK_LIST_STORE (model), &iter, -1);
    }

  /* Set the data for the new row */
  gtk_list_store_set (GTK_LIST_STORE (model), &iter,
                      COLUMN_ITEM_NUMBER, foo.number,
                      COLUMN_ITEM_IP, foo.ip,
                      COLUMN_ITEM_PORT, foo.port,
                      -1);

  /* Move focus to the new row */
  path = gtk_tree_model_get_path (model, &iter);
  column = gtk_tree_view_get_column (treeview, 0);
  gtk_tree_view_set_cursor (treeview, path, column, FALSE);

  gtk_tree_path_free (path);
}

//Удаляет сервре из списка
static void remove_item (GtkWidget *widget, gpointer data)
{
  GtkTreeIter iter;
  GtkTreeView *treeview = (GtkTreeView *)data;
  GtkTreeModel *model = gtk_tree_view_get_model (treeview);
  GtkTreeSelection *selection = gtk_tree_view_get_selection (treeview);

  if (gtk_tree_selection_get_selected (selection, NULL, &iter))
    {
      gint i;
      GtkTreePath *path;

      path = gtk_tree_model_get_path (model, &iter);
      i = gtk_tree_path_get_indices (path)[0];
      gtk_list_store_remove (GTK_LIST_STORE (model), &iter);

      g_array_remove_index (articles, i);

      gtk_tree_path_free (path);
    }
}
//

int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);
	GtkWidget *window,*hbox,*VboxServer,*HboxButton;
	GtkWidget *treeview;
	GtkTreeModel *items_model;
    GtkTreeModel *numbers_model;
	GtkWidget *sw;
	GtkWidget *button;


    //Параметр окна
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Laba: client");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	//Инициализация боксов
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5); // Основной бокс;
	VboxServer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); //Бокс для серверов.
	HboxButton = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5); //Бокс для кнопок управления серверами

	gtk_box_pack_start(GTK_BOX(hbox),VboxServer,FALSE,FALSE,5);//Привязываем серверный бокс к главному
	gtk_container_add(GTK_CONTAINER(window), hbox); // Привязываем главный бокс к окну.
	
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(closeApp), NULL); // Связываем действие "Закрыть окно" с дейтвием "Закрыть приложение".

	//Скролить список ip
	sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start (GTK_BOX (VboxServer), sw, TRUE, TRUE, 0);

	//Список серверов
	//Создание модели
	items_model = create_items_model ();
	numbers_model = create_numbers_model ();
	//Виджет с таблицей
	treeview = gtk_tree_view_new_with_model (items_model);
    gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview)), GTK_SELECTION_SINGLE);
    add_columns (GTK_TREE_VIEW (treeview), items_model, numbers_model);

    g_object_unref (numbers_model);
    g_object_unref (items_model);

	gtk_widget_set_size_request (sw,250,200);
    gtk_container_add (GTK_CONTAINER (sw), treeview);// Добавляем в скрол таблицу.

	//Кнопки
	
	button = gtk_button_new_with_label ("Добавить сервер");
    g_signal_connect (button, "clicked",G_CALLBACK (add_item), treeview);
    gtk_box_pack_start (GTK_BOX (HboxButton), button, TRUE, TRUE, 0);//Добавляем кнопки

    button = gtk_button_new_with_label ("Удалить сервер");
    g_signal_connect (button, "clicked",G_CALLBACK (remove_item), treeview);
    gtk_box_pack_start (GTK_BOX (HboxButton), button, TRUE, TRUE, 0);
	
	gtk_box_pack_start(GTK_BOX(VboxServer),HboxButton,FALSE,FALSE,5);//Кнопки в северный бокс
	
	//Отправляем чило и ждём ответа.
	long long answer;
    answer = data_up ("127.0.0.1",18666,13);

	//Если ответ равен нулю, значит ответ не найден.
	if (answer!=0) printf ("Ответ: %lu \n",answer);

	gtk_widget_show_all(window);

	gtk_main();
	
	return 0;
}
