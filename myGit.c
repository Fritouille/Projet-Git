#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "List.h"
#include "WorkTree.h"
#include "Commit.h"

int main(int argc,char* argv[]){
    if(argv[1]==NULL){
        printf("Usage : ./myGit <commande>\n");
        return 0;
    }
    if(strcmp(argv[1],"init")==0){
        initRefs();
        initBranch();
    }

    if(strcmp(argv[1],"list-refs")==0){
        printf("REFS : \n");
        if(file_exists(".refs")){
            List* L= listdir(".refs");
            for(Cell* ptr=*L;ptr!=NULL;ptr=ptr->next){
                char* content=getRef(ptr->data);
                printf ("- %s \t %s \n",ptr->data,content);
                free(content);
            }
            liberer_List(L);
        }
    }

    if(strcmp(argv[1],"create-ref")==0){
        if(argv[2]==NULL || argv[2]==NULL){
            printf("Usage : ./myGit <commande> <ref> <hash>\n");
            return 0;
        }
        createUpdateRef(argv[2],argv[3]);
    }

    if(strcmp(argv[1],"delete-ref")==0){
        if(file_exists(".refs")==1){
            if(argv[2]==NULL){
                printf("Usage : ./myGit <commande> <ref>\n");
            return 0;
            }
            deleteRef(argv[2]);
        }
        else{
            printf("le repertoire est deja supprime\n");
        }
    }

    if(strcmp(argv[1],"add")==0){
        for(int i=2; i<argc;i++){
            myGitAdd(argv[i]);
        }
    }

    if(strcmp(argv[1],"clear-add")==0){
        if(file_exists(".add")==1){
            system("rm .add");
        }
        else{
            printf("le fichier est deja supprime\n");
        }
    }

    if(strcmp(argv[1],"list-add")==0){
        printf("Zone de preparation : \n");
        if(file_exists(".add")==1){
            WorkTree* wt = ftwt(".add");
            char*ch = wtts(wt);
            printf("%s\n", ch);
            free(ch);
            liberer_WT(wt); 
        }
    }

    if(strcmp(argv[1],"commit")==0){
        if(argv[2]==NULL){
            printf("Usage : ./myGit <commande> <branch> [<-m> <message> optionnel]\n");
            return 0;
        }
        if(argv[3]!=NULL && strcmp(argv[3],"-m")==0){
            myGitCommit(argv[2],argv[4]);
        }
        else{
            myGitCommit(argv[2],NULL);
        }
    }

    if(strcmp(argv[1],"get-current-branch")==0){
        char* ch = getCurrentBranch();
        printf("%s\n",ch);
        free(ch);
    }

    if(strcmp(argv[1],"branch")==0){
        if(argv[2]==NULL){
            printf("Usage : ./myGit <commande> <branch>\n");
            return 0;
        }
        if(branchExists(argv[2])==0){
            createBranch(argv[2]);
        }
        else{
            printf("La branche existe deja\n");
        }
    }
    
    if(strcmp(argv[1],"branch-print")==0){
        if(argv[2]==NULL){
            printf("Usage : ./myGit <commande> <branch>\n");
            return 0;
        }
        if(branchExists(argv[2])==0){
            printf("La branche n'existe pas\n");
        }
        else{
            printBranch(argv[2]);
        }
    }

    if(strcmp(argv[1],"checkout-branch")==0){
        if(argv[2]==NULL){
            printf("Usage : ./myGit <commande> <branch>\n");
            return 0;
        }
        if(branchExists(argv[2])==0){
            printf("La branche n'existe pas\n");
        }
        else{
            myGitCheckoutBranch(argv[2]);
        }
    }

    if(strcmp(argv[1],"checkout-commit")==0){
        if(argv[2]==NULL){
            printf("Usage : ./myGit <commande> <hashpattern>\n");
        return 0;
        }
        myGitCheckoutCommit(argv[2]);
    }

    if(strcmp(argv[1],"merge")==0){
        if(argv[2]==NULL){
            printf("Usage : ./myGit <commande> <branch> [<message> optionnel] \n");
            return 0;
        }
        List* conf=merge(argv[2],argv[3]);
        if(conf==NULL){
            printf("La fusion s'est bien passe\n");
        }
        else{
            int val;
            printf("AVANT de choisir de votre option veuillez vérifiez qu'il existe au moins un fichier différent entre les 2 commits\n");
            printf("1: Garder les fichiers de la branche courante, et donc créer un commit de suppression pour la branche %s, avant de faire appel a la fonction merge.\n",argv[2]);
            printf("2: Garder les fichiers de la branche %s, et donc creer un commit de suppression pour la branche courante, avant de faire appel a la fonction merge.\n",argv[2]);
            printf("3: Choisir manuellement,les fichier/repertoires en conflit a garder parmis les 2 branches avant de faire la fusion avec la fonction merge.\n");
            printf("Veuillez saisir votre chiffre compris 1 et 3 : \n");
            
            scanf("%d",&val);
            while(val<1 || val>3){
                printf("Veuillez rechoisir une valeur compris entre 1 et 3 : \n");
                scanf("%d",&val);
            }
            char* chconf;
            char* current;
            switch(val){
                case 1:
                    createDeletionCommit(argv[2],conf,argv[3]);
                    merge(argv[2],argv[3]);
                    liberer_List(conf);
                    break;
                    
                case 2:
                    current=getCurrentBranch();
                    myGitCheckoutBranch(argv[2]);
                    createDeletionCommit(current,conf,argv[3]);
                    merge(current,argv[3]);
                    free(current);
                    liberer_List(conf);
                    break;

                case 3:
                    chconf=ltos(conf);
                    printf("Les fichier/repertoire en conflit %s\n",chconf);
                    List* conf_current=initList();
                    List* conf_branch=initList();
                    printf("1: branche courante\n2: branche %s\n",argv[2]);
                    int val2=0;
                    for(int i=0;i<ListSize(conf);i++){
                        Cell* tmp=buildCell(listGet(conf,i)->data);
                        printf("Choisir dans quel branche garde t on %s\n",tmp->data);
                        scanf("%d",&val2);

                        while(val2!=1 && val2!=2){
                            printf("Veuillez rechoisir une valeur compris entre 1 et 2: \n");
                            scanf("%d",&val2);
                        }
                        if(val2==1){
                            insertFirst(conf_current,tmp);
                            val2=0;
                        }
                        if(val2==2){
                            insertFirst(conf_branch,tmp);
                            val2=0;
                        }
                    }

                    char* current2=getCurrentBranch();
                    createDeletionCommit(argv[2],conf_branch,argv[3]);
                    createDeletionCommit(current2,conf_current,argv[3]);
                    merge(argv[2],argv[3]);

                    free(current2);
                    free(chconf);
                    liberer_List(conf);
                    liberer_List(conf_branch);
                    liberer_List(conf_current);
                    break;
            }
        }
    }

    return 0;
}

