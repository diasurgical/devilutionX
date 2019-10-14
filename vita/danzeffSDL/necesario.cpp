#include "danzeff.h"
 //Todo a salido del SRC de snes9x TYL (gracias)
#define DANZEFF_LEFT   1
#define DANZEFF_RIGHT  2
#define DANZEFF_SELECT 3
#define DANZEFF_START  4

int danzeffSDL(char *, int ,COSASSDL &, int = NULL, int = 270, int = 2) ;
void printfSDL(char *, COSASSDL &, int , int );


void printfSDL(char *texto, COSASSDL &imgas, int x, int y)
{ 
	//debug("+++++++++++++Función printfSDL...+++++++++++++++++++");
/*
	if(!imgas.fuente)
	{
		error("Error en PrintfSDL, no se ha cargado la fuente!");
		sceKernelExitGame();
	}
	if(!texto)
	{
		error("Error en PrintfSDL, no se especifico ningun array de carácteres");
		sceKernelExitGame();
	}
	if(!imgas.screen)
	{
		error("Error en PrintfSDL, screen no esta indicializado!!");
		sceKernelExitGame();
	}
*/
	if(texto!=""){
SDL_Surface *text = NULL;
//debug("surfcs ok");
SDL_Color c = textcolor.damecolor();
//debug("SDL color ok");
SDL_Rect dest={x,y,0,0};
//debug("SDL Rect ok");
//debug("Texto a poner:");
//debug(texto);
//debug("Se va a renderizar el texto");
text = TTF_RenderText_Blended(imgas.fuente,texto, c);
/*
if(!text){
error("Error al renderizar la fuente");
sceKernelExitGame();
}*/
//debug("text Render OK");
SDL_BlitSurface(text, NULL,imgas.screen, &dest);
//debug("SDL Blit ok");
SDL_FreeSurface(text);
//debug("Liverdo Surface TEXT");
}
}


void imagenSDLdanzeff(SDL_Surface *screen, SDL_Surface *imagen)
{
	//debug("****Funcion Imagen SDL Danzeff****");
	SDL_Rect dest={0,0,0,0};
	//debug("Dest OK");
	SDL_BlitSurface(imagen, NULL, screen, &dest);
	//debug("Blit Imagen OK");
	//SDL_FreeSurface(imagen);
	//debug("Liverado imagen OK");
}

int danzeffSDL(char *name, int modo,COSASSDL &imgas, int dni, int posx, int posy) 
{
	//h//debug=1;
	/*
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0) {
		error("No se puedo iniciar SDL :-( ");
		sceKernelExitGame();
	};*/
		//Quito solo en DNI ya que hace carga globlal.
	/*
	TTF_Font *fonts;
	//debug("iniciando TTF");
	if (TTF_Init() !=0){
		error("****ERRROR CRÍTICO!!!!****\nFallo al iniciar el motor TTF!!");
		sceKernelExitGame();
		}
	//debug("TTfont ok");
	fonts = TTF_OpenFont("datos/fuente/comic.ttf", 12);
	//debug("SDL open font ok");
	if(!font){
		error("****ERRROR CRÍTICO!!!!****\nError al cargar la fuente (Font null)");
		sceKernelExitGame();
	}
	*/
	//SDL_Event event;
	
	//Quito solo en DNI ya que hace carga globlal.
	/*
	SDL_Surface *screen;
	screen = SDL_SetVideoMode(480,272,16, SDL_SWSURFACE | SDL_DOUBLEBUF );
	if(!screen){
		error("****ERROR CRíTICO!!!****\nNo se pudo iniciar screen");
		SDL_Quit();
		sceKernelExitGame();
	}
	//debug("Set video mode OK");
	*/
	
	//***MOD IMAGEN***/
	
	//SDL_Surface *imagen=NULL;
	//debug("SDL Surface imagen OK");
	//imagen = IMG_Load(fondo);
	//if (!imagen){
	//	error("Error al cargar la Imagen de fondo:");
	//	//debug(fondo);
	//	sceKernelExitGame();
	//}
	//debug("Carga de Imagen OK");
	//FIN MOD****
	
	
	//danzeff_load(); Pasado a la funcion principal

	//debug("Danzeff load OK");
	//danzeff_set_screen(imgas.screen);
	//debug("Danzeff ser screen OK");
	danzeff_moveTo(posx,posy);
	//debug("Danzeff Move to OK");
	unsigned char name_pos;
	unsigned int key;
	int exit_osk;
	exit_osk=0;
	SDL_Joystick* paddata;
	name_pos=0;
	//debug("Definiciones y demas OK");
	while (name[name_pos]) name_pos++;
	//debug("Primer While");		
	while (!exit_osk) {	
		//delaya(4);
		//debug("Entramso en el IF de SDL joy");
		if (SDL_NumJoysticks() >= 1) {
			paddata = SDL_JoystickOpen(0);
			SDL_JoystickEventState(SDL_ENABLE);
		}
		//debug("A la igualdad de Key");
		key=danzeff_readInput(paddata);
		//debug("Key modificada... a Switch");
		//debug("Key: ");
		//sprintf(temp,"%d",key);
		//debug(temp);
			switch (key) {
		//	//debug("Switch");
				case DANZEFF_START:exit_osk=1;break;
		//	//debug("opcion2");
				case DANZEFF_SELECT:exit_osk=2;break;
		//	//debug("opcion3");
				case 8://backspace
					if (name_pos>0) {
		//				//debug("En case 8");
						name_pos--;
		//				//debug("pos--");	
					}
					name[name_pos]=0;
		//			//debug("Name Pos");
					break;
			default:
		//		//debug("default");
				if (key>=32) {
		//			//debug("primer if");
					name[name_pos]=key;
					//if (name_pos<127) name_pos++;
					if (name_pos<16) name_pos++; //Posicion Maxima
						name[name_pos]=0;
				}
				break;
			}
		if (modo ==1)
		{
			imagenSDLdanzeff(imgas.screen,imgas.fondni);
		}
		if (modo == 2)
		{
			imagenSDLdanzeff(imgas.screen,imgas.lista);
		}
		//debug("Impime imagen OK");
		textcolor="AZUL";
		printfSDL("Escribe el Nombre de este DNI:",imgas,10,10);
		printfSDL("Pulsa Start para aceptar.",imgas,183,235);
		printfSDL("Pulsa Slect para cancelar.",imgas,183,255);
		char dnis[9];
		sprintf(dnis,"%d",dni);
		textcolor="ROJO";
		printfSDL(dnis,imgas,190,10);
		textcolor="VERDE";
		printfSDL(name,imgas,20,235);
		//debug("printfcolorb OK ");
		danzeff_render();
		//debug("Danzeff render fuera shitch OK");
		SDL_Flip(imgas.screen);
		delaya(1);
		//debug("FLIP y delaya OK");
	}
	//danzeff_free();
	//Quitado por DNI********************
	//debug("Descarga de Danzeff OK");
	//SDL_FreeSurface(screen);
	//debug("SDL_FreeSurface ok");
	//TTF_CloseFont(font);
	//debug("Descarga de Fuente OK");
	//SDL_Quit();
	//debug("SDL quit ok");
	//debug("\n****Todo ha salido bien.*****\n");
	if (exit_osk==1) {
		int pru;
		pru = strlen(name);
		name[pru] = '\0';
	return 1;
	}
	if (exit_osk==2) return 2;
	//debug("\n****Todo ha salido bien.*****\n");
	return 0;
}
