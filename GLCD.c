//////////////////////////////////////////////////////////////////////////////////////////
//                       LIBRAIRIE POUR AFFICHEUR GRAPHIQUE LCD                        //
////////////////////////////////////////////////////////////////////////////////////////
// Auteur                 : Steve Tchuinte                                           //
//                                         
/////////////////////////////////////////////////////////////////////////////////////


// Pins ARM utilisées pour contrôler LCD (actives état bas)
// --------------------------------------------------------
// P0.19 : LCD_EN
// P0.20 : LCD_LE
// P0.21 : LCD_DIR
// P0.22 : LCD_CS
// P0.23 : LCD_RS
// P0.24 : LCD_WR
// P0.25 : LCD_RD

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <lpc17xx.h>
#include "LPC17xx_Bits.h"
#include "GLCD2.h"
#include "Font_Regular_24_16.h"
#include "Timing.h"
#include "type.h"
	
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                                   DEFINES                                            //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

#define SSD1289 0x8989
#define SHORTWAIT 100
#define Wait Timing_WaitUs			// les boucles d'attente utilisent la librairie Timing

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                                 PROTOTYPES                                           //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

static unsigned short ReadRegister (unsigned short register);
static void WriteRegister(unsigned char reg, unsigned short value);
static __inline void WriteData (unsigned short dat);
static __inline unsigned short ReadData(void);
static __inline void Put0 (uint32_t pin);
static __inline void Put1 (uint32_t pin);
static __inline void WriteCommand (unsigned char cmd);
static __inline void FreePins(void);

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                              VARIABLES GLOBALES                                      //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

static unsigned short _driver = 0;					 // Identificateur du driver
static unsigned short _foreColor = 0xFFFF;	// Couleur d'écriture
static unsigned short _backColor = 0;			 // Couleur de fond
static Boolean _lcd_OK = false;						// indique si écran correctement configuré

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                                PROPRIÉTÉS                                            //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
Boolean GLCD_GetStatus(void) {return _lcd_OK;}		// Obtient si l'écran a été correctement configuré

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                              FONCTIONS PUBLIQUES                                     //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//                              INITIALISATION GLCD                                     //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------

Boolean GLCD_Initialize(void)
{
	// CONFIGURATION DES PINS  
	//-----------------------
	LPC_GPIO0->FIODIR   |= 0x03f80000;				// P0.19 à P0.25 en sortie
	LPC_GPIO0->FIOSET   |= 0x03f00000;				// État haut = inactives sauf LCD_EN
	
	// ATTENDRE LCD PRET
	//------------------
	Wait(50000);									// Attendre 50ms
	
	// INTERROGER DRIVER
	//------------------
	_driver = ReadRegister(0x00);					// Device Code Read: 0x8989 pour SSD1289

	// INITIALISATION SSD1289
	//-----------------------
	if (_driver == SSD1289)							// uniquement si driver pris en charge
	{
		// Étape 1 datasheet : Power supply settings
		WriteRegister(0x0C,0x00);					// Tension 5.1V, minimale car pas besoin de vitesse
		WriteRegister(0x0D,0x009);					// Facteur d'amplification Vlcd
		WriteRegister(0x0E,0x2B00);					// Facteur d'amplification Vcom
		WriteRegister(0x1E,0xB0);					// Tension VcomH
	
		// Étapes 2-7 : Allumer LCD
		GLCD_On();									// Allumage
		
		// Étape 8 : Entry mode setting R11
		WriteRegister(0x11,0x6078);						// 65K couleurs, écriture inversée cause rotation écran, zone via 0x4E/0x4F
		WriteRegister(0x0F,0x00);						 // Position début de l'image
		WriteRegister(0x16,0xEF1C);					// 240 points par ligne, 28 clocks avant dummy data
		WriteRegister(0x017,0x103);				 // Délais pour synchros
		WriteRegister(0x23,0x00);					// Masque d'accès
		WriteRegister(0x24,0x00);
		WriteRegister(0x25,0x8000);			// Fréquence verticale = 65Hz
		WriteRegister(0x30,0x707);		 // Corrections Gamma
		WriteRegister(0x31,0x204);
		WriteRegister(0x32,0x204);
		WriteRegister(0x33,0x502);
		WriteRegister(0x34,0x507);
		WriteRegister(0x35,0x204);
		WriteRegister(0x36,0x204);
		WriteRegister(0x37,0x502);
		WriteRegister(0x3A,0x302);
		WriteRegister(0x3B,0x302);
		WriteRegister(0x41,0x00);					// Scroll vertical
		WriteRegister(0x42,0x00);					
		WriteRegister(0x44,0xEF00);			// Début de ligne = 0, fin de ligne = 239
		WriteRegister(0x45,0x00);			 // Début de colonne = 0
		WriteRegister(0x46,0x13F);		// Fin de colonne = 319
		WriteRegister(0x48,0x00);		 // Numéro de première ligne du premier écran = 0
		WriteRegister(0x49,0x13F);	// Numéro de dernière ligne du premier écran = 319
		WriteRegister(0x4A,0x00);	 // Numéro de première ligne du second écran = 0
		WriteRegister(0x4B,0x00);	// Numéro de dernière ligne du second écran = 0
		WriteRegister(0x4E,0x00);// Position de départ X
		WriteRegister(0x4F,0);	// Position de départ Y

		// Étape 9 : MCD driver AC R02
		WriteRegister(0x02,0x0600);					// Interlace
		WriteRegister(0x01,0x2B3F);					// Reverse, coul.RGB, 320 lignes : 1/2 écran L impair, 1/2 pairs !!
		WriteRegister(0x03,0xA8A4);					// Horloges et taille écran
		WriteRegister(0x05,0x00);					// Registres de comparaison non utilisés
		WriteRegister(0x06,0x00);
		WriteRegister(0x0B,0x4000);					// Délais
		_lcd_OK = true;								// Tout est OK
	}

	// LIBÉRER PINS ET RETOURNER RÉSULTAT
	//-----------------------------------
	FreePins();										// libérer P2
	return (_lcd_OK);								// retourner résultat de l'opération
}	

//////////////////////////////////////////////////////////////////////////////////////////
//                               ALLUMAGE GLCD                                          //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Allume l'afficheur précédemment initialisé
//----------------------------------------------------------------------------------------
void GLCD_On(void)
{
	if (_driver == SSD1289)
	{
		// Étape 2 datasheet : R07 = 0x21
		WriteRegister(0x07,0x21);					// GON = 1, DTE = 0, D[1:0] = 01

		// Étape 3 : R0 = 0x01
		WriteRegister(0x00,0x01);					// Oscillateur LCD en service
		
		// Étape 4 : R7 = 0x23
		WriteRegister(0x07,0x23);					// GON = 1, DTE = 0, D[1:0] = 11
		
		// Étape 5 : R10 = 0x00
		GLCD_ExitSleep();							// Quitter le mode sleep
		
		// Étape 6 : Attendre 30ms
		Wait(30000);								// ATtendre 30ms
		
		// Étape 7 : R07 = 0x33 
		WriteRegister(0x07,0x33);					// GON = 1, DTE = 1, D[1:0] = 11
	}
	// Libère pins
	FreePins();										// libérer P2
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                 ÉTEINDRE GLCD                                        //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Éteint l'écran GLCD
//----------------------------------------------------------------------------------------
void GLCD_Off (void)
{
	if (_driver == SSD1289)
	{
		// Étape 1 du datasheet
		WriteRegister(0x07,0x00);				// Arrêt des opérations
		
		// Étape 2
		WriteRegister(0x00,0x00);				// Arrêt de l'oscillateur
		
		// Étape 3
		GLCD_EnterSleep();						// Entrer en mode sleep
	}
	FreePins();									// libérer P2 
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                 ENTRER EN VEILLE                                     //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Fait entrer l'écran en veille
//----------------------------------------------------------------------------------------
void GLCD_EnterSleep(void)
{
	if (_driver == SSD1289)
	{
		WriteRegister(0x10,0x01);
	}
	FreePins();										// libérer P2
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                 SORTIR DE VEILLE                                     //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Fait sortir l'écran de veille
//----------------------------------------------------------------------------------------
void GLCD_ExitSleep(void)
{
	if (_driver == SSD1289)
		WriteRegister(0x10,0x00);

	FreePins();										// libérer P2
}

//////////////////////////////////////////////////////////////////////////////////////////
//                    DÉFINIT LA FENÊTRE D'ÉCRITURE SUR TOUT L'ÉCRAN                    //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// On dispose de la possibilité d'écrire dans une partie fenêtrée de l'écran.
// Cette routine fait correspondre la fenêtre d'écriture à la taille totale de l'écran
//----------------------------------------------------------------------------------------
void GLCD_WindowScreen(void)
{
	if (_driver == SSD1289)
	{
		WriteRegister(0x44,(LCD_HEIGHT-1) << 8);	// X départ = 0, X fin = 239
		WriteRegister(0x45,0x00);					// Y départ = 0
		WriteRegister(0x46,LCD_WIDTH-1);			// Y fin = 319
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//                        DÉTERMINE UNE FENÊTRE D'ÉCRITURE                              //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// On peut déterminer une fenêtre dans l'écran, permettant de limiter la zone d'écriture
// tout en conservant l'avantage de l'incrémentation automatique d'adresse.
// L'écran est couché, donc avec une largeur de 320 pixels et une hauteur de 240 pixels
// x: abcisse du coin supérieur gauche de la zone, par rapport au coin supérieur gauche LCD
// y : ordonnée du coin supérieur gauche : idem
// width : largeur de la zone
// height : hauteur de la zone
// Éviter d'écrire trop de données, sous peine de provoquer le scrolling
// return : true si les paramètres sont corrects
//----------------------------------------------------------------------------------------
Boolean GLCD_SetWindow(unsigned short x, unsigned short y, unsigned short width, unsigned short height)
{
	if(_driver == SSD1289 && width > 0 && height > 0 && (x + width)<= LCD_WIDTH && (y + height <= LCD_HEIGHT))
	{
		// vu le retournement écran, X est par rapport au coin supérieur droit, avec début zone à droite
		// et fin à gauche
		x = LCD_WIDTH - x - width;					// x début par rapport au coin supérieur droit
		WriteRegister(0x45,x);						// x dans début vertical (écran retourné)
		WriteRegister(0x46,x+width-1);				// x de fin de zone
		
		WriteRegister(0x44,y | ((y + height - 1)<<8)); // Y Début et fin de zone
		
		WriteRegister(0x4E,y);						// Première coordonnée d'écriture
		WriteRegister(0x4F,x);
		return true;								// OK
	}
	return false;									// échec
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                  EFFACER ÉCRAN                                       //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Un pixel est représenté par 16 bits (mode 65K), avec :
// Rouge : 5 bits : D15/D11
// Vert  : 6 bits : D10/D5
// Bleu  : 5 bits : D4/D0
//----------------------------------------------------------------------------------------
void GLCD_Clear (unsigned short color)
{
	int   i;
	
	if (_driver == SSD1289)
	{
		GLCD_WindowScreen();							// Sélectionner tout l'écran
		WriteRegister(0x4E,0);							// début d'écriture en 0,0
		WriteRegister(0x4F,0);

		// Effacement	
		WriteCommand(0x22);								// écrire adresse registre
		for(i = 0; i< (LCD_WIDTH * LCD_HEIGHT); i++)	// Pour chaque pixel de l'écran
			WriteData(color);							// effacer pixel
	}
	FreePins();											// libérer P2
}

//////////////////////////////////////////////////////////////////////////////////////////
//                            SÉLECTIONNE LES COULEURS DE TEXTE                         //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Sélectionne les couleurs du texte
// backColor: couleur de fond
// foreColor : Couleur d'écriture
//----------------------------------------------------------------------------------------
void GLCD_SetTextColors(unsigned short backColor, unsigned short foreColor)
{
	_backColor = backColor;		// affecter couleurs
	_foreColor = foreColor;
}

//////////////////////////////////////////////////////////////////////////////////////////
//                            SÉLECTIONNE LA COULEUR DE TEXTE                           //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Sélectionne la couleur du texte
// foreColor : Couleur d'écriture
//----------------------------------------------------------------------------------------
void GLCD_SetTextColor(unsigned short foreColor)
{
	_foreColor = foreColor;
}

//////////////////////////////////////////////////////////////////////////////////////////
//                           ÉCRIT UN CARACTÈRE : BAS NIVEAU                            //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Écrit un caractère de taille 16X24
// Utiliser de préférence la méthode de plus haut niveau: GLCD_PrintChar
// La couleur de texte et celle de fond sont précisées par GLCD_SetTextColors
// x : abscisse de départ du coin supérieur gauche par rapport au coin supérieur gauche LCD
// y : ordonnée de départ du coin supérieur gauche : idem
// car : pointeur sur le premier mot de 16 bits composant le caractère à afficher
//----------------------------------------------------------------------------------------
void GLCD_WriteChar(unsigned short x, unsigned short y, unsigned short *car)
{
	int i,j;
	unsigned short color;
	
	if ((car != NULL) &&  GLCD_SetWindow(x, y, FONT_W, FONT_H)) // Sélectionne la fenêtre de dessin du caractère
	{
		WriteCommand(0x22);					// envoyer adresse du registre

		for(i = 0; i < FONT_H;i++,car++) 	// Pour chaque ligne du caractère (16 bits)
		{
			for(j = 0; j< FONT_W; j++)		// pour chaque pixel de la ligne
			{
				color = (*car & (1 << j))?  // bit = 0 = couleur fond, 1 = couleur texte
					_foreColor : _backColor;
				WriteData(color);			// écrire pixel
			}								// pixel suivant
		}									// ligne suivance
		FreePins();							// libérer pins
	}
}




//////////////////////////////////////////////////////////////////////////////////////////
//                           ÉCRIT UN CARACTÈRE : BAS NIVEAU                            //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Écrit un caractère de taille 16X24
// Utiliser de préférence la méthode de plus haut niveau: GLCD_PrintChar
// La couleur de texte et celle de fond sont précisées par GLCD_SetTextColors
// x : abscisse de départ du coin supérieur gauche par rapport au coin supérieur gauche LCD
// y : ordonnée de départ du coin supérieur gauche : idem
// car : pointeur sur le premier mot de 16 bits composant le caractère à afficher
//----------------------------------------------------------------------------------------
void GLCD_WriteChar1(unsigned int x, unsigned int y, unsigned char *car)
{
	int i,j;
	unsigned int color;
	
	if ((car != NULL) &&  GLCD_SetWindow(x, y, FONT_W, FONT_H)) // Sélectionne la fenêtre de dessin du caractère
	{
		WriteCommand(0x22);					// envoyer adresse du registre

		for(i = 0; i < FONT_H;i++,car++) 	// Pour chaque ligne du caractère (16 bits)
		{
			for(j = 0; j< FONT_W; j++)		// pour chaque pixel de la ligne
			{
				color = (*car & (1 << j))?  // bit = 0 = couleur fond, 1 = couleur texte
					_foreColor : _backColor;
				WriteData(color);			// écrire pixel
			}								// pixel suivant
		}									// ligne suivance
		FreePins();							// libérer pins
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
//                           ÉCRIT UN CARACTÈRE : HAUT NIVEAU                           //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Écrit un caractère de la font Font_Regular_24_16
// La couleur de texte et celle de fond ont été précisées par GLCD_SetTextColors
// x : abscisse du coin supérieur gauche du caractère
// y : ordonnée du coin supérieur gauche du caractère
// car : code ASCII du caractère
//----------------------------------------------------------------------------------------
void GLCD_PrintChar(uint16_t x, uint16_t y, unsigned char car)
{
	if (car < 127)
		car -= 32;
	else
		switch (car)		// transformer valeur ascii en numéro d'index dans table FONT
		{
			case 'µ':
				car = 95;
				break;
		
			case 'à':
				car = 96;
				break;
			
			case 'é':
				car = 97;
				break;
			
			case 'è':
				car = 98; 
				break;

			case 'ê':
				car = 99; 
				break;
				
			case 'ù':
				car = 100;
		
			default:			// â
				car = 101;
				break;
	}
	GLCD_WriteChar(x, y,(unsigned short *)&Font_Regular_24_16[car * FONT_H]);
}

//////////////////////////////////////////////////////////////////////////////////////////
//                             ÉCRIT UNE LIGNE DE TEXTE                                 //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Écrit un texte
// x : abscisse du coin supérieur gauche du premier caractère
// y : ordonnée du coin supérieur gauche du premier carac
// *str : pointeur sur la chaine de caractères terminée par un zéro
// Le caractère 0x0D provoque un saut de ligne, 0x00 indique la fin de la chaîne
// 0x0D = séquence \N , 0x00 = \END
//----------------------------------------------------------------------------------------
void GLCD_Print(uint16_t x, uint16_t y, unsigned char *str)
{
	uint16_t x1 = x;
	
	if (str == NULL)
		return;
	
	while (*str)
	{
		if (*str == 0x0A)				// si caractère retour de ligne
		{
			x1 = x;						// on recommence début ligne suivante
			y++;
		}
		else
		{
			GLCD_PrintChar(x1,y,*str);
			x1 += FONT_W;                     //FONT_W; j'ai fais un "define" en lui attribuant la valeur 16       
		}
		str++;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//                   ÉCRIT UNE LIGNE DE TEXTE DANS LA COULEUR SPÉCIFIÉE                 //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Écrit une ligne ou une ligne partielle de texte dans la couleur précisée
// x: Abscisse du coin supérieur gauche du premier caractère
// y: Ordonnée du coin supérieur gauche du premier caractère
// *str : pointeur sur la chaine de caractères terminée par un zéro
// color : couleur du texte
// Ne modifie pas la couleur de texte par défaut
//----------------------------------------------------------------------------------------
void GLCD_PrintColor(uint16_t x, uint16_t y, unsigned char *str, uint16_t color)
{
	uint16_t temp = _foreColor;

	if (str == NULL)
		return;

	_foreColor = color;
	while (*str)
	{
		GLCD_PrintChar(x,y, *str++);
		x += FONT_W;
	}
	_foreColor = temp;
}



//////////////////////////////////////////////////////////////////////////////////////////
//                              EFFACE LES LIGNES PRÉCISÉES                             //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Efface les lignes de texte précisées
// y: ordonnée du coin supérieur gauche de la ligne
// nbLines : nombre de lignes
//----------------------------------------------------------------------------------------
void GLCD_ClearLines(uint16_t y, unsigned char nbLines)
{
	int i;

	if (GLCD_SetWindow(0, y, LCD_WIDTH, FONT_H * nbLines)) 		// zone d'écriture = nbre lignes de fonte
	{
		WriteCommand(0x22);										// écrire adresse registre
		for(i = 0; i < (LCD_WIDTH * FONT_H * nbLines); i++)		// pour chaque pixel à effacer
			WriteData(_backColor);								// écrire pixel
		FreePins();												// libérer P2
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//                              EFFACE LA LIGNE PRÉCISÉE                                //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Efface la ligne de texte précisée
// y: Ordonnée coin supérieur gauche de la ligne
//----------------------------------------------------------------------------------------
void GLCD_ClearLine(uint16_t y)
{
	GLCD_ClearLines(y,1);
}

//////////////////////////////////////////////////////////////////////////////////////////
//                      DESSINE UN POINT DE LA COULEUR PRÉCISÉE                         //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Allume un pixel avec la couleur spécifiée
// x : abscisse du point
// y : ordonnée du point
// color : Couleur du point
// return: true si OK, false si coordonnées incorrectes
//----------------------------------------------------------------------------------------
Boolean GLCD_SetPoint(uint16_t x, uint16_t y, uint16_t color)
{
	if ((x >= LCD_WIDTH) || (y >= LCD_HEIGHT))	// si coordonnées incorrectes
		return false;							// erreur
		
	GLCD_SetWindow(x, y, 1, 1);					// Fenêtre d'écriture de 1 pixel
	WriteRegister(0x22,color);					// écrire pixel
	FreePins();									// libérer pins
	return true;								// OK
}



//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//                               FONCTIONS PRIVÉES                                      //
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
	
//////////////////////////////////////////////////////////////////////////////////////////
//                                MET UNE PIN À 1                                       //
//////////////////////////////////////////////////////////////////////////////////////////
static __inline void Put1(uint32_t pin)
{ 
	LPC_GPIO0->FIOSET = pin;
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                MET UNE PIN À 0                                       //
//////////////////////////////////////////////////////////////////////////////////////////
static __inline void Put0 (uint32_t pin)
{
	LPC_GPIO0->FIOCLR = pin;
}

//////////////////////////////////////////////////////////////////////////////////////////
//                      ÉCRIT UNE COMMANDE VERS L'AFFICHEUR                             //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// cmd : Commande à envoyer (8 bits)
//----------------------------------------------------------------------------------------
static __inline void WriteCommand (unsigned char cmd)
{
	Put0(LCD_CS);					// communication série en service
	Put0(LCD_RS);					// Sélection pour Commande
	Put1(LCD_RD);					// Lecture OFF

	Put0(LCD_EN);					// Transceiver sur canal 2 : D8/D15
	Put1(LCD_DIR);					// ARM -> LCD
	LPC_GPIO2->FIODIR0 = 0xFF;		// P2.0 à P2.7 en sortie (DB00/DB07)

	LPC_GPIO2->FIOPIN0 = cmd;		// Placer commande
	Put1(LCD_LE);					// Acquisition valeur latch
	Wait(0);						// délai
	Put0(LCD_LE);					// Verrouillage D0/D7
	LPC_GPIO2->FIOCLR0 = 0xFF;		// D8/D15 = 0
	Wait(0);						// délai
	Put0(LCD_WR);					// Ordre d'écriture dans le LCD
	Wait(0);						// délai
	Put1(LCD_WR);					// Fin de commande
	Put1(LCD_CS);					// fin de communication
}

//////////////////////////////////////////////////////////////////////////////////////////
//                        ÉCRIT UNE DATA VERS L'AFFICHEUR                               //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// dat : Data à envoyer (16 bits)
// Condition: doit avoir été précédée d'un WriteCommand
//----------------------------------------------------------------------------------------
static __inline void WriteData (unsigned short dat)
{
	Put0(LCD_CS);								// communications en service
	Put1(LCD_RS);								// Sélection pour Data
	Put1(LCD_RD);								// Lecture OFF

	LPC_GPIO2->FIOPIN0 = (unsigned char)(dat & 0xFF);	// placer D7/D0
	Put1(LCD_LE);								// Acquisition valeur latch
	Wait(0);									// délai
	Put0(LCD_LE);								// Verrouillage D0/D7

	LPC_GPIO2->FIOPIN0 = (unsigned char)(dat >> 8);		// D8/D15 sur transceiver
	Wait(0);									// délai

	Put0(LCD_WR);								// Ordre d'écriture dans le LCD
	Wait(0);									// délai
	Put1(LCD_WR);								// Fin de commande

	Put1(LCD_CS);								// fin des communications
}

//////////////////////////////////////////////////////////////////////////////////////////
//                           LIT UNE DATA DE L'AFFICHEUR                                //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// Reçoit un mot de 16 bits en provenance du LCD
// D0/D7 sont reçus via le canal A du Transceiver
// D8/D15 sont reçus via le canal B du transceiver
//----------------------------------------------------------------------------------------
static __inline unsigned short ReadData(void) 
{
	unsigned short result = 0;
  
	LPC_GPIO2->FIODIR0 = 0x00;						// P2.0/P2.7 en entrée (DB00/DB07)
	Put0(LCD_CS);									// communications en service
	Put1(LCD_WR);									// Pas d'écriture
	Put0(LCD_DIR);									// LCD -> ARM
	Put1(LCD_RS);									// Data
		
	Put0(LCD_RD);									// Ordre de lecture
	Put0(LCD_EN);									// Transceiver D8/D15
	Wait(10);										// Délai
	result =(unsigned short) (LPC_GPIO2->FIOPIN & 0xFF); // Lire D8/D15
	Put1(LCD_EN);									// Transceiver D0/D7
	Wait(0);										// Délai
	result = (result << 8) | ((unsigned short)LPC_GPIO2->FIOPIN & 0xFF); // Lire D0/D7

	Put1(LCD_RD);									// fin de lecture
	Wait(0);										 // délai
	Put0(LCD_EN);								// Ne pas mettre DIR et EN à 1 en même temps
	Put1(LCD_DIR);						 // Remettre ARM->LCD
	Put1(LCD_CS);							// Fin des communications
	
	return result;					// Retourner data
}

//////////////////////////////////////////////////////////////////////////////////////////
//                        LIT UN REGISTRE DE L'AFFICHEUR                                //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// reg : registre à interroger
//----------------------------------------------------------------------------------------
static unsigned short ReadRegister(unsigned short reg)
{
	unsigned short result = 0;

	Put0(LCD_CS);				// communications en service
	Put0(LCD_RS);				// Mode registre

	WriteCommand(reg);			// Écrire adresse du registre
	Wait(0);					// Attendre
	result = ReadData();		// Lire le contenu du registre
	
	Put1(LCD_RS);				// Fin du mode registre	
	Put1(LCD_CS);				// fin des communications

	return result;				// retourner résultat
}

//////////////////////////////////////////////////////////////////////////////////////////
//                        ÉCRIRE DANS UN REGISTRE DE L'AFFICHEUR                        //
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// reg : adresse du registre
// value: Valeur à placer
//----------------------------------------------------------------------------------------
static void WriteRegister(unsigned char reg, unsigned short value)
{
	WriteCommand(reg);			// envoyer adresse du registre
	Wait(0);					// Attendre
	WriteData(value);			// envoyer valeur à écrire
}

//////////////////////////////////////////////////////////////////////////////////////////
//                          LIBÉRER LE PORT P2.0->P2.7                                  //
//////////////////////////////////////////////////////////////////////////////////////////
static __inline void FreePins(void)
{
	LPC_GPIO2->FIODIR0 = 0x00;					// P2.0/P2.7 en entrée (DB00/DB07)	
	Put0(LCD_EN);								// Transceiver sur canal 2 : D8/D15
	Put1(LCD_DIR);								// ARM->LCD
}




