#ifndef WORKTREE_H
#define WORKTREE_H

#define NBWF 50

typedef struct {
    char* name;
    char* hash;
    int mode;
} WorkFile;

typedef struct {
    WorkFile* tab;
    int size;
    int n;
} WorkTree;

// On crée un WorkFile et on l'initialise
WorkFile* createWorkFile(char*name);

// Conversion d'un WorkFile en chaine de caractere avec les differents éléments séparé par une tabulation
char* wfts(WorkFile* wf);

// Conversion d'une chaine de caractère représentant un WorkFile en un WorkFile
WorkFile* stwf(char* ch);

// On crée un WorkTree qui contiendra à taille fixe NBWF ,des WorkFiles et on l'initialise
WorkTree* initWorkTree();

// Vérifie la présence d'un fichier/répertoire dans le WorkTree et renvoie la position de celle-ci sinon renvoie -1
int inWorkTree(WorkTree* wt, char* name);

// Ajoute un fichier/répertoire dans le WorkTree , renvoie 1 en case de réussite et -1 en cas d'échec
int appendWorkTree(WorkTree* wt, char* name, char* hash, int mode);

// Conversion d'un WorkTree en chaine de caractère des WorkFile avec un \n
char* wtts(WorkTree* wt);

// Conversion d'une chaine de caractere représentant un WorkTree en WorkTree
WorkTree* stwt(char* ch);

// On écrit dans le fichier file le contenu du WorkTree
int wttf(WorkTree* wt, char* file);

// Crée en WorkTree à partir d'un fichier file
WorkTree* ftwt(char* file);

// On libère les WorkFiles
void liberer_WF(WorkFile* wf);

// On libère le WorkTree
void liberer_WT(WorkTree* wt);

// EXO5

// On récupère le Chmod du fichier/répertoire
int getChmod(const char* path);

// On modifie le Chmod d'un fichier répertoire
void setMode (int mode , char* path );

// Retourne en path le hash d'un WorkTree , avec son extension ".t"
char* hashToPathWT(char* hash);

// Crée un enregistrement instantané du WorkTree et renvoie le hash du fichier temporaire
char* blobWorkTree(WorkTree* wt);

// Vérifie si on a un répertoire ou un fichier
int isFile(char* path);

// Concatene 2 path avec un / qui sépare
char* concat_path(char* path1,char* path2);

// Fait une sauvegarde/enregistrement de plusieurs fichiers à un instant donné
char* saveWorkTree(WorkTree* wt, char* path);

// Vérifie si c'est un WorkTree ou non
int isWorkTree(char* hash);

// Restaure les fichiers qui sont dans le worktree wt defini au répertoire path
void restoreWorkTree(WorkTree* wt,char* path);

#endif // WORKTREE_H