#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <ctime>
#include <windows.h>
#include <cstdlib>
#include <sstream>
#define record struct

using namespace std;

enum Colores { NEGRO = 0, AZUL = 1, VERDE = 2, CIAN = 3, ROJO = 4, MAGENTA = 5,
                 MARRON = 6, GRIS_CLARO = 7, GRIS_OSCURO = 8, AZUL_CLARO = 9,
                 VERDE_CLARO = 10, CIAN_CLARO = 11, ROJO_CLARO = 12,
                 MAGENTA_CLARO = 13, AMARILLO = 14, BLANCO = 15
             };

typedef char str25[26];

namespace MenuyExt {
    void Borrado(short c);
}
using namespace MenuyExt;

namespace Screen {

  void setConsoleColor(WORD colText, WORD colBack) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),colText + 16*colBack);
  } // setConsoleColor

  void _window(short izq, short sup, short der, short inf) { // Clon de window()
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    SMALL_RECT window = csbi.srWindow;

    window.Left = izq - 1;
    window.Top = sup - 1;
    window.Right = der - 1;
    window.Bottom = inf - 1;
  } // _window

  void _gotoxy(short x, short y) { // Clon de gotoxy()
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    SMALL_RECT window = csbi.srWindow;
    COORD coord;

    coord.X = window.Left + x;
    coord.Y = window.Top + y;
    if (coord.X <= window.Right && coord.Y <= window.Bottom)
        SetConsoleCursorPosition(hConsole, coord);
  } // _gotoxy

  int _wherex() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
      return csbi.dwCursorPosition.X;
    return -1;
  } // _wherex

  int _wherey() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
      return csbi.dwCursorPosition.Y;
    return -1;
  } // _wherey

  void ActualizaColores(WORD colTextAct, WORD colBackAc) {
  	                         // Clon de textcolor() y textbackground() combinados.
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // En Windows, el fondo se desplaza 4 bits a la izq. (se multiplica por 16)
    WORD atributo = colTextAct | (colBackAc << 4);
    SetConsoleTextAttribute(hConsole, atributo);
  } // ActualizaColores

  WORD ObtenerColorTextoActual() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        // Aplica una máscara para quedarse únicamente con el color del texto
        return csbi.wAttributes & 0x000F;
    }
    return 0; // Retorna 0 (negro) si hubo un error
  } // ObtenerColorTextoActual

  WORD ObtenerColorFondo() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        // El color de fondo está en los primeros 4 bits de la parte alta
        // Por lo tanto, dividimos entre 16 para extraerlo
        return (csbi.wAttributes >> 4) & 0x0F;
    }
    return -1; // Retorna -1 si ocurre un error
  } // ObtenerColorFondo

  void _textcolor(WORD color) {
    ActualizaColores(color,ObtenerColorFondo());
  } // _textcolor

  void _textbackground(WORD color) {
    ActualizaColores(ObtenerColorTextoActual(),color);
  } // _textbackground

  void _clrscr() { //Clon de _clrscr() LOCALIZADO (Solo limpia área de la ventana)
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    SMALL_RECT window = csbi.srWindow;
    DWORD escritos;
    WORD atributo = ObtenerColorTextoActual() | (ObtenerColorFondo() << 4);
    // Calcular ancho y alto de nuestra ventana
    int ancho = window.Right-3 - window.Left;

    window.Left += 2;
    // Limpiar fila por fila dentro de los límites coordenados
    for (short y = window.Top+2; y <= window.Bottom-1; ++y) {
        COORD inicio_fila = { window.Left, y };
        // Llena la fila actual con espacios en blanco
        FillConsoleOutputCharacter(hConsole, ' ', ancho, inicio_fila, &escritos);
        // Aplica el color de fondo actual a esos espacios en blanco
        FillConsoleOutputAttribute(hConsole,atributo,ancho,inicio_fila,&escritos);
    }
    _gotoxy(1, 1); // Igual a Borland, al limpiar, regresa a (1,1) relativo.
  } // _clrscr

  void _clreol() {
    COORD coord;
    DWORD escrito;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info; // Obtener la posición actual del cursor
    GetConsoleScreenBufferInfo(hStdOut, &info);

    coord = info.dwCursorPosition;
    // Calcular cuantos caracteres faltan para el final de la línea
    int longitud = info.dwSize.X - coord.X;
    // Sobrescribir con espacios y restaurar posición del cursor
    FillConsoleOutputCharacter(hStdOut, ' ', longitud, coord, &escrito);
    SetConsoleCursorPosition(hStdOut, coord);
  } // _clreol

  void MnsgBox(short x, short y, string mensaje, char alinea, short ancho = 0) {
    _gotoxy(x,y);
    if (alinea == 'i')
      cout << setw(ancho) << left << mensaje;
  	else
      cout << setw(ancho) << right << mensaje;
  } // MnsgBox

  void MnsgBox(short x, short y, string mensaje) {
    _gotoxy(x,y);
    cout << mensaje;
  } // MnsgBox

  void MnsgBox(short x, short y, short colorText, string mensaje) {
    _gotoxy(x,y);
    _textcolor(colorText);
    cout << mensaje;
  } // MnsgBox

  void Marco(short x1, short y1, short x2, short y2, short colorTexto) {
	_textcolor(colorTexto);
    MnsgBox(x1, y1, "╔");
    MnsgBox(x2 - 1, y1, "╗");
    for (short i = 1; i < x2 - x1 - 1; i++)
        MnsgBox(x1 + i, y1, "═");

    for (short i = 1; i < y2 - y1 - 1; i++) {
        MnsgBox(x1, y1 + i, "║");
        MnsgBox(x2 - 1, y1 + i, "║");
    }
    MnsgBox(x1, y2 - 1, "╚");
    MnsgBox(x2 - 1, y2 - 1, "╝");
    for (short i = 1; i < x2 - x1 - 1; i++)
        MnsgBox(x1 + i, y2 - 1, "═");
  } // Marco

  string Separador(int ancho = 60, char car = '-') {
    return string(ancho, car);
  } // Separador

  void OcultarCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(consoleHandle, &info);
    info.bVisible = FALSE; // Oculta el cursor
    SetConsoleCursorInfo(consoleHandle, &info);
  } // OcultarCursor

  void MostrarCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(consoleHandle, &info);
    info.bVisible = TRUE; // Muestra el cursor
    SetConsoleCursorInfo(consoleHandle, &info);
  } //MostrarCursor

	void Espera(short tiempo) {
		_textcolor(7);
  	MnsgBox(5,20,"Espere "+to_string(tiempo/1000)+" segundos");
		_textcolor(15);
    Sleep(tiempo);
    _gotoxy(5,20);
    Borrado(30);
  } // Espera

  void BarraTitulo() {
    SetConsoleTitleA("Home Banking Bank-e");
  } // BarraTitulo

  void Pausa(string mensg="") {
	  short i = 1;

    MnsgBox(12,8,mensg);
    GetAsyncKeyState(VK_SPACE);
    while(not GetAsyncKeyState(VK_SPACE)) {
			_textbackground(8);
  	  MnsgBox(5,17,i,"Oprima la tecla");
  	  setConsoleColor(AZUL,VERDE);
	    MnsgBox(21,17,AMARILLO+16*VERDE,"ESPACIO");
		  _textbackground(8);
	    MnsgBox(29,17,i++,"para continuar...");
  	  Sleep(1000);
	    if (i == 16)
		  	i = 1;
    }
  } // Pausa

  void Pausa(int y) {
	  short i = 1;
        string mensg="";
    MnsgBox(12,8,mensg);
    GetAsyncKeyState(VK_SPACE);
    while(not GetAsyncKeyState(VK_SPACE)) {
			_textbackground(8);
  	  MnsgBox(5,y,i,"Oprima la tecla");
  	  setConsoleColor(AZUL,VERDE);
	    MnsgBox(21,y,AMARILLO+16*VERDE,"ESPACIO");
		  _textbackground(8);
	    MnsgBox(29,y,i++,"para continuar...");
  	  Sleep(1000);
	    if (i == 16)
		  	i = 1;
    }
  } // Pausa

void BloquearCambioTamano(){
    HWND hwnd = GetConsoleWindow(); // Obtiene identificador de la ventana.
    // Obtiene los estilos actuales de la ventana
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~WS_MAXIMIZEBOX; // Quita botones de maximizar y redimensionar.
    style &= ~WS_SIZEBOX;
    SetWindowLong(hwnd, GWL_STYLE, style); // Aplica los nuevos estilos
} // BloquearCambioTamaño

  string Left(str25 cad, short cant) {
    return string(cad,cant);
  } // Left

  int MenuNavegar(char aMenu[][25],short lIni, short lFin, short cIni) {
    int Menu = lIni;

    OcultarCursor();
    MnsgBox(6,20,"Flecha ↑ ↓ para moverse sobre el menú");
    MnsgBox(6,21,"Tecla ESCAPE seleccionar la opción del menú");
    //GoToXY(cIni,lIni);
    _gotoxy(cIni,lIni);
    while(not GetAsyncKeyState(VK_ESCAPE)) {
      Sleep(200);
      if (GetAsyncKeyState(VK_UP)) {
        if (Menu == lIni) {
          _textcolor(15);
          _gotoxy(cIni,lIni);
          cout << aMenu[Menu-lIni];
          Menu = lFin;
          _textcolor(14);
          _gotoxy(cIni,lFin);
          cout << aMenu[Menu-lIni];
        }else{
          _textcolor(15);
          _gotoxy(cIni,_wherey());
          cout << aMenu[Menu-lIni];
          Menu--;
          _gotoxy(cIni,_wherey()-1);
          _textcolor(14);
          cout << aMenu[Menu-lIni];
        }
      }else if (GetAsyncKeyState(VK_DOWN)) {
        if (Menu == lFin) {
          _textcolor(15);
          _gotoxy(cIni,lFin);
          cout << aMenu[Menu-lIni];
          Menu = lIni;
          _gotoxy(cIni,lIni);
          _textcolor(14);
          cout << aMenu[Menu-lIni];
        }else {
          _textcolor(15);
          _gotoxy(cIni,_wherey());
          cout << aMenu[Menu-lIni];
          Menu++;
          _gotoxy(cIni,_wherey()+1);
          _textcolor(14);
          cout << aMenu[Menu-lIni];
        }
      }
    } // Fin While
    MostrarCursor();
    return Menu - lIni;
  } // MenuNavegar
}
namespace FechaHora {

  long GetTime(int &hh, int &mm, int &ss) {
    time_t     rawtime;
    record tm *timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    hh = timeinfo->tm_hour;
    mm = timeinfo->tm_min;
		ss = timeinfo->tm_sec;
    return timeinfo->tm_hour * 10000 + timeinfo->tm_min * 100 + timeinfo->tm_sec;
  } // GetTime

  long GetDate(int &year, int &mes, int &dia, int &ds) {
    time_t     rawtime;
    struct tm *timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    year = 1900 + timeinfo->tm_year;
    mes  = 1 + timeinfo->tm_mon;
    dia  = timeinfo->tm_mday;
    ds   = 1 + timeinfo->tm_wday;
    return (1900 + timeinfo->tm_year) * 10000 + (1 + timeinfo->tm_mon) * 100
            + timeinfo->tm_mday;
  } // GetDate

  int FechaHoy() {
    int ano, mes, dia, dsem;

    GetDate(ano,mes,dia,dsem);
    cout << "Dia: " << setw(2) << dia << " Mes: " << setw(2) << mes
         << " Año:" << ' ' << setw(2) << ano << endl;
	  return dia * 10000 + mes * 100 + ano;
  } // FechaHoy

}

const int MAX_USUARIOS = 5;

using namespace Screen;
using namespace FechaHora;

namespace Archivos{
	typedef record{
		long fecha;
		short dia, mes, anio;
	    char tipoMov;
	    char detalle[25];
	    float importe;
	}MovimientosCA;

	typedef record{
		long fecha;
		short dia, mes, anio;
	    char detalle[25];
	    float importe;
	}MovimientosTD;

	typedef record{
		long fecha;
		short dia, mes, anio;
	    char detalle[25];
	    char cuotas[6];
	    float importe;
	}MovimientosTC;

	const char asuntosCA[5][25] = {
	    "Dep._Haberes_Mensual",
	    "Extraccion_Cajero_Link",
	    "Transf._Recibida_Varios",
	    "Pago_Intereses_PlazoF",
	    "Reintegro_Promocion_Bco"
	};


	const char asuntosTC[5][25] = {
	    "Supermercado_COTO",
	    "Estacion_Servicio_YPF",
	    "Suscripcion_Streaming",
	    "Compra_MercadoLibre",
	    "Pasajes_Aerolineas_Arg"
	};
	
	const char asuntosTD[5][25] = {
	    "Farmacia_Del_Centro",
	    "Pago_Servicios_Luz",
	    "Compra_Kiosco_24hs",
	    "Restaurante_Comida_Rap",
	    "Carniceria_La_Estancia"
	};

	const int MAX_MOVIMIENTOS = 100;
	MovimientosCA datos_ca[MAX_MOVIMIENTOS];
	MovimientosTD datos_td[MAX_MOVIMIENTOS];
	MovimientosTC datos_tc[MAX_MOVIMIENTOS];
	int cantDatosCA = 0;
	int cantDatosTD = 0;
	int cantDatosTC = 0;
	
	void ordenamiento(MovimientosCA datos_ca[], MovimientosTD datos_td[], MovimientosTC datos_tc[],
	                  int cant_ca, int cant_td, int cant_tc){
		bool intercambio;
		
		for (int i = 0; i < cant_ca - 1; i++) {
		    intercambio = false;
		
		    for (int j = 0; j < cant_ca - 1 - i; j++) {
		        if (datos_ca[j].fecha < datos_ca[j + 1].fecha) {
		            swap(datos_ca[j], datos_ca[j + 1]);
		            intercambio = true;
		        }
		    }

		    if (!intercambio)
		        break;
		}
		
		for (int i = 0; i < cant_td - 1; i++) {
		    intercambio = false;
		
		    for (int j = 0; j < cant_td - 1 - i; j++) {
		        if (datos_td[j].fecha < datos_td[j + 1].fecha) {
		            swap(datos_td[j], datos_td[j + 1]);
		            intercambio = true;
		        }
		    }

		    if (!intercambio)
		        break;
		}
		
		for (int i = 0; i < cant_tc - 1; i++) {
		    intercambio = false;
		
		    for (int j = 0; j < cant_tc - 1 - i; j++) {
		        if (datos_tc[j].fecha < datos_tc[j + 1].fecha) {
		            swap(datos_tc[j], datos_tc[j + 1]);
		            intercambio = true;
		        }
		    }

		    if (!intercambio)
		        break;
		}
	}

	void carga(MovimientosCA datos_ca[], MovimientosTD datos_td[], MovimientosTC datos_tc[]){
		srand(time(NULL));
		int diaHoy, mesHoy, anioHoy, ds;

    for(short i = 0; i < 15; i++){

        short d = (rand() % 28) + 1;
        short m = (rand() % 12) + 1;
        short a = 2025 + (rand() % 2);

        long fechaAleatoria = (a * 10000) + (m * 100) + d;

        datos_ca[i].fecha = fechaAleatoria;
        datos_ca[i].dia = d;
        datos_ca[i].mes = m;
        datos_ca[i].anio = a;
        datos_ca[i].tipoMov = (i % 2 == 0) ? 'D' : 'H';

        int indiceRandom = rand() % 5;
        strcpy(datos_ca[i].detalle, asuntosCA[indiceRandom]);
        datos_ca[i].importe = rand() % 10000;
    }


    for(short i = 0; i < 15; i++){
        short d = (rand() % 28) + 1;
        short m = (rand() % 12) + 1;
        short a = 2025 + (rand() % 2);
        long fechaAleatoria = (a * 10000) + (m * 100) + d;

        datos_td[i].fecha = fechaAleatoria;
        datos_td[i].dia = d;
        datos_td[i].mes = m;
        datos_td[i].anio = a;

        int indiceRandom = rand() % 5;
        strcpy(datos_td[i].detalle, asuntosTD[indiceRandom]);
        datos_td[i].importe = rand() % 100000;
    }

    for(short i = 0; i < 15; i++){
        short d = (rand() % 28) + 1;
        short m = (rand() % 12) + 1;
        short a = 2025 + (rand() % 2);
        long fechaAleatoria = (a * 10000) + (m * 100) + d;

        datos_tc[i].fecha = fechaAleatoria;
        datos_tc[i].dia = d;
        datos_tc[i].mes = m;
        datos_tc[i].anio = a;

        int indiceRandom = rand() % 5;
        strcpy(datos_tc[i].detalle, asuntosTC[indiceRandom]);
        strcpy(datos_tc[i].cuotas, "01/03");
        datos_tc[i].importe = rand() % 25000;
    }
	ordenamiento(datos_ca, datos_td, datos_tc, 15, 15, 15);
	}

	void impresion(MovimientosCA datos_ca[], MovimientosTD datos_td[], MovimientosTC datos_tc[], int cant_ca, int cant_td, int cant_tc){
	    ofstream archCA("MovimientosCA.Txt");
	    ofstream archTD("MovimientosTD.Txt");
	    ofstream archTC("MovimientosTC.Txt");

	    for(int i = 0; i < cant_ca; i++) {
	        archCA << right << setw(2) << datos_ca[i].dia << " "
	               << right << setw(2) << datos_ca[i].mes << " "
	               << right << setw(4) << datos_ca[i].anio << " "
	               << datos_ca[i].tipoMov << " "
	               << left << setw(25) << datos_ca[i].detalle << " "
	               << right << fixed << setprecision(2) << setw(11) << datos_ca[i].importe << endl;
	    }
    
	    for(int i = 0; i < cant_td; i++) {
	        archTD << right << setw(2) << datos_td[i].dia << " "
	               << right << setw(2) << datos_td[i].mes << " "
	               << right << setw(4) << datos_td[i].anio << " "
	               << left << setw(25) << datos_td[i].detalle << " "
	               << right << fixed << setprecision(2) << setw(11) << datos_td[i].importe << endl;
	    }

	    for(int i = 0; i < cant_tc; i++) {
	        archTC << right << setw(2) << datos_tc[i].dia << " "
	               << right << setw(2) << datos_tc[i].mes << " "
	               << right << setw(4) << datos_tc[i].anio << " "
	               << left << setw(25) << datos_tc[i].detalle << " "
	               << left << setw(5) << datos_tc[i].cuotas << " "
	               << right << fixed << setprecision(2) << setw(11) << datos_tc[i].importe << endl;
	    }

	    archCA.close();
	    archTD.close();
	    archTC.close();
	}

	void CreacionArchivos(){
		MovimientosCA datos_ca[100];
		MovimientosTD datos_td[100];
		MovimientosTC datos_tc[100];

		carga(datos_ca, datos_td, datos_tc);
		impresion(datos_ca, datos_td, datos_tc, 15, 15, 15);
	}
}
using namespace Archivos;

namespace MenuyExt{
	//tamaño de la ventana
	void RedimensionarVentana(int ancho, int alto){
	    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	    COORD coord = { (SHORT)ancho, (SHORT)alto };
	    SMALL_RECT rect = { 0, 0, (SHORT)(ancho - 1), (SHORT)(alto - 1) };

	    SMALL_RECT tempRect = { 0, 0, 1, 1 };
	    SetConsoleWindowInfo(hConsole, TRUE, &tempRect);

	    SetConsoleScreenBufferSize(hConsole, coord);
	    SetConsoleWindowInfo(hConsole, TRUE, &rect);
	}

	//limpia y construye de nuevo el marco
	void LimpiarInteriorMarco(short x1, short y1, short x2, short y2) {
	    short anchoInterno = (x2 - x1) - 2;  // antes: -1
	    string espacios(anchoInterno, ' ');

	    for (short y = y1 + 1; y < y2; y++) {
	        _gotoxy(x1 + 1, y);
	        cout << espacios;
	    }

	    _gotoxy(x1 + 1, y1 + 1);
	}
	//Borra cierta cantidad de lugares en una linea
	void Borrado(short c){
		for(short i = 0; i < c; i++){
			cout << " ";
		}
	}

	//Impresion de menu
	void GenMenu(char aMenu[][25], short tam){
		LimpiarInteriorMarco(2, 2, 81, 22);
		_textcolor(3);
		_gotoxy(50, 3);
		cout << "HomeBanking Bank-e";

		_textcolor(2);
		_gotoxy(10, 3);
		cout << "Menú Dinámico";

		for(short i = 0; i < tam; i++){
			_gotoxy(25,5 + i);
			_textcolor(15);
			cout << aMenu[i];
		}
	}

	void Plantilla(char titulo[]){
		LimpiarInteriorMarco(2, 2, 81, 22);
		_textcolor(3);
		_gotoxy(50, 3);
		cout << "HomeBanking Bank-e";

		_textcolor(2);
		_gotoxy(10, 3);
		cout << titulo;
	}

	//modificacion de funcion
	void GetDate(int &year, int &mes, int &dia) {
	    time_t     rawtime;
	    struct tm *timeinfo;

	    time ( &rawtime );
	    timeinfo = localtime ( &rawtime );
	    year = 1900 + timeinfo->tm_year;
	    mes  = 1 + timeinfo->tm_mon;
	    dia  = timeinfo->tm_mday;
	}

	//Registro de Usuario
	typedef struct{
		long dni;
		char apellidoNombre[40];
		char fecNac[11];
		char usu[20];
		char clave[20];
		char nroCelular[40];
		char mail[40];
		char domicilio[40];
		char nroCtaCA[40];
		char cbu[40];
	}RegUsuario;

	void OpcToken(){
		OcultarCursor();
		Plantilla("Generar Token");
		srand(time(NULL));

		for(short i = 1; i <= 10; i++){
			if(i <= 6){
				Sleep(500);
				_gotoxy(25 + i,10);
				_textcolor(15);
				short dig = rand() % 9 + 1;
				cout << dig;
			}

			Sleep(500);
			_gotoxy(35 + i,10);
			_textcolor(2);
			cout << "█";

			Sleep(500);
			_gotoxy(50,10);
			_textcolor(15);
			cout << i * 10 << '%';
		}

		_gotoxy(5,13);
		cout << "Se ha generado el Token correctamente";
		_gotoxy(10,15);
		Pausa();
		OcultarCursor();
	}

	void datosINV(float &cap,float &tasa,int time){
		_textcolor(15);
		_gotoxy(30,7); cout << "Capital: ";
		_gotoxy(30,10); cout << "Tiempo: ";
		_gotoxy(30,13); cout << "Tasa: ";

		_textcolor(14);

		do{
			_gotoxy(40,7); cin >> cap;
			if(cap < 0){
				_gotoxy(40,7);
				Borrado(40);
				_textcolor(4);
				_gotoxy(30,8); cout << "*Error, capital menor a cero";
				_textcolor(14);
			}
		}while(cap < 0);

		_gotoxy(30,8);
		Borrado(40);

		do{
			_gotoxy(40,10); cin >> time;
			if(time < 0){
				_gotoxy(40,10);
				Borrado(40);
				_textcolor(4);
				_gotoxy(30,11); cout << "*Error, tiempo menor a cero";
				_textcolor(14);
			}
		}while(time < 0);

		_gotoxy(30,11);
		Borrado(40);

		do{
			_gotoxy(40,13); cin >> tasa;
			if(tasa < 0){
				_gotoxy(40,13);
				Borrado(40);
				_textcolor(4);
				_gotoxy(30,14); cout << "*Error, tasa menor a cero";
				_textcolor(14);
			}
		}while(tasa < 0);
	}

	void OpcSPF(){
		OcultarCursor();
		Plantilla("Simulación Plazo Fijo");
		float cap, tasa;
		int time;
		datosINV(cap, tasa, time);

		_gotoxy(30,14);
		Borrado(40);

		Sleep(1000);
		_textcolor(2);
		_gotoxy(30,15); cout << "Interés: $" << fixed << setprecision(2) << (cap * time * tasa) / 36500.0;

		_gotoxy(10,20);
		Pausa();
		OcultarCursor();
	}

	void agregarLineaArriba(string nombreArchivo, string nuevaLinea){
		ifstream lectura(nombreArchivo);
		stringstream contenidoViejo;
		contenidoViejo << lectura.rdbuf();
		lectura.close();

		ofstream escritura(nombreArchivo);
		escritura << nuevaLinea << endl;
		escritura << contenidoViejo.str();
		escritura.close();
	}

	void cargarMovimientosDesdeArchivos(){
		ifstream archCA("MovimientosCA.Txt");
		ifstream archTD("MovimientosTD.Txt");
		ifstream archTC("MovimientosTC.Txt");

		cantDatosCA = cantDatosTD = cantDatosTC = 0;

		if (archCA.is_open()) {
			int dia, mes, anio;
			char tipoMov;
			char detalle[26];
			float importe;

			while (archCA >> dia >> mes >> anio >> tipoMov >> detalle >> importe) {
				if (cantDatosCA >= MAX_MOVIMIENTOS) break;
				datos_ca[cantDatosCA].dia = dia;
				datos_ca[cantDatosCA].mes = mes;
				datos_ca[cantDatosCA].anio = anio;
				datos_ca[cantDatosCA].tipoMov = tipoMov;
				strcpy(datos_ca[cantDatosCA].detalle, detalle);
				datos_ca[cantDatosCA].importe = importe;
				datos_ca[cantDatosCA].fecha = anio * 10000 + mes * 100 + dia;
				cantDatosCA++;
			}
			archCA.close();
		}

		if (archTD.is_open()) {
			int dia, mes, anio;
			char detalle[26];
			float importe;

			while (archTD >> dia >> mes >> anio >> detalle >> importe) {
				if (cantDatosTD >= MAX_MOVIMIENTOS) break;
				datos_td[cantDatosTD].dia = dia;
				datos_td[cantDatosTD].mes = mes;
				datos_td[cantDatosTD].anio = anio;
				strcpy(datos_td[cantDatosTD].detalle, detalle);
				datos_td[cantDatosTD].importe = importe;
				datos_td[cantDatosTD].fecha = anio * 10000 + mes * 100 + dia;
				cantDatosTD++;
			}
			archTD.close();
		}

		if (archTC.is_open()) {
			string linea;
			while (getline(archTC, linea)) {
				if (linea.empty()) continue;
				if (cantDatosTC >= MAX_MOVIMIENTOS) break;

				stringstream ss(linea);
				int dia, mes, anio;
				string detalle;
				string token;
				float importe = 0;

				if (!(ss >> dia >> mes >> anio >> detalle)) continue;

				if (ss >> token) {
					stringstream valor(token);
					if (valor >> importe) {
						strcpy(datos_tc[cantDatosTC].cuotas, "");
					} else {
						strncpy(datos_tc[cantDatosTC].cuotas, token.c_str(), 5);
						datos_tc[cantDatosTC].cuotas[5] = '\0';
						ss >> importe;
					}
				}

				datos_tc[cantDatosTC].dia = dia;
				datos_tc[cantDatosTC].mes = mes;
				datos_tc[cantDatosTC].anio = anio;
				strcpy(datos_tc[cantDatosTC].detalle, detalle.c_str());
				datos_tc[cantDatosTC].importe = importe;
				datos_tc[cantDatosTC].fecha = anio * 10000 + mes * 100 + dia;
				cantDatosTC++;
			}
			archTC.close();
		}
	}

	void guardarMovimientosEnArchivos(){
		ofstream archCA("MovimientosCA.Txt");
		ofstream archTD("MovimientosTD.Txt");
		ofstream archTC("MovimientosTC.Txt");

		for (int i = 0; i < cantDatosCA; i++) {
			archCA << right << setw(2) << datos_ca[i].dia << " "
			       << right << setw(2) << datos_ca[i].mes << " "
			       << right << setw(4) << datos_ca[i].anio << " "
			       << datos_ca[i].tipoMov << " "
			       << left << setw(25) << datos_ca[i].detalle << " "
			       << right << fixed << setprecision(2) << setw(11) << datos_ca[i].importe << endl;
		}

		for (int i = 0; i < cantDatosTD; i++) {
			archTD << right << setw(2) << datos_td[i].dia << " "
			       << right << setw(2) << datos_td[i].mes << " "
			       << right << setw(4) << datos_td[i].anio << " "
			       << left << setw(25) << datos_td[i].detalle << " "
			       << right << fixed << setprecision(2) << setw(11) << datos_td[i].importe << endl;
		}

		for (int i = 0; i < cantDatosTC; i++) {
			archTC << right << setw(2) << datos_tc[i].dia << " "
			       << right << setw(2) << datos_tc[i].mes << " "
			       << right << setw(4) << datos_tc[i].anio << " "
			       << left << setw(25) << datos_tc[i].detalle << " "
			       << right << fixed << setprecision(2) << setw(11) << datos_tc[i].importe << endl;
		}

		archCA.close();
		archTD.close();
		archTC.close();
	}

	void OpcIPF(){
		OcultarCursor();
		Plantilla("Inversion Plazo Fijo");
		float cap, tasa;
		int time;
		datosINV(cap, tasa, time);

		_gotoxy(30,14);
		Borrado(40);

		Sleep(1000);
		_textcolor(2);
		float inv = (cap * time * tasa) / 36500.0;
		_gotoxy(30,15); cout << "Interés: $" << fixed << setprecision(2) << inv;

		int dia, mes, anio;
		GetDate(anio,mes, dia);

		ostringstream  line;
	    line << right << setw(2) << dia << " "
	            << right << setw(2) << mes << " "
	            << right << setw(4) << anio<< " "
	            << 'H' << " "
	            << left << setw(25) << "Inversion_Plazo_Fijo" << " "
	            << right << fixed << setprecision(2) << setw(11) << inv;

		agregarLineaArriba("MovimientosCA.Txt", line.str());

		_gotoxy(10,20);
		Pausa();
		OcultarCursor();
	}


	void OpcDP(RegUsuario listaUsuarios[], short t){

		OcultarCursor();
		Plantilla("Datos Personales");
		_textcolor(15);
		_gotoxy(30,5);
		cout << "D.N.I: " << listaUsuarios[t].dni << endl;
		_gotoxy(30,6);
		cout << "Ape. Nom: " << listaUsuarios[t].apellidoNombre << endl;
		_gotoxy(30,7);
		cout << "Fecha Nac: " << listaUsuarios[t].apellidoNombre << endl;
		_gotoxy(30,8);
		cout << "Usuario: " << listaUsuarios[t].usu << endl;
		_gotoxy(30,9);
		cout << "Clave: " << listaUsuarios[t].clave << endl;
		_gotoxy(30,10);
		cout << "Nro. Celular: " << listaUsuarios[t].nroCelular << endl;
		_gotoxy(30,11);
		cout << "E-mail: " << listaUsuarios[t].mail << endl;
		_gotoxy(30,12);
		cout << "Domicilio: " << listaUsuarios[t].domicilio << endl;
		_gotoxy(30,13);
		cout << "Nro. Cte CA: " << listaUsuarios[t].nroCtaCA << endl;
		_gotoxy(30,14);
		cout << "CBU: " << listaUsuarios[t].cbu << endl;

		Sleep(3000);
		_gotoxy(10,17);
		Pausa();
		OcultarCursor();
	}

	void OpcCBU(RegUsuario listaUsuarios[], short t){
		OcultarCursor();
		Plantilla("Clave Única Bancaria");
		_textcolor(15);

		_gotoxy(20,10);
		cout << "CBU";

		_gotoxy(20,11);
		cout << listaUsuarios[t].cbu;
		Sleep(3000);
		_gotoxy(10,17);
		Pausa();
		OcultarCursor();
	}

	void OpcMCA(RegUsuario listaUsuarios[], short t){
		ifstream archivo("MovimientosCA.Txt");
		string linea = "";
        int cantLin = 11;

        while (getline(archivo, linea)) {
            cantLin++;
        }
        archivo.close();


        RedimensionarVentana(85, cantLin+8);
        LimpiarInteriorMarco(2, 2, 85, cantLin+6);
		OcultarCursor();
		Plantilla("Movimientos de Caja de Ahorro");
		Marco(2, 2, 81, cantLin+6, AZUL_CLARO);
		_textcolor(BLANCO);
        _gotoxy(10,4);
		FechaHoy();
        _gotoxy(15,7);


		cout << Separador(48, '-');
		_gotoxy(15,8);
		cout << "     Fecha Descripcion                   Importe" << endl;
        _gotoxy(15,9);
        cout << Separador(48, '-');
        _textcolor(ROJO_CLARO);

        archivo.open("MovimientosCA.Txt");

        for(int i = 0; i < cantLin; i++){
            getline(archivo, linea);
            _gotoxy(15, cantLin-2-i);
            cout << linea;
        }

        archivo.close();
        archivo.open("MovimientosCA.Txt");
        int dia, mes, anio;
        string descripcion;
        double monto, montoFinal = 0;


        for(int i = 0; i < cantLin; i++){
            while(archivo >> dia >> mes >> anio >> descripcion >> monto){
                montoFinal += monto;
                _gotoxy(15, cantLin-2-i);
                //cout << dia << "-" << mes << "-" << anio << "-" << descripcion << "-" << monto;
            }
        }

        ostringstream stream;
        stream << fixed << setprecision(2) << montoFinal;

        string textoMonto = stream.str();

        _textcolor(BLANCO);
        _gotoxy(15,cantLin-1);
		cout << Separador(48, '-');
		MnsgBox(15, cantLin, textoMonto, 'd', 48);
		_gotoxy(15,cantLin);
		cout << "                     Total monto: $" << endl;
        _gotoxy(15,cantLin+1);
        cout << Separador(48, '-');

		Sleep(3000);
		Pausa(cantLin+4);
        _textbackground(NEGRO);
        _clrscr();
		RedimensionarVentana(85, 24);
		Marco(2, 2, 81, 23, AZUL_CLARO);

		OcultarCursor();
	}

	void OpcMTD(RegUsuario listaUsuarios[], short t){
		ifstream archivo("MovimientosTD.Txt");
		string linea = "";
        int cantLin = 11;

        while (getline(archivo, linea)) {
            cantLin++;
        }
        archivo.close();


        RedimensionarVentana(85, cantLin+8);
        LimpiarInteriorMarco(2, 2, 85, cantLin+6);
		OcultarCursor();
		Plantilla("Movimientos de Tarjeta de Débito");
		Marco(2, 2, 81, cantLin+6, AZUL_CLARO);
		_textcolor(BLANCO);
        _gotoxy(10,4);
		FechaHoy();
        _gotoxy(15,7);
		cout << Separador(48, '-');
		_gotoxy(15,8);
		cout << "     Fecha Descripcion                   Importe" << endl;
        _gotoxy(15,9);
        cout << Separador(48, '-');
        _textcolor(ROJO_CLARO);

        archivo.open("MovimientosTD.Txt");

        for(int i = 0; i < cantLin; i++){
            getline(archivo, linea);
            _gotoxy(15, cantLin-2-i);
            cout << linea;
        }

        archivo.close();
        archivo.open("MovimientosTD.Txt");
        int dia, mes, anio;
        string descripcion;
        double monto, montoFinal = 0;


        for(int i = 0; i < cantLin; i++){
            while(archivo >> dia >> mes >> anio >> descripcion >> monto){
                montoFinal += monto;
                _gotoxy(15, cantLin-2-i);
                //cout << dia << "-" << mes << "-" << anio << "-" << descripcion << "-" << monto;
            }
        }

        ostringstream stream;
        stream << fixed << setprecision(2) << montoFinal;

        string textoMonto = stream.str();

        _textcolor(BLANCO);
        _gotoxy(15,cantLin-1);
		cout << Separador(48, '-');
		MnsgBox(15, cantLin, textoMonto, 'd', 48);
		_gotoxy(15,cantLin);
		cout << "                        Total TD: $" << endl;
        _gotoxy(15,cantLin+1);
        cout << Separador(48, '-');

		Sleep(3000);
		Pausa(cantLin+4);
        _textbackground(NEGRO);
        _clrscr();
		RedimensionarVentana(85, 24);
		Marco(2, 2, 81, 23, AZUL_CLARO);

		OcultarCursor();
	}
	void OpcDep(){
		OcultarCursor();
		Plantilla("Deposito");
		_textcolor(15);
		float cap;
		char fecha[11];
		int dia, mes, anio;
		string det;

		_gotoxy(30,7); cout << "Fecha: ";
		_gotoxy(30,10); cout << "Monto: ";
		_gotoxy(30,13); cout << "Detalle: ";
		
		_textcolor(14);
		
		_gotoxy(40,7);
		Borrado(40);

		_gotoxy(40,7); cin >> fecha;
		stringstream ss(fecha);
		char barra;
		ss >> dia >> barra >> mes >> barra >> anio;

		do{
			_gotoxy(40,10); cin >> cap;
			if(cap < 0){
				_gotoxy(40,10);
				Borrado(40);
				_textcolor(4);
				_gotoxy(30,11); cout << "*Error, monto menor a cero";
				_textcolor(14);
			}		
		}while(cap < 0);

		_gotoxy(30,11);
		Borrado(40);

		do{
			_gotoxy(40,13); cin >> det;
			if(det.length() > 25){
				_gotoxy(40,13);
				Borrado(40);
				_textcolor(4);
				_gotoxy(30,14); cout << "*Error, detalle demasiado largo";
				_textcolor(14);
			}		
		}while(det.length() > 25);

		_gotoxy(30,14);
		Borrado(40);

		ostringstream  line;
	    line << right << setw(2) << dia << " "
	            << right << setw(2) << mes << " "
	            << right << setw(4) << anio<< " "
	            << 'D' << " "
	            << left << setw(25) << "Deposito" << " "
	            << right << fixed << setprecision(2) << setw(11) << cap;
		agregarLineaArriba("MovimientosCA.Txt", line.str());
		cargarMovimientosDesdeArchivos();
		ordenamiento(datos_ca, datos_td, datos_tc, cantDatosCA, cantDatosTD, cantDatosTC);
		guardarMovimientosEnArchivos();
		
		Sleep(3000);
		_gotoxy(10,17);
		Pausa();
		OcultarCursor();	
	}
	
	//Agregar lineas a sus respectivos archivos en los modulos de compra y deposito
	//Ordenar dichas lineas
	//Borrar los textos de error una vez que se escribe bien (mirar modulo "Inversion de capital")
	//ordenamiento();
	void OpcCom(){
		OcultarCursor();
		Plantilla("Compra");
		_textcolor(15);
		float cap;
		char fecha[11];
		int dia, mes, anio;
		string det;
		string tipo;

		_gotoxy(30,7); cout << "Fecha: ";
		_gotoxy(30,10); cout << "Monto: ";
		_gotoxy(30,13); cout << "Detalle: ";
		_gotoxy(30,16); cout << "Monto D,C: ";
		

		_textcolor(14);
		
		_gotoxy(40,7);
		Borrado(40);

		_gotoxy(40,7); cin >> fecha;
		stringstream ss(fecha);
		char barra;
		ss >> dia >> barra >> mes >> barra >> anio;

		do{
			_gotoxy(40,10); cin >> cap;
			if(cap < 0){
				_gotoxy(40,10);
				Borrado(40);
				_textcolor(4);
				_gotoxy(30,11); cout << "*Error, monto menor a cero";
				_textcolor(14);
			}		
		}while(cap < 0);

		_gotoxy(30,11);
		Borrado(40);

		do{
			_gotoxy(40,13); cin >> det;
			if(det.length() > 25){
				_gotoxy(40,13);
				Borrado(40);
				_textcolor(4);
				_gotoxy(30,14); cout << "*Error, detalle demasiado largo";
				_textcolor(14);
			}		
		}while(det.length() > 25);

		_gotoxy(30,14);
		Borrado(40);

		do{
			_gotoxy(41,16); cin >> tipo;
			if(tipo != "D" && tipo != "C"){
				_gotoxy(40,16);
				Borrado(40);
				_textcolor(4);
				_gotoxy(30,17); cout << "*Error, modo no válido";
				_textcolor(14);
			}		
		}while(tipo != "D" && tipo != "C");

		_gotoxy(30,17);
		Borrado(40);

		ostringstream  line;
	    line << right << setw(2) << dia << " "
	            << right << setw(2) << mes << " "
	            << right << setw(4) << anio<< " "
	            << 'H' << " "
	            << left << setw(25) << det << " "
	            << right << fixed << setprecision(2) << setw(11) << cap;
		agregarLineaArriba("MovimientosCA.Txt", line.str());

		if (tipo == "D"){
			ostringstream  line;
	   		line << right << setw(2) << dia << " "
	            	<< right << setw(2) << mes << " "
	            	<< right << setw(4) << anio<< " "
	            	<< left << setw(25) << det << " "
	            	<< right << fixed << setprecision(2) << setw(11) << cap;
			agregarLineaArriba("MovimientosTD.Txt", line.str());
		}else{
			ostringstream  line;
	   		line << right << setw(2) << dia << " "
	            	<< right << setw(2) << mes << " "
	            	<< right << setw(4) << anio<< " "
	            	<< left << setw(25) << det << " "
					<< right << setw(5) << "     " << " "
	            	<< right << fixed << setprecision(2) << setw(11) << cap;
			agregarLineaArriba("MovimientosTC.Txt", line.str());
		}
		cargarMovimientosDesdeArchivos();
		ordenamiento(datos_ca, datos_td, datos_tc, cantDatosCA, cantDatosTD, cantDatosTC);
		guardarMovimientosEnArchivos();
		
		Sleep(3000);
		_gotoxy(10,19);
		Pausa();
		OcultarCursor();	
	}

	void OpcOU(RegUsuario listaUsuarios[]){
		OcultarCursor();
		Plantilla("Ordenar Usuarios");

		_textcolor(14);
		_gotoxy(5,5); cout << "Listado Usuarios ordenado por Apellido Nombre";
		_gotoxy(5,6); cout << "Apellido Nombre" << setw(13) << "DNI" << " " << "CBU";

		bool intercambio;

		for (int i = 0; i < MAX_USUARIOS; i++){
			intercambio = false;
		
		    for (int j = 0; j < MAX_USUARIOS - 1 - i; j++) {
		        if (listaUsuarios[j].apellidoNombre[0] > listaUsuarios[j + 1].apellidoNombre[0]) {
		            swap(listaUsuarios[j], listaUsuarios[j + 1]);
		            intercambio = true;
		        }
		    }
		
		    if (!intercambio)
		        break;
		}

		_textcolor(15);

		for (int i = 0; i < MAX_USUARIOS; i++){
			_gotoxy(5, i + 7); cout << listaUsuarios[i].apellidoNombre;
			_gotoxy(25, i + 7); cout << listaUsuarios[i].dni;
			_gotoxy(34, i + 7); cout << listaUsuarios[i].cbu;
		}	
		
		Sleep(3000);
		_gotoxy(10,17);
		Pausa();
		OcultarCursor();	
	}

	void OpcCS(){
		OcultarCursor();
		LimpiarInteriorMarco(2, 2, 81, 22);

		_textcolor(2);
		_gotoxy(25,11);
		cout << "Sesion Cerrada";
		_gotoxy(25,12);
		cout << "Gracias por operar con";

		_textcolor(3);
		_gotoxy(25,13);
		cout << "Bank-e";

		Sleep(3000);
		_gotoxy(10,17);
		Pausa();
		OcultarCursor();
	}


}
using namespace MenuyExt;

namespace User{


	short Busqueda(RegUsuario listaUsuarios[], char UserNom[], long UserDNI, char UserClave[]){
		short k = -1;
		for(int i = 0; i < MAX_USUARIOS; i++){
			if(strcmp(listaUsuarios[i].usu, UserNom) == 0 && listaUsuarios[i].dni == UserDNI && strcmp(listaUsuarios[i].clave, UserClave) == 0){
				k = i;
			}
		}
		return k;
	}

	short IniciarSesion(RegUsuario listaUsuarios[]){

		short i = 0;
		short t;
		char UserNom[40];
		long UserDNI;
		char UserClav[20];
		_textcolor(3);
		_gotoxy(50, 3);
		cout << "HomeBanking Bank-e";

		_textcolor(15);
		_gotoxy(10, 3);
		cout << "LOGIN";
			_gotoxy(20, 7); cout << "Nombre de Usuario: " << endl;
			_gotoxy(20, 10); cout << "DNI: " << endl;
			_gotoxy(20, 13); cout << "Contraseña: " << endl;
		do{
			_gotoxy(40, 7);
			Borrado(40);
			_gotoxy(40, 7);
			cin.getline(UserNom, 40);

			_gotoxy(40, 10);
			Borrado(40);
			_gotoxy(40, 10);
			cin >> UserDNI;
			cin.ignore(1000,'\n');

			_gotoxy(40, 13);
			Borrado(20);
			_gotoxy(40, 13);
			cin.getline(UserClav, 20);
			t = Busqueda(listaUsuarios, UserNom, UserDNI, UserClav);

			if(t == -1){
				_gotoxy(5, 18);
				cout << "Datos incorrectos (" << i + 1 << "/3)";
				_gotoxy(10, 18);
				Espera(5000);
				i++;
			}
		}while(t == -1 && i < 3);
		return t;
	}

	void MenuUser(RegUsuario listaUsuarios[], short t){
		short tam = 12;
		char aMenu[12][25] = {{"Token"},
							{"Simulacion Plazo Fijo"},
							{"Inversion Plazo Fijo"},
							{"Datos Personales"},
							{"CBU"},
							{"Movimientos CA"},
							{"Movimientos TD"},
							{"Movimientos TC"},
							{"Deposito"},
							{"Compra"},
							{"Ordenar Usuarios"},
							{"Cerrar Sesion"}};
		int opc;
		do{
			_textbackground(0);
			GenMenu(aMenu, tam);
	 		opc = MenuNavegar(aMenu, 5, 16, 25);
			switch(opc){
				case 0:
					OpcToken();
					break;
				case 1:
					OpcSPF();
					break;
				case 2:
					OpcIPF();
					break;
				case 3:
					OpcDP(listaUsuarios, t);
					break;
				case 4:
					OpcCBU(listaUsuarios, t);
					break;
				case 5:
					OpcMCA(listaUsuarios, t);
					break;
				case 6:
					OpcMTD(listaUsuarios, t);
					break;/*
				case 7:
					OpcMTC(listaUsuarios[]);
					break;*/
				case 8:
					OpcDep();
					break;
				case 9:
					OpcCom();
					break;
				case 10:
					OpcOU(listaUsuarios);
					break;
				case 11:
					OpcCS();
					break;
			}
		}while(opc != 11);
	}
}
using namespace User;

int main(){
	CreacionArchivos();
    SetConsoleOutputCP(CP_UTF8);
    RedimensionarVentana(85, 24);
    BloquearCambioTamano();
    BarraTitulo();
    _clrscr();

  	Marco(2, 2, 81, 23, 3);

    // Inicialización de los 5 usuarios
    RegUsuario listaUsuarios[MAX_USUARIOS] = {
        {12345678, "Juan Gonzalez", "01/01/1990", "jgonz", "clave1", "1234", "a@a.com", "Calle 1", "CA-001", "00000001"},
        {22333444, "Gomez Ana", "05/02/1992", "agomez", "clave2", "5678", "b@b.com", "Calle 2", "CA-002", "00000002"},
        {33444555, "Lopez Luis", "10/03/1985", "llopez", "clave3", "9101", "c@c.com", "Calle 3", "CA-003", "00000003"},
        {44555666, "Diaz Carla", "15/04/1995", "cdiaz", "clave4", "1122", "d@d.com", "Calle 4", "CA-004", "00000004"},
        {1, "1", "1", "1", "1", "1", "1", "1", "1", "1"}
    };


	short t = IniciarSesion(listaUsuarios);
	if(t != -1){
		MenuUser(listaUsuarios, t);
	}else{
		LimpiarInteriorMarco(2, 2, 81, 22);
		_gotoxy(25, 10); cout << "Ud. Deberá dirigirse a un" << endl;
		_gotoxy(25, 11); cout << "Cajero Automático o al propio Banco";
		_gotoxy(20, 12); Pausa();
	}

	return 0;
}
