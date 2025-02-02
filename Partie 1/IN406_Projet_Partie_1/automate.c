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
	AUTOMATE A = automate_creer(1);
	
	automate_ajouter_final (A, 0);
	
	return A;
}

AUTOMATE automate_une_lettre(char car){
	AUTOMATE A = automate_creer(2);
	
	automate_ajouter_final (A, 1);
	automate_ajouter_transition (A, 0, car, 1);
	
	return A;
}


AUTOMATE automate_concatenation(AUTOMATE A, AUTOMATE B){
	AUTOMATE C = automate_creer(A.Q + B.Q);
	
	//Les états finaux de C sont les même que ceux de B
	for( int i=0 ; i < B.Q ; i++ )
	{
		if( B.F[i] == 1 )
			automate_ajouter_final(C, i+A.Q);
	} 
	
	//Recréer les transitions de l'automate A dans l'automate C
	TRANSITION T;
	for( int i=0 ; i<A.Q ; i++ )
	{
		T = A.T[i];
		if(T != NULL)
			automate_ajouter_transition(C, i, T->car, T->arr);
	}
	
	//Recréer les transitions de l'automate B dans l'automate C
	for( int i = A.Q ; i < A.Q + B.Q ; i++ )
	{
		T = B.T[i - A.Q];
		if(T != NULL)
			automate_ajouter_transition(C, i, T->car, T->arr+A.Q);
	}
	
	//Ajout des e-transitions des etats finaux de A à l'état initial de B 
	for( int i=0 ; i < A.Q ; i++ )
	{
		if( A.F[i] == 1)
			automate_ajouter_transition(C, i, -1, A.Q);
	}
	
	return C;
}

AUTOMATE automate_disjonction(AUTOMATE A, AUTOMATE B){
	AUTOMATE C = automate_creer(A.Q + B.Q + 1);

	//Les états finaux de C sont les même que ceux de A
	for( int i=0 ; i < A.Q ; i++ )
	{
		if( A.F[i] == 1 )
			automate_ajouter_final(C, i + 1);
	} 

	//Les états finaux de C sont les même que ceux de B
	for( int i=0 ; i < B.Q ; i++ )
	{
		if( B.F[i] == 1 )
			automate_ajouter_final(C, i + A.Q + 1);
	} 
	
	//Recréer les transitions de l'automate A dans l'automate C
	TRANSITION T;
	for( int i=0 ; i<A.Q ; i++ )
	{
		T = A.T[i];
		if(T != NULL)
			automate_ajouter_transition(C, i + 1, T->car, T->arr + 1);
	}
	
	//Recréer les transitions de l'automate B dans l'automate C
	for( int i=A.Q ; i < A.Q + B.Q ; i++ )
	{
		T = B.T[i - A.Q];
		if(T != NULL)
			automate_ajouter_transition(C, i + 1, T->car, T->arr + A.Q + 1);
	}
	
	//Ajout des e-transitions des etats initial de C vers les états initiales de A et B
	automate_ajouter_transition(C, 0, -1, 1);
	automate_ajouter_transition(C, 0, -1, (C.Q - B.Q));

	
	return C;
}

AUTOMATE automate_etoile(AUTOMATE A){
	AUTOMATE B = automate_copier(A, 1);
	
	automate_ajouter_final(B, 0);
	
	//Ajout des e-transitions des etats finaux de A à l'état initial de B 
	for( int i=0 ; i < B.Q ; i++ )
	{
		if( B.F[i] == 1)
			automate_ajouter_transition(B, i, -1, 0);
	}
	
	return B;
}

static float puissance(float x, int n)
{
	return (n == 0) ? 1 : (x*puissance(x,n-1));
}

AUTOMATE automate_determiniser(AUTOMATE A){
	AUTOMATE B = automate_supprimer_epsilon(A);
	AUTOMATE C = automate_creer(puissance(2.0,B.Q)); //Creation d'un automate avec le nombre max d'etat possible
	
	char *memCar;
	int  *memEtat; 
	int nbreTransi = 0;
	
	
  	for(int i = 0; i < B.Q; i++)
  	{
  		TRANSITION l = B.T[i];
  		
  		//On regarde combien il y a de transition pour un même sommet
  		while(l)
  		{
  			nbreTransi++;
  			l = l-> suiv;
  		}
  		
  		int x 	 = 0;
  		int same = 0;
  		TRANSITION T = B.T[i];
  		//on creer un tab avec l'etat d'arriver et la lettre
  		memEtat = malloc (sizeof (int)  * nbreTransi) ;
  		memCar  = malloc (sizeof (char) * nbreTransi) ;
  		
  		//On remplis les tableaux
  		while(l)
  		{
			memEtat[x] = T->arr;
			memCar [x] = T->car;
			x++;
		}
  		
  		//On regarde si pour un meme sommet il n'y a pas plusieur fois la meme lettre
  		for(x = 0; x<nbreTransi; x++)
  		{
			for(int y = 0; y<nbreTransi; y++)
			{
				if(memCar[x] == memCar[y] && x != y)
					same = 1;
			}
			
			if(same == 0)
				automate_ajouter_transition(C, i, memCar[x], memEtat[x]);
				
			same = 0;
		}
  		
  		free(memEtat);
  		free(memCar);
  	}
	
	
	return C;
}

/////////////
/////////////

//Cette fonction permet de connaitre le nombre unique de lettre de l'automate
//Retourne un tableau avec ses lettres
char *nombreLettreUnique(AUTOMATE A, int *taille)
{
	AUTOMATE B = automate_supprimer_epsilon(A);
	TRANSITION T;
	
	int	nbreTransi = 0;
	int x = 0;
	
	for( int i = 0 ; i < B.Q ; i++ )
	{
		T = B.T[i];
		
		while(T)
  		{
  			nbreTransi++;
  			T = T-> suiv;
  		}
	}
	
	//Allocation d'un tableau contenant toute les lettres de chaque transitions
	char *tabLettre = malloc(nbreTransi * sizeof(char));
	
	for( int i=0 ; i < B.Q ; i++ )
	{
		T = B.T[i];
 		
		while(T)
  		{
			tabLettre[0] = T -> car;
			T = T -> suiv;
			x++;
  		}
	}
	
	//Allocation du tableau avec les lettres uniques de l'automate
    char *tabLettreUnique = malloc( nbreTransi * sizeof(char) );
    
    if(tabLettreUnique == NULL)
		printf("Error allocation malloc : tabLettreUnique\n");
    
	int j = 0, i = 0, count = 0;
	while( i < nbreTransi ) 
	{
	  while( tabLettre[j] == tabLettre[i] )
		  i++;
		  
	  tabLettreUnique[count] = tabLettre[j];
	  j=i;
	  count++;
	}
	
	tabLettreUnique = (char *)realloc(tabLettreUnique, count * sizeof(char) );
	
	if(tabLettreUnique == NULL)
		printf("Error allocation remalloc : tabLettreUnique\n");
	
	free(tabLettre);
	
	*taille = count;
	
	return tabLettreUnique;
}

AUTOMATE automate_minimisation(AUTOMATE A)
{
	AUTOMATE B = automate_supprimer_epsilon(A);

	int nbreLettre;
	char *tabLettre = nombreLettreUnique(B, &nbreLettre);
	
	//Allocation de la matrice de verification
	int **matrice = calloc( (nbreLettre + 2), sizeof(int*) );
	
	for(int i = 0; i < (nbreLettre + 2); i++)
		matrice[i] = calloc( (B.Q + 1), sizeof(int*) );

	//Init la classe de depart de la matrice
	for(int i=0; i < (B.Q + 1); i++)
	{
		if( B.F[i] == 1)
			matrice[0][i] = 2;
		
		else
			matrice[0][i] = 1;
	}
	
	//Poubelle
	for(int i = 0; i < (nbreLettre + 2); i++)
		matrice[i][B.Q + 1] = 1;
	
	//Init matrice classe + transition lettre
	for(int i = 0; i < (B.Q); i++)
  	{
  		TRANSITION l = B.T[i];
  		while(l)
  		{
			for(int cmpt=0; cmpt < nbreLettre && l != NULL; cmpt++)
			{	
				if(tabLettre[cmpt] == l -> car)
				{
					matrice[cmpt+1][cmpt] = matrice[0][l -> arr];
					printf("cmpt 1 : %d\n",cmpt);
				}
			}
			
			l = l-> suiv;
  		}
	}
	
	
	int nbP = 1; //Variable qui permet de savoir combiende fois on va boucler 
	
	while(nbP<3)
	{
		//Si pas de transition on met la val de la poubelle
		for(int i = 0; i < (nbreLettre + 1)*nbP; i++)
		{
			for(int j = 0; j < (B.Q + 1); j++)
			{
				if(matrice[i][j] == 0)
					matrice[i][j] = matrice[((nbreLettre + 1)*nbP) - (nbreLettre + 1)][A.Q];
			}		
		}	
		
		//Allocation d'un tableau qui addition chaques lignes d'une même colonnes 
		//C'est à dire classe + nombre de lettre
		int *resultatAd = malloc( (2*(B.Q + 1)) * sizeof(int)); // on rajoute *2pour connaitre le stkocker le postion de la colonne 
		int resultat = 0;
		
		//Addition des colonnes pour savoir si les nombres sont similaires ou non 
		if(resultatAd == NULL)
			printf("Error allocation malloc : resultat\n");
		
		int val = 0;

		for(int colonne = 0; colonne < (B.Q + 1); colonne++)
		{	
			for(int ligne = ((nbreLettre + 1)*nbP) - (nbreLettre + 1); ligne < ((nbreLettre + 1) * nbP) + 1; ligne++)
			{
				resultat += matrice[ligne][colonne];
			}
			
			resultatAd[val] = resultat; //Stocker aussi les postions lignes colonnes? //changer la variable par colonnes par une autres 
			val++;
			resultatAd[val] = colonne;
			val++;
			resultat = 0;
		}
		
		//Affiche la contenue de resultatAd
		printf("contenu resultatAd : ");
		for(int i = 0; i < (2*(B.Q + 1)); i++)
		{
			printf("%d, ",resultatAd[i]);
		}
		printf("\n");
		
		//Test des resultats egaux par additions si egaux test egaux avec multiplications
		for(int i = 0; i < ((B.Q + 1) * 2); i += 2)
		{	
			for(int j = 0; j < ((B.Q + 1) * 2); j += 2)
			{
				if(resultatAd[i] == resultatAd[j] && i != j)
				{
					int resultatMult1 = 1;
					int resultatMult2 = 1;
					printf("resultatAd[i] : %d\t resultatAd[j] : %d\n",resultatAd[i],resultatAd[j]);
					for(int ligne = ((nbreLettre + 1)*nbP) - (nbreLettre + 1); ligne < ((nbreLettre + 1) * nbP) + 1; ligne++)
					{
							resultatMult1 *= matrice[ligne][resultatAd[i + 1]];
							resultatMult2 *= matrice[ligne][resultatAd[j + 1]];
					}
					
					//Si egale ils ont le même resultat
					if(resultatMult1 == resultatMult2)
					{
						matrice[(nbreLettre + 1)*nbP][resultatAd[i + 1]] = resultatAd[i + 1] + 1;
						printf("matrice i meme resultat : %d\n", resultatAd[i + 1] + 1);
						
						matrice[(nbreLettre + 1)*nbP][resultatAd[j + 1]] = resultatAd[i + 1] + 1;
						printf("matrice j meme resultat : %d\n\n", resultatAd[i + 1] + 1);
					}
					
					//Sinon ils prennent le numéros de leur colonne
					else
					{
						matrice[(nbreLettre + 1)*nbP][resultatAd[i + 1]] = resultatAd[i + 1] + 1;
						printf("matrice i resultat diff : %d\n", resultatAd[i + 1] + 1);
						
						matrice[(nbreLettre + 1)*nbP][resultatAd[j + 1]] = resultatAd[j + 1] + 1;
						printf("matrice j resultat diff : %d\n\n", resultatAd[j + 1] + 1);
					}
					
					resultatMult1 = 0;
					resultatMult2 = 0;
				}
				
				else if(resultatAd[i] != resultatAd[j] && i != j)
				{
					//~ matrice[(nbreLettre + 1)*nbP][resultatAd[i + 1]] = resultatAd[i + 1] + 1;
					printf("matrice i PAS egale : %d\n", resultatAd[i + 1] + 1);
				}	
			}
		}
		
		free(resultatAd);
		
		//Afficher la matrice
		printf("\n");
		for(int i = 0; i < ((nbreLettre + 1) * nbP) + 1; i++)
		{
			for(int j = 0; j < B.Q + 1; j++)
			{
				printf("%d ",matrice[i][j]); 
			}
			printf("\n");
		}	
		
		//Verifiaction de si l'automate n'est pas deja minimisé
		int minimise = 1;
		for(int i = 0; i < (B.Q + 1); i++)
		{
			for(int j = 0; j < (B.Q + 1); j++)
			{
				if(matrice[(nbreLettre + 1)*nbP][i] == matrice[(nbreLettre + 1)*nbP][j] && i != j)
				{
					minimise = 0;
					break;
				}
			}
			
			if(minimise == 0)
				break;
		}
		
		if(minimise == 1)
		{
			printf("l'automate est deja minimal\n");
			//return B;
		}
		
		//Comparer en partant de la derniere classe les autres classes
		int cmpt = 0;
		for(int ligne = (nbreLettre + 1)*nbP; ligne > 0; ligne -= (nbreLettre + 1))
		{
			for(int colonne = 0; colonne < B.Q + 1; colonne++)
			{
				if(matrice[(nbreLettre + 1)][colonne] == matrice[ligne][colonne])
					cmpt++;
			}
			
			//Si on a une classe egale à une autre on minimise 
			if(cmpt == B.Q + 1)
			{
				//Creation de l'automate minimisé
				
				//return B;
			}
			
			cmpt=0;
		}
		
		printf("ALED\n");
		//On realloc la matrice pour l'agrandir tant qu'une classe n'est pas egale à une autre	
		nbP++;
			
		matrice = (int**)realloc(matrice, (((nbreLettre + 1) * nbP) + 1)* sizeof(int*) );
		
		for(int i = 0; i < (((nbreLettre + 1) * nbP) + 1); i++)
			matrice[i] = (int*)realloc(matrice[i], (B.Q + 1) * sizeof(int*) );
		
		if(matrice == NULL)
			printf("Error allocation malloc : matrice\n");
		
		printf("ALED2\n");
		
		//On remplit les cases vide de la matrice avec des zero
		for(int ligne = ((nbreLettre + 1) * (nbP)) - 1; ligne < ((nbreLettre + 1) * nbP) + 1; ligne++)
		{
			for(int colonne = 0; colonne < (B.Q + 1); colonne++)
			{
				matrice[ligne][colonne] = 0;
			}
		}
		
		//On remplit avec les transitions precedante
		printf("nbreLettre : %d\n", nbreLettre);
		int nbreColonne = 0;
		for(int i = 0; i < (B.Q); i++)
		{
			TRANSITION l = B.T[i];
			while(l)
			{
				for(int cmpt = 0; cmpt < nbreLettre && l != NULL; cmpt++)
				{
					printf("cmpt : %d\n", nbreColonne);
					if(tabLettre[cmpt] == l -> car)
					{
						matrice[((nbreLettre + 1) * (nbP-1)) + cmpt+1][nbreColonne] = matrice[((nbreLettre + 1) * (nbP-1))][l -> arr];
					}
					
					nbreColonne++;
				}
				
				l = l-> suiv;
			}
		}
		
	}
	//Liberation de la memoire
	//~ for( int i = 0 ; i < (nbreLettre + 2) ; i++)
		//~ free(matrice[i]);
		
	//~ free(matrice);

	return B;
}












