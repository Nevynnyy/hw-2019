#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    /*char*man[]={"qqq","we","wer"};
    argv=man;
    argc=2;*/
    printf("Program name: %s\n", argv[0]);

    if(argc==0){
        printf("Arguments: NONE");
    }else{
       printf("Arguments: ");
       int caunt=1;

        while(caunt<argc+1){
            if(caunt==argc){
                printf("%s",argv[argc]);
            }else{
                printf("%s,",argv[caunt]);
            }
            ++caunt;
        }

        printf("\n");

    }


    return 0;
}
