#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

//references its children, character it represents for ease of concatenation, and if it is the end of a file name (will be 0 or 1)
struct node{
    int endOfWord;
    struct node *children[128];
    char character;
};

//fills out all initial node values, pretty much sets everything to null & false
struct node* createNode(){
    struct node *newNode=malloc(sizeof(struct node));
    newNode->endOfWord=0;
    newNode->character='\0';
    for(int i=0;i<128;i++){
        newNode->children[i]=NULL;
    }
    return newNode;
}


//looks at every file name and creates connected trie nodes out of them
struct node* fillTrie(struct node* currentNode, char** files, int arraySize){
    struct node* thisNode=currentNode;
    for(int i=0;i<arraySize;i++){
        for(int x=0;x<strlen(files[i]);x++){
            int childPos;
            childPos=(int)files[i][x];
            if(thisNode->children[childPos]==NULL) {
                thisNode->children[childPos] = createNode();
            }
            thisNode=thisNode->children[childPos];
            thisNode->character=files[i][x];
        }
        thisNode->endOfWord=1;
        thisNode=currentNode;
    }

    return currentNode;

}

//goes through connected nodes of initially given substring and finds the file name values that start with it
void completeFileName(struct node* currentNode, char* currentSubstring, int currentSubstringPos){
    if(currentNode->endOfWord==1){
        currentSubstring[currentSubstringPos+1]='\0';
        printf("%s\n",currentSubstring);
    }
    for(int i=0;i<128;i++){
        if(currentNode->children[i]!=NULL){
            //printf("We found more!\n");
            char newSubstring[100];
            strcpy(newSubstring,currentSubstring);
            newSubstring[currentSubstringPos+1]=currentNode->children[i]->character;
            int newPos=currentSubstringPos+1;
            completeFileName(currentNode->children[i],newSubstring,newPos);
        }
    }
}

//simple comparison to put strings in order based on ASCII values
int stringCmp(const void *a, const void *b)
{
    return strcmp(a, b);
}

//takes a user-inputted file substring and if that substring exists, we send it over to completeFileName to get the possible file names
int driver(struct node* root, char *directoryName){
    printf(">");
    char name[200];
    char *exitCase="EXIT\n";
    fgets(name,sizeof(name),stdin);
    char stringBuilder[100];
    struct node* pointerNode=root;
    int found=1;
    int currentStringBuilderPos=0;
    for(int i=0;i<strlen(name)-1;i++){
        int arrayPos=(int)name[i];
        pointerNode=pointerNode->children[arrayPos];
        if(pointerNode==NULL){
            //printf("%c not found\n",name[i]);
            found=0;
            break;
        }else{
            //printf("char found.\n");
            stringBuilder[i]=name[i];
            currentStringBuilderPos=i;
        }

    }
    if(stringCmp(name,exitCase)!=0) {
        printf("\n");
        if (found == 1) {
            stringBuilder[currentStringBuilderPos+1]='\0';
            printf("Files starting with %s in %s:\n",stringBuilder,directoryName);
            completeFileName(pointerNode, stringBuilder, currentStringBuilderPos);
            pointerNode = root;
            printf("\n");
            driver(pointerNode,directoryName);
        } else {
            printf("No file names found starting with that substring");
            pointerNode = root;
            printf("\n");
            driver(pointerNode,directoryName);
        }
    }else {
        return 0;
    }
}

int main(){
    //have the user enter a directory name later, not needed for testing purposes
    //create a directory entry pointer
    struct dirent *dep;
    //now create a DIR pointer
    //"C:\\Users\\theif\\downloads\\project1Files"
    printf("Enter a folder name:");
    char dirName[200];
    fgets(dirName,sizeof(dirName),stdin);
    //if fgets finds "EXIT" then close the program
    if(stringCmp(dirName,"EXIT\n")==0){
        exit(0);
    }
    for(int i=0;i<strlen(dirName);i++){
        if(dirName[i]=='\n'){
            dirName[i]='\0';
            i=strlen(dirName);
        }
    }
    DIR *dp=opendir(dirName);
    //If no directory is found with the given name, start over
    if(dp==NULL){
        printf("No directory of that name\n");
        main();
    }
    //while the entry pointer still has entries to process
    int dirSize=0;
    //only takes in file names, not subdirectory names
    while((dep=readdir(dp))!=NULL){
        char *name=dep->d_name;
        DIR* currentFile=opendir(name);
        if(currentFile==NULL) {
            dirSize++;
        }
    }
    closedir(dp);
    dp=opendir(dirName);
    //create an array to hold file names. make sure it isn't string literals so they can be changed when sorted later!
    char files[dirSize][100];
    int arrayStart=0;
    while (((dep=readdir(dp)))!=NULL){
        char *name=dep->d_name;
        DIR* currentFile=opendir(name);
        if(currentFile==NULL) {
            strcpy(files[arrayStart], name);
            arrayStart++;
        }
    }


    qsort(files,dirSize,100,stringCmp);
   char *sortedFiles[dirSize];
   for(int i=0;i<dirSize;i++){
       sortedFiles[i]=files[i];
   }
   struct node *root=createNode();
   root=fillTrie(root,sortedFiles,dirSize);
   driver(root,dirName);

    return 0;
}
