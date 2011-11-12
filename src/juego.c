#include <stdlib.h>
#include <stdio.h>
#include "juego.h"

int modo = 0;
int turno = 0;
int comer = 0;

int turnos[MODOS][COLORES+1] = {
						{FICHA_VERDE,FICHA_AMARILLA,NADA,NADA,NADA,NADA,NADA},
						{FICHA_VERDE,FICHA_AZUL,FICHA_ROJA,NADA,NADA,NADA,NADA},
						{FICHA_VERDE,FICHA_AZUL,FICHA_AMARILLA,FICHA_NEGRA,NADA,NADA,NADA},
						{FICHA_VERDE,FICHA_MARRON,FICHA_AZUL,FICHA_ROJA,FICHA_NEGRA,NADA,NADA},
						{FICHA_VERDE,FICHA_MARRON,FICHA_AZUL,FICHA_AMARILLA,FICHA_ROJA,FICHA_NEGRA,NADA},
						{FICHA_VERDE,FICHA_AMARILLA,FICHA_MARRON,FICHA_ROJA,FICHA_AZUL,FICHA_NEGRA,NADA},
						{FICHA_VERDE,FICHA_AZUL,FICHA_AMARILLA,FICHA_NEGRA,NADA,NADA,NADA},
						{FICHA_VERDE,FICHA_MARRON,FICHA_AZUL,FICHA_AMARILLA,FICHA_ROJA,FICHA_NEGRA,NADA},					
						{FICHA_VERDE,FICHA_AMARILLA,FICHA_MARRON,FICHA_ROJA,NADA,NADA,NADA}					
					};

int puntajes[COLORES] = {0,0,0,0,0,0};

int fichasxturno[MODOS] = {1,1,1,1,1,2,2,3,2};

int tablero[17][13] = 	{
			       {0,0,0,0,0,0,1,0,0,0,0,0,0},
			      {0,0,0,0,0,0,1,1,0,0,0,0,0},
			       {0,0,0,0,0,1,1,1,0,0,0,0,0},
			      {0,0,0,0,0,1,1,1,1,0,0,0,0},
			       {1,1,1,1,1,1,1,1,1,1,1,1,1},
			      {0,1,1,1,1,1,1,1,1,1,1,1,1},
			       {0,1,1,1,1,1,1,1,1,1,1,1,0},
			      {0,0,1,1,1,1,1,1,1,1,1,1,0},
			       {0,0,1,1,1,1,1,1,1,1,1,0,0},
			      {0,0,1,1,1,1,1,1,1,1,1,1,0},
			       {0,1,1,1,1,1,1,1,1,1,1,1,0},
			      {0,1,1,1,1,1,1,1,1,1,1,1,1},
			       {1,1,1,1,1,1,1,1,1,1,1,1,1},
			      {0,0,0,0,0,1,1,1,1,0,0,0,0},
			       {0,0,0,0,0,1,1,1,0,0,0,0,0},
			      {0,0,0,0,0,0,1,1,0,0,0,0,0},
			       {0,0,0,0,0,0,1,0,0,0,0,0,0}
			};

int iniciales[COLORES][FICHAS*2] =
					{
							{0,6,1,6,1,7,2,5,2,6,2,7,3,5,3,6,3,7,3,8},
							{13,5,13,6,13,7,13,8,14,5,14,6,14,7,15,6,15,7,16,6},
							{4,0,4,1,4,2,4,3,5,1,5,2,5,3,6,1,6,2,7,2},
							{9,11,10,10,10,11,11,10,11,11,11,12,12,12,12,11,12,10,12,9},
							{9,2,10,1,10,2,11,1,11,2,11,3,12,0,12,1,12,2,12,3},
							{4,12,4,11,4,10,4,9,5,12,5,11,5,10,6,11,6,10,7,11}
					};

int fichas_comidas[COLORES] = {0,0,0,0,0,0};

/* Esta funcion indica si la ficha consultada pertenece o no al turno actual. */
int FichaEnTurno(int ficha)
{
	int i;
	for (i=0;i<fichasxturno[modo];i++)
	{
		if (ficha == turnos[modo][turno+i]) return TRUE;
	}
	return FALSE;
}

/* Retorna el color de la ficha cuya base corresponde como objetivo al color de la ficha consultada.*/
int DameContrario(int ficha)
{
	switch (ficha)
	{
		case FICHA_AMARILLA: return FICHA_VERDE;
		case FICHA_VERDE: return FICHA_AMARILLA;
		case FICHA_AZUL: return FICHA_NEGRA;
		case FICHA_NEGRA: return FICHA_AZUL;
		case FICHA_ROJA: return FICHA_MARRON;
		case FICHA_MARRON: return FICHA_ROJA;
	}
}

/* Verifica si puede o no comer la ficha, en caso positivo elimina la ficha comida del trablero, 
   lo contabiliza y actualiza el puntaje del que la comio. */
void ComerFicha(int m,int n)
{
	if (!comer){return;}
	if (FichaEnTurno(tablero[m][n])){return;}
	fichas_comidas[DameIndice(tablero[m][n])]++;
	tablero[m][n] = LIBRE;
	puntajes[turno]++;
}

/* Verifica si el movimiento es valido o no, en caso positivo indica que tipo de movimiento fue realizado.*/
int MovidaValida(int m0, int n0, int m1, int n1,int mirar)
{
	if ( (m1 < 0 || m1 > 16) || (n1 < 0 || n1 > 12)) {return FALSE;}
	if (tablero[m1][n1]!=LIBRE) {return FALSE;}
	//return TRUE;

	int rest_m = (m1-m0);
	int rest_n = (n1-n0);
	int dist_m = abs(rest_m);
	int dist_n = abs(rest_n);
	int sent_m = (rest_m) ? (rest_m)/dist_m : 0 ;
	int sent_n = (rest_n) ? (rest_n)/dist_n : 0;
	int paridad = (m0%2==0) ? 1 : -1;

	if( (m0+(2*sent_m)==m1 && sent_m!=0 ) && (n0+(sent_n)==n1 && sent_n!=0))
	{
		if (tablero[m0+(sent_m)][n0-1+((paridad+1)/2)+((sent_n+1)/2)]>LIBRE)
		{
			if (!mirar){ComerFicha(m0+(sent_m),n0-1+((paridad+1)/2)+((sent_n+1)/2));}
			return PASO_LARGO;
		}
	}
	if( m0==m1 && (n0+(2*sent_n)==n1 && tablero[m0][n0+(sent_n)]>LIBRE) ) 
	{
		if(!mirar){ComerFicha(m0,n0+(sent_n));}
		return PASO_LARGO;
	}
	if( m0+(sent_m)==m1 && n0==n1 ) {return PASO_CORTO;}
	if( m0==m1 && n0+(sent_n)==n1 ) {return PASO_CORTO;}
	if( m0+(sent_m) == m1 && n0+paridad == n1 ) {return PASO_CORTO;}
	return FALSE;
}

/* Verifica si es el unico turno con fichas en el tablero. */
int TurnoSobreviviente()
{
	int i;
	for(i=0;turnos[modo][i]!=NADA;i+=fichasxturno[modo])
	{
			if(i!=turno && TotalFichasTurno(i)){return FALSE;}
	}
	return TRUE;
}

/* Indica el total de fichas en el tablero que posee el turno consultado. */
int TotalFichasTurno(int algunturno)
{
	int i,total;
	total = 0;
	for(i=0;i<fichasxturno[modo];i++)
	{
		total+= (FICHAS-fichas_comidas[DameIndice(turnos[modo][algunturno+i])]);
	}
	return total;
}

/* Indica el puntaje actual del turno consultado. */
int PuntajeTurno(int algunturno) /* Mayor puntaje conseguido */
{
	int i,acumulado;
	acumulado = 0;
	for (i=0;i<fichasxturno[modo];i++)
	{
		acumulado+=FichasEnBase(turnos[modo][algunturno+i]);
	}
	return (puntajes[algunturno]+acumulado);
}

/* Contabiliza la cantidad de fichas que estan en la base destino. */
int FichasEnBase(int ficha)
{
	int i,j,cont;
	cont = 0;
	i = DameIndice(DameContrario(ficha));
	for (j=0;j<20;j+=2)
	{
		if (tablero[iniciales[i][j]][iniciales[i][j+1]] == ficha){cont++;}
	}
	return cont;
}

/* Indica el total de fichas del turno consultado las cuales se encuentran dentro de la base objetivo. */
int TotalFichasEnBase(int algunturno)
{
	int i,fichasenbase = NADA;
	for (i=0;i<fichasxturno[modo];i++)
	{
		fichasenbase+=FichasEnBase(turnos[modo][algunturno+i]);
	}
	return fichasenbase;
}

/* Condicion de fin del juego, mientras no todos los turnos hayan llegado. */
int FinJuego()
{
	int i,fichasenbase,fichastotal;
	fichasenbase = fichastotal = NADA;

	for (i=0;turnos[modo][i]!=NADA;i+=fichasxturno[modo])
	{
		fichastotal = TotalFichasTurno(i);
		//printf("turno: %d total: %d\n",i,fichastotal);
		fichasenbase = TotalFichasEnBase(i);
		//printf("base: %d\n",fichasenbase);
		if (comer)
		{
			if (fichasenbase!=fichastotal){return FALSE;}
		}
		else
		{
			if (fichasenbase==fichastotal)
			{
				//printf("RI: %d\n",TurnoGanador());
				return TRUE;
			}
		}
	}
	if (!comer){return FALSE;}
	//printf("RO: %d\n",TurnoGanador());
	return TRUE;
}

/* Retorna el turno quien tiene el mayor puntaje */
int TurnoGanador()
{
	int i,mayorpuntaje,mayorturno,empate,puntaje;
	empate = FALSE;
	mayorpuntaje = NADA;
	for (i=0;turnos[modo][i]!=NADA;i+=fichasxturno[modo])
	{
		puntaje = PuntajeTurno(i);
	//	printf("T: %d, P: %d\n",i,puntaje);
		if (puntaje>=mayorpuntaje && TotalFichasTurno(i)!=0)
		{
			if (puntaje==mayorpuntaje){empate = TRUE;}else{empate=FALSE;}
			mayorturno = i;
			mayorpuntaje = puntaje;
		}
	}
	if(empate) {return -1;}
	return mayorturno;
}

/* Verifica si el turno actual todavia puede realizar movidas. */
int SigueJugando(int m,int n)
{
	if (MovidaValida(m,n,m-2,n+1,TRUE)) return TRUE;
	if (MovidaValida(m,n,m-2,n-1,TRUE)) return TRUE;
	if (MovidaValida(m,n,m,n-2,TRUE)) return TRUE;
	if (MovidaValida(m,n,m,n+2,TRUE)) return TRUE;
	if (MovidaValida(m,n,m+2,n-1,TRUE)) return TRUE;
	if (MovidaValida(m,n,m+2,n+1,TRUE)) return TRUE;
	return FALSE;
}

/* Pasa el control al siguiente turno. */
void SiguienteTurno()
{
	int totalfichasturno = NADA;
	do
	{
		if (turnos[modo][turno+=fichasxturno[modo]] == NADA){turno = 0;}
		totalfichasturno = TotalFichasTurno(turno);
	}
	while((totalfichasturno==0)||(totalfichasturno==TotalFichasEnBase(turno)));
}

/* Coloca las fichas del color en cuestion en su posicion inicial. */
void PintarTablero(int ficha)
{
	int i,j;
	i = DameIndice(ficha);
	for (j=0;j<(FICHAS*2);j+=2)
	{
		tablero[iniciales[i][j]][iniciales[i][j+1]] = ficha;
	}
}

/* Inicializa todos los valores del tablero */
void InicializarTablero()
{
	int i = 0;
	while(turnos[modo][i]!=NADA)
	{
		PintarTablero(turnos[modo][i]);
		i++;
	}
}
