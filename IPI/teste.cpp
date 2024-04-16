#include <stdio.h>

int main() {
    int L, C, X, Y;
    scanf("%d %d %d %d", &L, &C, &X, &Y);
    int position = (X * C) + Y; 
    if (position % 2 == 0) printf("Direita\n");
    else printf("Esquerda\n");   
    return 0;
}
