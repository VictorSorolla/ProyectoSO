#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <mysql.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>
//#include <my_global.h>
#define MAX 100

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//
// Estructura para un usuario conectado al servidor.
//


typedef struct {
	int id; // si vale -1 es que este elemento de la tabla está libre
	char nombre [10];
} Telemento;

typedef Telemento Ttabla [MAX];


typedef struct 
{
	char nombre [20];
	int socket;
} Conectado;
//
// Estructura de lista de conectados.
//
typedef struct 
{
	Conectado conectados [100];
	int num;
} ListaConectados;

ListaConectados miLista;



void inicializa (Ttabla t)
{
	int i;
	for ( i=0; i<MAX; i++)
		t[i].id = -1;
}


void escribir_tabla (Ttabla t)
{
	int i;
	printf ("Contenido de la tabla:\n");
	for (i=0; i<MAX; i++)
		if (t[i].id !=-1)
			printf ("Nombre: %s, Id: %d\n", t[i].nombre, t[i].id);
}

int pon_elemento (Ttabla t, Telemento e)
	/* Busca un sitio libre en la tabla y coloca allí el elemento
	* Retorna 0 si oK y -1 si la lista está llena
	*/
{
	int encontrado = 0;
	int i=0;
	while ((i<MAX) && ! encontrado)
	{
		if (t[i].id == -1)
			encontrado = 1;
		if (!encontrado)
			i=i+1;
	}
	if (encontrado)
	{
		t[i]=e;
		return 0;
	}
	else
		return -1;
}

int eliminar (Ttabla t, char nombre [10])
	/* Elimina de la tabla el usuario cuyo nombre se recibe como parámetro
	* Devuelve 0 si Ok y -1 si el usuario no está en la lista
	*/
{
	int encontrado = 0;
	int i=0;
	while ((i<MAX) && ! encontrado)
	{
		if (strcmp (t[i].nombre,nombre) == 0)
			encontrado = 1;
		if (!encontrado)
			i=i+1;
	}
	if (encontrado)
	{
		t[i].id=-1;
		return 0;
	}
	else
		return -1;
}


int Login (char respuesta[512], char username[20],char password[20],MYSQL *conn)
{
	char consulta[200];
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	
	strcpy (consulta, "SELECT Jugador.USERNAME FROM (Jugador) WHERE Jugador.USERNAME='");
	strcat (consulta, username); 
	strcat (consulta, "'");
	strcat (consulta, " AND Jugador.PASSWORD='"); 
	strcat (consulta, password); 
	strcat (consulta, "';");
	
	printf("consulta = %s\n", consulta);
	
	int err = mysql_query(conn, consulta);
	if (err!=0)
	{
		printf ("El USERNAME y el PASSWORD no coinciden %u %s\n", mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	resultado = mysql_store_result (conn);
	row = mysql_fetch_row (resultado);
	
	if (row == NULL)
	{
		printf ("El USERNAME y el PASSWORD no coinciden\n");
		strcpy(respuesta,"El usuario NO ha podido loguearse, revise si el usuario y la contrase?a coinciden.");
		return -1;
	}
	
	else
		while (row !=NULL) 
	{
			printf ("Username: %s\n", row[0]);
			row = mysql_fetch_row (resultado);
			return 0;
	}
}



int Registrar(char consulta[200], MYSQL *conn, char respuesta[512], char username[20], char password[20]) {
	
	int err;
	char prueba[1];
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	sprintf(prueba, "%d",0);
	// construimos SQL
	strcpy(consulta, "INSERT INTO Jugador VALUES (7,'");
	strcat (consulta, username); 
	strcat (consulta, "'");
	strcat (consulta, ",'"); 
	strcat (consulta, password); 
	strcat (consulta, "');");
	
	printf("consulta = %s\n", consulta);
	
	err = mysql_query(conn, consulta);
	if (err!=0)
	{
		printf ("El USERNAME y el PASSWORD no coinciden %u %s\n", mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	printf("\n");
	printf("Despues de dar alta al jugador deseado la BBDD queda de la siguiente forma:\n");
	err=mysql_query (conn, "SELECT * FROM Jugador");
	if (err!=0) 
	{
		printf ("Error al consultar datos de la base %u %s\n", mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	resultado = mysql_store_result (conn);
	row = mysql_fetch_row (resultado);
	
	if (row == NULL)
	{
		printf ("No se han obtenido datos en la consulta\n");
	}
	else
		while (row !=NULL) 
	{
			printf ("Username: %s\n", row[1]);
			row = mysql_fetch_row (resultado);
			strcpy(respuesta,"3-El usuario se ha REGISTRADO correctamente");
	}
		
}



//
// Funcion que pone en la lista de conectados un usuario.
//
int Pon (ListaConectados *lista, char nombre[20], int socket)
{
	if(lista->num == 100)
	{
		return -1;
	}
	else
	{
		strcpy(lista->conectados[lista->num].nombre, nombre);
		lista->conectados[lista->num].socket = socket;
		lista->num++;
		return 0;
	}
}

//
// Funcion que devuelve la posicion de un usuario pasado por parámetro
//
int DamePosicion (ListaConectados *lista, char nombre [20]) //----------------------------------modificar!!!!!!!!--------------------
{
	int i=0;
	int encontrado = 0;
	while((i<lista->num) && (!encontrado))
	{
		//if (lista->conectados[i].nombre,nombre)
		//{
		if (strcmp(lista->conectados[i].nombre,nombre) == 0)
		{
			encontrado = 1;
		}
		if (!encontrado)
		{
			i=i+i;
		}
		//}
	}
	if(encontrado)
	{
		return i;
	}
	else
	{
		return -1;
	}
}
//
// Funcion que elimina de la lista de conectados el usuario pasado como parámetro.
//
int Elimina (ListaConectados *lista, char nombre[20])
{
	printf("%s:%d \n", lista->conectados[0].nombre, lista->num);
	printf("Nombre recibido como parametro: %s \n", nombre);
	int pos = DamePosicion(lista, nombre);
	if (pos == -1)
	{
		return -1;
	}
	else
	{
		int i;
		for (i=pos; i < lista->num-1; i++)
		{
			//lista->conectados[i] = lista->conectados[i+1];
			strcpy(lista->conectados[i].nombre, lista->conectados[i+1].nombre);
			lista->conectados[i].socket = lista->conectados[i+1].socket;
		}
		lista->num--;
		printf("REsultado:%d\n", lista->num);
		return 0;
	}
	
}
//
// Funcion que llena una vector de carácteres con la lista de conectados.
//
void DameConectados (ListaConectados *lista, char conectados[300])
{
	sprintf(conectados,"%d", lista->num);
	int i; 
	for (i=0; i<lista->num; i++)
	{
		sprintf(conectados, "%s/%s", conectados, lista->conectados[i].nombre);
	}
}

void *AtenderCliente (void *socket)
{
	int socket_inv;
	
	int *s;
	s= (int *) socket;
	int sock_conn = * (int *) socket;
	sock_conn = *s;
	//s= (int *) socket;
	//socket = miLista.conectados[num].socket;
	
	//ListaConectados miLista;
	int i = miLista.num;
	miLista.conectados[i].socket = *s;
	
	Ttabla mi_tabla;
	Telemento invitado;
	
	// Base de datos
	char peticion[512];
	char respuesta[512];
	int ret;
	MYSQL *conn;
	int err;
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	int numID = 6;
	char consulta [80];
	char connected[200]; // Para mostrar los usuarios conectados por pantalla.
	
	//Creamos una conexion al servidor MYSQL 
	conn = mysql_init(NULL);
	if (conn==NULL) 
	{
		printf ("Error al crear la conexion: %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	//inicializar la conexion, entrando nuestras claves de acceso y
	//el nombre de la base de datos a la que queremos acceder 
	conn = mysql_real_connect (conn, "localhost","root", "mysql", "bd",0, NULL, 0);
	if (conn==NULL)
	{
		printf ("Error al inicializar la conexion: %u %s\n",
				mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	
	int terminar =0;
	// Entramos en un bucle para atender todas las peticiones de este cliente hasta que se desconecte.
	while (terminar ==0)
	{
		
		ret=read(sock_conn,peticion, sizeof(peticion));
		printf ("Recibido\n");
		peticion[ret]='\0';
		printf ("Peticion: %s\n",peticion);
		
		char *p = strtok( peticion, "/");
		int codigo =  atoi (p);
		char username[20];
		char password[20];
		
		// if ((codigo !=4) && (codigo !=5))
		if ((codigo !=0))
		{
			p = strtok( NULL, "/");
			strcpy (username, p);
		}
		//
		// Peticion de DESCONEXIÓN.
		//
		if (codigo ==0)
		{
			pthread_mutex_lock(&mutex);
			int elim = Elimina(&miLista,username);
			pthread_mutex_unlock(&mutex);
			if (elim == 0)
				printf("Usuario eliminado de la lista de conectados\n");
			else	
				printf("Error al eliminar el usuario de la lista de conectados\n");
			// El problema es el connected.
			DameConectados(&miLista,connected);
			printf("Estos son los usuarios conectados actualmente: %s\n", connected);
			
			terminar=1;
			
			
		}
		//
		// Petición de LOGUEAR.
		//
		else if (codigo ==1) 
		{
			p = strtok( NULL, "/");
			strcpy (password, p);
			
			int result = Login(respuesta,username,password, conn );
			if (result == 0)
			{
				
				pthread_mutex_lock(&mutex);
				int res = Pon(&miLista, username, sock_conn);
				pthread_mutex_unlock( &mutex);
				printf("%s",username);
				if (res == 0)
					printf("Anadido a la lista de conectados\n");
				if (res != 0)
					printf("Lista llena. No se pudo a?adir el usuario a la lista de conectados.\n");
				DameConectados(&miLista,connected);
				printf("Estos son los usuarios conectados actualmente: %s\n", connected);
				
			}
			else
				printf("El usuario NO ha podido loguearse, revise si el usuario y la contrase?a coinciden.");
			
			char notificacion[200];
			sprintf(notificacion, "1-%s", connected);
			//write (sock_conn,respuesta, strlen(respuesta));
			for (int i=0; i<miLista.num ; i++)
			{
				write (miLista.conectados[i].socket, notificacion, strlen(notificacion));
			}
		}
		//
		// Peticion de ELIMINAR USUARIO.
		//
		else if (codigo == 2)
		{
			
			p = strtok( NULL, "/");
			strcpy (password, p);
			
			strcpy (consulta, "SELECT Jugador.USERNAME FROM (Jugador) WHERE Jugador.USERNAME='");
			strcat (consulta, username); 
			strcat (consulta, "'");
			strcat (consulta, " AND Jugador.PASSWORD='"); 
			strcat (consulta, password); 
			strcat (consulta, "';");
			
			printf("consulta = %s\n", consulta);
			
			err = mysql_query(conn, consulta);
			if (err!=0)
			{
				printf ("El USERNAME y el PASSWORD no coinciden %u %s\n", mysql_errno(conn), mysql_error(conn));
				exit (1);
			}
			
			resultado = mysql_store_result (conn);
			row = mysql_fetch_row (resultado);
			
			if (row == NULL)
			{
				printf ("El USERNAME y el PASSWORD no coinciden\n");
				strcpy(respuesta,"2/El usuario NO ha podido loguearse, revise si el usuario y la contraseï¿±a coinciden.");
			}
			
			else
				while (row !=NULL) 
			{
					
					strcpy (consulta, "DELETE FROM Jugador WHERE Jugador.USERNAME='");
					strcat (consulta, username); 
					strcat (consulta, "';");
					
					printf("consulta = %s\n", consulta);
					
					strcpy(respuesta,"2-El usuario ha sido eliminado de la base de datos ");
					
					err = mysql_query(conn, consulta);
					if (err!=0)
					{
						printf ("Error al introducir datos la base %u %s\n", mysql_errno(conn), mysql_error(conn));
						strcpy(respuesta,"2-El usuario NO ha sido eliminado de la base de datos ");
						exit (1);
					}
					
					printf("\n");
					printf("Despues de dar baja al jugador deseado la BBDD queda de la siguiente forma:\n");
					err=mysql_query (conn, "SELECT * FROM Jugador");
					if (err!=0) 
					{
						printf ("Error al consultar datos de la base %u %s\n", mysql_errno(conn), mysql_error(conn));
						exit (1);
					}
					
					resultado = mysql_store_result (conn);
					row = mysql_fetch_row (resultado);
					
					if (row == NULL)
					{
						printf ("No se han obtenido datos en la consulta\n");
					}
					else
						while (row !=NULL) 
					{
							printf ("Username: %s\n", row[1]);
							row = mysql_fetch_row (resultado);							
					}
			}
				write (sock_conn,respuesta, strlen(respuesta));
		}
		//
		// Peticion de REGISTRAR.
		//
		else if (codigo ==3) 
		{
			p = strtok( NULL, "/");
			strcpy (password, p);
			
			Registrar(consulta, conn, respuesta, username, password);
			
			write (sock_conn,respuesta, strlen(respuesta));
			
		}
		
		else if (codigo ==4) 
		{	int i;		
			//char invitado[10];
			//p = strtok( NULL, "/");
			//strcpy (invitado, p);
			//int res = CrearInvitados(&miListai, invitado);
			
			printf("Usarios invitados: %s\n", username);
			for(i=0; i<miLista.num; i++)
			{
				if(strcmp(username , miLista.conectados[i].nombre) == 0)
				{
					socket_inv = miLista.conectados[i].socket;
				}
			}
			
			char respuesta[200];
			sprintf("Invitado xd", respuesta);
			write (miLista.conectados[socket_inv].socket, respuesta, strlen(respuesta));
			
			/*strcpy(invitado.nombre, username);
			printf("klk %s\n", invitado.nombre);
			int err = pon_elemento (mi_tabla, invitado);
			if (err ==0)
				printf ("Elemento anadido\n");
			else
				printf ("Tabla llena\n");
			escribir_tabla (mi_tabla);
			*/
			//write (sock_conn,respuesta, strlen(respuesta));
			
		}
		
	}
	//mysql_close (conn);
	// Se acabo el servicio para este cliente
	close(sock_conn); 
	
}
//
// Aquí empieza el main.
//
int main(int argc, char *argv[])
{
	int sock_conn, sock_listen;
	struct sockaddr_in serv_adr;
	
	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("Error creant socket");
	// Fem el bind al port
	
	
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	
	// asocia el socket a cualquiera de las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// establecemos el puerto de escucha
	serv_adr.sin_port = htons(9300);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
		printf ("Error al bind");
	
	if (listen(sock_listen, 3) < 0)
		printf("Error en el Listen");
	
	int i;
	int sockets[100];
	//pthread_t thread;
	pthread_t thread[10];
	ListaConectados miLista;
	i=0;
	// Bucle para atender a 5 clientes
	for (;;){
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexion\n");
		
		sockets[i] =sock_conn;
		//sock_conn es el socket que usaremos para este cliente
		miLista.conectados[i].socket = sock_conn;
		miLista.num = i;
		
		// Crear thead y decirle lo que tiene que hacer
		
		pthread_create (&thread[i], NULL, AtenderCliente,&sockets[i]);
		//pthread_create (&thread[i], NULL, AtenderCliente, &miLista);
		i++;
		
	}
}
