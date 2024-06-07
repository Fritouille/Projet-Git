#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "List.h"
// EXO1

//Calcule le hash du fichier source et ecrit dans le fichier dest
int hashFile(char* source, char* dest){ // //
    char buffer[1000];
    sprintf(buffer,"cat %s | sha256sum > %s",source,dest);
    system(buffer);
    return 0;
}
//Retourne le hash du fichier file 
char* sha256file(char* file){
    //Creation d un fichier temporaire fname
    static char template[] = "/tmp/fichierXXXXXX";
    char fname[20];
    char* buffer = malloc(65*sizeof(char));
    strcpy (fname,template);
    int fd = mkstemp(fname);
    //Calcule le hash du fichier file
    hashFile(file,fname);
    FILE* f = fdopen(fd,"r");
    //Recuperation du hash
    fgets(buffer,65,f);
    buffer[64] = '\0';
    fclose(f);
    char rm[1005];
    //Suppression du fichier temporaire fname
    sprintf(rm,"rm %s",fname);
    system(rm);
    return buffer;
}


// EXO2

//Initialise une liste vide 
List *initList(){
    List* nl=(List *)malloc(sizeof(List));
    if(nl==NULL){
        printf("Erreur allocation mémoire de la List\n");
        return NULL;
    }
    *nl=NULL;
    return nl;
}

//Alloue et retourne une cellule d'une liste
Cell* buildCell(char* ch){
    Cell* nc=(Cell *)malloc(sizeof(Cell));
    if(nc==NULL){
        printf("Erreur allocation mémoire de la cell dans buildCell\n");
        return NULL;
    }
    nc->data=strdup(ch);
    nc->next=NULL;
    return nc;
}

//Ajoute la cellule C en tete de la liste L
void insertFirst(List *L, Cell* C){
    // on vérifie que la liste L et la cellule C ne sont pas NULL
    if(L!=NULL && C!=NULL){
        C->next=*L;
        *L=C;
    }
    else{
        printf("Erreur insertion , la liste ou la cellule est null\n");
    }
}

//Conversion de la cellule C en chaine de caractere
char* ctos(Cell* c){
    if(c==NULL){
        printf("la cellule est null dans ctos\n");
        return NULL;
    }
    else{ 
        if(c->data==NULL){
            printf("Erreur pas de donnee de la cell dans ctos\n");
            return NULL;
        }
    }
    return c->data;
}

//Conversion d'une liste en chaine de caractere avec le format str1|str2|..
char* ltos(List* L){
    //On verifie qu'on a pas une liste NULL
    if(*L==NULL){
        printf("Liste non existante dans ltos\n");
        return "";
    }
    Cell* c=*L;
    char* tmp;
    int len=0;
    char* res=malloc(sizeof(char));
    strcpy(res,"");
    //On parcours la liste L
    while(c!=NULL){
        tmp=ctos(c);
        if(tmp==NULL){
            printf("Erreur pdt ltos\n");
            return "";
        }
        //On recalcule sa longueur max 
        len+=strlen(tmp)+2;
        //On realloue son espace memoire 
        res=realloc(res,len);
        strcat(res,tmp);
        strcat(res,"|");
        c=c->next;
    }
    return res;
}

//Renvoie la cellule à la position i  dans la liste L ou NULL si i est superieur à la liste L
Cell* listGet(List* L, int i){
    if(*L==NULL){
        printf("La liste est null dans listGet\n");
        return NULL;
    }
    int nb=0;
    Cell* c=*L;
    // Tant que on est pas a l indice i
    while(c!=NULL){
        if(nb==i){
            break;
        }
        c=c->next;
        nb++;
    }

    // si on a depasse la taille de la Liste  L
    if(c==NULL){
        printf("index out of range\n");
    }
    return c;
}

//Recherche un element dans la liste qui correspond a la chaine str et renvoie un pointeur dessus sinon NULL
Cell* searchList(List* L,char* str){
    // on verifie si la liste est NULL
    if(L==NULL){
        printf("La liste est null dans searchList\n");
        return NULL;
    }
    Cell* c=*L;
    // on parcourt la liste L
    while(c!=NULL){
        // on teste si la cellule correspond a str
        if(strcmp(ctos(c),str)==0){
            return c;
        }
        c=c->next;
    }
    return NULL;
}

//Convertit la chaine s et renvoie la liste ou NULL si s est vide
List* stol(char* s){
    // verifie si s est vide ou NULL
    if(strlen(s)==0 || s==NULL){
        printf("La chaine s est vide dans stol\n");
        return NULL;
    }
    List* Lres=initList();
    // extrait le 1er token de la chaine jusqu'au délimiteur pipe
    char* ch=strtok(s,"|");
    while(ch!=NULL){
        Cell* ctmp=buildCell(ch);
        insertFirst(Lres,ctmp);
        // on prend le token suivant
        ch=strtok(NULL,"|");
    }
    return Lres;
}

//Ecrit dans le fichier path la liste L
void ltof(List* L, char* path){
    FILE* f=fopen(path,"w");
    // on verifie le fichier s'ouvre
    if(f!=NULL){
        char *s = ltos(L);
        // ecrit dans le fichier
        fprintf(f,"%s",s);
        free(s);
        fclose(f);
    }
    else{
        printf("Erreur d'ouverture du fichier dans ltof\n");
    }
}

//Recupere une liste a partir du fichier path et renvoie la liste ou NULL
List* ftol(char* path){
    FILE* f=fopen(path,"r");
    // on veritie si le fichier s'ouvre
    if(f!=NULL){
        char buffer[256];
        // on recupere la ligne dans le fichier
        fgets(buffer,256,f);
        fclose(f);
        return stol(buffer);
    }
    else{
        printf("Erreur ouverture de fichier dans ftol\n");
        return NULL;
    }
}

//On libere la liste L
void liberer_List(List* L){
    Cell* c=*L;
    Cell* tmp;
    // on parcours la liste L
    while(c!=NULL){
        tmp=c->next;
        free(c->data);
        free(c);
        c=tmp;
    }
    free(L);
}

// EXO3


//Retourne une liste de tous les fichiers/repertoires a l'adresse root_dir
List* listdir(char* root_dir){  // ALLO
    DIR *dp=opendir(root_dir);
    struct dirent *ep;
    List* L=initList();
    if(dp!=NULL){
        //On parcourt les éléments récupérer par dp
        while((ep=readdir(dp))!=NULL){
            //On enleve les repertoire . et .. de root_dir
            if((strcmp(ep->d_name,".")!=0)&&(strcmp(ep->d_name,"..")!=0)){
                Cell *c=buildCell(ep->d_name);
                insertFirst(L,c);
            }
        }
        closedir(dp);
    }
    return L;
}

//On vérifie si le fichier existe dans le répertoire courant
int file_exists(char* file){
    struct stat buffer;
    return(stat(file,&buffer)==0);
}

//On copie le contenu du fichier from et on l'ecrit dans le fichier to
void cp(char *to,char *from){
    //On verifie si le fichier to n'est pas NULL et n'est pas vide
    if(to!=NULL && strlen(to)!=0){
        //On verifie si le fichier from existe
        if(file_exists(from)==1){
            FILE* ffrom=fopen(from,"r");
            FILE* fto=fopen(to,"w");
            char buffer[256];
            //On récupere ligne par ligne du fichier from et on l'écrit dans le fichier to
            while(fgets(buffer,256,ffrom)!=NULL){
                fputs(buffer,fto);
            }
            fclose(ffrom);
            fclose(fto);
        }
        else{
            printf("Fichier from non existant dans cp\n");
        }
    }
    else{
        printf("fichier to est inexistant dans cp\n");
    }
}

//On retourne le path de la chaine hash
char* hashToPath(char* hash){
    char *res=(char *)malloc((strlen(hash)+2)*sizeof(char));

    //On copie les 2 premiers caracteres du hash
    strncpy(res,hash,2);

    //On ajoute un / entre le 2e et 3e caracteres
    res[2]='/';

    //On copie le reste de hash
    strncpy(res+3,hash+2,strlen(hash)-2);
    res[strlen(hash)+1]='\0';
    return res;
}

//On enregistre un instantane du fichier file
void blobFile(char* file){
    //On recupere le hash du fichier
    char* hash=sha256file(file);

    //On convertit en path
    char* path=hashToPath(hash);
    char* rep=malloc(3*sizeof(char));
    
    strncpy(rep,hash,2);
    rep[2]='\0';

    //On verifie si le repetoire rep existe
    if(file_exists(rep)!=1){
        char cmd[10];
        //On creer le repertoire
        sprintf(cmd,"mkdir %s",rep);
        system(cmd);
    }

    //On copie le fichier file dans le fichier path
    cp(path,file);
    free(rep);
    free(hash);
    free(path);
}