#include <stdio.h>

#define NB_EXPERIMENTS 11

void error(char *str);

void main (int argc, char **argv){
  FILE *fpthgpt;
  float thgt0, thgt1, sumthgt0, sumthgt1;
  int   StartTimes[NB_EXPERIMENTS]=
  {2, 10, 20, 100, 150, 200, 
   250, 300,380, 390, 398 }; /* in milliseconds */

  int Bw, Tp;

  int i;
  char ns_command[512];

  printf("argc=%d\n",argc);
  if (argc != 3) {
    error("usage : driver Bw Tp\n");
  }

  Bw = atoi(argv[1]);
  Tp = atoi(argv[2]);


  sumthgt0 = 0;
  sumthgt1 = 0;
  for (i=0; i < NB_EXPERIMENTS; i++) {
    printf("\n\n >>>> Experiment %d\n",i);
    sprintf(ns_command,"ns essais.tcl %d %d %d\n",
	    Bw, Tp, StartTimes[i]);
    system(ns_command);

    fpthgpt = fopen("thgt.tr","r");
    if (!fpthgpt)
      error("cannot open thgpt\n");
    if (fscanf(fpthgpt,"%f %f\n",&thgt0,&thgt1) != 2) {
      error("expecting two values\n");
    }
    sumthgt0 += thgt0;
    sumthgt1 += thgt1;
    printf("thgt0 = %6.2f     thgt1 = %6.2f\n", thgt0, thgt1);
    
  }
  
  printf("\n\n >>>> Agerages");
  printf("Avg Thgt0 = %6.2f (Kbps)    Avg Thgt1 = %6.2f (Kbps) \n\n  ",
	 thgt0 / 1000.0, thgt1 / 1000.0);
  
}


void error(char *str){
  printf("%s",str);
  exit(1);
}
