#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


static int total_lines = 0, total_words = 0, total_chars = 0;
void wc(FILE *infile, char *inname);

int main(int argc, char *argv[]) {
    if(argc > 1) {

        FILE* infile;
        int caunt=1;
        while(caunt<argc+1){
            infile=fopen(argv[caunt],"r");
            wc(infile,argv[caunt]);
            ++caunt;
        }

        if(argc > 2) {
            printf("%7d %7d %7d   %s\n",  total_lines, total_words, total_chars, "total");
        }

    } else {

        char input[20];
        scanf("%s",input);
    }

    return 0;
}

void wc(FILE *infile, char *inname) {
    int lines = 0, words = 0, chars = 0;


    if(infile==NULL){

    }else{
        char character;
        /*int num=1;
        char inp[10000];*/



        character=getc(infile);
        /*inp[0]=character;*/

        while(character!=EOF){

            if(character=='\n'){
                ++lines;
            }

            /*if((inp[num-2]!=' ' || inp[num-2]!='\n' || inp[num-2]!='\t' || inp[num-2]!='\0') && (character==' ' || character=='\t' || character=='\0' || character=='\n')){
                ++words;
            }*/
            int a=0,b=1,c;
            if(character==' ' || character=='\t' || character=='\0' || character=='\n'){
                ++words;
            }


            ++chars;
            character=getc(infile);
            /*inp[num]=character;
            ++num;*/
        }


        total_lines += lines;
        total_words += words;
        total_chars += chars;
        printf("%7d %7d %7d   %s\n",  lines, words, chars, inname);

    }

}
