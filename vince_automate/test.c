#include "automate.h"


//Ces tests écrivent des fichiers représentant l'automate vide.
//Au fur et à mesure de l'implémentation les fichiers générés
//devront correspondre aux automates obtenus par ces opérations.
//Vous pouvez rajouter des tests pour plus de sécurité, 
//mais pour le rendu commentez-les et laissez uniquement les tests originaux.

int main(int argc, char * argv[]){
	AUTOMATE A = automate_epsilon();
	automate_ecrire(A,"test_epsilon.aut");
	AUTOMATE B = automate_une_lettre('a');
	automate_ecrire(B,"test_une_lettre.aut");
	AUTOMATE C = automate_concatenation(A,B);
	automate_ecrire(C,"test_concatenation.aut");
	AUTOMATE D = automate_une_lettre('c');
	AUTOMATE E = automate_disjonction(B,D);
	automate_ecrire(E,"test_disjonction.aut");
	AUTOMATE F = automate_etoile(E);
	automate_ecrire(F,"test_etoile.aut");
	AUTOMATE G = automate_supprimer_epsilon(F);
	automate_ecrire(G,"test_supprimer_epsilon.aut");
	AUTOMATE H = automate_determiniser(F);
	automate_ecrire(H,"test_determiniser.aut");
	automate_liberer_memoire(A);
	automate_liberer_memoire(B);
	automate_liberer_memoire(C);
	automate_liberer_memoire(D);
	automate_liberer_memoire(E);
	automate_liberer_memoire(F);
	automate_liberer_memoire(G);
	automate_liberer_memoire(H);
	return 0;
}