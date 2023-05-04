#include <stdlib.h>

int main() {
  char *ptr = (char*)malloc(5 * sizeof(char*));
  free(ptr);
  return ptr[3];
}