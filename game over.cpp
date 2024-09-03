#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <random>
#include <vector>
#include <chrono>
#include <thread>

// la inicializacion del motor con time 0 lo hacemos afuera de la funcion para evitar numeros repetidos debido a que si iniciamos 
// la cemilla dos veces en el mismo segundo obtenemos numeros repetidos
std::default_random_engine motor(static_cast<unsigned int>(time(0)));

void goto_xy(int,int);
void hide_cursor();
void limites();
bool limite_nave(int,int);
int  aleatorio(int, int);

struct join_cordenadas {
	int x;
	int y;
}n,k,l,h;

class nave;
class Meteoro;

std::pair <bool, bool> colicion(nave, std::vector<Meteoro>);

class nave{
private:
	// la nave controla su propia posicion del puntero por lo que permite mover el puntero en la rutina del juego 
	// y la pos de la nave no se ve alterada ni comprometida
	int X, Y;  // se actualiza automaticamente con la rutina nave::mover()
	int vida;
	int salud;

	/* Esta clase contendra la informacion de cada disparo de la clase nave y cada disparo se guardara
	   en el vector dinamico proyectiles*/
	class proyectil {
	private:
		int xp, yp;
	public:
		// la suma y resta de los 1 son para ubicar el disparo justo delante de la neve en su pocición
		proyectil(int x,int y):xp(x+1),yp(y-1){}
		void dibujar() {
			goto_xy(xp, yp);
			printf("%c", 33);
		}
		void borrar() {
			goto_xy(xp, yp);
			printf(" ");
		}
		void mover() {
			borrar();
			yp--;
			dibujar();
		}
		bool rango_proyectil() {
			/* solo ocupamos el rango superior por que x nunca varia y parte de la nave el proyectil por lo que su pocicion inicial
			   siempre esta dentro de rango */
			if (yp > 5 ) return true;
			else return false;
		}

	};
// cada disparo se gurdara en el vector proyectiles
	std::vector<proyectil> proyectiles;
public:
	nave(int x, int y):X(x),Y(y),vida(3),salud(3){}
	void dibujar();
	void borrar();
	void mover(char);
	void disparo();
	void explotar_Nave();
	join_cordenadas get_xy() {
		return { X,Y }; // Este es un constructor por defecto para la struct join_coidenadas 
						// como la funcion retorna un struct en la parte del return le mandamos los valores para su inicializacion de la estructura
						// es como si retornaramos join_cordenadas(X,Y) el cual es una estructura sin nombre pero sirve para inicializar otras estructuras
						// en este caso las ocupamo para las coliciones
	}
	/* retornamos el vector proyectil sin embargo solo accedemos para ver su contenido y no tenemos permiso de modificar
	el vector fuera de la clase por lo que mantiene encapsulamiento privado */
	std::vector<proyectil>& acceder_proyectil() { return proyectiles; }// lo hacemos por referencia por cuestiones de optimizacion
};

class Meteoro {
private:
	int mx, my;
public:
	/*los valores constante 5-82 son del rango del mapa en el eje x 
	el valor 5 de my es el valor maximo superior del eje y
	*/
	Meteoro(): mx(aleatorio(5,82)), my(5){}

	void dibujar_meteoro();
	void borrar_meteoro();
	void mover_meteoro();
	void explotar_meteoro();

	join_cordenadas get_xy() {
		return { mx,my };
	}
};

int main() {
	// centro de area de juego
	bool confirmar_frame = false;
	int x = 42, y = 25; //pocicion inicial de la nave
	char tecla;
	bool Game_over = false;
	int score = 0;
	int velocidad_meteoro = 0, N_frames = 0;

	hide_cursor();
	limites();

	/*Declaracion de objetos*/

	nave Nave(x,y);
	Nave.dibujar();
	std::vector<Meteoro> meteoros;
	meteoros.push_back(Meteoro());
	meteoros.push_back(Meteoro());
	meteoros.push_back(Meteoro());
	meteoros.push_back(Meteoro());
	meteoros.push_back(Meteoro());
	meteoros.push_back(Meteoro());
	meteoros.push_back(Meteoro());
	meteoros.push_back(Meteoro());
	meteoros.push_back(Meteoro());
	meteoros.push_back(Meteoro());


	

	/*Define la duración de un fotograma en milisegundos para aproximadamente 60 fotogramas por segundo
	  constexpr es una constante en tiempo de compilacion es decir se establece al momento de compilar mejora el rendimiento del programa
	  a diferencia de const que se obtiene al momento de ejecucion 
	  lo almacena en el objeto frame_duration
	*/
	constexpr std::chrono::milliseconds frame_duration(1000 / 60);

	while (!Game_over)
	{
		// Obtiene el tiempo actual y lo almacena en el objeto last_frame_time para comparar con respecto a current_time y 
		// verificar el tiempo de un frame y hacer los ajustes necesarios
		auto last_frame_time = std::chrono::steady_clock::now();	// tiempo inicial de ejecución del frame 

			

		// accede siempre y cuando halla proyectiles en el vector <proyectil>
		if (Nave.acceder_proyectil().size() > 0) {
			for (int i = 0; i < Nave.acceder_proyectil().size(); i++) {
				/* Nave.acceder_proyectil() me da acceso a el vector pero solo para verlo
				*  (Nave.acceder_proyectil())[i] accedo a los objetos del vector estos objetos son de tipo proyectil
				*  (Nave.acceder_proyectil())[i].rango_proyectil() accedo a el metodo del objeto rango_proyectil
				*   que se enceutra en el vector[i]
				*   para ver que la bala este en rango
				*/
				if ( (Nave.acceder_proyectil())[i].rango_proyectil() ) {
					(Nave.acceder_proyectil())[i].mover();
				}
				else {
					(Nave.acceder_proyectil())[i].borrar();
					// La siguiente instruccion es para borrar elementos del vector disparos
					(Nave.acceder_proyectil()).erase( (Nave.acceder_proyectil().begin()) +i ); // el erase no resive sub indices solo acepta iteradores por eso todo eso en el argumento
				}
			}
		}


		// con esta rutina controlamos la velocidad de los meteoros esto lo logramos moviendo los meteoros cada 
		// vez que pase cierto número de fotogramas por lo que lo mas rapido que se mueve cada meteoro es 
		// dependiedo a cuatos fotogramas corra el juego
		if (N_frames >= velocidad_meteoro)
		{
			for (int i = 0; i < meteoros.size(); i++)
			{
				meteoros[i].mover_meteoro();
			}
			N_frames = 0;
		}
		else {
			N_frames++;
		}


		/* _kbhit() espera que se precione una tecla */
		if (_kbhit())
		{	//_getch debuelve la tecla que se presiono
			tecla = _getch();
			Nave.mover(tecla);
			if (tecla == ' ') {
				Nave.disparo();
			}
			if (tecla == 'l')
			{
				Nave.explotar_Nave();
			}
		}

		// confirma que el frame dure 60 segundos axactos
		goto_xy(0, 0); printf("%d", confirmar_frame); 
		printf("   %d", (Nave.acceder_proyectil()).size() );

		/*Obtiene el tiempo en cuanto el programa llega a este punto y almacena este dato en un objeto llamado current_time*/
		auto current_time = std::chrono::steady_clock::now();
		/*Combierte la diferencia (current_time - last_frame_time) en milisegundos y almacena este dato en n objeto llamado elapsed_time*/
		auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_frame_time);

		if (elapsed_time < frame_duration) {
			/*verifica que la diferencia de tiempo entre el inicio del bucle y el final del bucle sea menor que
			  la duracion de un frame que lo tenemos marcado en 60hz en el objeto frame_duration*/
			  // lo que hace es detener el hilo actual por el tiempo necesario
			std::this_thread::sleep_for(frame_duration - elapsed_time);
			confirmar_frame = true;
		}
		else {
			confirmar_frame = false;
		}
	}

	return 0;
}

void goto_xy(int x, int y)
{
	HANDLE ID_consol; // handle es un tipo de dato para los identificadores de recursos en el sistema
	/*  
		GetStdHandle() sirve para obtener identificadores
		STD_OUTPUT_HANDLE sirve para hacer referencia a el identificador de la consola 
	*/
	ID_consol = GetStdHandle(STD_OUTPUT_HANDLE);     
	/*
		COORD estruct para cordenadas en un sistema bidimencional x,y
		pos_cursor es la instancia (objeto) de la struct osea el nombre
	*/
	COORD pos_cursor;
	pos_cursor.X = x;
	pos_cursor.Y = y;
	/*
		SetConsoleCursorPosition() esta funcion aparte de mover el cursor retorna un valor de tipo bool dependiendio de si los argumentos son correctos o no
		parametros (argumnetos)
		1. un identificador del sistema (HANDLE) en este caso del cursor de la consola
		2. una structura con la posision a establecer puede ser COORD o puedes hacer una 
	*/
	SetConsoleCursorPosition(ID_consol, pos_cursor);
	//una vez pocicionado el cursor donde queremos imprimimos un un caracter
}

void hide_cursor()
{
	HANDLE ID_console;
	ID_console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursor;
	cursor.bVisible = false; // false para aserlo invisible O cursor.dwSize para el tamaño
	cursor.dwSize = 10;
	SetConsoleCursorInfo(ID_console,&cursor);
}

/* Limites del ecenario */
void limites() {
	// los limites finales del eceanario son x = 0 - 77 , y = 0 - 22 sin tomar en cuenta que empieza en 4,4
	// por lo que las dimenciones en cordenadas reales son de x = 4 - 81, y = 4 - 26

	for (int y = 0; y < 25; y++) 
	{
		goto_xy(4, 4 + y);
		for (int x = 0; x < 80; x++) 
		{
			// imprimimos las lineas verticales y horizontales 
			if (y == 0 || y == 24) 
			{ 
				printf("%c", 205); 
			} 
			else if (x == 0 || x == 79) 
			{ 
				printf("%c", 186); 
			} 
			else { printf(" "); }
		}
		
	}
	// imprimimos las esquinas con codigo ascii
	goto_xy(4, 4);	printf("%c", 201);
	goto_xy(83,4);	printf("%c", 187);
	goto_xy(4,28);	printf("%c", 200);
	goto_xy(83,28);	printf("%c", 188);
}

bool limite_nave(int x,int y)
{
	if (x > 4 && x < 81 && y > 4 && y < 26) return true;
	else return false;
}

/*la funcion aleatorio debuelve un numero en un rango determinado */
int aleatorio(int min, int max)
{
	std::uniform_int_distribution<unsigned int> intAleatorio(min, max);
	return intAleatorio(motor);
}

std::pair<bool, bool> colicion(nave Obj1,std::vector<Meteoro> meteoros)
{
	join_cordenadas cordenadas_meteoro;
	join_cordenadas cordenadas_nave;
	for (auto i : meteoros) {
		cordenadas_meteoro = i.get_xy(); // obtenemos las cordenadas de cada meteoro
		cordenadas_nave = 

	}
	return std::pair<bool, bool>();
}

void nave::dibujar()
{
	/*
		cuando se imprime un valor automaticamente se recorre el puntero en una unidad en el eje x 
	*/
	goto_xy(X + 1, Y); printf("%c",94);
	goto_xy(X, Y + 1); printf("%c%c%c",47,176,92);
	goto_xy(X, Y + 2); printf("%c%c%c",126,126,126);
}

void nave::borrar()
{
	goto_xy(X , Y);  printf("   ");
	goto_xy(X, Y + 1);	printf("   ");
	goto_xy(X, Y + 2);	printf("   ");
}

void nave::mover(char tecla)
{
	int _x = X, _y = Y;
		switch (tecla)
		{
			case 'w': (_y)--; break;
			case 's': (_y)++; break;
			case 'd': (_x)++; break;
			case 'a': (_x)--; break;
			default: break;
		}
		if(limite_nave(_x,_y))
		{
			borrar();
			X = _x;
			Y = _y;
			dibujar();
		}
}

void nave::disparo()
{
	proyectiles.push_back(proyectil(X, Y));
	// es necesario dibujarlo para que se vea ya que el metodo mover de los proyectiles 
	// no dibuja el primer disparo
	(proyectiles.back()).dibujar();
}

void nave::explotar_Nave()
{
	goto_xy(X, Y);      printf("* *");
	goto_xy(X, Y + 1);	printf(" * ");
	goto_xy(X, Y + 2);	printf("* *");

}

void Meteoro::dibujar_meteoro()
{
	goto_xy(mx, my); printf("%c", 157); 
}

void Meteoro::borrar_meteoro()
{
	goto_xy(mx, my); printf(" ");
}

void Meteoro::mover_meteoro()
{
	if (my <= 26)
   	{
		borrar_meteoro();
		my++;
		dibujar_meteoro();
	}
	else if(my > 26)
	{
		borrar_meteoro();
		mx = aleatorio(5, 82);
		my = 5;
		dibujar_meteoro();
	}
}

void Meteoro::explotar_meteoro()
{
	goto_xy(mx, my); printf("*");
}
