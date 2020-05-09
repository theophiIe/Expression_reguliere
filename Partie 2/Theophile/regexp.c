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

//Construction de l'arbre noeud par noeud, récursivement
ADERIV construc_recursive(STATELISTE table[7][7], char *expr, int *index, int *error, PILE *p){
	ADERIV noeud  = NULL; 
	STATE symbole;
	char carCourant = expr[*index];
	int taille = -1;
	
	// Si on rencontre une erreur on return NULL
	// on ne continue pas à parcourir l'arbre
	if(*error == 1 || *error == 2)
		return NULL;
	
	//On dépile si la pile n'est pas vide
	if(!est_vide(*p)){
		symbole = depiler(p);
		taille = table[symbole][indice_char(carCourant)].taille;
	}
	
	else{
		printf("ERR : le mot %s n'est pas reconnu, la pile est vide\n", expr);
		*error = 1;
	}
	
	// Non terminaux
	if(symbole < CAR){
		if(taille != -1){
			for(int cmpt = taille; cmpt > 0; cmpt--)
				*p = empiler(*p, table[symbole][indice_char(carCourant)].liste[cmpt - 1]);
		}
		
		else{
			printf("ERR : le mot %s n'est pas reconnu, il n'y a pas de transition dans la table\n", expr);
			*error = 1;
		}
		
		noeud  = nouvel_arbre(symbole, carCourant);
		for(int cmpt = 0; cmpt < table[symbole][indice_char(noeud -> caractere)].taille; cmpt++)
			noeud -> fils[cmpt] = construc_recursive(table, expr, index, error, p);
	}
	
	// Terminaux
	else if(symbole > F){
		if(symbole == CAR){
			if(carCourant >= 'a' && carCourant <= 'z')
				(*index)++;
			
			else if(carCourant == '#')
				printf("Caractere de fin vu\n");

			else{
				printf("ERR : le mot %s n'est pas reconnu, le symbole n'est pas égal au caractere courant\n", expr);
				*error = 1;
			}
		}
		
		else if(symbole == PARO){
			if(carCourant == '(')
				(*index)++;
			
			else{
				printf("ERR : le mot %s n'est pas reconnu, le symbole n'est pas égal au caractere courant\n", expr);
				*error = 1;
			}
		}

		else if(symbole == PARF){
			if(carCourant == ')')
				(*index)++;
			
			else{
				printf("ERR : le mot %s n'est pas reconnue, le symbole n'est pas égal au caractere courant\n", expr);
				*error = 1;
			}
		}
		
		else if(symbole == PLUS){
			if(carCourant == '+')
				(*index)++;
			
			else{
				printf("ERR : le mot %s n'est pas reconnue, le symbole n'est pas égal au caractere courant\n", expr);
				*error = 1;
			}
		}
		
		else if(symbole == POINT ){
			if(carCourant == '.')
				(*index)++;
			
			else{
				printf("ERR : le mot %s n'est pas reconnue, le symbole n'est pas égal au caractere courant\n", expr);
				*error = 1;
			}
		}
		
		else if(symbole == ETOILE){
			if(carCourant == '*')
				(*index)++;
			
			else{
				printf("ERR : le mot %s n'est pas reconnue, le symbole n'est pas égal au caractere courant\n", expr);
				*error = 1;
			}
		}
		
		else{
			printf("ERR : le mot %s n'est pas reconnue, le symbole n'est pas reconnu\n", expr);
			*error = 1;
		}
		
		noeud = nouvel_arbre(symbole, carCourant);
	}
	
	else{
		printf("ERR : le mot %s n'est pas reconnue, le symbole n'est pas reconnu\n", expr);
		*error = 1;
	}
	
	return noeud;
}

//Vérifie que le parenthésage est correcte
int verif_Dyck(char *expr){
	PILE paro = nouvelle_pile(strlen(expr));
	
	for(int cmpt = 0; cmpt < strlen(expr); cmpt++){
		if(expr[cmpt] == '(')
			paro = empiler(paro, PARO);
			
		else if(expr[cmpt] == ')'){
			if(!est_vide(paro))
				depiler(&paro);
			
			else{
				liberer_pile(paro);
				return 0;
			}
		}
	}
	
	if(!est_vide(paro)){
		liberer_pile(paro);
		return 0;
	}
	
	liberer_pile(paro);
	return 1;
}

//Cette fonction permet de construire l'arbre 
ADERIV construire_arbre_derivation(char *expr){
	STATELISTE table[7][7] = {//cette table représente la table des transitions de l'énoncé
		{{-1},{-1},{-1},{2,{A,B}},{-1},{2,{A,B}},{-1}}, 	  // transition quand le STATE S est lu
		{{-1},{-1},{-1},{2,{C,D}},{-1},{2,{C,D}},{-1}},		  //STATE A
		{{3,{PLUS,A,B}},{-1},{-1},{-1},{0},{-1},{1,{CAR}}},	  //STATE B
		{{-1},{-1},{-1},{2,{E,F}},{-1},{2,{E,F}},{-1}},		  //STATE C
		{{0},{3,{POINT,C,D}},{-1},{-1},{0},{-1},{0}},		  //STATE D
		{{0},{0},{-1},{3,{PARO,S,PARF}},{-1},{1,{CAR}},{-1}}, //STATE E
		{{0},{0},{1,{ETOILE}},{-1},{0},{-1},{0}}			  //STATE F
	};
	//Une STATELISTE de taille 0 correspond à une règle dont la production est epsilon.
	//Une STATELISTE de taille -1 correspond à une erreur (expression rejetée)
	
	int taille = strlen(expr);
	int index  = 0;
	int error  = 0;
	
	PILE p = nouvelle_pile(taille*2);
	
	ADERIV arbre = NULL;
	
	if(expr[strlen(expr) - 1] != '#'){
		printf("ERR : le mot %s n'est pas reconnu, erreur de syntaxe il manque # en caractere de fin\n\n", expr);
		goto error;
	}
	
	if(!verif_Dyck(expr)){
		printf("ERR : le mot %s n'est pas reconnu, le mot de Dyck n'est pas respecté\n\n", expr);
		goto error;
	}
	
	// On empile S
	p = empiler(p, S);

	arbre = construc_recursive(table, expr, &index, &error, &p);
	
	if(error == 0 && est_vide(p) && expr[index] == '#')
		printf("le mot : %s est reconnue\n\n", expr);
	
	else if(error == 1){
		printf("le mot : %s n'est pas reconnue\n\n", expr);
		goto error;
	}
	
	else{
		printf("le mot : %s n'est pas reconnue\n\n", expr);
		goto error;
	}
	
	liberer_pile(p);
	
	return arbre;

error:
	liberer_pile(p);
	liberer_arbre(arbre);
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

