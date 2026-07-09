#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <string.h>
#include <windows.h>
#define record struct

using namespace std;
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
    record tm *timeinfo;

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
using namespace FechaHora;
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
	    char cuotas[5];
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
	
	void ordenamiento(MovimientosCA datos_ca[], MovimientosTD datos_td[], MovimientosTC datos_tc[]){
		bool intercambio;
		
		for (int i = 0; i < 15 - 1; i++) {
		    intercambio = false;
		
		    for (int j = 0; j < 15 - 1 - i; j++) {
		        if (datos_ca[j].fecha < datos_ca[j + 1].fecha) {
		            swap(datos_ca[j], datos_ca[j + 1]);
		            intercambio = true;
		        }
		    }
		
		    if (!intercambio)
		        break;
		}
		
		for (int i = 0; i < 15 - 1; i++) {
		    intercambio = false;
		
		    for (int j = 0; j < 15 - 1 - i; j++) {
		        if (datos_td[j].fecha < datos_td[j + 1].fecha) {
		            swap(datos_td[j], datos_td[j + 1]);
		            intercambio = true;
		        }
		    }
		
		    if (!intercambio)
		        break;
		}
		
		for (int i = 0; i < 15 - 1; i++) {
		    intercambio = false;
		
		    for (int j = 0; j < 15 - 1 - i; j++) {
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
    
    cout << "MOVIMIENTOS CAJA DE AHORRO" << endl;
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
        datos_ca[i].importe = 123000.00 + (i * 1000.50);
    }
    
    cout << "MOVIMIENTOS TARJETA DE DEBITO" << endl;
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
        datos_td[i].importe = 4500.00 + (i * 100.00);           
    }
    
    cout << "MOVIMIENTOS TARJETA DE CREDITO" << endl;   
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
        datos_tc[i].importe = 25000.00 + (i * 500.00);          
    }
	ordenamiento(datos_ca, datos_td, datos_tc);
	}
	
	void impresion(MovimientosCA datos_ca[], MovimientosTD datos_td[], MovimientosTC datos_tc[]){
	    ofstream archCA("MovimientosCA.Txt");
	    ofstream archTD("MovimientosTD.Txt");    
	    ofstream archTC("MovimientosTC.Txt");
    
	    if(!archCA.is_open() || !archTD.is_open() || !archTC.is_open()) {
	        cout << "Error critico al crear los archivos en disco." << endl;
	        return;
	    }

	    for(short i = 0; i < 15; i++) {
	        archCA << right << setw(2) << datos_ca[i].dia << " "
	               << right << setw(2) << datos_ca[i].mes << " "
	               << right << setw(4) << datos_ca[i].anio << " "
	               << datos_ca[i].tipoMov << " "
	               << left << setw(25) << datos_ca[i].detalle << " "
	               << right << fixed << setprecision(2) << setw(8) << datos_ca[i].importe << endl;
	    }
    
	    for(short i = 0; i < 15; i++) {
	        archTD << right << setw(2) << datos_td[i].dia << " "
	               << right << setw(2) << datos_td[i].mes << " "
	               << right << setw(4) << datos_td[i].anio << " "
	               << left << setw(25) << datos_td[i].detalle << " "
	               << right << fixed << setprecision(2) << setw(8) << datos_td[i].importe << endl;
	    }

	    for(short i = 0; i < 15; i++) {
	        archTC << right << setw(2) << datos_tc[i].dia << " "
	               << right << setw(2) << datos_tc[i].mes << " "
	               << right << setw(4) << datos_tc[i].anio << " "
	               << left << setw(25) << datos_tc[i].detalle << " "
	               << left << setw(5) << datos_tc[i].cuotas << " "
	               << right << fixed << setprecision(2) << setw(8) << datos_tc[i].importe << endl;
	    }

	    archCA.close();
	    archTD.close();
	    archTC.close();		
	}
	void CreacionArchivos(){
		MovimientosCA datos_ca[15];
		MovimientosTD datos_td[15];	
		MovimientosTC datos_tc[15];
		
		carga(datos_ca, datos_td, datos_tc);
		impresion(datos_ca, datos_td, datos_tc);
	}
	
	int main(){
		cout << "Iniciando generador de archivos base..." << endl;
		CreacionArchivos();	
		return 0;
	}