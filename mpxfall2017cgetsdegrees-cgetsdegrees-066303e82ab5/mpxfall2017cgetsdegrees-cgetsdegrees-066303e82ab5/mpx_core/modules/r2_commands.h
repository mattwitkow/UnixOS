#ifndef R2_COMMANDS_H
#define R2_COMMANDS_H


typedef struct PCB{
	char processName[20];
	int processClass; //0 for app, 1 for system process
	int priority;
	int state;	//state variable  0 = running, 1 = ready, 2 = blocked
	int isSuspended;	  //suspension variable. 1 = suspended, 0 = not suspended
	unsigned char* stack;
	unsigned char* stackTop;
	unsigned char* stackBase;
	struct PCB* next;
	struct PCB* prev;
}PCB;

typedef struct Queue{
	int count;
	PCB* head;
	PCB* tail;
}Queue;

struct Queue* ready;
struct Queue* blocked;


//begin internal functions
PCB* DQReady();
int MoveToFront(char* processName);
PCB* RemoveReturnPCB(PCB* pcbToRemove);
void BlockAllReady();
void UnblockAllReady();
void PutBackToReady(PCB* node);
//void ShowReadyWithPointers();
/**
 * @brief Makes space in memory for a PCB
 */
PCB* AllocatePCB();
/**
 *  @brief Frees a PCB from memory. Returns 1 if successful, returns 0 and prints error code if unsuccessful
 */
int FreePCB(PCB* pcbToFree); 
/**
 * @brief Initializes the properties of a PCB and returns a pointer to it
 */
PCB* SetupPCB(char* processName, int processClass, int processPriority);
/**
 * @brief Locates a PCB and returns a pointer to it
 */
PCB* FindPCB(char* processName);
/**
 * @brief Inserts a PCB into the appropriate queue
 */
void InsertPCB(PCB* pcbToInsert);
/**
 * @brief Remove a queued PCB. Return 1 if successful
 */
int RemovePCB(PCB* pcbToRemove);
//cmd funcs
/**
 * @brief Initializes the properties of a PCB. Returns 1 if successful
 */
int CreatePCB(char* processName, int processClass, int processPriority); 
/**
 * @brief Deletes a PCB. Returns 1 if successful
 */
int DeletePCB(char* processName);
/**
 * @brief Blocks a PCB. Returns 1 if PCB is successfully blocked and reinserted to the appropriate queue
 */
int BlockPCB(char* processName);
/**
 * @brief Unblocks a PCB. Returns 1 if PCB is successfully unblocked and reinserted to the appropriate queue
 */
int UnblockPCB(char* processName);
/**
 * @brief Suspends a PCB. Returns 1 if PCB is successfully placed in the suspended state and reinserted into the appropriate queue
 */
int SuspendPCB(char* processName);
/**
 * @brief Resumes a PCB. Returns 1 if PCB is successfuly placed in the not suspended state and reinserted into the appropriate queue
 */
int ResumePCB(char* processName);
/**
 * @brief Sets a PCB's priority. Returns 1 if PCB's priority is successfully set and the PCB is reinserted into the correct place in the correct queue
 */
int SetPCBPriority(char* processName, int newPriority);
/**
 * @brief Prints PCB name, class, state, suspended status, and priority
 */
void ShowPCB(char* processName);
/**
 * @brief Prints process name, class, state, suspended status, and priority of all PCBs in the ready queue
 */
void ShowReady();
/**
 * @brief Prints process name, class, state, suspended status, and priority of all PCBs in the blocked queue
 */
void ShowBlocked();
/**
 * @brief Prints process name, class, state, suspended status, and priority of all PCBs in the ready and blocked queues
 */
void ShowAll();
//helper functions
/**
 * @brief Checks to see if a process name is valid. Returns 1 if valid
 */
int isValidProcessName(char* processName);

/**
* @brief initializes queues
*/
void initQs();

/**
* @brief takes user input to give to suspend
*/
int SuspendHelp();
/**
* @brief takes user input to give to resume
*/
int ResumeHelp();
/**
* @brief takes user input to give to set pcb priority
*/
int PriorityHelp();
/**
* @brief takes user input to give to show pcb
*/
void ShowHelp();
/**
* @brief takes user input to give to blockPCB
*/
int BlockHelp();
/**
* @brief takes user input to give to UnblockPCB
*/
int UnblockHelp();
/**
* @brief takes user input to give to delete PCB
*/
int DeleteHelp();
/**
* @brief takes user input to give to create PCB
*/
int CreateHelp();
#endif
