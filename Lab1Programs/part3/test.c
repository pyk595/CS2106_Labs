#include<stdio.h>

typedef struct a {
  char b[17];
  int c;
} type;

int main() {
  printf("%ld", sizeof(type));
  return 0;
}
