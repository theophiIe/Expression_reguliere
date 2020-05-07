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
		{{-1},{-1},{-1},{2,{A,B}},{-1},{2,{A,B}},{-1}}, // transition quand le STATE S est lu
		{{-1},{-1},{-1},{2,{C,D}},{-1},{2,{C,D}},{-1}},//STATE A
		{{3,{PLUS,A,B}},{-1},{-1},{-1},{0},{-1},{1,{CAR}}},//STATE B
		{{-1},{-1},{-1},{2,{E,F}},{-1},{2,{E,F}},{-1}},//STATE C
		{{0},{3,{POINT,C,D}},{-1},{-1},{0},{-1},{0}},//STATE D
		{{0},{0},{0},{3,{PARO,S,PARF}},{-1},{1,{CAR}},{-1}},//STATE E
		{{0},{0},{2,{ETOILE,F}},{-1},{0},{-1},{0}}//STATE F
	};
	//Une STATELISTE de taille 0 correspond à une règle dont la production est epsilon.
	//Une STATELISTE de taille -1 correspond à une erreur (expression rejetée)
	int taille = strlen(expr);
	PILE p = nouvelle_pile(taille*2);
	PILE p_renverser = nouvelle_pile(taille*2);
	
	ADERIV arbre = nouvel_arbre(S,'0');
	p = empiler(p,S);
	
	int statelist_taille, sommet_pile;
	STATE state_sommet;
	
	int i;
	for( i=0 ; i < taille ; i++ ){
		
		sommet_pile = p.sommet - 1;
		state_sommet = p.contenu[sommet_pile];
		
		printf("EXPRESSION = ");
		for( int k=i ; k < taille ; k++ )
			printf("%c",expr[k]);
		printf("\n");
		printf("PILE p = ");
		affiche_pile(p);
		
		if( state_sommet < CAR ){		//Si le symbole en haut de la pile est un non terminal...
			
			statelist_taille = table[state_sommet][indice_char(expr[i])].taille;
			if( statelist_taille != -1){		//Si la règle éxiste...
				depiler(&p);	//On dépile le symbole
				for( int j=0 ; j < statelist_taille ; j++ )	//On empile les symboles correspondants dans p_renverser
					p_renverser = empiler(p_renverser, table[state_sommet][indice_char(expr[i])].liste[j]);	//necessaire pour mettre les STATE dans le bon ordre dans la PILE p
				
				for( int j=0 ; j < statelist_taille ; j++ )	//On dépile p_renverser pour empiler les symboles dans le bon ordre dans p
					p = empiler(p, depiler(&p_renverser));
				i--;				//Permet de rester sur le même caractère courant pour la prochaine itération
			}
				
				
			else{ 							//Si la règle n'éxiste pas...
				printf("Le mot n'est pas reconnu... (1er else)\n");
				liberer_pile(p);
				liberer_pile(p_renverser);
				return NULL;
			}
				
		}
		
		else{							//Si le symbole en haut de la pile est un terminal...
			
			//~ if( state_sommet == CAR ){		//Si c'est un caractère...
				//~ printf("arbre->caractere = %c	expr[i] = %c\n",arbre->caractere,expr[i]);
				//~ if( arbre->caractere == expr[i] )
					//~ depiler(&p);	//On dépile le symbole
			//~ }
			
			//~ else if( state_sommet > CAR ){	//Si le symbole du haut de la pile est un terminal autre qu'un caractère...
				
				//~ STATE tmp;
				//~ switch(expr[i]){
					//~ case '+': tmp = PLUS;
					//~ case '.': tmp = POINT;
					//~ case '*': tmp = ETOILE;
					//~ case '(': tmp = PARO;
					//~ case ')': tmp = PARF;
				//~ }
				//~ //printf("state_sommet = %d	tmp = %d\n",state_sommet,tmp);
				//~ if( state_sommet == tmp-1 )		//POURQUOI le '-1' ?? Je ne sais pas...
					//~ depiler(&p);	//On dépile le symbole
				//~ else if( expr[i] == '#')
					//~ break;
			//~ }
			
			//~ else{										//Si le symbole du haut de la pile est différent du caractère courant...
				//~ printf("Le mot n'est pas reconnu... (2ème else)\n");
				//~ liberer_pile(p);
				//~ liberer_pile(p_renverser);
				//~ return NULL;
			//~ }
			depiler(&p);
		}
		printf("\n");
	}
	
	affiche_pile(p);
	
	if( ! est_vide(p) ){
		printf("Le mot n'est pas reconnu... (pile non vide)\n");
		liberer_pile(p);
		liberer_pile(p_renverser);
		return NULL;
	}
	
	liberer_pile(p);
	liberer_pile(p_renverser);
	
	printf("---------- Le mot est reconnu ! -----------\n");
	
	return arbre;
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
