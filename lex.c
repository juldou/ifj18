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

unsigned int i = 0;
char *string;
int checkKeywords(char *tmp){

  char * keyWords[] = {"def","do","else","end","if","not","nil","then","while"};
  if(strcmp(keyWords[0],tmp)==0) return KEYWORD_DEF;
  if(strcmp(keyWords[1],tmp)==0) return KEYWORD_DO;
  if(strcmp(keyWords[2],tmp)==0) return KEYWORD_ELSE;
  if(strcmp(keyWords[3],tmp)==0) return KEYWORD_END;
  if(strcmp(keyWords[4],tmp)==0) return KEYWORD_IF;
  if(strcmp(keyWords[5],tmp)==0) return KEYWORD_NOT;
  if(strcmp(keyWords[6],tmp)==0) return KEYWORD_NIL;
  if(strcmp(keyWords[7],tmp)==0) return KEYWORD_THEN;
  if(strcmp(keyWords[8],tmp)==0) return KEYWORD_WHILE;
  else return 0;

}
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
int getToken(char *value, int *line){

  int s,state=START,f;
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
                case '<':
                  s = fgetc(file);
                  if(s =='='){
                    return LESS_EQUAL;
                  }
                  else {
                    ungetc(s,file);
                    return LESS;}
                case '>':
                  s = fgetc(file);
                  if(s =='='){
                    return MORE_EQUAL;
                  }
                  else {
                    ungetc(s,file);
                    return MORE;}
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
                default:
                    printf("LEX_ERR\n");
              }

      case IDENTIF:
        if(islower(s)){
        addCharToArray(s,string);
        }

        if(isalnum(s)|| s=='_'||s=='?'||s=='!'){
          while(isalnum(s)|| s=='_'){
          s = fgetc(file);
          addCharToArray(s,string);
          }
          s = fgetc(file);
          if(s=='?'||s=='!'){
          addCharToArray(s,string);
          }
          else{
          ungetc(s,file);
          }
          int a = checkKeywords(string);
          if (a == 0){
            return IDENTIFIER;
            value = string;
          }
          else{
            return a;
            value = string;
          }

      case NUM:
          f=0;
          while(isdigit(s)){
            if(isdigit(s)){
              addCharToArray(s,value);
            }
            else if(s=='.'||s=='e'||s=='E'){
              f++;
              addCharToArray(s,value);
              return FLOAT;
            }
          }
        if(f==0) return INT;
        else return FLOAT;
        value=string;
    }
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
