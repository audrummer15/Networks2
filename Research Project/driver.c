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
  FILE *fpthgpt, *output;
  double *thgpt;
  double *sumThgpt;
  double *hsThgpt;
  double *hsSumThgpt;
  double *sumFairnessReno;
  double *sumFairnessHighSpeed;

  
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
	  double renoFairness_bottom =0, highSpeedFairness_bottom =0, renoSum=0, highSpeedSum =0, renoAverage =0, highSpeedAverage =0;

          thgpt = (double*)malloc( sizeof(double) * nodeValue);
          sumThgpt = (double*)malloc( sizeof(double) * nodeValue);
          hsThgpt = (double*)malloc( sizeof(double) * nodeValue);
          hsSumThgpt = (double*)malloc( sizeof(double) * nodeValue);
          sumFairnessReno = (double*)malloc( sizeof(double) * nodeValue);
          sumFairnessHighSpeed = (double*)malloc( sizeof(double) * nodeValue);

          for( i=0; i < nodeValue; i++ ) {
            thgpt[i] = 0;
            sumThgpt[i] = 0;
            hsThgpt[i] = 0;
            hsSumThgpt[i] = 0;
            sumFairnessReno[i] = 0;
            sumFairnessHighSpeed[i] =0;
          }

          printf("\n\n >>>> Experiment %d <<<<\n", ++EXPERIMENT_COUNTER);
          for( i=0; i<NB_SIMULATIONS; i++ ) {
	    printf("--Sim:%d --\n",i);
            int randValue = rand() % (PROPAGATION_DEVIATION * propagationValue);
            //Reno Run
            sprintf(ns_command,"ns reno.tcl %ld %d %d %d %d\n",
                    bandwidthValue, propagationValue, delayValue, nodeValue, randValue);
            printf("%s", ns_command);
            system(ns_command);

            fpthgpt = fopen("thgt.tr","r");

            if (!fpthgpt)
              error("cannot open thgpt\n");
            
	    printf("Node throughput: [");
            int j=0;
            while( fscanf(fpthgpt, "%lf", &thgpt[j]) == 1 ) {
              sumThgpt[j] += thgpt[j];
              printf("(%d = %6.2lf )", j, thgpt[j]);
              j++;
            }
            
	    fclose(fpthgpt);

            printf("]\n");

            // High Speed Run
            sprintf(ns_command,"ns hstcp.tcl %ld %d %d %d %d\n",
                    bandwidthValue, propagationValue, delayValue, nodeValue, randValue);
            printf("%s", ns_command);
            system(ns_command);

            fpthgpt = fopen("hsthgt.tr","r");

            if (!fpthgpt)
              error("cannot open hsthgpt\n");
            
	    printf("Node Throughput: [");
            j=0;
            while( fscanf(fpthgpt, "%lf", &hsThgpt[j]) == 1 ) {
              hsSumThgpt[j] += hsThgpt[j];
              printf("(%d = %6.2lf )", j, hsThgpt[j]);
              j++;
            }

	    fclose(fpthgpt);		
          }
	
          printf("]\n\n >>>> Averages per node <<<<\n");
          for( i=0; i < nodeValue; i++ ) {
            printf("Avg Thgt%d = %6.2lf (Kbps)\n", i, sumThgpt[i] / 1000.0 / NB_SIMULATIONS);
            printf("HS Avg Thgt%d = %6.2lf (Kbps)\n", i, hsSumThgpt[i] / 1000.0 / NB_SIMULATIONS);
          }

	  //calculates sum
          for( i=0; i < nodeValue; i++ ) {
            renoSum += sumThgpt[i]/ NB_SIMULATIONS;
            highSpeedSum += hsSumThgpt[i]/ NB_SIMULATIONS;
            renoFairness_bottom += (sumThgpt[i]/ NB_SIMULATIONS) * (sumThgpt[i]/ NB_SIMULATIONS);
            highSpeedFairness_bottom += (hsSumThgpt[i]/ NB_SIMULATIONS) * (hsSumThgpt[i]/ NB_SIMULATIONS);
          }
	
	  renoAverage = renoSum / 1000.0  / nodeValue;
          highSpeedAverage = highSpeedSum / 1000.0  / nodeValue;
	
	  //output info
	  //printf("Fairness Sum: \n Reno: %6.2lf  HS: %6.2lf  \n Fariness Top:  \n Reno: %6.2lf   HS: %6.2lf \n Fairness Bottom \n Reno: %6.2lf  HS: %6.2lf",renoSum, highSpeedSum, renoSum * renoSum, highSpeedSum * highSpeedSum,  nodeValue * renoFairness_bottom, nodeValue* highSpeedFairness_bottom);

	  printf("\n\n >>>>> Overall Stats <<<<< \n");
	  printf("Average Throughput Reno: %6.2lf \n", renoAverage);
          printf("Average Throughput High Speed: %6.2lf \n", highSpeedAverage);
          printf("Fairness Reno: %6.2lf \n", (renoSum * renoSum) / (nodeValue * renoFairness_bottom) );
          printf("Fairness High Speed: %6.2lf \n", (highSpeedSum * highSpeedSum) / (nodeValue * highSpeedFairness_bottom));
          
	  // Write to the output file

       	  output = fopen("output.txt","a+");

  	  if (!output)
    	    error("Error Opening output file 'output.txt'\n");

	  fprintf(output,"%d, %ld, %d, %d, %d,%6.2lf,%6.2lf,%6.2lf,%6.2lf, \n", EXPERIMENT_COUNTER, bandwidthValue, propagationValue, delayValue, nodeValue, renoSum, highSpeedSum,(renoSum * renoSum) / (nodeValue * renoFairness_bottom), (highSpeedSum * highSpeedSum) / (nodeValue * highSpeedFairness_bottom));
 	  

	  fclose(output);
          free(thgpt);
          free(sumThgpt);
          free(hsThgpt);
	  free(hsSumThgpt);
	  free(sumFairnessReno);
	  free(sumFairnessHighSpeed);
        }
      }
    }
  }
}




void error(char *str){
  printf("%s",str);
  exit(1);
}
