#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "List.h"
#include "WorkTree.h"
#include "Commit.h"

//EXO 6

// Alloue une structure Cle-Val et initialise un élément
kvp* createKeyVal(char* key, char* val){
	kvp* nkv=malloc(sizeof(kvp));
	if(nkv==NULL){
		printf("Erreur allocation nkv\n");
		return NULL;
	}
	// On vérifie que les 2 paramètres ne sont pas NULL et la clef n'est pas vide
	if(key!=NULL && val!=NULL && strlen(key)!=0){
		nkv->key=strdup(key);
		nkv->value=strdup(val);
	}
	else{
		return NULL;
	}
	return nkv;
}

// Libere la structure Cle-Val
void freeKeyVal(kvp* kv){
	free(kv->key);
	free(kv->value);
	free(kv);
}

// Conversion d'un élément en une chainde de type "cle:val"
char* kvts(kvp* k){
	char* res=malloc(256*sizeof(char));
	sprintf(res,"%s : %s",k->key,k->value);
	return res;
}

//Conversion d'une chaine en un élément Cle-Val
kvp* stkv(char* str){
	//On verifie si str n'est pas vide et n'est pas NULL
	if(str==NULL && strlen(str)!=0){
		printf("Erreur chaine donné vide ou NULL dans stkv\n");
		return NULL;
	}
	char k[256];
	char v[256];
	sscanf(str,"%s : %s",k,v);
	return createKeyVal(k,v);
}
 
//Alloue et initialise un Commit de taille fixe NBC
Commit* initCommit(){
	Commit* nc=malloc(sizeof(Commit));
	if(nc==NULL){
		printf("Erreur d'allocation du Commit\n");
		return NULL;
	}
	nc->size=NBC;
	nc->n=0;
	nc->T=malloc(NBC*sizeof(kvp*));
	for(int i=0;i<nc->size;i++){
		nc->T[i]=NULL;
	}
	return nc;
}

//Fonction de hachage lose lose
unsigned long hash(unsigned char *str){ 
	unsigned int hash =0;
	int c;
	while((c = *str++))
	    hash += c;
	return hash;
}

//Insere une cle-val dans le Commit
void commitSet(Commit* c,char* key,char* value){
	if(key==NULL || strlen(key)==0){
		return;
	}
	// On hash la clé
	int ind=hash((unsigned char*)key)%c->size;
	// Tant qu'on trouve aucun espace mémoire non NULL
	while(c->T[ind]!=NULL){
		ind=(ind+1)%c->size;//probing lineaire
	}
	c->T[ind]=createKeyVal(key,value);
	c->n++;
}

//Crée,initialise le Commit et on ajoute le hash correspondant à la clé "tree"
Commit* createCommit(char* hash){
	Commit* newc=initCommit();
	commitSet(newc,"tree",hash);
	return newc;
}

//Cherche dans le Commit un élément qui a la clé key et retourne la valeur de l'élément
char* commitGet(Commit* c,char* key){
	// si au moins l'un des 2 paramètres est NULL ou la clef est vide
	if(key==NULL || c==NULL || strlen(key)==0 ){
		printf("Erreur key NULL ou vide ou Commit NULL dans commitGet\n");
		return NULL;
	}
	int ind=hash((unsigned char*)key)%c->size;
	int attempt=0;
	// On cherche l'élément
	while (c->T[ind]!=NULL && attempt<c->size){
		// on compare si la clé correspond
		if(strcmp(c->T[ind]->key,key)==0){
			return c->T[ind]->value;
		}
		ind=(ind +1)%c->size;
		attempt++;
	}
	return NULL;
}

//Conversion d'un commit en chaine de caractère
char* cts(Commit* c){
	char* chaine=malloc(sizeof(char)*100*c->n);
	char* temporaire = "";
	// strcpy pour initialiser a vide
	strcpy(chaine,"");
	for(int i=0;i<c->size;i++){
	 	if(c->T[i]!=NULL){
			// on recupere la chaine representant la paire cle-val
			char *tmp = kvts(c->T[i]);
			// on vérifie qu'on l'a pas déjà trouvé une fois
			if(strcmp(tmp,temporaire)==0){
				free(tmp);
				continue;
			}
			else{
				// si la chaine est celle du vide de départ
				if(strcmp(temporaire,"")!=0){
					free(temporaire);
				}
				strcat(chaine,tmp);
				free(tmp);
				strcat(chaine,"\n");
				// on récupére temporairement le contenu trouvé pour comparer plus tard
				temporaire = kvts(c->T[i]);
			}
		}
	}
	free(temporaire);
	return chaine;
}

//Conversion d'une chaine de caractere en Commit
Commit* stc(char* ch){
	Commit* nc=initCommit();
	size_t pos=0;
	int posl=0;
	char* key=malloc(sizeof(char)*100);
	char* value=malloc(sizeof(char)*100);
	char nkvp[205] = "";
	while(pos<(size_t)strlen(ch)){
		// si le caractère à l'indice pos correspond à un saut de ligne
		if(ch[pos]=='\n'){
			// on coupe la chaine à l'indice posl pour éviter de copier le contenu de la ligne précédente
			nkvp[posl] = '\0';
			sscanf(nkvp,"%s : %s",key,value);
			commitSet(nc,key,value);
			pos++;
			posl=0; // on recommence la copie depuis le début
		}
		// On copie caractère par caractère le contenu
		else{
			nkvp[posl]=ch[pos];
			posl++;
			pos++;
		}
	}
	free(key);
	free(value);
	return nc;

}

//Ecrit dans le fichier file le Commit c
void ctf(Commit* c, char* file){
	FILE *f =fopen(file,"w");
	if(f!=NULL){
		char* ecrit=cts(c);
		fputs(ecrit,f);
		free(ecrit);
		fclose(f);
	}
}

//Charge un Commit à partir d'un fichier
Commit* ftc(char* file){
	FILE *f =fopen(file,"r");
	if(f==NULL){
		printf("ERREUR : impossible a lire le fichier ou n'existe pas dans ftc \n");
		return NULL;
	}
    char* lecture=malloc(sizeof(char)*2000);
	// strcpy pour éviter les erreurs de chaine
	strcpy(lecture,"");
	char buffer[256];
	//Tant qu'on lit une ligne
	while(fgets(buffer,256,f)){
		strcat(lecture,buffer);
		strcat(lecture,"\n");
	}
	// Creation du Commit
	Commit* nc=stc(lecture);
	fclose(f);
	free(lecture);
	return nc;
}

//Retourne le Path d'un commit à partir d'un hash
char* hashToPathCommit(char* hash){
	// on verifie que ce n'est pas vide et que ce n'est pas NULL
    if(strlen(hash)!=0 && hash!=NULL){
        char *tmp_hash = hashToPath(hash);
        char* buff = malloc(sizeof(char)*(strlen(tmp_hash)+3));
        sprintf(buff,"%s.c",tmp_hash);
        free(tmp_hash);
        return buff;
    }
    return "";
}

//Crée un enregistrement instantané du commit et retourne le hash du fichier 
char* blobCommit(Commit* c){
	// Creation d'un fichier temporaire
    static char template[] = "/tmp/fichierXXXXXX";
    char fname[100];
    strcpy (fname,template);
	mkstemp(fname);
	// Copie le contenu de C vers fname
	ctf(c,fname);
    char* hash=sha256file(fname);
    char* path=hashToPathCommit(hash);
	char* rep=strdup(hash);
    rep[2]='\0';
    if(file_exists(rep)!=1){ // si le répertoire n'existe pas
        char cmd[100];
        sprintf(cmd,"mkdir %s",rep);
        system(cmd);
    }
	// on copie dans le fichier final
	cp(path,fname);
    char rm[1005];
	// on supprime le fichier temporaire
    sprintf(rm,"rm %s",fname);
    system(rm);
	free(path);
	free(rep);
	return hash;
}

// Libere le Commit
void liberer_Commit(Commit* c){
	if(c!=NULL){
		if(c->T!=NULL){
			for(int i=0;i<c->size;i++){
				if(c->T[i]!=NULL){
					freeKeyVal(c->T[i]);
				}	
			}
			free(c->T);
		}
		free(c);
	}
}


// EXO 7


// Crée le répertoire .refs et les fichiers master et HEAD
void initRefs(){
	// on vérifie si c'est déjà crée
	if(file_exists(".refs")==1){
		printf("le repertoire .refs existe dans initRefs\n");
	}else{
		system("mkdir .refs");
		system("touch .refs/master");
		system("touch .refs/HEAD");
	}
}

// On met à jour une référence en remplacant son contenu par hash
void createUpdateRef(char* ref_name,char* hash){
	if(ref_name==NULL || strlen(ref_name)==0){
		printf("Erreur sur votre parametre ref_name par Update\n");
		return;
	}
	if(hash==NULL){ // on vérifie que NULL dans le cas du premier Commit
		printf("Erreur sur votre paramètre hash par Update\n");
		return;
	}
	char ref[256];
	sprintf(ref,".refs/%s",ref_name);
	if(file_exists(ref)==0){ // si le fichier n'existe pas on le crée
		char cmd[256];
		sprintf(cmd,"touch .refs/%s",ref_name);
		system(cmd);
	}
	char tmp[256];
	// écrit le hash dans le fichier ref_name
	sprintf(tmp,"echo %s > .refs/%s",hash,ref_name);
	system(tmp);
}

// supprime une référence
void deleteRef(char* ref_name){
	if(ref_name==NULL || strlen(ref_name)==0){
		printf("Erreur sur votre parametre ref_name par delete\n");
		return;
	}
 	char tmp[1005];
	sprintf(tmp,".refs/%s",ref_name);
	if(file_exists(tmp)==1){ // si la référence existe on supprime
		sprintf(tmp,"rm .refs/%s",ref_name);
		system(tmp);
	}
	else{
		printf("La reference n'existe pas dans deleteRef\n");
	}
}

// récupere le contenu hash du pointeur référence
char* getRef(char* ref_name){
	if(ref_name==NULL || strlen(ref_name)==0){
		printf("Erreur sur votre parametre ref_name\n");
		return NULL;
	}
	char buffer_ref[256];
	sprintf(buffer_ref,".refs/%s",ref_name);
 	if(file_exists(buffer_ref)==0){ // si le fichier n'existe pas
 		printf("le fichier n'existe pas dans getRef\n");
 		return NULL;
 	}
 	FILE *f=fopen(buffer_ref,"r");
	if(f!=NULL){
 		char* buff=malloc(sizeof(char)*256);
		strcpy(buff,"");
		fgets(buff,256,f);
		if(strlen(buff)!=0){	// en cas de contenu non vide on enleve le \n
			buff[strlen(buff)-1]='\0';	
		}
		fclose(f);
		return buff;
	}
	else{
		printf("Le fichier ne s'ouvre pas dans getRef\n");
		return NULL;
	}
}

// permet à l'utilisateur d'ajouter un fichier/répertoire dans le WorkTree de la zone de préparation
void myGitAdd(char* file_or_folder){
	if(file_or_folder==NULL || strlen(file_or_folder)==0){
		printf("Le fichier ou le repertoire est inexistant dans myGitAdd\n");
	}
	else{
		WorkTree* wt;
		if(file_exists(".add")==0){ // vérifie si on a le fichier .add
			system("touch .add");
			wt=initWorkTree();
		}
		else{
			wt=ftwt(".add"); // lit le fichier .add et fait son WT
		}

		if(file_exists(file_or_folder)==1){ // si le fichier/répertoire existe
			appendWorkTree(wt,file_or_folder,NULL,0);
			wttf(wt,".add");
		}else{
			printf("Fichier ou repertoire n'existe pas dans myGitAdd\n") ;
		}
		liberer_WT(wt);
	}
}

// permet à l'utilisateur de commit/enregistrer les fichiers de la zone de préparation
void myGitCommit(char* branch_name, char* message){
	if(branch_name==NULL || strlen(branch_name)==0){
		printf("Erreur sur le paramètre branch_name par gitcommit\n");
		return;
	}
	if(file_exists(".refs")==0){ // vérifie l'existance du répertoire .refs
		printf("Initialiser d’abord les references du projet\n");
		return;
	}

	char branchref[256];
	sprintf(branchref,".refs/%s",branch_name);

	if(file_exists(branchref)==0){ // vérifie l'existance de la branche
		printf("La branche n’existe pas\n");
		return;
	}
	// on récupére leurs hash correspondant
	char* hash_branch=getRef(branch_name);
	char* hash_HEAD=getRef("HEAD");
	// Vérifie si les hash correspondent bien
	if(strcmp(hash_branch,hash_HEAD)!=0){
		printf("HEAD doit pointer sur le dernier commit de la branche\n");
		free(hash_branch);
		free(hash_HEAD);
		return;
	}
	// Création du WorkFree à partir du fichier .add
	WorkTree* wt=ftwt(".add");
	char* hash_wt=saveWorkTree(wt,".");
	// Création du Commit
	Commit* c=createCommit(hash_wt);
	if(strlen(hash_branch)!=0){ // si on a récupéré un hash 
		commitSet(c,"predecessor",hash_branch);
	}
	if(message!=NULL && strlen(message)!=0){ // si on a un message
		commitSet(c,"message",message);
	}

	char* hash_c=blobCommit(c);
	// on met à jour les hash dans chaque branches
	createUpdateRef(branch_name,hash_c);
	createUpdateRef("HEAD",hash_c);
	system("rm .add");
	
	liberer_WT(wt);
	liberer_Commit(c);
	free(hash_branch);
	free(hash_HEAD);
	free(hash_c);
	free(hash_wt);
}


// EXO8

// Crée le fichier .current_branch contenant la chaine "master" dedans
void initBranch(){
    FILE* f = fopen(".current_branch","w");
    fputs("master",f);
    fclose(f);
}

// Vérifie l'existence d'une branche
int branchExists(char* branch){
    if(branch==NULL || strlen(branch)==0){
        printf("Erreur sur le paramètre branch par branchExists\n");
        return -1;
    }
    List* refs=listdir(".refs");
    int i = 0;
    // on cherche la branche parmi les fichiers dans .refs
    if(searchList(refs,branch)!=NULL){
        i = 1;
    }
    liberer_List(refs);
    return i;
}

//crée une branche et la pointe vers le même commit que la référence HEAD
void createBranch(char* branch){
    if(branch==NULL || strlen(branch)==0){
        printf("Erreur sur le paramètre branch par CreateBranch\n");
        return;
    }
    char *hash = getRef("HEAD");
    createUpdateRef(branch,hash);
    free(hash);
}

// Lit le fichier .current_branch et retourne le nom de la branche
char* getCurrentBranch(){
    FILE* f=fopen(".current_branch","r");
    char* buff=malloc(sizeof(char)*256);
    if(f!=NULL){
        fscanf(f,"%s",buff);
        fclose(f);
        return buff;
    }
    else{
        printf("Le fichier .current_branch n'existe pas dans getCurrentBranch\n");
        free(buff);
        return NULL;
    }
}

// affiche tout le contenu de la branche et pour chaque commit , affiche son hash et son message
void printBranch(char* branch){
    if(branch==NULL || strlen(branch)==0){
        printf("Erreur sur le paramètre branch par printB\n");
        return;
    }
    char *commit_hash = getRef(branch);
    char *hash_to_path = "";
    if(strlen(commit_hash)!=0){ // si on essaye de print une branche avant d'avoir commit au moins 1 fois
        hash_to_path = hashToPathCommit(commit_hash);
    }
    Commit* c = ftc(hash_to_path);
    // Tant qu'on a un commit 
    while(c!=NULL){
        char *CM = commitGet(c,"message");
        if(CM!=NULL){ // si on a un de message
            printf("%s -> %s \n",commit_hash,CM);
        }
        else{
            printf("%s \n",commit_hash);
        }
        char *CP = commitGet(c,"predecessor");
        if(CP!=NULL){ // si on a pas de predecesseur
            free(commit_hash);
            commit_hash=strdup(CP);
            free(hash_to_path);
            hash_to_path=hashToPathCommit(commit_hash);
            // on libere le commit avant de lui donner le commit predecesseur
            liberer_Commit(c);
            c=ftc(hash_to_path);
        }
        else{
            liberer_Commit(c);
            c = NULL;
        }
    }
    free(commit_hash);
    if(strcmp(hash_to_path,"")!=0){ 
        free(hash_to_path);
    }
}

// retourne une liste chaine represantant tous les commits de la branche
List* branchList(char* branch){
    if(branch==NULL || strlen(branch)==0){
        printf("Erreur sur le paramètre branch par branchList \n");
        return NULL;
    }
    List* L=initList();
    char* commit_hash=getRef(branch);
    char *hash_to_path = "";
    if(strlen(commit_hash)!=0){ // si on essaye de récupére une branche sans avoir commit au moins 1 fois
        hash_to_path = hashToPathCommit(commit_hash);
    }
    Commit* c=ftc(hash_to_path);
    // Tant qu'on a un commit
    while(c!=NULL){
        Cell* ctmp=buildCell(commit_hash);
        insertFirst(L,ctmp);
        char *CP = commitGet(c,"predecessor");
        if(CP!=NULL){ // si on a un predecesseur
            free(commit_hash);
            commit_hash=strdup(CP);
            free(hash_to_path);
            hash_to_path=hashToPathCommit(commit_hash);
            // on libere le commit avant de lui donner le commit predecesseur
            liberer_Commit(c);
            c=ftc(hash_to_path);
        }
        else{
            liberer_Commit(c);
            c=NULL;
        }
    }
    free(commit_hash);
    if(strcmp(hash_to_path,"")!=0){ 
        free(hash_to_path);
    }
    return L;
}

// renvoie une liste des hash de chaque commit des branches
List* getAllCommits(){
    List* L=initList();
    List* content=listdir(".refs"); // on récupére toutes les branches et on les parcours
    for(Cell* ptr=*content;ptr!=NULL;ptr=ptr->next){
        List* list=branchList(ptr->data); // on récupére les commits de chaque branche
        Cell* cell=*list;
        while(cell!=NULL){
            if(searchList(L,cell->data)==NULL){ // si la cell n'est pas présente dans la liste
                insertFirst(L,buildCell(cell->data));
            }
            cell=cell->next;
        }
        liberer_List(list);
    }
    liberer_List(content);
    return L;
}

// EXO9 

// restaure le WorkTree associé à un Commit dont le hash est donné
void restoreCommit(char *hash_commit){
    if(hash_commit==NULL || strlen(hash_commit)<3){
        printf("Erreur sur le parametre hash_commit par restorecommit \n");
        return;
    }
    // On change le hash en path d'un commit
    char* commit_path = hashToPathCommit(hash_commit);
    // lit le fichier
    Commit* c = ftc(commit_path);
    char* path = hashToPath(commitGet(c,"tree"));
    char* tree_hash = hashToPathWT(path);
    // Crée le WorkTree associé
    WorkTree* wt = ftwt(tree_hash);
    free(tree_hash);
    free(path);
    liberer_Commit(c);
    //  Restaure les fichiers associé
    restoreWorkTree(wt,".");
    free(commit_path);
    liberer_WT(wt);
}

// Modifie le current branch par le parametre "branch", modifie le HEAD et restaure le WorkTree associé au commit de branch
void myGitCheckoutBranch(char* branch){
    if(branch==NULL || strlen(branch)==0){
        printf("Erreur sur le parametre branch dans checkout-branch\n");
    }
    // On modifie le fichier current_branch
    FILE *f = fopen(".current_branch","w");
    fprintf(f,"%s",branch);
    fclose(f);
    char* hash_commit = getRef(branch);
    // on modifie le hash de HEAD par le dernier commit de "branch"
    createUpdateRef("HEAD",hash_commit);
    // On restaure le WorkTree correspondant
    restoreCommit(hash_commit);
    free(hash_commit);
}

// Retourne une liste contenant les éléments de L qui commencent par la chaine pattern
List* filterList(List* L, char* pattern){
    List* filtered = initList();
    if(pattern == NULL || strlen(pattern)==0){
        printf("Erreur sur le parametre pattern par filterList\n");
        return filtered; // On retourne une liste vide 
    }
    for(Cell* ptr= *L;ptr !=NULL;ptr = ptr->next){
        char* c=strdup(ptr->data);
        // On coupe la suite des éléments pour comparer
        c[strlen(pattern)]='\0';
        if(strcmp(c,pattern)==0){ // On fait la comparaison
            Cell *cell = buildCell(ptr->data);
            insertFirst(filtered,cell);
        }
        free(c);
    }
    return filtered;
}

// Calcule la longueur d'une liste L
int ListSize(List *L){
    Cell* c=*L;
    int i=0;
    while(c!=NULL){
        i++;
        c=c->next;
    }
    return i;
}

// Permet de retourner sur une version d'un commit choisi par l'utilisateur à l'aide de son hash correspondant/pattern correspondant
void myGitCheckoutCommit(char* pattern){
    if(pattern == NULL || strlen(pattern)==0){
        printf("Erreur sur le parametre pattern par checkout-commit\n");
        return;
    }
    // on récupère la liste de commits dans le projet
    List* L=getAllCommits();
    // on récupère le/les commits correspondant à l'utilisateur
    List* filtered_list=filterList(L,pattern);
    // si on trouve 1 commit correspondant
    if(ListSize(filtered_list) == 1){
        Cell* tmp=listGet(filtered_list,0);
        char* commit_hash = tmp->data;
        // On modifie le HEAD par le hash du commit trouvé
        createUpdateRef("HEAD",commit_hash);
        // On restaure cette version du projet
        restoreCommit(commit_hash);
        
    }
    else{
        // Si on en trouve pas
        if(ListSize(filtered_list)==0){
            printf("Aucun motif trouve\n");
        }
        else{
            // Si on en trouve plusieurs
            printf("Plusieur motif trouves\n");
            for(Cell* ptr=*filtered_list; ptr!=NULL; ptr=ptr->next){
                printf("->%s\n",ptr->data);
            }
            printf("Veuillez etre plus precis sur votre motif\n");
        }
    }
    liberer_List(L);
    liberer_List(filtered_list);
}

// EXO11

// Crée un WorkTree contenant la fusion des 2 WorkTree donné en paramètre et une liste contenant les fichiers/répertoire similaires
WorkTree* mergeWorkTrees(WorkTree* wt1, WorkTree* wt2, List** conflicts){
    WorkTree* wt=initWorkTree();
    if(ListSize((*conflicts))!=0){
        printf("Erreur la liste de conflit doit etre vide\n");
        return wt;
    }
    for(int i=0;i<wt1->n;i++){
        // on vérifie si un des éléments de wt1 est déjà dans wt2
        if(inWorkTree(wt2,wt1->tab[i].name)==-1){
            appendWorkTree(wt,wt1->tab[i].name,wt1->tab[i].hash,wt1->tab[i].mode);
        }
        else{
            // On l'insère dans la liste de conflit si déjà présent
            Cell* tmp=buildCell(wt1->tab[i].name);
            insertFirst(*(conflicts),tmp);
        }
    }
    for(int j=0;j<wt2->n;j++){
        // On cherche les éléments de wt2 correspondants à la liste de conflits rempli
        if(searchList(*(conflicts),wt2->tab[j].name)==NULL){
            appendWorkTree(wt,wt2->tab[j].name,wt2->tab[j].hash,wt2->tab[j].mode);
        }
        else{
            continue;
        }
    }
    return wt;
}

// passe d'un hash d'une branche au path d'un WorkTree 
WorkTree* branch_To_WT(char* branch){
    if(branch==NULL || strlen(branch)==0){
        printf("Erreur sur le parametre de branch WT \n");
        return NULL;
    }
    char* hash = getRef(branch);
    char* path_commit = hashToPathCommit(hash);
    Commit* c = ftc(path_commit);
    char* tree = commitGet(c,"tree");
    char* path_tree = hashToPathWT(tree);
    WorkTree* WT = ftwt(path_tree);
    free(hash);
    free(path_commit);
    free(path_tree);
    liberer_Commit(c);
    return WT;
}

// fusionne deux commits , supprime la branche secondaire et met a jour la version du projet.
List* merge(char* remote_branch, char* message){
    char* current_branch=getCurrentBranch();

    // On crée les WorkTree associé aux 2 branches
    WorkTree* Current_WT = branch_To_WT(current_branch);
    WorkTree* Remote_WT = branch_To_WT(remote_branch);
    
    List* conf=initList();
    WorkTree* WT_Fusion=mergeWorkTrees(Remote_WT,Current_WT,&conf);
    
    if(ListSize(conf)!=0){ // S'il existe une liste de conflit
        free(current_branch);
        liberer_WT(WT_Fusion);
        liberer_WT(Current_WT);
        liberer_WT(Remote_WT);
        return conf;
    }
    
    else{

        // On récupére les hash de chaque commit
        char* remote_hash = getRef(remote_branch);
        char* current_hash = getRef(current_branch);

        // On sauvegarde le WorkTree de fusion 
        char* hashWT=saveWorkTree(WT_Fusion,".");
        
        // Création du commit de cette sauvegarde
        Commit* cres=createCommit(hashWT);

        // si on a un message
         if(message!=NULL){
            commitSet(cres,"message",message);
        }
        commitSet(cres,"predecessor",current_hash);
        commitSet(cres,"merged_predecessor",remote_hash);

        char* hashCommit=blobCommit(cres);

        // mise a jour des hash dans la branche courante et le HEAD
        createUpdateRef(current_branch,hashCommit);
        createUpdateRef("HEAD",hashCommit);
        deleteRef(remote_branch);
        restoreCommit(hashCommit);

        free(current_branch);
        free(remote_hash);
        free(current_hash);
        free(hashWT);
        free(hashCommit);
        liberer_Commit(cres);
        liberer_List(conf);
        liberer_WT(WT_Fusion);
        liberer_WT(Current_WT);
        liberer_WT(Remote_WT);
        return NULL;
    }
}

// crée un commit de suppression des conflits
void createDeletionCommit(char* branch, List* conflicts, char* message){
    // on sauvegarde l'ancienne branche courante
    char *current_branch = getCurrentBranch();

    // on se déplace vers la branche secondaire
    myGitCheckoutBranch(branch);
    // on crée le WT associé au dernier commit
    WorkTree* WT = branch_To_WT(branch);

    // on vérifie l'existence du fichier add
    if(file_exists(".add")==1){
        system("rm .add");
    }
    
    // on inséere dans add les fichiers qui sont pas en conflit
    for(int i=0;i<WT->n;i++){
        if(searchList(conflicts,WT->tab[i].name)==NULL){
            myGitAdd(WT->tab[i].name);
        }
    }
    // on crée le commit de suppression
    myGitCommit(branch,message);

    // on retourne dans la branche de départ
    myGitCheckoutBranch(current_branch);
    free(current_branch);
    liberer_WT(WT);

}
