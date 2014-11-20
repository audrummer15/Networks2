#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NB_SIMULATIONS 1
#define NB_BANDWIDTH_VALUES 8
#define NB_DELAY_TIMES 2
#define NB_PROPOGATION_TIMES 4
#define NB_NODE_VALUES 4

#define PROPAGATION_DEVIATION 12

void error(char *str);

void main (int argc, char **argv){
  FILE *fpthgpt;
  double *thgpt;
  double *sumThgpt;
  double *hsThgpt;
  double *hsSumThgpt;
  
  long   BandwidthValues[NB_BANDWIDTH_VALUES] = 
  {1, 56, 128, 256, 512, 1024, 10240, 102400}; /*in bps*/
//{1kbps, 56kbps, 128kbps, 256kbps, 512kbps, 1Mbps, 10Mbps, 100Mbps};

  int   PropagationTimes[NB_PROPOGATION_TIMES] = 
  {50, 250, 500, 1000}; /* in milliseconds */

  int   DelayTimes[NB_DELAY_TIMES] = 
  {0, 100}; /* in milliseconds */

  int   NodeValues[NB_NODE_VALUES] =
  {2, 8, 16, 32};

  int BANDWIDTH_COUNTER, PROPOGATION_COUNTER, DELAY_COUNTER, NODE_COUNTER, EXPERIMENT_COUNTER=0;
  int i;
  char ns_command[512];

  srand(time(NULL));

  for(BANDWIDTH_COUNTER = 0; BANDWIDTH_COUNTER < NB_BANDWIDTH_VALUES; BANDWIDTH_COUNTER++ ) {
    long bandwidthValue = BandwidthValues[BANDWIDTH_COUNTER]; // for readability

    for(PROPOGATION_COUNTER = 0; PROPOGATION_COUNTER < NB_PROPOGATION_TIMES; PROPOGATION_COUNTER++ ) {
      int propagationValue = PropagationTimes[PROPOGATION_COUNTER];

      for(DELAY_COUNTER = 0; DELAY_COUNTER < NB_DELAY_TIMES; DELAY_COUNTER++ ) {
        int delayValue = DelayTimes[DELAY_COUNTER];

        for( NODE_COUNTER = 0; NODE_COUNTER < NB_NODE_VALUES; NODE_COUNTER++ ) {
          int nodeValue = NodeValues[NODE_COUNTER];

          thgpt = (double*)malloc( sizeof(double) * nodeValue);
          sumThgpt = (double*)malloc( sizeof(double) * nodeValue);
          hsThgpt = (double*)malloc( sizeof(double) * nodeValue);
          hsSumThgpt = (double*)malloc( sizeof(double) * nodeValue);

          for( i=0; i < nodeValue; i++ ) {
            thgpt[i] = 0;
            sumThgpt[i] = 0;
            hsThgpt[i] = 0;
            hsSumThgpt[i] = 0;
          }

          printf("\n\n >>>> Experiment %d <<<<\n", ++EXPERIMENT_COUNTER);
          for( i=0; i<NB_SIMULATIONS; i++ ) {

            int randValue = rand() % (PROPAGATION_DEVIATION * propagationValue);
            //Reno Run
            sprintf(ns_command,"ns reno.tcl %ld %d %d %d %d\n",
                    bandwidthValue, propagationValue, delayValue, nodeValue, randValue);
            printf("%s", ns_command);
            system(ns_command);

            fpthgpt = fopen("thgt.tr","r");

            if (!fpthgpt)
              error("cannot open thgpt\n");
            
            int j=0;
            while( fscanf(fpthgpt, "%lf", &thgpt[j]) == 1 ) {
              sumThgpt[j] += thgpt[j];
              printf("thgt%d = %6.2lf ", j, thgpt[j]);
              j++;
            }

            // High Speed Run
            sprintf(ns_command,"ns hstcp.tcl %ld %d %d %d %d\n",
                    bandwidthValue, propagationValue, delayValue, nodeValue, randValue);
            printf("%s", ns_command);
            system(ns_command);

            fpthgpt = fopen("hsthgt.tr","r");

            if (!fpthgpt)
              error("cannot open hsthgpt\n");
            
            j=0;
            while( fscanf(fpthgpt, "%lf", &hsThgpt[j]) == 1 ) {
              hsSumThgpt[j] += hsThgpt[j];
              printf("thgt%d = %6.2lf ", j, hsThgpt[j]);
              j++;
            }
          }

          printf("\n\n >>>> Averages <<<<\n");
          for( i=0; i < nodeValue; i++ ) {
            printf("Avg Thgt%d = %6.2lf (Kbps)\n", i, sumThgpt[i] / 1000.0 / NB_SIMULATIONS);
            printf("HS Avg Thgt%d = %6.2lf (Kbps)\n", i, hsSumThgpt[i] / 1000.0 / NB_SIMULATIONS);
          }

          free(thgpt);
          free(sumThgpt);
        }
      }
    }
  }
}


void error(char *str){
  printf("%s",str);
  exit(1);
}