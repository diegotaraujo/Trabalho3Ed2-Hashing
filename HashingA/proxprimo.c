#include <stdio.h> 

int main (void) { 

	int i, tam, flag = 0, c=0; 
  
	scanf ("%d", &tam);
	getchar();

	if(tam <= 2){
		printf("%d\n", 2);
		return 2;
	}
  	
	while(c<100){
 	
 		flag = 0;
		for (i = 2 ; i < tam; i++) 
		   	if (tam % i == 0){ 
		   		flag = 1; 
		   	}

		if (!flag){
			printf("%d\n", tam);
			return tam;
		}

		tam++;
		c++;
	}

  return 0; 
} 