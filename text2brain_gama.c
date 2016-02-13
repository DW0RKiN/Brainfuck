/***************************************************************************
 *                                                                         *
 *  String to Brainfuck, variant Gamma                                     *
 *  Converts a string to a brainfuck code that prints that string.         *
 *                                                                         *
 *  ++[<±...<A1<A0[>]<-------]B[<±...<C1<C0[>]<-]<D0<D1<±...<[.>]          *
 *  ++[<-P<±...<A1<A0[>+P]<-------]<B[<±...<C1<C0[>]<-]<D0<D1<±...<[.>]    *
 *                                                                         *
 *                                                                         *
 *  string[0] = FceA(A0)   + B*C0 + D0                                     *
 *  string[1] = FceA(A1+P) + B*C1 + D1                                     *
 *                                                                         *
 *  FceA[-3..3] = 182, 36, 146, 0, 110, 220, 74                            *
 *                                                                         *
 ***************************************************************************/


#include <stdio.h>
// #include <stdlib.h>



int abs( int i ) {
	if ( i < 0 ) return -i;
	return i;
}


// "..." + ´.´ 
char * pridej_znak( char znak, char * kam ) {
	*kam = znak;
	return ++kam;
}


// "..." + "..." 
char * pridej_retezec( char * retezec, char * kam ) {
	while ( *retezec ) {
		*kam = *retezec;
		kam++;
		retezec++;
	}
	return kam;
}


// (i < 0) "..." + "---..." 
// (i > 0) "..." + "+++..." 
char * opakuj(char i, char * kam) {
	if ( i == 0 ) return kam;
	char znak = '+';
	if ( i < 0 ) {
		i = -i;
		znak = '-';
	}
	while ( i-- ) {
		*kam = znak;
		kam++;
	}
	return kam;
}




// *************************
// Retezece znaku je preveden do nasledujiciho tvaru:
//
// "++[<±...<±...<±...<±...<±...[>]<-------]+...[<±...<±...<±...<±...[>]<-]<±...<±...<±...<±...[.>]"
//
// "++[<±...<±...<±...<±...<A[>]<-------]nasobek[<±...<±...<±...<B[>]<-]<C<±...<±...<±...[.>]"
//
// veta[0] = {0,36,74,110,146,182,220} + B*nasobek + C
// A = -3 => 182, A = -2 => 36, A = -1 => 146, A = 0 => 0, A = 1 => 110, A = 2 => 220, A = 3 => 74 
//
// Prvni smycka je vybrana tak, aby jedine plus vytvorilo hodnotu 110, ktera lezi uprostred malych pismen, ktera jsou nejpouzivanejsi.
// Tri male plus vytvori hodnotu 74, ktera je uprostred velkych pismen.
// Dve mele minus vytvori hodnotu 36, ktera je blizko mezery.
// Druha smycka a nasledujici korekce se snazi jen opravit chyby ±19 od hodnot prvni z smycky.
// Pametova narocnost algoritmu je co znak to bunka.
//
char * Varianta_C(char * veta, char * vysledek,  int nasobek ) {

// ++[-------<++>]	220	... + 36 = 0
// ++[-------<--->]	182	... + 38 = 220
// ++[-------<->]	146	... + 36 = 182
// ++[-------<+>]	110	... + 36 = 146
// ++[-------<+++>]	74	... + 26 = 110
// ++[-------<-->]	36	... + 28 = 74
// ++[-------<>]	0	... + 36 = 36
	
	int vysledky[7]	= { 0, 36, 74, 110, 146, 182, 220 };
	int posuny[7]	= { 0, -2,  3,   1,  -1,  -3,   2 };
	
	int delka = 0;
	while ( veta[delka] && veta[delka] != 27 ) delka++;	// ukonceni nulou nebo ESC znakem

	char optimalni_pocatky[delka];
	char optimalni_opravy[delka];
	char optimalni_korekce[delka];
	
	int i,j,k;
	int pricti = ( nasobek - 1) >> 1;
	int ignorovano = 0;
	
	i = delka;
	while ( i-- ) {
		j = 7;
		int best_d = 256;
		while ( j-- ) {
			k = abs( veta[i] - vysledky[j] );

			int nas = ( k + pricti ) / nasobek;
			int kor = ( k + pricti ) % nasobek;
			kor -= pricti;			
			int d = abs(posuny[j]) + nas + abs(kor);
		
			if ( d < best_d ) {
				
				if ( j == 0 ) {
					if ( i + ignorovano + 1 == delka ) {
						ignorovano++;
					} else break;
				}
				best_d = d;
				optimalni_pocatky[i] = posuny[j];
				if ( veta[i] - vysledky[j] >= 0 ) {
					optimalni_opravy[i] = nas;
					optimalni_korekce[i] = kor;
				} else {
					optimalni_opravy[i] = -nas;
					optimalni_korekce[i] = -kor;					
				}
			}
		}
	}
	
	
	int best_p;
	int best_d = -1;
	
	for ( j = -3; j <= 3; j++ ) {
	
		k = 0;
		if ( j ) {
			k += 2 + 2*abs( j ); // ++[<-j...[>j]<-------]>
			if ( ignorovano ) k--;
		}

		i = delka - ignorovano;
		while ( i-- > 1 ) k += abs( optimalni_pocatky[i] - j);

		if ( k < best_d  || best_d < 0 ) {
			best_p = j;
			best_d = k;
		}
	}
	
	
	if ( best_p ) {
		i = delka - ignorovano;
		while ( i-- > 1 ) optimalni_pocatky[i] -= best_p;
	}
	
//	==============================
//	Vypis pocatku
//	++[
	vysledek = pridej_retezec("++[", vysledek );
	if ( best_p ) {
		vysledek = pridej_znak('<', vysledek );
		vysledek = opakuj( -best_p , vysledek );
	}


//	==============================
//	Vypis hodnot prvni smycky
//	< optimalni_pocatky[delka-1] < optimalni_pocatky[delka-2] <...
	i = delka - ignorovano;
	while ( i-- ) {
		vysledek = pridej_znak('<', vysledek );
		vysledek = opakuj( optimalni_pocatky[i] , vysledek );
	}
	
	
//	==============================
//	Vypis konce prvni smycky
//	[>]<-------]
	if ( best_p ) {
		vysledek = pridej_retezec("[>", vysledek );
		vysledek = opakuj( best_p, vysledek );
		vysledek = pridej_retezec("]>-------]", vysledek );
	} else 
		vysledek = pridej_retezec("[>]<-------]", vysledek );

//	======================
//	Prodluzeni druhe smycky o predtim ignorovane (zkracuje '\n' na konci)
//	>
	i = ignorovano;
	if ( best_p ) i--;
	if ( i < 0 ) 
		vysledek = pridej_znak('<', vysledek );
	else 
		while ( i-- ) vysledek = pridej_znak('>', vysledek );

	
//	======================
//	Vypis pocitadla druhe smycky
//	+...[
	vysledek = opakuj( nasobek, vysledek );	// counter
	vysledek = pridej_znak('[', vysledek );
	

//	==============================
//	Vypis hodnot druhe smycky
//	< optimalni_opravy[delka-1] < optimalni_opravy[delka-2] <...
	i = delka;
	while ( i-- ) {
		vysledek = pridej_znak('<', vysledek );
		vysledek = opakuj( optimalni_opravy[i] , vysledek );
	}
	
	
//	==============================
//	Vypis konce druhe smycky
//	[>]<-]
	vysledek = pridej_retezec("[>]<-]", vysledek );
	
	
//	==============================
//	Vypis korekci
//	< optimalni_korekce[delka-1] < optimalni_korekce[delka-2] <...
	i = delka;
	while ( i-- ) {
		vysledek = pridej_znak('<', vysledek );
		vysledek = opakuj( optimalni_korekce[i] , vysledek );
	}
	

//	==============================
//	Dokonceni
//	[.>]
	vysledek = pridej_retezec("[.>]", vysledek );

	*vysledek = 0;
	return vysledek;

}




#define MAX_BUFFER	2048

int main() {

	char retezec[MAX_BUFFER]; 
	char vysledek[MAX_BUFFER*20];

	fprintf(stderr,"Ukonceni v linuxu ctrl+d, pokud se nema prevadet ENTER tak prvne stisknete ESC.\n");

	
	while ( 1 ) {
		fprintf(stderr,"Napis neco...\n");
		if ( fgets(retezec, sizeof(retezec)-1, stdin) == NULL ) return -1;	// nacte jeden radek do bufferu
		
		int nasobek;
		int best_n, best_d = MAX_BUFFER*20;

		for ( nasobek = 3; nasobek <= 13; nasobek++ ) { 
			char * pocatek = &vysledek[0];
			pocatek = Varianta_C( &retezec[0], pocatek, nasobek );
#if 0
		fprintf(stderr,"nasobek: %i delka %li\n", nasobek, pocatek - &vysledek[0] );
		printf("%s", &vysledek[0]);
		putchar('\n');		
#endif		
			
			if ( pocatek && pocatek - &vysledek[0] < best_d ) {
				best_d = pocatek - &vysledek[0];
				best_n = nasobek;
			}
		}
		
		fprintf(stderr,"nasobek: %i delka %i\n", best_n, best_d);
		Varianta_C( &retezec[0], &vysledek[0], best_n );		
		printf("%s", &vysledek[0]);
		putchar('\n');		
	}
	return 0;

}
