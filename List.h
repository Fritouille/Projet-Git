#ifndef LIST_H
#define LIST_H

typedef struct cell {
    char* data;
    struct cell* next;
} Cell ;

typedef Cell * List;


//Calcule le hash du fichier source et ecrit dans le fichier dest
int hashfile(char* source,char* dest); // /////

//Retourne le hash du fichier file 
char* sha256file(char* file);

// EXO2

//Initialise une liste vide 
List *initList();

//Alloue et retourne une cellule d'une liste
Cell* buildCell(char* ch);

//Ajoute la cellule C en tete de la liste L
void insertFirst(List *L, Cell* C);

//Conversion de la cellule C en chaine de caractere
char* ctos(Cell* c);

//Conversion d'une liste en chaine de caractere avec le format str1|str2|..
char* ltos(List* L);

//Renvoie la cellule à la position i  dans la liste L ou NULL si i est superieur à la liste L
Cell* listGet(List* L, int i);

//Recherche un element dans la liste qui correspond a la chaine str et renvoie un pointeur dessus sinon NULL
Cell* searchList(List* L,char* str);

//Convertit la chaine s et renvoie la liste ou NULL si s est vide
List* stol(char* s);

//Ecrit dans le fichier path la liste L
void ltof(List* L, char* path);

//Recupere une liste a partir du fichier path et renvoie la liste ou NULL
List* ftol(char* path);

//On libere la liste L
void liberer_List(List *L);

// EXO 3

//Retourne une liste de tous les fichiers/repertoires a l'adresse root_dir
List* listdir(char* root_dir);

//On vérifie si le fichier existe dans le répertoire courant
int file_exists(char *file);

//On copie le contenu du fichier from et on l'ecrit dans le fichier to
void cp(char *to,char *from);

//On retourne le path de la chaine hash
char* hashToPath(char* hash);

//On enregistre un instantane du fichier file
void blobFile(char* file);

#endif //LIST_H