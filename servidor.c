#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <mysql.h>
#include <netinet/in.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	MYSQL *conn;
	int err;
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	int numID = 6;
	char consulta [80];
	
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
	
	int sock_conn, sock_listen, ret;
	struct sockaddr_in serv_adr;
	char peticion[512];
	char respuesta[512];
	
	// INICIALITZACIONS
	// Obrim el socket
	if ((sock_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Error creant socket");
		// Fem el bind al port
	}
		
	
	memset(&serv_adr, 0, sizeof(serv_adr));// inicialitza a zero serv_addr
	serv_adr.sin_family = AF_INET;
	
	// asocia el socket a cualquiera de las IP de la m?quina. 
	//htonl formatea el numero que recibe al formato necesario
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	// establecemos el puerto de escucha
	serv_adr.sin_port = htons(6900);
	if (bind(sock_listen, (struct sockaddr *) &serv_adr, sizeof(serv_adr)) < 0)
	{
		printf ("Error al bind");
	}
	
	if (listen(sock_listen, 3) < 0)
	{
		printf("Error en el Listen");
		
	}
	
	int i;
	// Bucle infinito
	for (;;)
	{
		printf ("Escuchando\n");
		
		sock_conn = accept(sock_listen, NULL, NULL);
		printf ("He recibido conexion\n");
		//sock_conn es el socket que usaremos para este cliente
		
		int terminar =0;
		// Entramos en un bucle para atender todas las peticiones de este cliente
		//hasta que se desconecte
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
			
			if ((codigo !=0) && (codigo !=4))
			{
				p = strtok( NULL, "/");
				strcpy (username, p);
			}
			
			if (codigo ==0)
			{
				terminar=1;
			}//peticion de desconexion
			
			else if (codigo ==1) // Piden Loguearse
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
						strcpy(respuesta,"El usuario NO ha podido loguearse, revise si el usuario y la contraseña coinciden.");
					}
						
					else
						while (row !=NULL) 
					{
							printf ("Username: %s\n", row[0]);
							row = mysql_fetch_row (resultado);
							strcpy(respuesta,"El usuario ha podido loguearse correctamente");
					}
						
					write (sock_conn,respuesta, strlen(respuesta));
					
			}
			
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
					strcpy(respuesta,"El usuario NO ha podido loguearse, revise si el usuario y la contraseña coinciden.");
				}
				
				else
				while (row !=NULL) 
				{
					
					strcpy (consulta, "DELETE FROM Jugador WHERE Jugador.USERNAME='");
					strcat (consulta, username); 
					strcat (consulta, "';");
						
					printf("consulta = %s\n", consulta);
						
					strcpy(respuesta,"El usuario ha sido eliminado de la base de datos ");
						
					err = mysql_query(conn, consulta);
					if (err!=0)
					{
						printf ("Error al introducir datos la base %u %s\n", mysql_errno(conn), mysql_error(conn));
						strcpy(respuesta,"El usuario NO ha sido eliminado de la base de datos ");
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
			
			else if (codigo ==3) // Piden Registrarse
			{
				p = strtok( NULL, "/");
				strcpy (password, p);
				
				
				
				
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
					strcpy(respuesta,"El usuario se ha REGISTRADO correctamente");
				}
					
				write (sock_conn,respuesta, strlen(respuesta));
					
			}
			
			else if (codigo == 4) // Mostrar usuarios que hay en el sistema
			{
				char usuarios[80];
				printf("\n");
				printf("Los usuarios que actualmente estan registrados son:\n");
				err=mysql_query (conn, "SELECT * FROM Jugador");
				if (err!=0) 
				{
					printf ("Error al consultar datos de la base %u %s\n", mysql_errno(conn), mysql_error(conn));
					exit (1);
				}
				
				resultado = mysql_store_result (conn);
				row = mysql_fetch_row (resultado);
				strcpy(respuesta, "Los usuarios registrados son los siguientes: ");
				
				if (row == NULL)
				{
					printf ("No se han obtenido datos en la consulta\n");
				}
				else
				while (row !=NULL) 
				{
					printf ("Username: %s\n", row[1]);
					sprintf(respuesta,"%s%s,",respuesta,row[1]);
					row = mysql_fetch_row (resultado);
				}
				write (sock_conn,respuesta, strlen(respuesta));
				
			}
		}
		mysql_close (conn);
		// Se acabo el servicio para este cliente
		close(sock_conn); 
	}
}

