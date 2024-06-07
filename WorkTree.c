#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "List.h"
#include "WorkTree.h"
// EXO4

// On crée un WorkFile et on l'initialise
WorkFile* createWorkFile(char* name){
    WorkFile* wfn=malloc(sizeof(WorkFile));
    if(wfn==NULL){
        printf("Erreur d'allocation dans createWorkFile\n");
        return NULL;
    }
    wfn->name=strdup(name);
    wfn->hash=NULL;
    wfn->mode=0;
    return wfn;
}

// Conversion d'un WorkFile en chaine de caractere avec les differents éléments séparé par une tabulation
char* wfts(WorkFile* wf){
    // On vérifie que le WorkFile n'est pas NULL
    if(wf!=NULL){
        char* ch = malloc(256*sizeof(char));
        sprintf(ch,"%s\t%s\t%d",wf->name,wf->hash,wf->mode);
        return ch;
    }
    else{
        printf("WF inexistant pour wfts\n");
        return NULL;
    }
}

// Conversion d'une chaine de caractère représentant un WorkFile en un WorkFile
WorkFile* stwf(char* ch){
    // On vérifie que la chaine n'est pas NULL et ch n'est pas vide
    if(ch!=NULL && strlen(ch)!=0){
        char* name=malloc(sizeof(char)*100);
        char* hash=malloc(sizeof(char)*65);
        int mode;
        // On vérifie que chaque ligne est bien écrit avec 3 champs
        if(sscanf(ch,"%s\t%s\t%d",name,hash,&mode)==3){
            WorkFile *w=createWorkFile(name);
            w->hash=strdup(hash);
            w->mode=mode;
            free(name);
            free(hash);
            return w;
        }
        // Erreur mauvaise typage
        else{
            free(name);
            free(hash);
            printf("Format de ch incorrect dans stwf\n");
            return NULL;
        }
    }
    else{
        printf("ch inexistant ou vide dans swtf\n");
        return NULL;
    }
}

// On crée un WorkTree qui contiendra à taille fixe NBWF ,des WorkFiles et on l'initialise
WorkTree* initWorkTree(){
    WorkTree* wtn=malloc(sizeof(WorkTree));
    wtn->tab=malloc(sizeof(WorkFile)*NBWF);
    wtn->size=NBWF;
    wtn->n=0;
    return wtn;
}

// Vérifie la présence d'un fichier/répertoire dans le WorkTree et renvoie la position de celle-ci sinon renvoie -1
int inWorkTree(WorkTree* wt, char* name){
    // On vérifie qu'aucun des 2 paramètres n'est NULL et le name n'est pas vide
    if(wt!=NULL && name!=NULL && strlen(name)!=0){
        for(int i=0;i<wt->n;i++){
            // On vérifie l'élément qui correspond à la chaine name donnée.
            if(strcmp(wt->tab[i].name,name)==0){
                return i;
            }
        }
    }else{
        if(wt==NULL)
            printf("WorkTree inexistant dans inWorkTree\n");
        if(strlen(name)==0){
            printf("Le nom de la chaine est vide\n");
        }
    }
    return -1;
}

// Ajoute un fichier/répertoire dans le WorkTree , renvoie 1 en case de réussite et -1 en cas d'échec
int appendWorkTree(WorkTree* wt, char* name, char* hash, int mode){
    //On vérifie la présence du fichier dans le WorkTree
    if(inWorkTree(wt,name)!=-1){
        return -1;
    }
    else{
        // On vérifie que le nombre d'éléments dans le WorkTree est inférieur à la taille max
        if(wt->n < wt->size){
            // On vérifie s'il est NULL ou s'il correspond à "(null)"
            if(hash==NULL || (strcmp(hash,"(null)")==0)){
                wt->tab[wt->n].hash=NULL;
            }
            else{
                wt->tab[wt->n].hash=strdup(hash);
            }
            wt->tab[wt->n].name=strdup(name);
            wt->tab[wt->n].mode=mode;
            wt->n++;
            return 1;
        }
        return -1;
    }
}

// Conversion d'un WorkTree en chaine de caractère des WorkFile avec un \n
char* wtts(WorkTree* wt){
    // On vérifie que le WorkTree existe 
    if(wt!=NULL){
        char* res=malloc(NBWF*1000*sizeof(char));
        res[0]='\0';
        for(int i=0;i<wt->n;i++){
            // Appel de wfts pour faire la conversion de chaque WorkFile
            char* tmp=wfts(wt->tab+i);
            strcat(res,tmp);
            strcat(res,"\n");
            free(tmp);   
        }
        return res;
    }
    else{
        printf("WorkTree inexistant dans wtts\n");
        return NULL;
    }
}

// Conversion d'une chaine de caractere représentant un WorkTree en WorkTree
WorkTree* stwt(char* ch){
    // On vérifie que la chaine n'est pas NULL et ch n'est pas vide
    if(ch!=NULL && strlen(ch)!=0){
        WorkTree* wtres=initWorkTree();
        // On lit la ligne jusqu'au \n
        char* ligne=strtok(ch,"\n");
        // Tant qu'on peut lire une ligne , on l'ajoute dans le WorkTree 
        while(ligne!=NULL){
            // on transforme la ligne en WorkFile
            WorkFile* wtmp=stwf(ligne);
            appendWorkTree(wtres,wtmp->name,wtmp->hash,wtmp->mode);
            ligne = strtok(NULL, "\n");
            liberer_WF(wtmp);
        }
        return wtres;
    }
    else{
        printf("ch inexistant ou vide dans stwt\n");
        return NULL;
    }
}

// On écrit dans le fichier file le contenu du WorkTree
int wttf(WorkTree* wt, char* file){
    FILE* f=fopen(file,"w");
    // On vérifie qu'on a bien ouvert le fichier file
    if(f!=NULL){
        char* buffer=wtts(wt);
        fputs(buffer,f);
        fclose(f);
        free(buffer);
        return 1;
    }
    else{
        printf("fichier incorrect dans wttf\n");
        return 0;
    }
}

// Crée en WorkTree à partir d'un fichier file
WorkTree* ftwt(char* file){
    FILE* f=fopen(file,"r");
    // on vérifie la bonne ouverture du fichier
    if(f!=NULL){
        char ligne[256];
        char wf[2560]="";
        // Tant qu'on peut lire le fichier on concatène son contenu
        while(fgets(ligne,256,f)!=NULL){
            strcat(wf,ligne);
        }
        fclose(f);
        return stwt(wf);
    }else{
        printf("fichier introuvable dans ftwt\n");
        return NULL;
    }
}

// On libère les WorkFiles
void liberer_WF(WorkFile* wf){
    free(wf->name);
    free(wf->hash);
    free(wf);
}

// On libère le WorkTree
void liberer_WT(WorkTree* wt){
    for(int i=0;i<wt->n;i++){
        free(wt->tab[i].name);
        free(wt->tab[i].hash);
    }
    free(wt->tab);
    free(wt);
}


// EXO5 

// On récupère le Chmod du fichier/répertoire
int getChmod(const char* path){
    struct stat ret;

    if(stat(path,&ret)==-1){
        return -1;
    }

    return (ret.st_mode & S_IRUSR) | (ret.st_mode & S_IWUSR) | (ret.st_mode & S_IXUSR) |/*owner*/
           (ret.st_mode & S_IRGRP) | (ret.st_mode & S_IWGRP) | (ret.st_mode & S_IXGRP) |/*group*/
           (ret.st_mode & S_IROTH) | (ret.st_mode & S_IWOTH) | (ret.st_mode & S_IXOTH); /*other*/
}

// On modifie le Chmod d'un fichier répertoire
void setMode (int mode , char* path ){
    char buff[100];
    sprintf(buff,"chmod %o %s",mode,path);
    system (buff);
}

// Retourne en path le hash d'un WorkTree , avec son extension ".t"
char* hashToPathWT(char* hash){
    // On vérifie que le hash existe et sa longueur different de 0
     if(strlen(hash)!=0 && hash!=NULL){
        // On transforme le hash en path 
        char *tmp_hash = hashToPath(hash);
        char* buff = malloc(sizeof(char)*(strlen(tmp_hash)+3));
        sprintf(buff,"%s.t",tmp_hash);
        free(tmp_hash);
        return buff;
    }
    printf("Hash vide dans HashToPathWT\n");
    return "";
}

// Crée un enregistrement instantané du WorkTree et renvoie le hash du fichier temporaire
char* blobWorkTree(WorkTree* wt){
    // On crée un fichier temporaire
    static char template[] = "/tmp/fichierXXXXXX";
    char fname[1000];
    strcpy(fname,template);
    mkstemp(fname);
    wttf(wt,fname);
    // On fait le hash du fichier temporaire
    char* hash=sha256file(fname);
    char *dir = strdup(hash);
    dir[2] = '\0';
    char *path=hashToPathWT(hash);
    // on vérifie si le répertoire existe déjà
    if(file_exists(dir)==0){
        char mkdir[1005];
        sprintf(mkdir,"mkdir %s",dir);
        system(mkdir);
    }
    // On copie le contenu du fichier temporaire vers le fichier final
    cp(path,fname);
    char command[1005];
    // On supprime le fichier temporaire
    sprintf(command,"rm %s",fname);
    system(command);
    free(path);
    free(dir);
    return hash;
}

// Vérifie si on a un répertoire ou un fichier
int isFile(char* path) {
    struct stat statp;
    stat(path, &statp);
    if(strlen(path)==0){
        return -1; //path vide
    }

    else{
        if(S_ISREG(statp.st_mode)){
            return 1; //Fichier
        } 
        else if(S_ISDIR(statp.st_mode)){
            return 0; //Répertoire
        } 
        else {
            return -1; //Aucun des deux
        }
    }
    
}

// Concatene 2 path avec un / qui sépare
char* concat_path(char* path1,char* path2){
    char* pwd=malloc((strlen(path1) + strlen(path2) + 2)*sizeof(char));
    if(pwd==NULL){
        printf("Erreur d'allocation\n");
        return NULL;
    }
    sprintf(pwd,"%s/%s",path1,path2);
    return pwd;
}

// Fait une sauvegarde/enregistrement de plusieurs fichiers à un instant donné
char* saveWorkTree(WorkTree* wt, char* path){
    for(int i=0;i<wt->n;i++){
        // On récupére le chemin du fichier/répertoire
        char* pwd=concat_path(path,wt->tab[i].name);
        // Si c'est un fichier
        if(isFile(pwd)==1){
            blobFile(pwd);
            free(wt->tab[i].hash);
            wt->tab[i].hash=sha256file(pwd);
            wt->tab[i].mode=getChmod(pwd);

        }else{
            if(isFile(pwd)==-1){
                WorkTree* newWT=initWorkTree();
                List* L=listdir(pwd);
                Cell* c=*L;
                while(c!=NULL){
                    appendWorkTree(newWT,c->data,NULL,0);
                    c=c->next;
                }
                liberer_List(L);
                wt->tab[i].hash=saveWorkTree(newWT,pwd);
                wt->tab[i].mode=getChmod(pwd);
                liberer_WT(newWT);
            }
            else{
                printf("Erreur ce n'est ni un fichier ni un répertoire\n");
            }
        }
        // dans le cas où on est ni l'un ni l'autre
        if(isFile(pwd)==-1){
            free(pwd);
            return NULL;
        }
        free(pwd);
    }
    return blobWorkTree(wt);
}

// Vérifie si c'est un WorkTree ou non
int isWorkTree(char* hash){
    int res = -1; // si c'est aucun des 2
    if(file_exists(hash)){ // si c'est un fichier
        res = 0;
    }
    else{
        char *tmp =hashToPathWT(hash);
        if(file_exists(tmp)){ // si c'est un répertoire
            res = 1;
        }
        free(tmp);
    }
    return res;
}

// Restaure les fichiers qui sont dans le worktree wt defini au répertoire path
void restoreWorkTree(WorkTree* wt,char* path){
    for (int i =0;i<wt->n;i++){
        char* absPath=concat_path(path,wt->tab[i].name);
        char* copyPath=hashToPath(wt->tab[i].hash);
        char* hash=hashToPath(wt->tab[i].hash);
        if(isWorkTree(hash) == 0){//si c’est un fichier
            cp(absPath,copyPath);
            setMode(getChmod(copyPath),absPath);
        }
        else{
            if(isWorkTree(hash)==1){//si c’est un repertoire
                free(copyPath);
                // on passe le hash en path d'un WT
                copyPath =hashToPathWT(hash);
                WorkTree* nwt = ftwt(copyPath);
                restoreWorkTree(nwt,absPath);
                // on change le Chmod du répertoire restauré
                setMode(getChmod(copyPath),absPath);
                liberer_WT(nwt);
            }
        }
        free(copyPath);
        free(absPath);
        free(hash);
    }
}


