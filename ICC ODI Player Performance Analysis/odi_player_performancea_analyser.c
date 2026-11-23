#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "Players_data.h"

#define MAX_TEAMS 10
#define MAX_PLAYERS 50
#define MAX_ID 1000
#define MAX_NAME 50
#define ID_TABLE_SIZE 2000

typedef enum {
    BATSMAN = 0,
    BOWLER = 1,
    ALLROUNDER = 2
} Role;

static const char *Role_Name[3] = {"Batsman" , "Bowler" , "All-Rounder"};

typedef struct ODIPlayer{
    int playerID;
    char name[MAX_NAME + 1];
    char team_name[MAX_NAME + 1];
    Role role;
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;
    struct ODIPlayer *nextTeamPlayer; // list of all players of the team 
    struct ODIPlayer *nextRolePlayer; // role list, sorted by perfocmance_index
} ODIPlayer;

typedef struct {
    int teamId;
    char name[MAX_NAME + 1];
    int totalPlayers;
    float averageBattingStrikeRate;
    double totalBattingStrikeRate;
    int strikeRatePlayerCount;
    ODIPlayer *firstTeamPlayer, *lastTeamPlayer;
    ODIPlayer *firstPlayerByRole[3];
}Team;

static Team teamList[MAX_TEAMS];
static int numberOfTeams = 0;
static char playerIdExists[ID_TABLE_SIZE];

static float computePerformanceIndex(Role playerRole , float battingAverage , float strikeRate, int wickets ,float ecomonyRate){
    switch (playerRole)
    {
    case BATSMAN: 
        return (battingAverage * strikeRate) / 100.0f;
    
    case BOWLER:
        return (wickets * 2.0f ) + (100.0f - ecomonyRate);

    case ALLROUNDER:
        return ((battingAverage *strikeRate)/100.f) + (wickets * 2.0f);
    }
    return 0.0f;
}

static int convertStringToRole(const char *role, Role *outputValue){
    if(strcmp(role, "Batsman") == 0){
        *outputValue = BATSMAN;
        return 1;
    }
    if(strcmp(role, "Bowler") == 0){
        *outputValue = BOWLER;
        return 1;
    }
    if (strcmp(role, "All-Rounder") == 0 || strcmp(role, "All-rounder") == 0)
    {
        *outputValue = ALLROUNDER;
        return 1;
    }
    return 0; 
}

static Team *searchTeamById(int teamId){
    int low = 0 , high = numberOfTeams -1;
    while(low <= high){
        int middle = low + (high - low) / 2;
        if(teamList[middle].teamId == teamId) {
            return  &teamList[middle];
        }
        if(teamList[middle].teamId < teamId){
            low = middle + 1 ; 
        }
        else{
            high = middle - 1;
        }
    }
    return NULL;
}

static Team *searchTeamByName(const char *team_name){
    for(int index = 0 ; index < numberOfTeams ; index++){
        if(strcmp(teamList[index].name,team_name) == 0){
            return &teamList[index];
        }
    }
    return NULL;
}

static void upadateAverageBattingStrikeRate(Team *teamRecord){
    if(teamRecord -> strikeRatePlayerCount > 0){
        teamRecord -> averageBattingStrikeRate = (float) (teamRecord->totalBattingStrikeRate / teamRecord -> strikeRatePlayerCount);
    }
    else{
        teamRecord -> averageBattingStrikeRate = 0.0f;
    }
}

static void insertPlayerIntoTeam(Team *teamRecord , ODIPlayer * player){
    //all players list
    player ->nextTeamPlayer = NULL;
    if(teamRecord -> lastTeamPlayer){
        teamRecord ->lastTeamPlayer->nextTeamPlayer = player;
    }
    else{
        teamRecord -> firstTeamPlayer = player;
    }
    teamRecord ->lastTeamPlayer  = player;

    //role list : insert keeping descending performace index
    ODIPlayer **insertionPosition = &teamRecord -> firstPlayerByRole[player->role];
    while(*insertionPosition && (*insertionPosition) ->performanceIndex >= player -> performanceIndex){
        insertionPosition = &(*insertionPosition) ->nextRolePlayer;
    }
    player ->nextRolePlayer = *insertionPosition;
    *insertionPosition = player;

    teamRecord -> totalPlayers++;
    if(player -> role == BATSMAN || player-> role == ALLROUNDER){
        teamRecord -> totalBattingStrikeRate += player->strikeRate;
        teamRecord -> strikeRatePlayerCount++;
        upadateAverageBattingStrikeRate(teamRecord);
    }
}

static ODIPlayer *createPlayerRecord(int playerID, const char* playerName, const char* team_name, Role playerRole, int totalRuns, float battingAverage, float strikeRate, int wickets, float economyRate){
    ODIPlayer *player = (ODIPlayer *) calloc (1, sizeof(ODIPlayer));
    if(!player){
        fprintf(stderr,"Out of memory\n");
        exit(1);
    }
    player->playerID = playerID;
    strncpy(player->name ,playerName , MAX_NAME);
    strncpy(player->team_name,team_name, MAX_NAME);
    player->role = playerRole;
    player->totalRuns = totalRuns;
    player->battingAverage = battingAverage;
    player->strikeRate = strikeRate;
    player->wickets = wickets;
    player->economyRate = economyRate;
    player->performanceIndex = computePerformanceIndex(playerRole, battingAverage, strikeRate, wickets, economyRate);
    return player;
}

//initialization from Players_data.h

static void initializePlayerAndTeamData(){
    numberOfTeams = teamCount > MAX_TEAMS ? MAX_TEAMS : teamCount;
    for(int index = 0 ; index < numberOfTeams ; index++){
        memset(&teamList[index] , 0, sizeof(Team));
        teamList[index].teamId = index + 1;
        strncpy(teamList[index].name , teams[index] , MAX_NAME);
    }
    for(int index = 0 ;index < playerCount ;index++){
        const Player *sourcePlayer = &players[index];
        Role playerRole;
        Team *teamRecord = searchTeamByName(sourcePlayer ->team);
        if(!teamRecord || !convertStringToRole(sourcePlayer->role, &playerRole)){
            printf("Warning : skipped invalid recoed for '%s'\n" , sourcePlayer -> name);
            continue;
        }
        if(sourcePlayer -> id< 1 || sourcePlayer-> id >= ID_TABLE_SIZE || playerIdExists[sourcePlayer->id]){
            printf("Warning: skipped '%s' (invalid/duplicate id) %d \n" , sourcePlayer-> name , sourcePlayer->id);
            continue;
        }
        playerIdExists[sourcePlayer->id] = 1;
        ODIPlayer *player = createPlayerRecord(sourcePlayer->id, sourcePlayer->name, sourcePlayer->team, playerRole,sourcePlayer->totalRuns, sourcePlayer->battingAverage,
                                    sourcePlayer->strikeRate, sourcePlayer->wickets,sourcePlayer->economyRate);
        insertPlayerIntoTeam(teamRecord,player);
    }
}

static void printSeparatorLine(char c, int elementCount){
    for(int index = 0 ;index  < elementCount ; index++){
        putchar(c);
    }
    putchar('\n');
}

static void printPlayerTableHeader(int withTeam){
    if(withTeam){
        printSeparatorLine('=', 110);
        printf("%-5s %-24s %-14s %-12s %-7s %-6s %-6s %-5s %-5s %-10s\n",
               "ID", "Name", "Team", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
        printSeparatorLine('=', 110);
    }
    else {
        printSeparatorLine('=', 96);
        printf("%-5s %-24s %-12s %-7s %-6s %-6s %-5s %-5s %-10s\n",
               "ID", "Name", "Role", "Runs", "Avg", "SR",
               "Wkts", "ER", "Perf.Index");
        printSeparatorLine('=', 96);
    }
}

static void printPlayerDetails(const ODIPlayer *player, int withTeam)
{
    if (withTeam)
        printf("%-5d %-24s %-14s %-12s %-7d %-6.1f %-6.1f %-5d %-5.1f %-10.2f\n",
               player->playerID, player->name, player->team_name, Role_Name[player->role],
               player->totalRuns, player->battingAverage, player->strikeRate,
               player->wickets, player->economyRate, player->performanceIndex);
    else
        printf("%-5d %-24s %-12s %-7d %-6.1f %-6.1f %-5d %-5.1f %-10.2f\n",
               player->playerID, player->name, Role_Name[player->role],
               player->totalRuns, player->battingAverage, player->strikeRate,
               player->wickets, player->economyRate, player->performanceIndex);
}

static int readIntegerInput(const char* prompt , int *parsedValue){
    char inputBuffer[128];
    char* remainingInput;
    printf("%s" , prompt);
    if(!fgets(inputBuffer , sizeof inputBuffer , stdin)) return 0;
    long inputValue = strtol(inputBuffer,&remainingInput , 10);
    if(remainingInput == inputBuffer) {
        return 0;
    }
    while(*remainingInput != '\n' && *remainingInput != '\0'){
        return 0;
    }
    *parsedValue = (int)inputValue;
    return 1;
}

static int readFloatInput(const char *prompt, float *parsedValue)
{
    char inputBuffer[128];
    char *remainingInput;
    printf("%s", prompt);
    if (!fgets(inputBuffer, sizeof inputBuffer, stdin)) 
    {
        return 0;
    }
    float inputValue = strtof(inputBuffer, &remainingInput);
    if (remainingInput == inputBuffer) return 0;
    while (*remainingInput == ' ' || *remainingInput == '\t') 
    {
        remainingInput++;
    }
    if (*remainingInput != '\n' && *remainingInput != '\0') return 0;
    *parsedValue = inputValue;
    return 1;
}
static int readPlayerRole(Role *parsedValue)
{
    int selectedRoleNumber;
    if (!readIntegerInput("Role (1-Batsman, 2-Bowler, 3-All-rounder): ", &selectedRoleNumber)
        || selectedRoleNumber < 1 || selectedRoleNumber > 3) {
        printf("Invalid role.\n");
        return 0;
    }
    *parsedValue = (Role)(selectedRoleNumber - 1);
    return 1;
}

static void addNewPlayer(){
    int teamId;
    if(!readIntegerInput("Enter Team ID to add player: " , &teamId)){
        printf("Invalid input.\n");
        return;
    }
    Team *teamrecord = searchTeamById(teamId);
    if(!teamrecord){
        printf("Team ID %d not found.\n" ,teamId);
        return;
    }
    if(teamrecord ->totalPlayers >= MAX_PLAYERS){
        printf("Team %s already has maximum %d players.\n", teamrecord->name,MAX_PLAYERS);
        return;
    }
    printf("Enter Player Details:\n");
    int newPlayerId , totalRuns , wickets;
    float battingAverage , strikeRate, economyRate;
    char newPlayerName[256];
    Role newPlayerRole;
    if(!readIntegerInput("Player ID : ", &newPlayerId) || newPlayerId < 1 || newPlayerId > MAX_ID){
        printf("Invalid Player ID (must be 1-%d).\n",MAX_ID);
        return;
    }
    if (playerIdExists[newPlayerId]) { printf("Player ID %d already exists.\n", newPlayerId); return; }

    printf("Name: ");
    if (!fgets(newPlayerName, sizeof newPlayerName, stdin)) 
    {
        return;
    }
    newPlayerName[strcspn(newPlayerName, "\r\n")] = '\0';
    size_t nameLength = strlen(newPlayerName);
    if (nameLength < 1 || nameLength > MAX_NAME) {
        printf("Name length must be 1-%d characters.\n", MAX_NAME); return;
    }

    if (!readPlayerRole(&newPlayerRole)) return;
    if (!readIntegerInput("Total Runs: ", &totalRuns) || totalRuns < 0) {
        printf("Invalid runs.\n"); 
        return;
    }
    if (!readFloatInput("Batting Average: ", &battingAverage) || battingAverage < 0) {
        printf("Invalid batting average.\n"); 
        return;
    }
    if (!readFloatInput("Strike Rate: ", &strikeRate) || strikeRate < 0) {
        printf("Invalid strike rate.\n"); 
        return;
    }
    if (!readIntegerInput("Wickets: ", &wickets) || wickets < 0) {
        printf("Invalid wickets.\n"); 
        return;
    }
    if (!readFloatInput("Economy Rate: ", &economyRate) || economyRate < 0) {
        printf("Invalid economy rate.\n"); 
        return;
    }

    playerIdExists[newPlayerId] = 1;
    ODIPlayer *player = createPlayerRecord(newPlayerId, newPlayerName, teamrecord->name, newPlayerRole, totalRuns, battingAverage, strikeRate, wickets, economyRate);
    insertPlayerIntoTeam(teamrecord, player);
    printf("Player added successfully to Team %s!\n", teamrecord->name);
}

static void displayPlayersOfTeam(void)
{
    int teamId;
    if (!readIntegerInput("Enter Team ID: ", &teamId)) { 
        printf("Invalid input.\n"); 
        return; 
    }
    Team *teamRecord = searchTeamById(teamId);
    if (!teamRecord) { 
        printf("Team ID %d not found.\n", teamId); 
        return; 
    }

    printf("Players of Team %s:\n", teamRecord->name);
    printPlayerTableHeader(0);
    for (ODIPlayer *player = teamRecord->firstTeamPlayer; player; player = player->nextTeamPlayer){
            printPlayerDetails(player, 0);
    }
    printSeparatorLine('=', 96);
    printf("Total Players: %d\n", teamRecord->totalPlayers);
    printf("Average Batting Strike Rate: %.2f\n", teamRecord->averageBattingStrikeRate);
}

static int compareTeamsByAverageStrikeRate(const void *a, const void *b)
{
    const Team *firstTeam = *(const Team *const *)a;
    const Team *secondTeam = *(const Team *const *)b;
    if (firstTeam->averageBattingStrikeRate < secondTeam->averageBattingStrikeRate) 
    {
        return 1;
    }
    if (firstTeam->averageBattingStrikeRate > secondTeam->averageBattingStrikeRate) {
        return -1;
    }
    return firstTeam->teamId - secondTeam->teamId;
}

static void displayTeamsByAverageStrikeRate(void)
{
    Team *sortedTeamRecords[MAX_TEAMS];
    for (int index = 0; index < numberOfTeams; index++) {
        sortedTeamRecords[index] = &teamList[index];
    }
    qsort(sortedTeamRecords, numberOfTeams, sizeof(Team *), compareTeamsByAverageStrikeRate);

    printf("Teams Sorted by Average Batting Strike Rate\n");
    printSeparatorLine('=', 57);
    printf("%-4s %-18s %-14s %-12s\n", "ID", "Team Name", "Avg Bat SR", "Total Players");
    printSeparatorLine('=', 57);
    for (int index = 0; index < numberOfTeams; index++){
        printf("%-4d %-18s %-14.2f %-12d\n", sortedTeamRecords[index]->teamId, sortedTeamRecords[index]->name,
               sortedTeamRecords[index]->averageBattingStrikeRate, sortedTeamRecords[index]->totalPlayers);
    }
    printSeparatorLine('=', 57);
}

static void displayTopKPlayers(void)
{
    int teamId, numberOfTopPlayers;
    Role role;
    if (!readIntegerInput("Enter Team ID: ", &teamId)) { 
        printf("Invalid input.\n"); 
        return; 
    }
    Team *teamRecord = searchTeamById(teamId);
    if (!teamRecord) { 
        printf("Team ID %d not found.\n", teamId); 
        return; 
    }
    if (!readPlayerRole(&role)) return;
    if (!readIntegerInput("Enter number of players: ", &numberOfTopPlayers) || numberOfTopPlayers < 1) {
        printf("Invalid number.\n"); return;
    }

    printf("Top %d %s(s) of Team %s:\n", numberOfTopPlayers, Role_Name[role], teamRecord->name);
    printPlayerTableHeader(0);
    int displayedPlayerCount = 0;
    ODIPlayer *player = teamRecord->firstPlayerByRole[role];

    for (int i = 0; player && i < numberOfTopPlayers; i++, player = player->nextRolePlayer) {
        printPlayerDetails(player, 0);
        displayedPlayerCount++;
    }
    printSeparatorLine('=', 96);
    if (displayedPlayerCount < numberOfTopPlayers){
        printf("Only %d %s(s) available in this team.\n", displayedPlayerCount, Role_Name[role]);
    }
}

static void restoreMaxHeapOrder(ODIPlayer **playerHeap, int elementCount, int index)
{
    while (2 * index + 1 < elementCount) {
        int leftChildIndex = 2 * index + 1;
        int rightChildIndex = leftChildIndex + 1;
        int largestIndex = index;

        if (playerHeap[leftChildIndex]->performanceIndex > playerHeap[largestIndex]->performanceIndex) {
            largestIndex = leftChildIndex;
        }
        if (rightChildIndex < elementCount && playerHeap[rightChildIndex]->performanceIndex > playerHeap[largestIndex]->performanceIndex) {
            largestIndex = rightChildIndex;
        }
        if (largestIndex == index) {
            break;
        }

        ODIPlayer *temporaryPlayer = playerHeap[index];
        playerHeap[index] = playerHeap[largestIndex];
        playerHeap[largestIndex] = temporaryPlayer;

        index = largestIndex;
    }
}

static void displayPlayersByRoleAcrossTeams(void)
{
    Role role;
    if (!readPlayerRole(&role)) return;

    ODIPlayer *playerHeap[MAX_TEAMS];
    int elementCount = 0;
    for (int index = 0; index < numberOfTeams; index++){
        if (teamList[index].firstPlayerByRole[role]) {
            playerHeap[elementCount++] = teamList[index].firstPlayerByRole[role];
        }
    }
        
    for (int index = elementCount / 2 - 1; index >= 0; index--) {
        restoreMaxHeapOrder(playerHeap, elementCount, index);
    }

    printf("%s(s) of all teams:\n", Role_Name[role]);
    printPlayerTableHeader(1);
    int totalPlayerCount = 0;
    while (elementCount > 0) {
        ODIPlayer *top = playerHeap[0];
        printPlayerDetails(top, 1);
        totalPlayerCount++;
        if (top->nextRolePlayer) {
            playerHeap[0] = top->nextRolePlayer; //replace with next
        }  
        else{
            playerHeap[0] = playerHeap[--elementCount];       // list exhausted 
        }
        restoreMaxHeapOrder(playerHeap, elementCount, 0);
    }
    printSeparatorLine('=', 110);
    printf("Total %s(s): %d\n", Role_Name[role], totalPlayerCount);
}


static void freeAllPlayerRecords(void)
{
    for (int index = 0; index < numberOfTeams; index++) {
        ODIPlayer *player = teamList[index].firstTeamPlayer;
        while (player) { ODIPlayer *nextPlayer = player->nextTeamPlayer; free(player); player = nextPlayer; }
    }
}


int main(void)
{
    initializePlayerAndTeamData();

    int menuChoice;
    for (;;) {
        printf("\n");
        printSeparatorLine('=', 78);
        printf("ICC ODI Player Performance Analyzer\n");
        printSeparatorLine('=', 78);
        printf("1. Add Player to Team\n");
        printf("2. Display Players of a Specific Team\n");
        printf("3. Display Teams by Average Batting Strike Rate\n");
        printf("4. Display Top K Players of a Specific Team by Role\n");
        printf("5. Display all Players of specific role Across All Teams by performance index\n");
        printf("6. Exit\n");
        printSeparatorLine('=', 78);

        if (!readIntegerInput("Enter your choice: ", &menuChoice)) {
            if (feof(stdin)) break;
            printf("Invalid choice. Try again.\n");
            continue;
        }
        printf("\n");
        switch (menuChoice) {
            case 1: addNewPlayer();            break;
            case 2: displayPlayersOfTeam();   break;
            case 3: displayTeamsByAverageStrikeRate();     break;
            case 4: displayTopKPlayers();          break;
            case 5: displayPlayersByRoleAcrossTeams(); break;
            case 6: freeAllPlayerRecords(); printf("Exiting. Goodbye!\n"); return 0;
            default: printf("Invalid choice. Try again.\n");
        }
    }
    freeAllPlayerRecords();
    return 0;
}
