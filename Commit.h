#ifndef COMMIT_H
#define COMMIT_H
#define NBC 100

typedef struct key_value_pair {
	char* key;
	char* value;
}kvp;

typedef struct hash_table {
	kvp** T;
	int n;
	int size;
}HashTable;

typedef HashTable Commit;

// Alloue une structure Cle-Val et initialise un élément
kvp* createKeyVal(char* key, char* val);

// Libere la structure Cle-Val
void freeKeyVal(kvp* kv);

// Conversion d'un élément en une chainde de type "cle:val"
char* kvts(kvp* k);

//Conversion d'une chaine en un élément Cle-Val
kvp* stkv(char* str);

//Alloue et initialise un Commit de taille fixe NBC
Commit* initCommit();

//Fonction de hachage lose lose
unsigned long hash(unsigned char *str);

//Insere une cle-val dans le Commit
void commitSet(Commit* c,char* key,char* value);

//Crée,initialise le Commit et on ajoute le hash correspondant à la clé "tree"
Commit* createCommit(char* hash);

//Cherche dans le Commit un élément qui a la clé key et retourne la valeur de l'élément
char* commitGet(Commit* c,char* key);

//Conversion d'un commit en chaine de caractère
char* cts(Commit* c);

//Conversion d'une chaine de caractere en Commit
Commit* stc(char* ch);

//Ecrit dans le fichier file le Commit c
void ctf(Commit* c,char* file);

//Charge un Commit à partir d'un fichier
Commit* ftc(char* file);

//Retourne le Path d'un commit à partir d'un hash
char* hashToPathCommit(char* hash);

//Crée un enregistrement instantané du commit et retourne le hash du fichier 
char* blobCommit(Commit* c);

// Libere le Commit
void liberer_Commit(Commit* c);

// EXO7

// Crée le répertoire .refs et les fichiers master et HEAD
void initRefs();

// On met à jour une référence en remplacant son contenu par hash
void createUpdateRef(char* ref_name,char* hash);

// supprime une référence
void deleteRef(char* ref_name);

// récupere le contenu hash du pointeur référence
char* getRef(char* ref_name);

// permet à l'utilisateur d'ajouter un fichier/répertoire dans le WorkTree de la zone de préparation
void myGitAdd(char* file_or_folder);

// permet à l'utilisateur de commit/enregistrer les fichiers de la zone de préparation
void myGitCommit(char* branch_name, char* message);

// EXO8

// Crée le fichier .current_branch contenant la chaine "master" dedans
void initBranch();

// Vérifie l'existence d'une branche
int branchExists(char* branch);

//crée une branche et la pointe vers le même commit que la référence HEAD
void createBranch(char* branch);

// Lit le fichier .current_branch et retourne le nom de la branche
char* getCurrentBranch();

// affiche tout le contenu de la branche et pour chaque commit , affiche son hash et son message
void printBranch(char* branch);

// retourne une liste chaine represantant tous les commits de la branche
List* branchList(char* branch);

// renvoie une liste des hash de chaque commit des branches
List* getAllCommits();

// EXO9
 
// restaure le WorkTree associé à un Commit dont le hash est donné
void restoreCommit(char *hash_commit);

// Modifie le current branch par le parametre "branch", modifie le HEAD et restaure le WorkTree associé au commit de branch
void myGitCheckoutBranch(char* branch);

// Retourne une liste contenant les éléments de L qui commencent par la chaine pattern
List* filterList(List* L, char* pattern);

// Calcule la longueur d'une liste L
int ListSize(List *L);

// Permet de retourner sur une version d'un commit choisi par l'utilisateur à l'aide de son hash correspondant/pattern correspondant
void myGitCheckoutCommit(char* pattern);

// EXO11

// Crée un WorkTree contenant la fusion des 2 WorkTree donné en paramètre et une liste contenant les fichiers/répertoire similaires
WorkTree* mergeWorkTrees(WorkTree* wt1, WorkTree* wt2, List** conflicts);

// passe d'un hash d'une branche au path d'un WorkTree 
WorkTree* branch_To_WT(char* branch);

// fusionne deux commits , supprime la branche secondaire et met a jour la version du projet.
List* merge(char* remote_branch, char* message);

// crée un commit de suppression ders conflits
void createDeletionCommit(char* branch, List* conflicts, char* message);

#endif // COMMIT_H