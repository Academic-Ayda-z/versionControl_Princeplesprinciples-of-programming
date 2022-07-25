#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <direct.h>
#define size 100
#define MAX_SIZE 1000
#define BADKEY -1
typedef struct { char *key; int val; }strSwitch;
static strSwitch myStrSwitch[] = {
        { "init", 1 }, { "status", 2 }, { "select", 3 },
        { "unselect", 4 }, { "commit", 5 }, {"log",6},
        {"reset",7},{"stash",8}
};

//#define NKEYS (sizeof(myStrSwitch)/sizeof(strSwitch))
//const char typeNames[][10]={"ini","stat","sel","uns","com","log","res","stas"};
struct strSwitch{
    char str[10];
    int num;
};
struct mainInfo{
    char ** fileNames;
    int * commitId;
    int n;
    int len;
};
struct mainInfo* mainF;
struct  status {
    char ** names;
    int * allStatus;////-1=delete 1=add  3=changed
    int n;
    int len;
};
struct status* myStatus;
struct select{
    char ** selectNames;
    int len;
    int n;
};
struct select * selected;
struct delete{
    char** names;
    int * lastCommit;
    int n;
    int len;
};
struct delete * deleted;
int findValueOfString(char *key);
void menu();
//int findId(char * name);
void init();
void status();
void select();
void unselect();
void commit();
void log();
void resetOrSlash(int resetOrStash);
void compare(FILE * diff,char * str1,char * str2,int i);
void exactDiff(FILE * diff,char * str1,char * str2,int whoIslonger);
int compareFiles(FILE*pf1,FILE*pf2,int i,char *fileName);
void copyFile(FILE*source,FILE*second);
void readDiffFile(FILE*diff,FILE*firstF,int i,char * name,int resetOrStash);
char pathFoulder[MAX_SIZE];
//FILE *pathF;
int id;
int main(){
    //struct status* myStatus;
    id=1;
  /*  myStatus->n=0;
    deleted->len=0;
    deleted->n=0;
    mainF->n=0;
    mainF->len=0;
    selected->len=0;
    selected->n=0;
    */
  while(1){
        char path[size];
        system("cls");
        // printf("%s","please inter your directory path");
        //  scanf("%s",path);
        ;
        FILE * pathF=_popen("cd >path.txt","r");
        fscanf(pathF,"%s",pathFoulder);
        fclose(pathF);
        //remove(pathF);

        menu();
        system("pause");
    }
    return 0;
}
char command[MAX_SIZE];
//char pathFoulder[MAX_SIZE];
int findValueOfString(char *key)
{
    for (int i=0; i < 10; i++) {
        strSwitch *sym = & myStrSwitch[i];
        if (strcmp(sym->key, key) == 0)
            return sym->val;
    }
    return BADKEY;
}
void init(){
    int check;
    char path[MAX_SIZE];
    strcpy(path,pathFoulder);
    char name[]="myVersionControl";

    check = _mkdir(name);
    //printf("%s\n",path);
    if(check) {
        printf("%s","you have already initialized this project!");
        return;
    }
    sprintf(command,"mkdir %s\\%s\\%s",path,name,"commits");
    system(command);
    sprintf(command,"mkdir %s\\%s\\%s",path,name,"lastVersions");
    system(command);
    sprintf(command,"mkdir %s\\%s\\%s",path,name,"allMessages");
    system(command);
    sprintf(command,"mkdir %s\\%s\\%s",path,name,"reset");
    system(command);

    strcpy(pathFoulder,"\\myVersionControl");
    printf("%s","successfully initialized!");
}
void status(){
    int n=myStatus->n,state;
    while(n--){
        char name[size];
        strcpy(name,myStatus->names[n]);
        state=myStatus->allStatus[n];
        if(myStatus->allStatus==NULL)
            state=0;
        printf("%s",name);
        switch (state) {
            case (-1):
            {
                printf("%10s","deleted");
                break;
            }
            case (0):{
                printf("%10s","nothing changed.");
            }
            case (1):
            {
                printf("%10s","recently added");
                break;
            }
            case (3):
            {
                printf("%10s","changed file");
                break;
            }
            default:
                printf("%10s","unknown status");
                break;
        }
    }/////show selects
}
void initialize(){

    if(id==1){
        myStatus->n=0;
        deleted->len=0;
        deleted->n=0;
        mainF->n=0;
        mainF->len=0;
        selected->len=0;
        selected->n=0;

    }

    if(selected->len==0){
        selected->selectNames=(char**)malloc(sizeof(char*)*10);
        selected->len=10;
    }
    if(mainF->len==0) {
        mainF->fileNames = (char **) malloc(sizeof(char *) * 10);
        mainF->len = 10;
        mainF->commitId = (int *) malloc(sizeof(int) * 10);
    }
    if(deleted->len==0) {
        deleted->names = (char **) malloc(sizeof(char *) * 10);
        deleted->n = 10;
        deleted->lastCommit = (int *) malloc(sizeof(int) * 10);
    }
    if(myStatus->len==0){
        myStatus->names=(char**)malloc(sizeof(char*)*10);
        myStatus->allStatus=(int*)malloc(sizeof(int*)*10);
        myStatus->len=10;
        return;
    }
    if(selected->len-1==selected->n){
        selected->selectNames=(char**)malloc(sizeof(char*)*(selected->len)*2);
        selected->len*=2;
    }
    if(mainF->len-1==mainF->n){
        mainF->fileNames=(char**)malloc(sizeof(char*)*(selected->len)*2);
        mainF->commitId=(int*)malloc(sizeof(int)*mainF->len*2);
        mainF->len*=2;
    }

    if(deleted->len-1==deleted->n){
        deleted->names=(char**)malloc(sizeof(char*)*(deleted->len)*2);
        deleted->lastCommit=(int*)malloc((sizeof(int))*deleted->len*2);
        deleted->len*=2;
    }
    if(myStatus->len-1==myStatus->n){
        myStatus->names=(char**)malloc(sizeof(char*)*(myStatus->len)*2);
        myStatus->allStatus=(int*)malloc((sizeof(int))*myStatus->len*2);
        myStatus->len*=2;
    }
}
void copyMainFToSelected(){
    int n=mainF->n;
    if(selected->len<n){
        initialize();
    }
    while(n--){
        strcpy(selected->selectNames[n],mainF->fileNames[n]);
    }
}
int exist(char *name){
    for (int i = 0; i < mainF->n; i++) {
        if(strcmp(name,mainF->fileNames[i]))
            return 0;
    }
    return 1;
}
void select(){
    initialize();
    int end=0,n=0;
    char line[MAX_SIZE];
    gets_s(line,MAX_SIZE);
    while(end!=-1) {

        if (selected->len == selected->n)
            initialize();

        char word[size];

        end = sscanf(line, "%s", word);
        if (end == -1){

            break;
    }
        *line=*line+strlen(word);

        if(!strcmp(word,"-all")){
            copyMainFToSelected();
            return;
        }
        char * temp=(char*)malloc((strlen(word)+1)*sizeof(char));//initialize problem
        temp=word;
        if((id!=1) && exist(temp))
            selected->selectNames[selected->n++]=temp;
        else{
            printf("%s","at least one of the names is incorrect!");
        }
    }
    printf("%s","selected.");
}

int findFileNameInSelecteds(char * name,selected) {
    int n = selected->n;
    while (n--) {
        if (!strcmp(name,selected->selectNames[n]))
            return n;
    }
    return -1;
}

void delete(char * name,selected){
    int index=findFileNameInSelecteds(name);//////////
    if(index==-1)
        return;
    char* temp=selected->selectNames[index];
    selected->selectNames[index]=selected->selectNames[--selected->n];
    selected->selectNames[selected->n]=temp;

}
void unselect(deleted,myStatus,selected,mainF){
    int end=0;
    char line[MAX_SIZE],word[size];
    gets_s(line,MAX_SIZE);
    while(end!=-1) {
        end = sscanf(line, "%s", word);
        if(end==-1)
            break;
        *line=*(line+strlen(word)+1);
        if(!strcmp(word,"_all")){
            int n=selected->n;
            selected->n=0;
            while (n--) {
                selected->selectNames[n] = NULL;
            }
            return;
        }

        char*temp;
        temp=(char*)malloc((strlen(word)+1)*sizeof(char));
        temp=word;
        int exist =findFileNameInSelecteds(temp);
        if(exist){
            delete(temp);
        }
        else{
            printf("could not delete this file :%s",temp);
        }
    }
}

int trash(char * name){
    int n=deleted->n;
    while(n--){
        if(!strcmp(deleted->names[n],name))
            return n;
    }
    return 0;
}
void findExistence(){
    int n=mainF->n;
    char name[size];
    while(n--) {
        sprintf(name, "%s", mainF->fileNames[n]);
        FILE *pF = fopen(name, "r");
        if (pF == NULL) {
            initialize();
            deleted->names[n] = (char *) malloc((strlen(mainF->fileNames[n]) + 1) * (sizeof(char)));//////
            strcpy(deleted->names[n], mainF->fileNames[n]);
            deleted->n++;
            deleted->lastCommit[n] = id;

            myStatus->names[n] = (char *) malloc(sizeof(char) * (strlen(mainF->fileNames[n]) + 1));
            myStatus->allStatus[n] = -1;
            myStatus->n++;

            char *temp = mainF->fileNames[n];
            mainF->fileNames[n--] = mainF->fileNames[mainF->len - 1];
            mainF->fileNames[mainF->len - 1] = temp;

            fclose(pF);

        }
    }
}
void commit(){
    myStatus->allStatus=NULL;
    myStatus->n=0;
    myStatus->names=NULL;

    initialize();
    findExistence();

    char path[MAX_SIZE];
    char pathLastV[MAX_SIZE];
    char message[MAX_SIZE],num[size];

    while(gets_s(message,1000)==NULL)printf("%s","please enter a message.");

    int len = strlen(message);
    message[len]='\0';

    sprintf(path,"%s\\allMessages\\%s.txt",pathFoulder,num);
    FILE * m=fopen(path,"w") ;
    fprintf(m," %s\n",message);

    int n=selected->n;
    char name[size];
    strcpy(name,mainF->fileNames[n]);

    while(n--){
        initialize();
        if(trash(name)==-1) {
            printf("this file : %s  has been deleted!", name);
            return;
        }
        if(exist(name)) {

//            strcat(name, ".txt");
            FILE *changedF = fopen(name, "r");

            sprintf(pathLastV,"%s\\lastVersions\\%s",pathFoulder,name);

            FILE *lastV = fopen(pathLastV, "r");
            if(compareFiles(lastV, changedF, id, mainF->fileNames[n]))
            {
                myStatus->names[myStatus->n] = (char *) malloc(sizeof(char) * (strlen(name) + 1));
                strcpy(myStatus->names[myStatus->n], name);
                myStatus->allStatus[myStatus->n++] = 3;
            }
            fclose(lastV);
///////////////////////////////////////////////////make next version
            lastV=fopen(pathLastV,"w");
            copyFile(changedF,lastV);
            fclose(lastV);
            fclose(changedF);
        }
        else{
            myStatus->names[myStatus->n]=(char*)malloc(sizeof(char)*(strlen(name)+1));
            strcpy(myStatus->names[myStatus->n],name);
            myStatus->allStatus[myStatus->n++]=1;

            FILE * add=fopen(selected->selectNames[n],"r");
            sprintf(command,"mkdir %s\\%s\\%s",pathFoulder,"commits",selected->selectNames[n]);
            system(command);

            sprintf(path,"%s\\commits\\%s\\%s",pathFoulder,name,name);

            FILE * commit=fopen(path,"w");
            copyFile(add,commit);
            fclose(commit);

            sprintf(path,"mkdir %s\\%s",pathFoulder,"lastVersions",selected->selectNames[n]);

            sprintf(pathLastV,"%s\\lastVersions\\%s",pathFoulder,name);

            FILE * lastV=fopen(pathLastV,"w");
            copyFile(add,lastV);

            fclose(lastV);
            fclose(add);

            if(mainF->n==mainF->len)
                initialize();

            mainF->fileNames[mainF->n]=(char *)malloc((sizeof(char))*(strlen(name)+1));
            strcpy(mainF->fileNames[mainF->n++],name);
            mainF->commitId[n]=id;
        }
        fprintf(m," %s\n",name);
        fclose(m);
    }
    id++;
}
void log(){///date nadari
    int i=1;
    FILE *log;
    char path[MAX_SIZE],num[size],string[MAX_SIZE];
    while (i<=id) {
        itoa(i,num,10);
        sprintf(path, "%s\\allMessages\\%s.txt", pathFoulder,num);
        log=fopen(path,"r");
        printf("commit id: %d \n",i);
        while(fgets(string,1000,log) != NULL){
            printf("%s\n",string);
        }
        putchar('\n');
        fclose(log);
        i++;
    }
}
void resetOrSlash(int resetOrStash) {//reset=0 slash=1
    int commitId=0, lastId = 0,x;
    if (!resetOrStash){
        while (1) {
            x=scanf("%d", &commitId);
            if (x!=-1 && commitId && commitId <= id)
                break;
        }
    }
    else{
        if(scanf("%d", &commitId)==-1)
            commitId=id;
    }
    int i=1,j=0;
    while(j<mainF->n) {
        if (mainF->commitId[j]<commitId) {
            if(trash(mainF->fileNames[j])!=-1){
                lastId=deleted->lastCommit[j];
                if(lastId<=commitId) {
                    j++;////////////
                    break;
                }
            }
            char num[size];
            itoa(i,num,10);
            char name[size];
            strcpy(name,mainF->fileNames[j]);

            char path[MAX_SIZE];
            sprintf(path,"%s\\commits\\%s\\%s",pathFoulder,name,name);
            FILE * oldV=fopen(path,"r");
////////////////////////////////////////////////////////////
            sprintf(path,"%s\\commits\\%s\\%s%s.txt",pathFoulder,name,name,num);
            FILE *diff=fopen(path,"r");
            readDiffFile(oldV,diff,i,name,resetOrStash);

            while (i <= commitId) {
                itoa(i,num,10);

                readDiffFile(oldV,diff,i,name,resetOrStash);

                fclose(oldV);
                fclose(diff);

                sprintf(path,"%s\\new.txt",pathFoulder);
                oldV=fopen(path,"r");
                sprintf(path,"%s\\%s\\%s%s.txt",pathFoulder,"commits",name,num);
                diff=fopen(path,"r");

                i++;
            }
            i=1;
        }
        j++;
    }
    /*   if(resetOrStash){
           char pop[size];
           if(scanf("%s",pop));
               if(!strcmp("pop",pop));
                   ///go to last id;*/
//    }
}
void compare(FILE * diff,char * str1,char * str2,int i){
    while (str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            fprintf(diff, "%d%c", i, str2[i]);
//            printf("%d%c\n", i, str2[i]);
        }
        i++;
    }
}
void exactDiff(FILE * diff,char * str1,char * str2,int whoIslonger) {
    int i = 0;
    char * shorterStr;
    if (whoIslonger>0)   //len1>len2
        shorterStr=str2;
    else
        shorterStr=str1;

//    while (shorterStr[i] != '\0') {
    compare(diff,str1,str2,i);
    //      i++;
    // }
}
int compareFiles(FILE*pf1,FILE*pf2,int i,char * fileName){//two opened files in r mood
    char string1[MAX_SIZE];
    char string2[MAX_SIZE];
    int distance=1;
    int comp;
    char number[size];
    char path[MAX_SIZE];

    itoa(i,number,10);
    sprintf(path,"%s\\commits\\%s\\%s.txt",pathFoulder,fileName,number);

    FILE * diff=fopen(path,"w");

    int end1=0,end2=0;
    while(end1!=-1 && end2!=-1)
    {
        end1=fscanf(pf1, "%s", string1);
        end2=fscanf(pf2, "%s", string2);
        if(end1==-1 || end2==-1)
            break;
        comp=strcmp(string1,string2);
        if(comp)
        {
            fprintf(diff," %d %d ",strlen(string2),distance);
            exactDiff(diff,string1,string2,comp);
            fprintf(diff,"%c",'.');
            distance=0;
        }
        distance++;
    }
    if(end1 == -1 && end2 != -1){
        fprintf(diff,"%c",':');
        while(end2!=-1){
            end2=fscanf(pf2,"%s", string2);
            fprintf(diff, " %s", string2);
        }
    }

    fclose(pf1);
    fclose(diff);
    FILE * result=fopen(path ,"r");
    if(result==NULL){
        fclose(result);
        return 0;}
    fclose(pf2);
    return 1;
}
void copyFile(FILE*source,FILE*second){
    char ch;
    while( ( ch = fgetc(source) ) != EOF )
        fputc(ch, second);
    fclose(second);
    fclose(source);
}

void readDiffFile(FILE*diff,FILE*firstF,int i,char * name,int resetOrStash){
    if(firstF==NULL)
        return;
    int endDiff=0;
    int flag=1,num,len,index;
    char character;
    char string[MAX_SIZE];
    char string1[MAX_SIZE];
    char number[size];
    char path[MAX_SIZE];

    sprintf(path,"%s\\reset\\%s",pathFoulder,name);
    if(resetOrStash && i==id) {
        sprintf(path, "%s\\%s", pathFoulder, name);
    }
    FILE*secondF=fopen(path,"w");
    //char c;
    //c = fgetc(diff);
    //if(c!=EOF){
    if (diff==NULL)
        copyFile(firstF,secondF);
    // return name;
    while(flag){//endDiff!=-1
        //  fseek(firstF,SEEK_SET,0);
        endDiff=fscanf(diff,"%d",&len);
        if(endDiff==-1)
            return;
        char * string2;
        string2 = (char*)malloc(len*sizeof(char));

        fscanf(diff,"%d",&num);
//        printf("%d\n",num);
        num--;
        while(num--)fscanf(firstF,"%s",string1);

        fscanf(firstF,"%s",string1);////////////////////
        string1[len]='\0';
        strcpy(string2,string1);

        while(1) {
            index = -1;
            fscanf(diff, "%d", &index);
            if (index != -1){
                fscanf(diff, "%c", &character);
                string2[index] = character;
            }
            fscanf(diff, "%c", &character);
            //printf("%c",character);
            if(character=='.'){
                string2[len]='\0';
                fprintf(secondF," %s ",string2);
                endDiff=fscanf(diff, "%c", &character);

                if(character==':') {
                    while(fscanf(diff, "%s", string)!=-1)
                        fprintf(secondF," %s",string);
                    flag = 0;
                }
                break;
            }
        }
    }
    fclose(firstF);
    fclose(diff);
    fclose(secondF);
//    fclose(name);
//    return name;
}
void menu(){
    enum input in;
    char string[MAX_SIZE];
    scanf("%s",string);

    switch(findValueOfString(string)) {
        case(1):
            init();
            break;
        case(2):
            status();
            break;
        case(3):
            select();
            break;
        case(4):
            unselect();
            break;
        case(5):
            commit();
            break;
        case(6): {
            log();
            break;
        }
        case (7):{
            resetOrSlash(0);
            break;
        }
        case (8):{
            resetOrSlash(1);
            break;
        }
        default:
            printf("wrong key! pleas try again.");
            break;
    }
}




/*  FILE * pf1=fopen("a.txt","w");
fprintf(pf1,"%s","aydaa zamania");// zamania
fclose(pf1);

FILE * pf2=fopen("b.txt","w");
fprintf(pf2,"%s","ayda zamanian"); //zamanian
fclose(pf2);

pf2=fopen("b.txt","r");
pf1=fopen("a.txt","r");

compareFiles(pf1,pf2,1);
FILE * diff=fopen("diff1.txt","r");
pf1=fopen("a.txt","r");
readDiffFile(diff,pf1,1);*/
