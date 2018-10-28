#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include <string.h>

#define  EOL '\n'

//keywords
#define LEXERR -1 //ERR
#define KEYWORD 20 //KEYWORD
#define IDENTIF 21 //IDENTIF
#define ONECOM 22 //JEDNORIADKOVY KOMENT
#define COM 23//KOMENTAR
#define PLUS 24// +
#define MINUS 25// -
#define MUL 26// *
#define DIV 27// /
#define ROUNDL 28//(
#define ROUNDR 29//)
#define SQUAREL 30//[
#define SQUARER 31//]
#define DOUAPOS 34//"
#define ONEAPOS 35//'
#define EXCL 36//!
#define LESS 37//<
#define MORE 38//>
#define LESSEQ 39 //<=
#define MOREEQ 40 //>=
#define EQ 41 //==
#define IS 42 //=
#define INTEGER 43 //INT
#define DOUBLE 44 //DOUBLE
#define STRING 45 //STRING
#define PLUS_EQ 46 //+=
#define MINUS_EQ 47 //-=
#define COMPOSEDL 48 //{
#define COMPOSEDR 49 //}
#define BLOCK_BEGIN 50 //{
#define BLOCK_END 51 //}
#define START 51 //}
#define NUM 52 //}


char * keyWords[] = {"def","do","else","end","if","not","nil","then","while"};
unsigned int i = 0;

void clearArray(char *str){
  int i=0;
  while(str[i]!='\0'){
    str[i]='\0';
    i++;
  }
}

void addCharToArray(char c,char *str){
  str[i] = c;
  i++;
}

int main() {
  /* code */
  return 0;
}

int getToken(FILE *file,char *type, char *value, int *line){

  clearArray(value);
  clearArray(type);

  int s,state=START;
  int finished = 0;

  while(!finished){

    s = fgetc(file);

    switch(state){
      case START:
        if (isspace(s))
        ;
        else if(isalpha(s)||s=='_'){
          state = IDENTIF;
        }
        else if(isdigit(s)){
          state = NUM;
        }
        // else if(s == '\'){
        //   state = BACK_SLASH;
        // }
        else{
              switch(s){
            //operatory
              case '+':
                  s = fgetc(file);
                  if(s == '='){
                    *type = PLUS_EQ;
                    *value = '+=';
                  }
                  else{
                    ungetc(s,file);
                    *type = PLUS;
                    *value = '+';
                  }

              case '-':
                  s = fgetc(file);
                  if(s == '='){
                    *type = MINUS_EQ;
                    *value = '-=';
                  }
                  else{
                    ungetc(s,file);
                    *type = MINUS;
                    *value = '-';
                  }

              case '*':
                *type = MUL;
                *value = '*';

              case '/':
                *type = DIV;
                *value = '/';

              case '(':
                *type = ROUNDL;
                *value = '(';

              case ')':
                *type = ROUNDR;
                *value = ')';

              case '[':
                *type = SQUAREL;
                *value = '[';

              case ']':
                *type = SQUARER;
                *value = ']';

              case '{':
                *type = COMPOSEDL;
                *value = '{';

              case '}':
                *type = COMPOSEDR;
                *value = '}';

              case '=':
                  s = fgetc(file);
                  if(s == '='){
                    *type = EQ;
                    *value = '==';
                  }
                  if(s == 'b'||s == 'e'){
                    char *tmp;
                    while(!isspace(s)){
                      s = fgetc(file);
                      addCharToArray(s,tmp);
                    }
                    if(strcmp(tmp,"begin")==0){
                      *type = BLOCK_BEGIN;
                    }
                    if(strcmp(tmp,"end")==0){
                      *type = BLOCK_END;
                    }
                    else{
                      *type = LEXERR;
                    }
                    *type = IS;
                    *value = '=';
                    }
              }

              }

      case IDENTIF:
        addCharToArray(s,value);
        while(!isspace(s)||!isalpha(s)||!isdigit(s)){
          s = fgetc(file);
          addCharToArray(s,value);
        }
        //TODO check keywords

      case NUM:

    }
}
