#include "tanacc.h"


int main(int argc, char** argv)
{
  if (argc != 2)
  {
    printf("Please set source string on second argument.\n");
    exit(1);
  }

  user_input = argv[1];
  token = tokenize();
  Node* node = expr();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  gen(node);

  printf("\tpop rax\n");
  printf("\tret\n");

  return 0;
}
