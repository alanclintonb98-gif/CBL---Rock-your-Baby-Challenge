//Skeleton code algorithm
#include <stdio.h>
#include <unistd.h>
int matrix[5][5] = {{1,2,3,4,5},
                    {6,7,8,9,10},
                    {11,12,13,14,15},
                    {16,17,18,19,20},
                    {21,22,23,24,25}}; // there's an array for every row

void goLeft(int currentMatrixPosition[]){ 
     if (currentMatrixPosition[1] > 0) {
        currentMatrixPosition[1] -= 1; 
    } else {
     printf("already at left the edge of the matrix.\n");
    }}
void goRight(int currentMatrixPosition[]) { 
    if (currentMatrixPosition[1] < 4) {
        currentMatrixPosition[1] += 1;
    }
}
 
void goUp(int currentMatrixPosition[]) { 
     if (currentMatrixPosition[0] > 0) {
        currentMatrixPosition[0] -= 1; 
    } else {
     printf("already at the top edge of the matrix.\n");
    }
 }
 
int Data [2] = {0, 0}; //     Heartbeat , crying

int getStresslevel(float heartbeat, float crying) { 
     return 0; //?????????????
 }

 //---------------------------------------------main------------------------------------------
void main(){
int stresslevel = 9;
int currentMatrixPosition[2] = {4,4};
int dontGoLeft[25] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  //if dontGoLeft[i] == 0, then the algorithm will go left at 
do{
    if (dontGoLeft[currentMatrixPosition[0]][currentMatrixPosition[1]] == 0){
          goLeft(currentMatrixPosition);
     }
     else {
         goRight(currentMatrixPosition);
     }
     sleep(5);
     if (getStresslevel(Data[0], Data[1]) >= stresslevel) {
            
            dontGoLeft[currentMatrixPosition[0]][currentMatrixPosition[1]] = 1;
        }
     stresslevel = getStresslevel(Data[0], Data[1]);
} while ( stresslevel != 1);

}
