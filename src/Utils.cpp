#include "Utils.h"

int rand_ps()
{
  int n = rand() % 100;
  n = n * n * n;
  return n / 100;
}
