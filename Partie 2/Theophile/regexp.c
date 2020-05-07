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

ADERIV construc_recursive(ADERIV *a, STATELISTE table[7][7], char *expr, int *index, int taille, char *carCourant, PILE *p, PILE *paro, STATE *symbole){
	//On dépile si la pile n'est pas vide
	if(!est_vide(*p)){
		*symbole = depiler(p);
	}
	
	else{
		printf("ERR : expression reguliere %s non reconnue\n", expr);
		return NULL;
	}
	
	// Non terminaux
	if(*symbole < CAR){
		if(table[*symbole][indice_char(*carCourant)].taille != -1){
			
			for(int cmpt = table[*symbole][indice_char(*carCourant)].taille - 1; cmpt > -1; cmpt--){
				*p = empiler(*p, table[*symbole][indice_char(*carCourant)].liste[cmpt]);
				*a -> fils[table[*symbole][indice_char(*carCourant)].taille - cmpt];
			}
			
			a = construc_recursive(*a, table[7][7], *expr, *index, taille, *carCourant, *p, *paro,*symbole)
		}
		
		else{
			printf("ERR : expression reguliere %s non reconnue\n", expr);
			return NULL;
		}
		
		affiche_pile(*p);	
	}
	
	// Terminaux
	else if(*symbole > F){
		if(*symbole == CAR){
			if(*carCourant >= 'a' && *carCourant <= 'z'){
				*carCourant = expr[*index];
				(*index)++;
			}
			
			else if(*carCourant == '#'){
				return a;
			}

			
			else{
				printf("ERR : expression reguliere %s erreur de syntaxe manque # en caractere de fin\n", expr);
				return NULL;
			}
		}
		
		else if(*symbole == PARO){
			if(*carCourant == '('){
				*carCourant = expr[*index];
				(*index)++;
				*paro = empiler(*paro, PARO);
			}
			
			else{
				printf("ERR : expression reguliere %s erreur de syntaxe manque # en caractere de fin\n", expr);
				return NULL;
			}
		}
		
		else if(*symbole == PARF){
			if(*carCourant == ')'){
				*carCourant = expr[*index];
				(*index)++;
				
				if(!est_vide(*p))
					depiler(paro);
					
				else{
					printf("ERR : expression reguliere %s erreur de syntaxe le mot de Dick n'est pas respecté\n", expr);
					return NULL;
				}
			}
			
			else{
				printf("ERR : expression reguliere %s erreur de syntaxe manque # en caractere de fin\n", expr);
				return NULL;
			}
		}
		
		else if(*symbole == PLUS){
			if(*carCourant == '+'){
				*carCourant = expr[*index];
				(*index)++;
			}
			
			else{
				printf("ERR : expression reguliere %s erreur de syntaxe manque # en caractere de fin\n", expr);
				return NULL;
			}
		}
		
		else if(*symbole == POINT ){
			if(*carCourant == '.'){
				*carCourant = expr[*index];
				(*index)++;
			}
			
			else{
				printf("ERR : expression reguliere %s erreur de syntaxe manque # en caractere de fin\n", expr);
				return NULL;
			}
		}
		
		else if(*symbole == ETOILE){
			if(*carCourant == '*'){
				*carCourant = expr[*index];
				(*index)++;
			}
			
			else{
				printf("ERR : expression reguliere %s erreur de syntaxe manque # en caractere de fin\n", expr);
				return NULL;
			}
		}
		
		else{
			printf("ERR : expression reguliere %s erreur de syntaxe manque # en caractere de fin\n", expr);
			return NULL;
		}
	}
	

	affiche_pile(*p);

	printf("dernier car : %c\n", *carCourant);
	if( est_vide(*p) && *index == taille && est_vide(*paro))
		printf("l'expression reguliere : %s est reconnue\n", expr);

	else
		printf("l'expression reguliere : %s n'est pas reconnue\n", expr);
	
	return a;
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
	int index = 1;
	char carCourant;
	PILE p 	  = nouvelle_pile(taille*2);
	PILE paro = nouvelle_pile(taille);
	STATE symbole;
	ADERIV a;
	
	if(expr[strlen(expr) - 1] != '#'){
		printf("ERR : expression reguliere %s erreur de syntaxe manque # en caractere de fin\n", expr);
		liberer_pile(p);
		return NULL;
	}
	
	// On init le caractere courant
	carCourant = expr[0];
	printf("premier car : %c\n" ,carCourant);
	
	// On empile S
	p = empiler(p, S);
	symbole = depiler(&p);
	
	//Création de l'arbre
	a = nouvel_arbre(symbole, carCourant);
	
	p = empiler(p, S);

	
	affiche_pile(p);	
	
	a = construc_recursive(&a, table, expr, &index, taille, &carCourant, &p, &paro, &symbole);
	
	liberer_pile(p);
	liberer_pile(paro);
	
	return a;
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

