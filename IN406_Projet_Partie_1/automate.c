#include "automate.h"


AUTOMATE automate_creer (int Q){
	AUTOMATE A;
	A.sigma = 26;
	A.Q = Q;
	A.F = calloc(Q,sizeof(int)); //calloc place des zéros dans le tableau: aucun état final
	A.T = calloc(Q,sizeof(TRANSITION));//les 0 placés par calloc correspondent à NULL: toutes les listes sont vides, pas de transition
	return A;
} 

void automate_ajouter_transition (AUTOMATE A, int dep, char car, int arr){
	TRANSITION T = A.T[dep];
	while(T){ //teste si la transition existe et ne fait rien si c'est le cas
		if(T->car == car && T->arr == arr) return;
		T = T->suiv;
	}
	TRANSITION new = malloc(sizeof(struct transition));
	new->car = car;
	new->arr = arr;
	new->suiv = A.T[dep];
	A.T[dep] = new;
}

/*TRANSITION supprimer_transition(TRANSITION T, char car, int arr){
	if(!T) return NULL;
	if(T->car == car && T->arr == arr) {
		TRANSITION suivant = T->suiv;
		free(T);
		return suivant;
	}
	T->suiv = supprimer_transition(T->suiv,car,arr);
	return T;
}*/

/*void automate_supprimer_transition (AUTOMATE A, int dep, char car, int arr){
	supprimer_transition(A.T[dep],car,arr);
}*/

void automate_ajouter_final (AUTOMATE A, int q){
	A.F[q] = 1;
}

/*void automate_supprimer_final (AUTOMATE A, int q){
	A.F[q] = 0;
}*/


TRANSITION copie_liste(TRANSITION T, int decalage, int conserve_epsilon){
	if(!T) return NULL;
	if(!conserve_epsilon && T->car == -1) return copie_liste(T->suiv,decalage,conserve_epsilon);
	TRANSITION nouveau = malloc(sizeof(struct transition));
	nouveau->car = T->car;
	nouveau->arr = T->arr + decalage;	
	nouveau->suiv = copie_liste(T->suiv,decalage,conserve_epsilon);
	return nouveau;
}

AUTOMATE automate_copier(AUTOMATE A, int conserve_epsilon){
	AUTOMATE B = automate_creer(A.Q);
	B.Q = A.Q;
	B.sigma = A.sigma;
	B.F = malloc(sizeof(int)*A.Q);
	memcpy(B.F,A.F,A.Q*sizeof(int));
	B.T = malloc(sizeof(TRANSITION)*A.Q);
	for(int i = 0; i < A.Q; i++) B.T[i] = copie_liste(A.T[i],0,conserve_epsilon);
	return B;
}

void liberer_transition(TRANSITION T){
	if(T){
		liberer_transition(T->suiv);
		free(T);
	}
}

void automate_liberer_memoire(AUTOMATE A){
	free(A.F);
	for(int i = 0; i < A.Q; i++){
		liberer_transition(A.T[i]);
	}
	free(A.T);
}


void automate_ecrire (AUTOMATE A, char* nomfic){
	FILE *F = fopen(nomfic,"w");
	if(F == NULL){
    	printf("échec ouverture fichier %s\n", nomfic);
    	exit(0);
  	}
  	int nombre_finaux = 0;
  	for(int i = 0; i < A.Q; i++) nombre_finaux += A.F[i];
  	int nombre_transition = 0;
  	for(int i = 0; i < A.Q; i++){
  		TRANSITION l = A.T[i];
  		while(l){
  			nombre_transition++;
  			l = l-> suiv;
  		}
  	}
  	fprintf(F,"%d %d %d\n",A.Q,nombre_finaux,nombre_transition);
  	for(int i = 0; i < A.Q; i++){
  		if(A.F[i]) fprintf(F,"%2d ",i);
  	}
  	fprintf(F,"\n");
  	for(int i = 0; i < A.Q; i++){
  		TRANSITION l = A.T[i];
  		while(l){
  			fprintf(F,"%2d %c %2d\n",i,l->car,l->arr);
  			l = l-> suiv;
  		}
  	}
  	fclose(F);
}

/*AUTOMATE automate_lire (char* nomfic){
	FILE *F = fopen(nomfic,"r");
	if(F == NULL){
    	printf("échec ouverture fichier %s\n", nomfic);
    	exit(1);
  	}
  	int nombre_etat, nombre_finaux, nombre_transition;
  	if( fscanf(F,"%d%d%d\n",&nombre_etat,&nombre_finaux,&nombre_transition) != 3) {
  		printf("Entete de fichier mal formé\n");
  		exit(2);
  	}
  	AUTOMATE A = automate_creer(nombre_etat);
  	for(int i = 0; i < nombre_finaux; i++){
  		int etat_final;
  		if(fscanf(F,"%d",&etat_final) != 1){
  			printf("État finaux mal formés\n");
  			exit(2);
  		}
  		automate_ajouter_final(A,etat_final);	
  	} 
  	for(int i = 0; i < nombre_transition; i++){
  		int dep, arr;
  		char car;
  		if(fscanf(F,"%d %c %d\n",&dep,&car,&arr) != 3){
  			printf("Transitions mal formées\n");
  			exit(2);	
  		}
  		automate_ajouter_transition(A,dep,car,arr);
  	}
  	fclose(F);
  	return A;
}*/


void parcours(AUTOMATE A, int etat, int* accessible){//parcours en profondeur du graphe des epsilons transitions
	//calcul accessible l'ensemble des états accessibles à partir de etat
	TRANSITION T = A.T[etat];
	while(T){
		if(T->car == -1 && !accessible[T->arr]){
			accessible[T->arr] = 1;
			parcours(A, T->arr, accessible);
		}
		T = T->suiv;
	}
}

int ** matrice_accessibilite(AUTOMATE A){//calcul la matrice d'accessibilité entre les sommets en utilisant uniquement
//les epsilon transitions
	int ** matrice = malloc(sizeof(int*)*A.Q);
	for(int i = 0; i < A.Q; i++) {
		matrice[i] = calloc(A.Q,sizeof(int));
		parcours(A,i,matrice[i]);
	}
	return matrice;
}


AUTOMATE automate_supprimer_epsilon(AUTOMATE A){

	AUTOMATE B = automate_copier(A,0);//recopie A en enlevant les epsilon transitions
	int **matrice = matrice_accessibilite(A);
	for(int i = 0; i < A.Q; i++){//fermeture avant à partir de A
		for(int j = 0; j < A.Q; j++){
			if(matrice[i][j]){
				if(A.F[j]) automate_ajouter_final(B,i);//on accède de i à un état final par un epsilon chemin donc i final
				TRANSITION T = B.T[j];
				while(T){//on parcourt toutes les transitions (j,c,j') pour ajouter la transition (i,c,j')
					automate_ajouter_transition(B,i,T->car,T->arr);
					T = T->suiv;
				}
			}
		}
	}
	free(matrice);
	return B;
}


/////////////////////////// Partie à faire par les étudiants /////////////////////////////////////////



AUTOMATE automate_epsilon(){
	AUTOMATE A = automate_creer(0);
	
	return A;
}

AUTOMATE automate_une_lettre(char car){
	AUTOMATE A = automate_creer(0);
	
	return A;
}


AUTOMATE automate_concatenation(AUTOMATE A, AUTOMATE B){
	AUTOMATE C = automate_creer(0);
	
	return C;
}

AUTOMATE automate_disjonction(AUTOMATE A, AUTOMATE B){
	AUTOMATE C = automate_creer(0);
	
	return C;
}

AUTOMATE automate_etoile(AUTOMATE A){
	AUTOMATE B = automate_creer(0);
	
	return B;
}



AUTOMATE automate_determiniser(AUTOMATE A){
	AUTOMATE B = automate_supprimer_epsilon(A);
	
	return B;
}