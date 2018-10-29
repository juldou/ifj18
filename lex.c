#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include <string.h>

#define MAX_LENGTH 100
#define INCREMENT 100
#define LEX_ERR -1

typedef enum {
  KEYWORD_DEF,
  KEYWORD_DO,
  KEYWORD_ELSE,
  KEYWORD_END,
  KEYWORD_IF,
  KEYWORD_NOT,
  KEYWORD_NIL,
  KEYWORD_THEN,
  KEYWORD_WHILE,

  ID,
  INT,
  FLOAT,
  STRING,
  EOL,
  COMMA,
  ROUNDL,
  ROUNDR,
  ASSIGN,
  PLUS,
  MINUS,
  MUL,
  DIV,
  LESS,
  MORE,
  LESS_EQUAL,
  MORE_EQUAL,
  EQUAL,
  NOT_EQUAL,

  START,
  LEX_EOF,
  NUM,
  IDENTIFIER,
  COMMENT,
  BLOCK_COMMENT,
  IDENTIF

} Types ;

FILE *file;
char * keyWords[] = {"def","do","else","end","if","not","nil","then","while"};
unsigned int i = 0;
char *string;

int addCharToArray(char c,char *str){

  if (i>=100){
    string = realloc(string,INCREMENT);
    if(string == NULL){
      printf("realloc err\n");
      return LEX_ERR;
    }
  }
  str[i] = c;
  i++;
  str[i] = '\0';
  return 0;
}
void clearArray(char *tmp){
  free(tmp);
}
int getToken(char *value, int *line){

  clearArray(value);

  int s,state=START;
  while(1){

    s = fgetc(file);

    switch(state){
      case START:
        if(s =='\n'){
          line++;
        }
        else if(s == EOF){
          return LEX_EOF;
        }
        if (isspace(s))
        ;
        else if(isalpha(s)||s=='_'){
          state = IDENTIF;
        }
        else if(isdigit(s)){
          state = NUM;
        }
        else{
              switch(s){
                //operatory
                case '+':
                    return PLUS;
                case '-':
                    return MINUS;
                case '*':
                  return MUL;
                case '/':
                  return DIV;
                case '(':
                  return ROUNDL;
                case ')':
                  return ROUNDR;
                case ',':
                  return COMMA;
                case '=':
                    s = fgetc(file);
                    if(s == '='){
                      return EQUAL;
                    }
                    else if(s == 'b'||s == 'e'){
                      while(!isspace(s)){
                        s = fgetc(file);
                        addCharToArray(s,string);
                        }
                        if(strcmp(string,"begin")==0){
                          state = BLOCK_COMMENT;
                        }
                        else if(strcmp(string,"end")==0){
                          state = START;
                        }
                        else{
                          return LEX_ERR;
                        }
                    }
                    else{
                      ungetc(s,file);
                      return ASSIGN;
                    }
                  }

              }

      case IDENTIF:
        addCharToArray(s,string);
        while(!isspace(s)||isalpha(s)||isdigit(s)){
          s = fgetc(file);
          addCharToArray(s,value);
        }
        //TODO check keywords

    }
}}
int main() {
  int a;
  char value[100];
  int line;
  string = malloc(sizeof(char) * MAX_LENGTH);
  if(string == NULL){
    printf("malloc err\n");
    return LEX_ERR;
  }
  file = stdin;
  if(file == NULL){
    printf("file cant open\n");
    return LEX_ERR;
  }
  while((a=getToken(value,&line))!= EOF){
    printf("Value: %s\n line: %d\n type: %d",value,line,a);
    if(a==LEX_ERR){
      break;
    }

  }
  return 0;
}
