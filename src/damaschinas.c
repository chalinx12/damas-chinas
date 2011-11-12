#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "juego.h"
#include "bmp.h"
#define PI 3.14159265358979323846

#define M 17
#define N 13		

int width=800;
int height=600;

extern int textures[10];
extern int puntajes[COLORES];
extern int tablero[17][13];
extern int modo;
extern int comer;
extern int turnos[MODOS][COLORES+1];
extern int fichasxturno[MODOS];
extern int fichas_comidas[COLORES];
struct scolor
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
};		
		
float camx,camy,camz; //posicion de la camara
float camang=0; //angulo de la camara
int camdist;//distancia del centro de la camara
int i,j;
int rotar;//1 si se esta rotando la camara 0 en caso contrario
int seli,selj;//posicion seleccionada			

int menusel=-1;//guarda el elemento del menu sobre el cual se encuentra el mouse
int showmenu=1;//que menu se esta mostrando si hay alguno

int ct;

int tablerobase[17][13] = 	{
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
			};//tablero sin fichas

int mov=0;

float centrox,centroy,centroz;//centro del tablero
float animx,animy,animz;//posicion de la ficha que se esta animando
float x0,y01,z0;//posicion inicial de la animacion
double v0x,v0y,v0z,t0;//velocidad y tiempo inicial de la animacion
float g=0.00003;//constante de gravedad
int fichaanim;//valor de la ficha animada
int anim=0;//se esta animando o no alguna ficha			
int gi,gj;//guarda la posicion del tablero a la cual se movio la ficha animada			
int fin=0;//termino la partida?
double tf;//tiempo final de la animacion

			
/*
	definiciones de materiales			
*/			
GLfloat mat_specular[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat mat_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };   
GLfloat mat_shininess[] = { 50.0 };

GLfloat mat2_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat2_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };   
GLfloat mat2_shininess[] = { 1.0 };

GLfloat mat3_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat3_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };   
GLfloat mat3_shininess[] = { 50.0 };

GLfloat fichas_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat fichas_diffuse[6][4] = 	{{ 1.0, 1.0, 0.0, 1.0 },
								{ 0.0, 0.5, 0.0, 1.0 },
								{ 0.0, 0.0, 1.0, 1.0 },
								{ 0.1, 0.1, 0.1, 1.0 },
								{ 1.0, 0.5, 0.0, 1.0 },
								{ 1.0, 0.0, 0.0, 1.0 }};  
								

GLfloat sel_shininess[] = { 3.0 };
/*
	funciones para asignar materiales
*/
int matred()
{
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_diffuse);   	
}
int matwhite()
{
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat3_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat3_shininess);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat3_diffuse);   	
}
int matsel()
{
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat2_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat2_shininess);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat2_diffuse);   	
}

			
GLfloat light_position1[] = { 0, 0, -1, 0.0 };
GLfloat light_position2[] = { 1, 1, 1, 0.0 };
/*
	inicializa las luces
*/
void init_light(void)
{
	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat white_light[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat black_light[] = { 0.0, 0.0, 0.0, 0.0 };	
	GLfloat lmodel_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	
	glLightfv(GL_LIGHT2, GL_POSITION, light_position2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT2, GL_SPECULAR, white_light);	
	
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT1, GL_SPECULAR, white_light);
	
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT2);	
	glEnable(GL_NORMALIZE);

}
/*
	inicializa algunas variables,llama a la funcion para inicializar las luces
	carga las texturas
*/   
void init(void)
{
   glClearColor(1.0, 1.0, 1.0, 0.0);
   glShadeModel(GL_FLAT);	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	camdist=15;
	
	camx=camdist;
	camy=0;
	camz=10;
	seli=-1;
	selj=-1;
	
	centrox=8;
	centroy=6;
	centroz=0.5;
	
	init_light();

	LoadImage1("holz.bmp",0);
	LoadImage1("sphere_map_128x128.bmp",1);
	LoadImage1("stars3.bmp",2);
	LoadImage1("menu.bmp",3);
	LoadImage1("menu2.bmp",4);
}
/*
	dibuja un hueco del tablero
*/
int drawhole(float x,float y,float z)
{
	GLUquadric* quad;
	quad=gluNewQuadric ();
	glShadeModel(GL_FLAT);
	glPushMatrix();
		glScalef(1,1,1);
		glTranslatef(x,y,z);
		gluDisk (quad , 0, 0.5, 10 , 10);
	glPopMatrix();
}
/*
	dibuja una ficha
*/
int drawficha(float x,float y,float z,float r,int color,int sel)
{
	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_SPECULAR, fichas_specular);
	//si una ficha esta seleccionada se aumenta su brillo
	if(sel){
		glMaterialfv(GL_FRONT, GL_SHININESS, sel_shininess);
	}else{
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	}
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, fichas_diffuse[color-2]); 
	
	glEnable( GL_TEXTURE_2D );
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glBindTexture( GL_TEXTURE_2D, textures[1]);			
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);		
	glPushMatrix();
		glTranslatef(x,y,z);
   		glutSolidSphere(r, 20, 16);
	glPopMatrix();
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable( GL_TEXTURE_2D );
	
}
/*
	funcion para rotar la camara una cierta cantidad de grados
*/
void rotcam(float angulo)
{
	int b=1;
	camang = (int)(camang + angulo+360)%360;
	camx=cos(PI*(camang)/180.0)*(camdist);
	camy=sin(PI*(camang)/180.0)*(camdist);	
}
/*
	escribe un texto en la pantalla en la posicion especificada
*/
void output(int x, int y, char *string)
{
	int len, i;
	glRasterPos2f(x, y);
	len = (int) strlen(string);
	for (i = 0; i < len; i++){
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
	}
}
/*
	dibuja el fondo
*/
void fondo(int size)
{
	glEnable( GL_TEXTURE_2D );
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
	glBindTexture( GL_TEXTURE_2D, textures[2]);		
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);glVertex3f(-size, -size, 0); 
		glTexCoord2f(0.0, 1.0);glVertex3f(-size, size, 0);
		glTexCoord2f(1.0, 1.0);glVertex3f(size, size, 0);		
		glTexCoord2f(1.0, 0.0);glVertex3f(size, -size, 0); 
	glEnd();
	glDisable( GL_TEXTURE_2D );		
}
/*
	dibuja el tablero
*/
void drawtablero()
{
	int i,j;
	float ang;
	float gsin,gcos;	
	matwhite();
	ang=atan(4/6.0);
	
	gsin=10*sin(ang);
	gcos=10*cos(ang);
	
	glEnable(GL_LIGHTING);
	glEnable( GL_TEXTURE_2D );
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glBindTexture( GL_TEXTURE_2D, textures[0]);			
	glNormal3f(0,0,1);
	glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0, 0.0);glVertex3f(centrox, centroy, 0.5);    
		glTexCoord2f(0.0, 1.0);glVertex3f(centrox-10, centroy, 0.5); 
		glTexCoord2f(1.0, 1.0);glVertex3f(centrox-gsin, centroy+gcos, 0.5);   
	glEnd();
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);glVertex3f(centrox-10, centroy, 0.5); 
		glTexCoord2f(0.0, 1.0);glVertex3f(centrox-gsin, centroy+gcos, 0.5);
		glTexCoord2f(1.0, 1.0);glVertex3f(centrox-gsin, centroy+gcos, -2);		
		glTexCoord2f(1.0, 0.0);glVertex3f(centrox-10, centroy, -2); 
	glEnd();
	glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0, 0.0);glVertex3f(centrox, centroy, 0.5);    
		glTexCoord2f(0.0, 1.0);glVertex3f(centrox-gsin, centroy+gcos, 0.5);   
		glTexCoord2f(1.0, 1.0);glVertex3f(centrox+gsin, centroy+10*cos(ang), 0.5); 
	glEnd();
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);glVertex3f(centrox-gsin, centroy+gcos, 0.5);   
		glTexCoord2f(0.0, 1.0);glVertex3f(centrox+gsin, centroy+gcos, 0.5); 
		glTexCoord2f(1.0, 1.0);glVertex3f(centrox+gsin, centroy+gcos, -2); 
		glTexCoord2f(1.0, 0.0);glVertex3f(centrox-gsin, centroy+gcos, -2);   	
	glEnd();	
	glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0, 0.0);glVertex3f(centrox, centroy, 0.5);    
		glTexCoord2f(0.0, 1.0);glVertex3f(centrox+gsin, centroy+gcos, 0.5); 
		glTexCoord2f(1.0, 1.0);glVertex3f(centrox+10, centroy, 0.5); 
	glEnd();
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);glVertex3f(centrox+gsin, centroy+gcos, 0.5); 
		glTexCoord2f(0.0, 1.0);glVertex3f(centrox+10, centroy, 0.5); 
		glTexCoord2f(1.0, 1.0);glVertex3f(centrox+10, centroy, -2); 
		glTexCoord2f(1.0, 0.0);glVertex3f(centrox+gsin, centroy+gcos, -2); 
	glEnd();
	
	glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0, 0.0);glVertex3f(centrox, centroy, 0.5);    
		glTexCoord2f(0.0, 1.0);glVertex3f(centrox-10, centroy, 0.5); 
		glTexCoord2f(1.0, 1.0);glVertex3f(centrox-gsin, centroy-gcos, 0.5);   
	glEnd();
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);glVertex3f(centrox-10, centroy, 0.5); 
		glTexCoord2f(0.0, 1.0);glVertex3f(centrox-gsin, centroy-gcos, 0.5);   
		glTexCoord2f(1.0, 1.0);glVertex3f(centrox-gsin, centroy-gcos, -2);   		
		glTexCoord2f(1.0, 0.0);glVertex3f(centrox-10, centroy, -2); 
	glEnd();	
	glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0, 0.0);glVertex3f(centrox, centroy, 0.5);    
		glTexCoord2f(0.0, 1.0);glVertex3f(centrox-gsin, centroy-gcos, 0.5);   
		glTexCoord2f(1.0, 1.0);glVertex3f(centrox+gsin, centroy-gcos, 0.5); 
	glEnd();
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);glVertex3f(centrox-gsin, centroy-gcos, 0.5);   
		glTexCoord2f(0.0, 1.0);glVertex3f(centrox+gsin, centroy-gcos, 0.5); 
		glTexCoord2f(1.0, 1.0);glVertex3f(centrox+gsin, centroy-gcos, -2); 
		glTexCoord2f(1.0, 0.0);glVertex3f(centrox-gsin, centroy-gcos, -2);   
	glEnd();
	
	glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0, 0.0);glVertex3f(centrox, centroy, 0.5);    
		glTexCoord2f(0.0, 1.0);glVertex3f(centrox+gsin, centroy-gcos, 0.5); 
		glTexCoord2f(1.0, 1.0);glVertex3f(centrox+10, centroy, 0.5); 
	glEnd();
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0);glVertex3f(centrox+gsin, centroy-gcos, 0.5); 
		glTexCoord2f(0.0, 1.0);glVertex3f(centrox+10, centroy, 0.5); 
		glTexCoord2f(1.0, 1.0);glVertex3f(centrox+10, centroy, -2); 
		glTexCoord2f(1.0, 0.0);glVertex3f(centrox+gsin, centroy-gcos, -2); 
	glEnd();
	
	
	glDisable( GL_TEXTURE_2D );

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glColor3f(0,0,0);
	for(i=0;i<M;i++){
		for(j=0;j<N;j++){
			if((i%2)==0){
				if(tablero[i][j]>0){
					drawhole(i,j,0.5);
				}
			}else{
				if(tablero[i][j]>0){			
					drawhole(i,j-0.5,0.5);
				}
			}
		}
	}
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);		
}
/*
	dibuja la informacion de turnos y puntajes
*/
void info()
{
	int offset;
	int offx,offy;
	char buff[100];
	GLUquadric* quad,*quadratic;
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(	-20,20,-20,20);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDepthMask(GL_FALSE);
			glColor3f(1,1,1);
			/*	si no termino el juego dibuja las fichas del turno correspondiente
				si termino imprime un mensaje indicando
			*/
			glEnable(GL_LIGHT1);
			if(fin==0){
				glDisable(GL_LIGHTING);
				output(-19,18,"TURNO:");
				glEnable(GL_LIGHTING);				
				glPushMatrix();
					offset=-18;
					for(i=2;i<=7;i++){
						if(FichaEnTurno(i)){
							GLUquadric* quad;
							quad=gluNewQuadric ();
							glShadeModel(GL_FLAT);
							glPushMatrix();
								glDisable(GL_LIGHTING);											
								glScalef(1,1,1);
								glTranslatef(offset,16,0);
								gluDisk (quad , 0, 1.1, 10 , 10);
								glEnable(GL_LIGHTING);											
							glPopMatrix();
							drawficha(offset,16,0,1,i,1);
							offset+=2;
						}
					}
				glPopMatrix();
			}else{
				i=TurnoGanador();
				if(i!=-1){
    				glDisable(GL_LIGHTING);
    				output(-10*2,18,"Fin del juego! Ganador:");				
    				glEnable(GL_LIGHTING);
    				offset=-18;
    				for(j=0;j<fichasxturno[modo];j++){
    					GLUquadric* quad;
    					quad=gluNewQuadric ();
    					glShadeModel(GL_FLAT);
    					glPushMatrix();
    						glDisable(GL_LIGHTING);											
    						glScalef(1,1,1);
    						glTranslatef(offset,16,0);
    						gluDisk (quad , 0, 1.1, 10 , 10);
    						glEnable(GL_LIGHTING);											
    					glPopMatrix();
    					drawficha(offset,16,0,1,turnos[modo][i+j],1);
    					offset+=2;
    				}
                }else{
    				glDisable(GL_LIGHTING);
    				output(-10*2,18,"Fin del juego! Empate!!!");				                      
                }
			}
			
			//imprime los puntajes
			glColor3f(1,1,1);
			glDisable(GL_LIGHTING);
			output(9,18,"TOTAL");
			output(13,18,"FICHAS");			
			glEnable(GL_LIGHTING);		
			offy=16;
			for(i=0;turnos[modo][i]!=NADA;i+=fichasxturno[modo]){
				offx=11;
				glColor3f(1,1,1);
				glDisable(GL_LIGHTING);
				sprintf(buff,"%d",PuntajeTurno(i));
				output(offx-2,offy,buff);
				glEnable(GL_LIGHTING);		
				offx+=3;
				for(j=0;j<fichasxturno[modo];j++){
					glPushMatrix();
						glTranslatef(0.5,0.5,0);
						drawficha(offx-1,offy,0,0.5,turnos[modo][i+j],0);
					glPopMatrix();
					sprintf(buff,"%d",10-fichas_comidas[DameIndice(turnos[modo][i+j])]);
					glDisable(GL_DEPTH_TEST);
					glDisable(GL_LIGHTING);
					glColor3f(1,1,1);
					output(offx,offy,buff);
					glEnable(GL_LIGHTING);
					glEnable(GL_DEPTH_TEST);
					offx+=2;
				}
				offy-=1;
			}
			
						
			glDisable(GL_LIGHT1);
		glDepthMask(GL_TRUE);
		glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);			
} 

/*
	funcion principal para dibujar
*/
void display(void)
{
	int sel;
	
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

	//dibuja el fondo
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(	-20,20,-20,20);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDepthMask(GL_FALSE);
		fondo(20);
		glDepthMask(GL_TRUE);
		glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); 
	
	//posiciona la camara
	gluLookAt(camx+centrox, camy+centroy, camz+centroz, centrox,centroy,centroz, 0.0, 0.0, 1.0);
	
	//dibuja el tablero
	drawtablero();

	//si se esta animando una ficha
	if(anim){
		double t;
		t=glutGet(GLUT_ELAPSED_TIME);
		if(t>=tf){
			tablero[gi][gj]=fichaanim;
			anim=0;
			//verifica si termino la partida
			if(FinJuego()||TurnoSobreviviente()){	
				fin=1;									
			}else{
				if(ct)SiguienteTurno();
			}
		}else{
			//actualiza las posiciones y dibuja la ficha
			matsel();
			animz=z0+v0z*(t-t0)-0.5*g*(t-t0)*(t-t0);
			animx=x0+v0x*(t-t0);
			animy=y01+v0y*(t-t0);
			drawficha(animx,animy,animz,0.5,fichaanim,1);
		}
	}
	
	//dibuja las fichas del tablero
	for(i=0;i<M;i++){
		for(j=0;j<N;j++){
			if(selj==j && seli==i){
				sel=1;
				matsel();
			}else{
				sel=0;
				matred();
			}
			glColor3f(0,0,0);
			if((i%2)==0){
				if(tablero[i][j]>1){
					drawficha(i,j,0.5,0.5,tablero[i][j],sel);
				}					
			}else{
				if(tablero[i][j]>1){
					drawficha(i,j-0.5,0.5,0.5,tablero[i][j],sel);
				}					
			}			
		}
	}				
	
	//dibuja la informacion de turnos y puntajes
	info();	
	
	glutSwapBuffers();
}
/*
	dibuja en el back buffer para poder seleccionar(juego)
*/
void display_back(void)
{
	glClearColor(0.0, 0.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	glLoadIdentity(); 
	gluLookAt(camx+centrox, camy+centroy, camz+centroz, centrox,centroy,centroz, 0.0, 0.0, 1.0);
	glDisable(GL_LIGHTING);
	
	for(i=0;i<M;i++){
		for(j=0;j<N;j++){
			if((i%2)==0){
				if(tablero[i][j]>0){					
					glColor3ub(i*4,j*4,0);
					drawhole(i,j,0.5);
				}
			}else{
				if(tablero[i][j]>0){
					glColor3ub(i*4,j*4,0);
					drawhole(i,j-0.5,0.5);
				}				
			}			
		}
	}
	
	glEnable(GL_LIGHTING);
	glClearColor(1.0, 1.0, 1.0, 0.0);	
}
/*
	dibuja un rectangulo que cubre el ancho de la pantalla
*/
void rect2d(float y0_,float y1)
{
	glFrontFace(GL_CW);
	glBegin(GL_POLYGON);
		 glVertex2f(-10.0,  y1);
		 glVertex2f(-10.0,  y0_);
		 glVertex2f(10.0,  y0_);
		 glVertex2f(10.0,  y1);
	glEnd();	
}
/*
	dibuja el menu principal
*/
void display_menu()
{	
	//usleep(1);

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);	
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(	-10,10,-10,10);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	
		glEnable(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, textures[3]);
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
		glFrontFace(GL_CW);
		glBegin(GL_POLYGON);
			glTexCoord2f(0,0);glVertex2f(-10,-10);
			glTexCoord2f(0,1);glVertex2f(-10,10);
			glTexCoord2f(1,1);glVertex2f(10,10);
			glTexCoord2f(1,0);glVertex2f(10,-10);
		glEnd();	
		glDisable(GL_TEXTURE_2D);
		
		glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(1,0.0,0.0,0.5);
			if(menusel==4) rect2d(5,6);		
			if(menusel==3) rect2d(4,5);		
			if(menusel==2) rect2d(3,4);			
			if(menusel==1) rect2d(2,3);		
			if(menusel==5) rect2d(1,2);			
			if(menusel==0) rect2d(0,1);		
			if(menusel==8) rect2d(-1,0);		
			if(menusel==6) rect2d(-2,-1);				
			if(menusel==7) rect2d(-3,-2);		
			if(menusel==9) rect2d(-4,-3);			
		glDisable(GL_BLEND);
	
	
	
		glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);	
	
	glutSwapBuffers();		
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);			
}
/*
	dibuja en el back buffer para poder seleccionar(menu principal)
*/
void display_menu_back()
{	
	glDisable(GL_LIGHTING);
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(	-10,10,-10,10);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glColor3ub(4*4,0*4,0);
		rect2d(5,6);
		glColor3ub(3*4,0,0);
		rect2d(4,5);
		glColor3ub(2*4,0,0);
		rect2d(3,4);
		glColor3ub(1*4,0,0);
		rect2d(2,3);
		glColor3ub(5*4,0,0);
		rect2d(1,2);
		glColor3ub(0*4,0,0);
		rect2d(0,1);
		glColor3ub(8*4,0,0);
		rect2d(-1,0);
		glColor3ub(6*4,0,0);
		rect2d(-2,-1);
		glColor3ub(7*4,0,0);
		rect2d(-3,2);
		glColor3ub(9*4,0,0);
		rect2d(-4,2);
		
		glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);	
	
	glEnable(GL_LIGHTING);
}
/*
	dibuja el menu de comer o no fichas
*/
void display_menu_comer()
{	
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);		
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(	-10,10,-10,10);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	
		//usleep(1);
	
		glEnable(GL_TEXTURE_2D);
		glBindTexture( GL_TEXTURE_2D, textures[4]);
		glFrontFace(GL_CW);
		glColor3f(1,1,1);	
		glBegin(GL_POLYGON);
			glTexCoord2f(0,0);glVertex2f(-10,-10);
			glTexCoord2f(0,1);glVertex2f(-10,10);
			glTexCoord2f(1,1);glVertex2f(10,10);
			glTexCoord2f(1,0);glVertex2f(10,-10);
		glEnd();	
		glDisable(GL_TEXTURE_2D);
		
		glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(1,0.0,0.0,0.5);
			if(menusel==0) rect2d(5,6);		
			if(menusel==1) rect2d(4,5);		
		glDisable(GL_BLEND);
	
		glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);	
	
	glutSwapBuffers();		
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);			
}
/*
	dibuja en el back buffer para poder seleccionar(menu comer o no fichas)
*/
void display_menu_comer_back()
{	
	glDisable(GL_LIGHTING);
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(	-10,10,-10,10);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	
		glColor3ub(0*4,0*4,0);
		rect2d(5,6);
		glColor3ub(1*4,0,0);
		rect2d(4,5);

		glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);	
	
	glEnable(GL_LIGHTING);
}

int gx,gy;	

/*
	procesa el movimiento del mouse si no se esta presionando ningun boton del mouse
*/
void mousemotion2(int x, int y)
{
	gx=x;
	gy=y;
	if(showmenu){
		struct scolor color;
		if(showmenu==1)display_menu_back();
		if(showmenu==2)display_menu_comer_back();	
 		glReadPixels(x,height-y,1,1,GL_RGB,GL_UNSIGNED_BYTE,&color);
		if(color.g){
			menusel=-1;
		}else{
			menusel=color.r/4;
		}
	}
}
/*
	procesa el movimiento del mouse si se esta presionando algun boton del mouse
*/
void mousemotion(int x, int y)
{
	int dx,dy;
	dx=(x-gx);
	dy=(y-gy);
	if(rotar){
		rotcam(dx);
	}
	gx=x;
	gy=y;
}
extern int turno;
extern int fichas_comidas[COLORES];
/*
	inicializa una partida
*/
int juegonuevo()
{
	for(i=0;i<17;i++){
		for(j=0;j<13;j++){
			tablero[i][j]=tablerobase[i][j];
		}	
	}
	for(i=0;i<COLORES;i++){	
		fichas_comidas[i]=0;
		puntajes[i]=0;
	}
	turno=0;
	InicializarTablero();
}
/*
	inicializa una animacion
*/
void inianim(int i0,int j0,int i1,int j1)
{
	float xf,yf;
	t0=glutGet(GLUT_ELAPSED_TIME);
	tf=t0+1000.0;
	if((i0%2)==0){
		x0=i0;
		y01=j0;
	}else{
		x0=i0;
		y01=j0-0.5;		
	}	
	z0=0.5;
	if((i1%2)==0){
		xf=i1;
		yf=j1;
	}else{
		xf=i1;
		yf=j1-0.5;
	}
	v0z=(0.5*g*(1000000))/1000;
	v0x=(xf-x0)/1000;
	v0y=(yf-y01)/1000;
	
	anim=1;

}
/*
	procesa los eventos de botones del mouse
*/
void mouse(int button, int state, int x, int y)
{
	struct scolor color;
	int curi,curj;
	static int ban=0;
	int movtipo;
	switch (button) {
		case GLUT_LEFT_BUTTON:
			if(anim)return;
			if(state)return;
			if(fin)return;				
			if(showmenu==1){
				if(menusel==-1)return;
				if(menusel<9){
					modo=menusel;
					glutIdleFunc(display_menu_comer);
					glutDisplayFunc(display_menu_comer);
					showmenu=2;
					return;					
				}
				exit(0);				
			}
			if(showmenu==2){
				if(menusel==-1)return;
				glutIdleFunc(display);
				glutDisplayFunc(display);
				showmenu=0;
				juegonuevo();
				comer=menusel;	
			}			
			ct=0;
			display_back();
 			glReadPixels(x,height-y,1,1,GL_RGB,GL_UNSIGNED_BYTE,&color);
			if(color.b==0 && state==0){
				curi=color.r/4;
				curj=color.g/4;	
				if(mov==0){
					if(FichaEnTurno(tablero[curi][curj])){
						mov=1;
						seli=curi;
						selj=curj;
					}else{
						seli=-1;
						selj=-1;						
					}
				}else{
					if(!(curi==seli && curj==selj)){
						int gseli,gselj;
						movtipo=MovidaValida(seli,selj,curi,curj,0);
						if((ban && movtipo==PASO_LARGO)||(!ban && movtipo)){
							fichaanim=tablero[seli][selj];
							tablero[curi][curj]=1;
							tablero[seli][selj]=1;
							gi=curi;
							gj=curj;
							gseli=seli;
							gselj=selj;
							inianim(seli,selj,curi,curj);							
							if(movtipo==PASO_LARGO && SigueJugando(curi,curj)){
								seli=curi;
								selj=curj;
								ban=1;
							}else{
								ban=0;
								mov=0;	
								seli=-1;
								selj=-1;
								ct=1;
							}
						}else{
							if(!ban){
								mov=0;	
								seli=-1;
								selj=-1;
							}
						}						
					}else{
						if(ban){
							ban=0;
							if(FinJuego()||TurnoSobreviviente()){	
								fin=1;									
							}else{	
								SiguienteTurno();
							}								
						}
						mov=0;	
						seli=-1;
						selj=-1;														
					}
				}
			}
			display();
			break;
		case GLUT_MIDDLE_BUTTON:
			break;
		case GLUT_RIGHT_BUTTON:
			rotar=!state;
			break;		
		default:
			break;
	}
}
/*
	si se modifica el tamanho de la ventana
*/
void reshape(int w, int h)
{
     if(!h||!w)return;
	height=h;
	width=w;
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60,width/height,1.5,200);
	glMatrixMode(GL_MODELVIEW);
}
/*
	procesa el input del teclado
*/
void processNormalKeys(unsigned char key, int x, int y) 
{
	int width,height;
	static int full=0;
	switch(key) {
		case 'f':
             if(full)
                  glutReshapeWindow 	(800 ,600);
             else
                  glutFullScreen();
             full=!full;
			break;		
		case 27 :
			anim=0;
			showmenu=1;
			glutIdleFunc(display_menu);
			glutDisplayFunc(display_menu);
			fin=0;
			break;
	}			
}
int main(int argc, char** argv)
{
	int win;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB| GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	init();
	
	showmenu=1;
	glutDisplayFunc(display_menu);
	glutIdleFunc(display_menu);
	glutReshapeFunc(reshape);
	glutMotionFunc(mousemotion);
	glutPassiveMotionFunc(mousemotion2);
	glutMouseFunc(mouse);
	glutKeyboardFunc(processNormalKeys);
	
	glutMainLoop();
	return 0;
}
