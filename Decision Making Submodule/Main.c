#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>


/* Details for this program - Version 1:

- We want to automatically find a pathway without knowing the K values of the matrix
- This skeleton program does not incorporate the communication protocol part of the project
- We set the different values for crying and heartbeat to internally check the working of our decision making logic.
- Program uses derivatives rather than only relying on the stress function value to make faster analyses of which pathway to choose.
- We also incorporate memory of past successful movements to be able to backtrack back to a point in case of panic block or panic jump.

Todo:

- Implement communication protocol into data for reciving and transmitting data (Alan)
- Stress analysis
    - implement function for updating variable values for the incoming values recieved from heartbeat and crying
    - implement function to determine stress level
    - implement function for checking if stress is stable
    - implement function for using derivative to calculate how much the stress is decreasing
    - implement function for stability reference (which updates automatically depending on how the stress function goes)
- Pathfinding and Actual Decision making
    - implement function for next decision to be carried out
    - record previously attempted decisions that were successful (for backlog)
    - Implement a function to check if one neighbouring cell works or not (say F-1) and if it doesn't revert back to previous cell and try A - 1
- Implement panic recovery (essentially just coming back to previous cell after panic using the backlog)


*/

// Below are the parameters to test on the baby (which is to be changed accordingly) to match with how the baby system works.

#define SAMPLE_INTERVAL       200 // time interval between samples in ms
#define MIN_RESPONSE         2000 // minimum wait time before evaluating a response
#define NO_CHANGE_WAIT       8000 // Minimum time delay required before concluding no change occured stress level in the system.
#define STABILITY_TIME       2000 // Time delay until we can definitively say measurements are stable
#define MAX_OBSERVATION     25000 // Max response time delay for making a decision (if system is not stable before that, we're doing something wrong)
#define PANIC_WAIT          10000 // Time delay for waiting uder panic block
#define MAX_RECOVERIES             3 // Maximum number of panic recovery attempts (to avoid us from being in an infinite loop if we fail to find the correct pathway)

#define BPM_CHANGE_THRESHOLD     2.0f // A minimum threshold for us to consider a change in stress level
#define BPM_STABILITY_THRESHOLD  1.0f // The maximum tolerated variation from our stability reference for us to consider the measurements still stable
#define BPM_MATCH_THRESHOLD      3.0f // Tolerance for matching previously recorded BPM values when attemping recovery from panic block or jump from K9
#define CRY_CHANGE_THRESHOLD     5.0f // A minimum threshold for us to consider a change in stress level for change in crying level

struct position {
    int frequency; // We are defining a structure with following characterisations instead of having to make new variables for each
    int amplitude;

};

struct pathstep { // Structure allows us to create of type struct pathstep which contains the BPM and crying level as well as the coodinates which are accessed from another structure (Position)
    struct position position; // We can access struct position's data
    float bpm; // to record what values of measurements were present for a position in the matrix which helps us in attemping recovery in case of panic block or jump
    float crying;
};

/* we use enumerations here as a way to keep track of what task we are doing as of now instead of using int state for values that correspond to different tasks*/

enum state {

    INITIALIZATION, // records state for getting values in the beginning from other sub groups at F5 and A5
    OBSERVATION, // state for any point where we're observing if change has led to change or not to help us make our decision through the matrix
    PANIC_RECOVERY, // If we've got panic block or jump and back at K9 and are recovering back to the last position in the matrix
    RETURN_PREVIOUS_STEP, // This state is to return back if change either by F-1 or A-1 leads to no change or worse stress, then we return back to the last position and try the other neighbouring position in the matrix (only positions considered are up and down)
    MAINTAIN, // State to maintain if we've reached K1 with F1 and A1
    FAULT // If something goes wrong in the algorithm and it continuosly doesn't work, instead of having an infinite loop of failed attempts, we can do something else.

};


struct controller {
    struct pathstep path[9]; // allows us to create an array of type struct pathstep which contains the characteristic variables of pathstep and position
    int depth; // a characteristic variable for how far along the correct path we are currently on
    int knownDepth; // How far along the correct path have we previously discovered (for backlog purposes)

    struct position command; // we are storing a characteristic variable of type struct position (storing F and A) and this variable is for the next command (or decision) to calm the baby


    bool freq_tried; // purpose of these variables is to remember which direction we have already attempted in our current matrix position (up or left)
    bool amp_tried;

    int recoveries; // to keep track of how many times we are going into panic block or jump and attempting recovery (this works as a prevention in case our program fails to find a correct path and is in a constant loop which is killed by MAX_RECOVERIES = 3)

    float current_bpm; // These two variables hold the current BPM and crying level
    float current_crying;

    float baseline_bpm; // These two variables are recorded just before changing the rocking motion (F or A) to be able to compare this with the new values to check if it lead to a decrease or not
    float baseline_crying;

    float stability_reference_bpm; // These two variables are a stability reference to see if a change in F or A is still changing BPM and crying or if it has become stable (which would allow us to further evaluate what to do next)
    float stability_reference_crying; // unlike baseline variables, stability reference should update if the values were to deviate from the baseline

    /* uint is an integer data type same as int but stores only non negative numbers*/

    uint last_rocking_change_time; // stores how long it's been since we changed F or A so that we can compare with MIN_RESPONSE and
    uint time_since_stability;

    bool initialized; // To check if we have recieved a first sample of measurements from the heartbeat and crying subgrou





};

bool SendMotorCommand(int frequency, int amplitude) {
    if ((frequency < 1 || frequency > 5) || (amplitude < 1 || amplitude > 5)) {
        return false;
    } else {
        printf("Motor command: F%d A%d", frequency, amplitude); // placeholder for actual communication protocol
    }
}




void main (void) {

    struct controller baby = {0}; // Initialising variable baby and the characteristic variables of the structures to 0.
    baby.depth = 0;
    baby.knownDepth = 0;





}
