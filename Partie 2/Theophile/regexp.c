#include "regexp.h"
#include <string.h>

ADERIV nouvel_arbre(STATE s, char c){
	ADERIV a = malloc(sizeof(struct aderiv));
	if(!a) {printf("Plus de mémoire, malloc a échoué.\n"); exit(5);}
	a->s = s;
	a->caractere = c;
	a->fils[0] = NULL;
	a->fils[1] = NULL;
	a->fils[2] = NULL;
	return a;
}

void liberer_arbre(ADERIV a){
	if(a){
		for(int i = 0; i < 3; i++) liberer_arbre(a->fils[i]);
		free(a);
	}
}


int indice_char(char c){//retourne l'indice correspondant au caractère dans le tableau des états
	switch(c){
		case '+': return 0;
		case '.': return 1;
		case '*': return 2;
		case '(': return 3;
		case ')': return 4;
		case '#': return 6;
		default:
			if( c >= 'a' && c <= 'z') return 5;
			printf("Caractère inconnu dans l'expression régulière.\n");
			exit(3);
	}
}

ADERIV construire_arbre_derivation(char *expr){
	STATELISTE table[7][7] = {//cette table représente la table des transitions de l'énoncé
		{{-1},{-1},{-1},{2,{A,B}},{-1},{2,{A,B}},{-1}}, 	// transition quand le STATE S est lu
		{{-1},{-1},{-1},{2,{C,D}},{-1},{2,{C,D}},{-1}},		//STATE A
		{{3,{PLUS,A,B}},{-1},{-1},{-1},{0},{-1},{1,{CAR}}},	//STATE B
		{{-1},{-1},{-1},{2,{E,F}},{-1},{2,{E,F}},{-1}},		//STATE C
		{{0},{3,{POINT,C,D}},{-1},{-1},{0},{-1},{0}},		//STATE D
		{{0},{0},{0},{3,{PARO,S,PARF}},{-1},{1,{CAR}},{-1}},//STATE E
		{{0},{0},{2,{ETOILE,F}},{-1},{0},{-1},{0}}			//STATE F
	};
	//Une STATELISTE de taille 0 correspond à une règle dont la production est epsilon.
	//Une STATELISTE de taille -1 correspond à une erreur (expression rejetée)
	
	int taille = strlen(expr);
	PILE p = nouvelle_pile(taille*2);
	STATE symbole;
	STATE courant;
	char carCourant, tampon;
	
	if(expr[strlen(expr) - 1] != '#'){
		printf("ERR : expression reguliere %s erreur de syntaxe manque # en caractere de fin\n", expr);
		return NULL;
	}
	
	p = empiler(p, S);
	
	carCourant = expr[0];
	printf("premier car : %c\n" ,carCourant);
	switch(carCourant){
		case '+': 
			courant = PLUS;
			break;
		
		case '.': 
			courant = POINT;
			break;
		
		case '*': 
			courant = ETOILE;
			break;
		
		case '(': 
			courant = PARO;
			break;
			
		case ')': 
			courant = PARF;
			break;
		//case '#': symbole = ; A definir
		default:
			if( carCourant >= 'a' && carCourant <= 'z')
				courant = CAR;
			
			else{
				printf("Caractère inconnu dans l'expression régulière.\n");
				exit(3);
			}
	}
	
	p = empiler(p, courant);
	symbole = depiler(&p);
	p = empiler(p, courant);
	
	affiche_pile(p);	
	
	int index = 1;
	while( index != 4 ){
		// Non terminaux
		if(symbole < CAR){
			printf("car : %c\n", tampon);
			printf("taille : %d\n", table[symbole][indice_char(tampon)].taille);
			for(int cmpt = 0; cmpt < table[symbole][indice_char(tampon)].taille; cmpt++){
				p = empiler(p, table[symbole][indice_char(tampon)].liste[cmpt]);
			}
			
			symbole = depiler(&p);
			p = empiler(p, courant);
			affiche_pile(p);
		}
		
		// Terminaux
		else if( symbole > F ){
			if( symbole == courant ){
				symbole = depiler(&p);
				
				symbole = depiler(&p);
				p = empiler(p, symbole);
				
				tampon = carCourant;
				carCourant = expr[index];
				affiche_pile(p);
				index++;
			}
		}
	}
	
	if( p.sommet == 0 && p.taille_max == index )
		printf("l'expression reguliere : %s est reconnue", expr);
	
	else
		printf("l'expression reguliere : %s n'est pas reconnue", expr);
	
	liberer_pile(p);
	return NULL;
}

void affiche_aderiv(ADERIV a, int space){//rendre joli
	//affiche les fils de gauche à droite
	if(a){
		affiche_aderiv(a->fils[2], space + 1);
		affiche_aderiv(a->fils[1], space + 1);
		for(int i = 0; i < space; i++) printf("   ");
		affiche_state(a->s);
		if(a->s == CAR) printf(" : %c",a->caractere);
		printf("\n");
	    affiche_aderiv(a->fils[0], space + 1);
	}
}
