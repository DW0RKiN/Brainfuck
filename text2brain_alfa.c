/************************************************************************
 *                                                                      *
 *  String to Brainfuck, variant Alpha                                  *
 *  Converts a string to a brainfuck code that prints that string.      *
 *                                                                      *
 *  <±...<±...< ±A1 < ±A0 < +B [> ±C0 > ±C1 >±...>±...<[> +D <<]-]>[.>] *
 *                                                                      *
 *  string[0] = B*D + B*C0 + A0                                         *
 *  string[1] = B*D + B*C1 + A1                                         *
 *                                                                      *
 ************************************************************************/


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



// [>+++++>+++++>+++++>++++>++++++<<<<<-] -> [>>>>->+<[>+++++<<]>-]
//
// Hledani nejlepsi hodnoty pro ...[>..>..>..<[> ??? <<]>-]...
// Ktera usetri co nejvice znaku.
// Nejlepsich variant muze byt ale nekolik se shodnym min_sum. Fce vraci prvni nalezenou.
int nejlepsi_substituce( char * buffer, int delka )
{
	int i, sub, best_sub;
	int min_sum = 0;
	int min =  127;
	int max = -128;
	
	for ( i = 0; i < delka; i++ ) {
		min_sum += 128;
		if ( min > buffer[i] ) min = buffer[i];
		if ( max < buffer[i] ) max = buffer[i];
	}
	
	for ( sub = min; sub <= max; sub++ ) {
		int sum = abs( sub );
		i = delka;
		while ( i-- ) sum += abs ( buffer[i] - sub );

		if ( sum < min_sum ) {
			min_sum = sum;
			best_sub = sub;
		}
	}
	return best_sub;
}



// *************************
// Retezece znaku je preveden do nasledujiciho tvaru:
//
// "<±...<±...<±...<±...<...[>±...>±...>±...>±...<[>...<<]-]>[.>]"
//
// "<±...<±A<±...<±...<B[>±...>±...>±C>±...<[>D<<]>-]>[.>]"
//
// veta[2] = B*D + B*C + A
// A = buffer[2] = -2, B = nasobek = 10, C = 1 D = 11 = substituce = nejlepsi_substituce + posun
// veta[2] = 110 + 10 - 2 = 118
//
// ktery setri delku programu diky vhodne substituci "D".
// Usetrime hromadne za znaky + nebo - ve smycce a i za opakovany < pro navrat k citaci. 
// Vznika tim ale problem, ze pri zpetnem chodu nesmi byt nikdy zadna pametova hodnota nulova.
// Pametova narocnost algoritmu je co znak to bunka.
//
char * Varianta_A(char * veta, char * vysledek,  int nasobek, int posun ) {

	int delka = 0;
	while ( veta[delka] && veta[delka] != 27 ) delka++;	// ukonceni nulou nebo ESC znakem

// 	char buffer[delka];	misto tech 4 nasledujicich staci jeden, ale je to opruz se v tom pak vyznat, spousta deleni a nasobeni navic
	char optimalni_nasobky[delka];
	char optimalni_pocatky[delka];
	char upravene_nasobky[delka];
	char upravene_pocatky[delka];
	
	int i;
	int substituce;
	
	i = delka;
	while ( i-- ) {
// 		buffer[i] = (veta[i] +  (nasobek >> 1) ) / nasobek;
		optimalni_nasobky[i] = (veta[i] +  (nasobek >> 1) ) / nasobek;
		optimalni_pocatky[i] = (veta[i] +  (nasobek >> 1) ) % nasobek;
		optimalni_pocatky[i] -= (nasobek >> 1);
		upravene_nasobky[i] = optimalni_nasobky[i];
		upravene_pocatky[i] = optimalni_pocatky[i];
	}

	substituce = nejlepsi_substituce( &optimalni_nasobky[0], delka ) + posun;

//	=================================
//	Test na nulu pri provadeni smycky
	i = delka;
	while ( i-- ) {

		int zvedni = 0;	// pokud se rovna 1, bude "zmena" navysena
		int zmena = -1;	// -1 1 -2 2 -3 3 -4 4... nebo s otocenym znamenkem
		if ( optimalni_pocatky[i] >= 0 ) zmena = 1;	// pocatek je kladny? optimalni_nasobky[i]*nasobek je pod hledanou hodnotou, nejblizsi nasledujici je optimalni_nasobky[i]+1
		
		while ( i < delka - 1 || ( substituce < 3 ) ) {	// Posledni hodnota se da ignorovat protoze diky tomu ze se snazime nezvednout i pocitadlo smycky <[>...<<]>-] kolize nenastane

			if ( zmena > 8 || zmena < -8 ) {
				fprintf(stderr, "Nenalezeno reseni u nasobek = %i, posun = %i pro veta[%i] = %i\n", nasobek, posun, i, veta[i] );
				return NULL;
// 				jeste by slo udelat reseni ze az po tento index se vse vyjme ze substituce
			}

			char bunka = upravene_pocatky[i] - substituce;

			int j = 1 + nasobek;
			while ( --j ) {
				bunka += upravene_nasobky[i];
				if ( bunka == 0 ) break;	// chyba! hodnota nabyva nuly pred chodem zpet ve smycce
			}
		
			if ( j ) {	// reseni chyby
				upravene_nasobky[i] = optimalni_nasobky[i] + zmena;
				upravene_pocatky[i] = veta[i] - nasobek * upravene_nasobky[i];
				
				zmena = - zmena;
				if ( zvedni ) {	// kazde druhe zvedne "zmena"
					if ( zmena > 0 ) zmena++; else zmena--;	// posunuti o 1
				}
				zvedni = 1 - zvedni;
				continue;
			}
			break;
		}
	}
	
//	=========================================================
//	Test zkraceni pri vynechani substituce u poslednich znaku
//	[............>--------<[>++++++++++<<]>-] -> [............>++<<[>++++++++++<<]>-]

	int ignorovano = 0;
	i = delka;
	while ( i-- > 1 ) {

		int   bez_substituce =  abs( optimalni_nasobky[i] )              + abs( optimalni_pocatky[i] ) + 1;
		int	se_substituci  =  abs( upravene_nasobky[i] - substituce )  + abs( upravene_pocatky[i] );

		// Tahle cast se snazi zohlednit alespon pro posledni znak i to ze pokud na zacatku bude 0<0<0< tak to lze cele vynechat.
		if ( ignorovano == 0 ) {
			if ( optimalni_pocatky[i] ) bez_substituce++;
			if ( upravene_pocatky[i] ) se_substituci++;
		}

		int   bez_substituce2 = bez_substituce + abs( optimalni_nasobky[i-1] )              + abs( optimalni_pocatky[i-1] ) + 1;
		int   se_substituci2  = se_substituci  + abs( upravene_nasobky[i-1] - substituce )  + abs( upravene_pocatky[i-1] );
		
		if ( bez_substituce < se_substituci || bez_substituce2 < se_substituci2 ) {
			ignorovano++;
			upravene_nasobky[i] = optimalni_nasobky[i];
			upravene_pocatky[i] = optimalni_pocatky[i];
		} else break;
		
	}

//	====================
//	Vypis pocatku odzadu
//	<...<...<...<...
	i = delka;
	char * odkud = vysledek;
	while ( i-- ) {
		vysledek = opakuj( upravene_pocatky[i] , vysledek );
		if ( vysledek - odkud ) vysledek = pridej_znak('<', vysledek);	// dokud se nic nevypsalo tak se neposouvame
	}

//	======================
//	Vypis pocitadla smycky
//	+...[
	vysledek = opakuj( nasobek, vysledek );	// counter
	vysledek = pridej_znak('[', vysledek );

//	==============================
//	Vypis zkracenych hodnot smycky
//	> ( upravene_nasobky[i] - substituce ) >...>...
	i = 0;
	while ( i < delka - ignorovano ) {
		vysledek = pridej_znak('>', vysledek );
		vysledek = opakuj( upravene_nasobky[i] - substituce , vysledek );
		i++;
	}

//	==============================
//	Vypis nezkracenych hodnot smycky
//	> optimalni_nasobky[i] >...>...
	while ( i < delka ) {
		vysledek = pridej_znak('>', vysledek );
		vysledek = opakuj( optimalni_nasobky[i] , vysledek );
		i++;
	}

//	===============================================
//	Hromadny vypis < u ignorovanych pres substituci
//	<<<<... 
	i = ignorovano;
	while ( i-- ) vysledek = pridej_znak('<', vysledek );

//	==========================
//	Vypis smycky se substituci
	if ( substituce < 3 ) {	// [+++<]>----]
		vysledek = pridej_znak('[', vysledek );
		vysledek = opakuj( substituce, vysledek );
		vysledek = pridej_retezec("<]>", vysledek );
		vysledek = opakuj( -substituce -1, vysledek );
		vysledek = pridej_znak(']', vysledek );
	} else {			// <[>+++<<]>-]
		vysledek = pridej_retezec("<[>", vysledek );
		vysledek = opakuj( substituce, vysledek );
		vysledek = pridej_retezec("<<]>-]", vysledek );
	}
// vysledek = pridej_znak('#', vysledek );

	vysledek = pridej_retezec(">[.>]", vysledek );
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
		
		int nasobek, posun;
		int best_n, best_p, best_d = MAX_BUFFER*20;

		for ( nasobek = 7; nasobek <= 13; nasobek++ ) { 
			for ( posun = -4; posun <= 4; posun++ ) {
				char * pocatek = &vysledek[0];
				pocatek = Varianta_A( &retezec[0], pocatek, nasobek, posun );
				if ( pocatek && pocatek - &vysledek[0] < best_d ) {
					best_d = pocatek - &vysledek[0];
					best_n = nasobek;
					best_p = posun;
				}
		
			}
		}
		
		fprintf(stderr,"nasobek: %i posun %i delka %i\n", best_n, best_p, best_d);
		Varianta_A( &retezec[0], &vysledek[0], best_n, best_p );		
		printf("%s", &vysledek[0]);
		putchar('\n');		
	}
	return 0;

}
