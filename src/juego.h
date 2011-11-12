
#define FALSE 0
#define TRUE 1

#define NADA 0
#define LIBRE 1
#define FICHA_AMARILLA 2
#define FICHA_VERDE 3
#define FICHA_AZUL 4
#define FICHA_NEGRA 5
#define FICHA_MARRON 6
#define FICHA_ROJA 7
#define NINGUNO -1

#define MODOS 9
#define COLORES 6
#define FICHAS 10

#define PASO_CORTO 1
#define PASO_LARGO 2

#define DameIndice(x) (x-2)

int MovidaValida(int m0, int n0, int m1, int n1,int mirar);
void InicializarTablero();
void SiguienteTurno();
int FinJuego();
int FichaEnTurno(int ficha);
int PuntajeMayorTurnos();
int TotalFichasTurno(int turno);
int TurnoSobreviviente();
int TurnoGanador();
