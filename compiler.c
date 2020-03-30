#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum
{
  TK_RESERVED,
  TK_NUM,
  TK_EOF
} TokenKind;

typedef struct Token Token;

struct Token
{
  TokenKind kind;
  Token* next;
  int val;
  char* str;
};

Token* token;

typedef enum
{
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_NUM,
} NodeKind;

typedef struct Node Node;

struct Node
{
  NodeKind kind;
  Node* lhs;
  Node* rhs;
  int val;
};

/* void error(char* fmt, ...) */
/* { */
  /* va_list ap; */
  /* va_start(ap, fmt); */
  /* vfprintf(stderr, fmt, ap); */
  /* fprintf(stderr, "\n"); */
  /* exit(1); */
/* } */

char* user_input;

void error_at(char* loc, char* fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s\n", pos, "");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

bool consume(char op)
{
  if (token->kind != TK_RESERVED || token->str[0] != op)
    return false;
  token = token->next;
  return true;
}

void expect(char op)
{
  if (token->kind != TK_RESERVED || token->str[0] != op)
    error_at(token->str, "expected '%c'", op);
  token = token->next;
}

int expect_number()
{
  if (token->kind != TK_NUM)
    error_at(token->str, "It is'nt a number");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof()
{
  return token->kind == TK_EOF;
}

Token* new_token(TokenKind kind, Token* cur, char* str)
{
  Token* tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

Token* tokenize()
{
  char* p = user_input;
  Token head;
  head.next = NULL;
  Token* cur = &head;

  while (*p)
  {
    if (isspace(*p))
    {
      p++;
      continue;
    }
    else if (*p == '+' || *p == '-')
    {
      cur = new_token(TK_RESERVED, cur, p++);
      continue;
    }
    else if (isdigit(*p))
    {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }
    else
    {
      error_at(p, "Clouldn't tokenize.");
    }
  }
  new_token(TK_EOF, cur, p);
  return head.next;
}

Node* new_node(NodeKind kind, Node* lhs, Node* rhs)
{
  Node* node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node* new_node_num(int val)
{
  Node* node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

Node* expr();

Node* primary()
{
  if (consume('('))
  {
    Node* node = expr();
    expect(')');
    return node;
  }
  return new_node_num(expect_number());
}

Node* mul()
{
  Node* node = primary();
  for (;;)
  {
    if (consume('*'))
      node = new_node(ND_ADD, node, primary());
    else if (consume('/'))
      node = new_node(ND_DIV, node, primary());
    else
      return node;
  }
}

Node* expr()
{
  Node* node = mul();
  for (;;)
  {
    if (consume('+'))
      node = new_node(ND_ADD, node, mul());
    else if (consume('-'))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

void gen(Node* node)
{
  if (node->kind == ND_NUM)
  {
    printf("\tpush %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("\npop rdi\n");
  printf("\npop rax\n");

  switch (node->kind)
  {
    case ND_ADD:
      printf("\tadd rax, rdi\n");
      break;
    case ND_SUB:
      printf("\tsub rax, rdi\n");
      break;
    case ND_MUL:
      printf("\timul rax, rdi\n");
      break;
    case ND_DIV:
      printf("\tcqo\n");
      printf("\tidiv rdi\n");
      break;
    default:
      error_at(user_input, "Invalid Node type.");
  }
  printf("\tpush rax\n");
}

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    printf("Please set source string on second argument.\n");
    exit(1);
  }

  user_input = argv[1];
  token = tokenize();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  printf("\tmov rax, %d\n", expect_number());

  while (!at_eof())
  {
    if (consume('+'))
    {
      printf("\tadd rax, %d\n", expect_number());
    }
    else
    {
      expect('-');
      printf("\tsub rax, %d\n", expect_number());
    }
  }
  printf("\tret\n");

  return 0;
}
