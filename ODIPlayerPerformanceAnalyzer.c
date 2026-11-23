/*
    Created By: Ashmit Mathur
    Date: 23-11-2026
    Title: ODI Player Analyzer using Linked Lists
*/

#include <stdio.h>
#include <string.h>
#include "Players_data.h"

#define MAX_TEAMS 10
#define MAX_NAME_LEN 51
#define MAX_PLAYERS 50

typedef enum{
    BATSMAN_ROLE = 1,
    BOWLER_ROLE = 2,
    ALL_ROUNDER_ROLE = 3
} PlayerRole;

typedef struct OdiPlayer{
    int Id;
    char name[MAX_NAME_LEN];
    char teamName[MAX_NAME_LEN];
    PlayerRole role;
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;

    struct OdiPlayer *next, *roleNext;
} OdiPlayer;

typedef struct{
    int teamId;
    char name[MAX_NAME_LEN];
    int totalPlayers;
    float averageBattingStrikeRate;
    float battingStrikeRateSum;
    int batterAndAllRounderCount;

    OdiPlayer *allPlayersHead;
    OdiPlayer *batsmenHead;
    OdiPlayer *bowlersHead;
    OdiPlayer *allRoundersHead;
} Team;

const char *getRoleString(PlayerRole role);
PlayerRole parseRoleString(const char *roleStr);
float calculatePerformanceIndex(PlayerRole role, float avg, float sr, int wickets, float econ);
int findTeamIndexbyId(Team teamsarr[],int n,int  targetId);
int findTeamIndexByName(Team teamsarr[], int n, const char *name);
void insertSortedbyPerformance(OdiPlayer **headRef, OdiPlayer *newNode);
void appendtoRoster(OdiPlayer **headRef, OdiPlayer *newNode);
int isPlayerIdDuplicates(Team teamsarr[], int n, int playerId);
int addPlayertoTeam(Team *team, int playerId, const char *name, PlayerRole role, int runs, float avg, float sr, int wickets, float econ);
void initializeSystem(Team teamsarr[]);
void handleAddPlayer(Team teamsarr[], int numTeams);
void handleDisplayTeamPlayers(Team teamsarr[], int numTeams);
int compareTeamsbyStrikeRate(const void *a,const void *b);
void handleDisplayTeamsSortedbySR(Team teamsarr[], int numTeams);
void handleTopKPlayersByRole(Team teamsarr[], int numTeams);
void maxHeapify(OdiPlayer *heap[], int size, int idx);
void handleDisplayAllPlayersbyRole(Team teamsarr[], int numTeams);
void cleanupMemory(Team teamsarr[], int numTeams);
void displayMenu();

// String Representation for given Role
const char *getRoleString(PlayerRole role){
    switch(role){
        case BATSMAN_ROLE: return "Batsman";
        case BOWLER_ROLE: return "Bowler";
        case ALL_ROUNDER_ROLE: return "All-rounder";
        default: return "Unknown";
    }
}

// Converts role string from header data into enum PlayerRole
PlayerRole parseRoleString(const char *roleStr){
    if(strcmp(roleStr, "Batsman") == 0) return BATSMAN_ROLE;
    if(strcmp(roleStr, "Bowler") == 0) return BOWLER_ROLE;
    return ALL_ROUNDER_ROLE;
}

// Calculates player's performance index
float calculatePerformanceIndex(PlayerRole role, float avg, float sr, int wickets, float econ){
    switch(role){
        case BATSMAN_ROLE: return (avg * sr) / 100.0f;
        case BOWLER_ROLE: return (wickets * 2.0f) + (100.0f - econ);
        case ALL_ROUNDER_ROLE: return ((avg * sr) / 100.0f) + (wickets * 2.0f);
        default: return 0.0f;
    }
    return -1.0f;
}

// Performs binary search on teamarray
int findTeamIndexbyId(Team teamsarr[],int n,int  targetId){
    int low = 0;
    int high = n - 1;

    while(low <= high){
        int mid = low + (high - low) / 2;
        if(teamsarr[mid].teamId == targetId) return mid;
        else if(teamsarr[mid].teamId < targetId) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

// Searches team array to find index matching given team name
int findTeamIndexByName(Team teamsarr[], int n, const char *name){
    for(int i=0 ; i<n;  i++){
        if(strcmp(teamsarr[i].name, name) == 0) return i;
    }
    return -1;
}

// Insert player node into role-specific LL in descending order of performance index
void insertSortedbyPerformance(OdiPlayer **headRef, OdiPlayer *newNode){
    if(*headRef == NULL || newNode->performanceIndex > (*headRef)->performanceIndex){
        newNode->roleNext = *headRef;
        *headRef = newNode;
        return;
    }
    OdiPlayer *current = *headRef;
    while(current->roleNext != NULL && current->roleNext->performanceIndex >= newNode->performanceIndex){
        current = current->roleNext;
    }
    newNode->roleNext = current->roleNext;
    current->roleNext = newNode;
}

// Appends player node to the end of team's complete roster LL
void appendtoRoster(OdiPlayer **headRef, OdiPlayer *newNode){
    newNode->next = NULL;
    if(*headRef == NULL){
        *headRef = newNode;
        return;
    }
    OdiPlayer *current = *headRef;
    while(current->next != NULL){
        current = current->next;
    }
    current->next = newNode;
}

// Checks across all teams whether player ID exists
int isPlayerIdDuplicates(Team teamsarr[], int n, int playerId){
    for(int i=0 ; i<n ; i++){
        OdiPlayer *curr = teamsarr[i].allPlayersHead;
        while(curr != NULL){
            if(curr->Id == playerId){
                return 1;
            }
            curr = curr->next;
        }
    }
    return 0;
}

// Allocates a new Player node and sorted role list, and updates strike rate of team stats.
int addPlayertoTeam(Team *team, int playerId,const char *name, PlayerRole role, int runs, float avg, float sr, int wickets, float econ){
    if(team->totalPlayers >= MAX_PLAYERS) return 0;

    OdiPlayer *newPlayer = (OdiPlayer*)malloc(sizeof(OdiPlayer));
    if(!newPlayer){
        printf("Memory Allocation Failed\n");
        return 0;
    }

    newPlayer->Id = playerId;
    strncpy(newPlayer->name, name, MAX_NAME_LEN - 1);
    newPlayer->name[MAX_NAME_LEN - 1] = '\0';
    strncpy(newPlayer->teamName, team->name, MAX_NAME_LEN - 1);
    newPlayer->teamName[MAX_NAME_LEN - 1] = '\0';
    newPlayer->role = role;
    newPlayer->totalRuns = runs;
    newPlayer->battingAverage = avg;
    newPlayer->strikeRate = sr;
    newPlayer->wickets = wickets;
    newPlayer->economyRate = econ;
    newPlayer->performanceIndex = calculatePerformanceIndex(role, avg, sr, wickets, econ);
    newPlayer->next = NULL;
    newPlayer->roleNext = NULL;

    appendtoRoster(&team->allPlayersHead, newPlayer);
    team->totalPlayers++;

    if(role == BATSMAN_ROLE){
        insertSortedbyPerformance(&team->batsmenHead, newPlayer);
    } else if(role == BOWLER_ROLE){
        insertSortedbyPerformance(&team->bowlersHead, newPlayer);
    } else{
        insertSortedbyPerformance(&team->allRoundersHead, newPlayer);
    }

    if(role == BATSMAN_ROLE || role == ALL_ROUNDER_ROLE){
        team->battingStrikeRateSum += sr;
        team->batterAndAllRounderCount++;
        team->averageBattingStrikeRate = team->battingStrikeRateSum / team->batterAndAllRounderCount;
    }
    return 1;
}

// Populates teams array and loads all initial player records from Players_data.h
void initializeSystem(Team teamsarr[]){
    for(int i=0 ; i<teamCount && i < MAX_TEAMS ; i++){
        teamsarr[i].teamId = i + 1;
        strncpy(teamsarr[i].name, teams[i], MAX_NAME_LEN - 1);
        teamsarr[i].name[MAX_NAME_LEN - 1] = '\0';
        teamsarr[i].totalPlayers = 0;
        teamsarr[i].averageBattingStrikeRate = 0.0f;
        teamsarr[i].battingStrikeRateSum = 0.0f;
        teamsarr[i].batterAndAllRounderCount = 0;
        teamsarr[i].allPlayersHead = NULL;
        teamsarr[i].batsmenHead = NULL;
        teamsarr[i].bowlersHead = NULL;
        teamsarr[i].allRoundersHead = NULL;
    }

    for(int i=0 ; i<playerCount ; i++){
        int tIdx = findTeamIndexByName(teamsarr, teamCount, players[i].team);
        if(tIdx != -1){
            PlayerRole role = parseRoleString(players[i].role);
            addPlayertoTeam(&teamsarr[tIdx], players[i].id, players[i].name, role, 
            players[i].totalRuns, players[i].battingAverage, players[i].strikeRate, 
        players[i].wickets, players[i].economyRate);
        }
    }
}

// Promps user for new Player Details.
void handleAddPlayer(Team teamsarr[], int numTeams){
    int teamId, playerId, roleChoice, runs, wickets;
    float avg, sr, econ;
    char name[MAX_NAME_LEN];

    printf("Enter Team Id to add player: ");
    if(scanf("%d", &teamId) != 1) return;

    int tIdx = findTeamIndexbyId(teamsarr, numTeams, teamId);
    if(tIdx == -1){
        printf("Invalid Team ID! Please enter an ID between 1 and %d\n", numTeams);
        return;
    }

    if(teamsarr[tIdx].totalPlayers >= MAX_PLAYERS){
        printf("Team %s has reached the maximum capacity of %d players. \n", teamsarr[tIdx].name, MAX_PLAYERS);
        return;
    }

    printf("Enter Player Details:\n");
    printf("Player ID: ");
    scanf("%d", &playerId);
    if(playerId < 1 || playerId > 1000){
        printf("Invalid Player ID! Must be between 1 and 1000\n");
        return;
    }

    if(isPlayerIdDuplicates(teamsarr, numTeams, playerId)){
        printf("Player ID %d already exists in the system! \n", playerId);
        return;
    }

    printf("Name: ");
    scanf(" %50[^\n]", name);

    printf("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    scanf("%d", &roleChoice);
    if(roleChoice < 1 || roleChoice > 3){
        printf("Invalid Role Selection. \n");
        return;
    }

    printf("Total Runs: ");
    scanf("%d", &runs);
    printf("Batting Average: ");
    scanf("%f", &avg);
    printf("Strike Rate: ");
    scanf("%f", &sr);
    printf("Wickets: ");
    scanf("%d", &wickets);
    printf("Economy Rate: ");
    scanf("%f", &econ);

    addPlayertoTeam(&teamsarr[tIdx], playerId, name, (PlayerRole)roleChoice, runs, avg, sr, wickets, econ);
    printf("Player added successfully to Team %s!\n", teamsarr[tIdx].name);
}

// Displays all players, total players count and average batting strike rate for a chosen team.
void handleDisplayTeamPlayers(Team teamsarr[], int numTeams){
    int teamId;
    printf("Enter Team ID: ");
    if(scanf("%d", &teamId) != 1) return;

    int tIdx = findTeamIndexbyId(teamsarr, numTeams, teamId);
    if(tIdx == -1){
        printf("Team with Id %d not found. \n", teamId);
        return;
    }

    Team *team = &teamsarr[tIdx];
    printf("\n Players of the Team %s: \n", team->name);
    printf("==============================================================================\n");
    printf("%-5s %-22s %-12s %-7s %-7s %-7s %-6s %-6s %-10s\n",
        "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("==============================================================================\n");
    OdiPlayer *curr = team->allPlayersHead;
    while(curr != NULL){
        printf("%-5d %-22s %-12s %-7d %-7.1f %-7.1f %-6d %-6.1f %-10.2f\n", curr->Id, curr->name,
        getRoleString(curr->role), curr->totalRuns, curr->battingAverage, curr->strikeRate, 
        curr->wickets, curr->economyRate, curr->performanceIndex);
        curr = curr->next;
    }  
     printf("==============================================================================\n");
     printf("Total Players: %d\n", team->totalPlayers);
     printf("Average Batting Strike Rate: %.2f\n", team->averageBattingStrikeRate);
}

// Comparator function used by qSort to order team pointers in descending order
int compareTeamsbyStrikeRate(const void *a,const void *b){
    const Team *A = *(const Team**) a;
    const Team *B = *(const Team**) b;

    if(B->averageBattingStrikeRate > A->averageBattingStrikeRate) return 1;
    if(B->averageBattingStrikeRate < A->averageBattingStrikeRate) return -1;
    return 0;
}

// Sorts and displays all teams in descending order of their average batting strike rate
void handleDisplayTeamsSortedbySR(Team teamsarr[], int numTeams){
    Team *sortedTeams[MAX_TEAMS];
    for(int i=0 ; i<numTeams ; i++){
        sortedTeams[i] = &teamsarr[i];
    }
    qsort(sortedTeams, numTeams, sizeof(Team *), compareTeamsbyStrikeRate);

    printf("\n Teams Sorted by Average Batting Strike Rate\n");
    printf("==============================================================================\n");
    printf("%-4s %-18s %-15s %-15s\n", "ID", "Team Name", "Avg Bat SR", "Total Players");
    printf("==============================================================================\n");

    for(int i=0 ; i<numTeams ; i++){
        printf("%-4d %-18s %-15.2f %-15d\n", sortedTeams[i]->teamId, sortedTeams[i]->name, sortedTeams[i]->averageBattingStrikeRate, sortedTeams[i]->totalPlayers);
    }
    printf("==============================================================================\n");
}

// Displays the top k players of specific role within a chosen time in O(K) time
void handleTopKPlayersByRole(Team teamsarr[], int numTeams){
    int teamId, roleChoice, k;
    printf("Enter Team ID: ");
    if(scanf("%d", &teamId) != 1) return;

    int tIdx = findTeamIndexbyId(teamsarr, numTeams, teamId);
    if(tIdx == -1){
        printf("Team with ID %d not found.\n", teamId);
        return;
    }

    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-Rounder): ");
    scanf("%d", &roleChoice);
    if (roleChoice < 1 || roleChoice > 3) {
        printf("Invalid Role selection!\n");
        return;
    }

    printf("Enter number of players: ");
    scanf("%d", &k);
    if (k <= 0) {
        printf("Number of players (K) must be greater than 0.\n");
        return;
    }
    Team *team = &teamsarr[tIdx];
    OdiPlayer *curr = NULL;
    char *rolePlural = "";

    if (roleChoice == BATSMAN_ROLE) {
        curr = team->batsmenHead;
        rolePlural = "Batsmen";
    } else if(roleChoice == BOWLER_ROLE){
        curr = team->bowlersHead;
        rolePlural = "Bowlers";
    } else{
        curr = team->allRoundersHead;
        rolePlural = "All-rounders";
    }
    printf("\nTop %d %s of Team %s:\n", k, rolePlural, team->name);
    printf("====================================================================================\n");
    printf("%-5s %-22s %-12s %-7s %-7s %-7s %-6s %-6s %-10s\n",
           "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("====================================================================================\n");

    int count = 0;
    while(curr != NULL && count < k){
        printf("%-5d %-22s %-12s %-7d %-7.1f %-7.1f %-6d %-6.1f %-10.2f\n", curr->Id,
        curr->name, getRoleString(curr->role), curr->totalRuns, curr->battingAverage,
        curr->strikeRate, curr->wickets, curr->economyRate, curr->performanceIndex);
        curr = curr->roleNext;
        count++;
    }
    printf("====================================================================================\n");
    if(count < k){
        printf("(Note: Only %d %s available in Team %s)\n", count, rolePlural, team->name);
    }
}

// Restores max-heap property for array of OdiPlayer pointers based on performance index
void maxHeapify(OdiPlayer *heap[], int size, int idx){
    int largest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if(left < size && heap[left]->performanceIndex > heap[largest]->performanceIndex){
        largest = left;
    }
    if(right < size && heap[right]->performanceIndex > heap[largest]->performanceIndex){
        largest = right;
    }

    if(largest != idx){
        OdiPlayer *temp = heap[idx];
        heap[idx]=  heap[largest];
        heap[largest] = temp;
        maxHeapify(heap, size, largest);
    }
}

// Merges and Displays all players of a specific role across all teams in descending order
void handleDisplayAllPlayersbyRole(Team teamsarr[], int numTeams){
    int roleChoice;
    printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
    if (scanf("%d", &roleChoice) != 1) return;

    if (roleChoice < 1 || roleChoice > 3) {
        printf("Invalid Role selection!\n");
        return;
    }

    OdiPlayer *heap[MAX_TEAMS];
    int heapSize = 0;

    for(int i=0 ; i<numTeams; i++){
        OdiPlayer *head = NULL;
        if(roleChoice == BATSMAN_ROLE){
            head = teamsarr[i].batsmenHead;
        } else if(roleChoice == BOWLER_ROLE){
            head = teamsarr[i].bowlersHead;
        } else{
            head = teamsarr[i].allRoundersHead;
        }

        if(head != NULL){
            heap[heapSize++] = head;
        }
    }

    for(int i= (heapSize / 2) - 1; i>=0 ; i--){
        maxHeapify(heap, heapSize, i);
    }

    char *rolePlural = (roleChoice == BATSMAN_ROLE) ? "Batsmen" : (roleChoice == BOWLER_ROLE) ? 
    "Bowlers" : "All-rounders";
    printf("\n%s of all teams:\n", rolePlural);
    printf("====================================================================================================\n");
    printf("%-5s %-22s %-15s %-12s %-7s %-7s %-7s %-6s %-6s %-10s\n",
           "ID", "Name", "Team", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    printf("====================================================================================================\n");

    while(heapSize > 0){
        OdiPlayer *topPlayer = heap[0];

        printf("%-5d %-22s %-15s %-12s %-7d %-7.1f %-7.1f %-6d %-6.1f %-10.2f\n", topPlayer->Id, topPlayer->name, topPlayer->teamName,
        getRoleString(topPlayer->role), topPlayer->totalRuns, topPlayer->battingAverage,
        topPlayer->strikeRate, topPlayer->wickets, topPlayer->economyRate, topPlayer->performanceIndex);

        if(topPlayer->roleNext != NULL){
         heap[0] = topPlayer->roleNext;
            } else{
        heap[0] = heap[heapSize - 1];
        heapSize--;
            }
            if(heapSize > 0){
                maxHeapify(heap, heapSize, 0);
            }
    }
    printf("====================================================================================================\n");
}

// Free all Allocated Memory of OdiPlayer
void cleanupMemory(Team teamsarr[], int numTeams){
    for(int i=0 ; i<numTeams ; i++){
        OdiPlayer *curr = teamsarr[i].allPlayersHead;
        while(curr != NULL){
            OdiPlayer *nextNode = curr->next;
            free(curr);
            curr = nextNode;
        }
    }
}

// Displays an interactive menu for User
void displayMenu(){
    printf("==============================================================================\n");
    printf("ICC ODI Player Performance Analyzer\n");
    printf("==============================================================================\n");
    printf("1. Add Player to Team\n");
    printf("2. Display Players of a Specific Team\n");
    printf("3. Display Teams by Average Batting Strike Rate\n");
    printf("4. Display Top K Players of a Specific Team by Role\n");
    printf("5. Display all Players of specific role Across All Teams by performance index\n");
    printf("6. Exit\n");
    printf("==============================================================================\n");
    printf("Enter your choice: ");
}

int main(){
    Team teamsarr[MAX_TEAMS];
    initializeSystem(teamsarr);

    int choice;
    while(1){
        displayMenu();

        if(scanf("%d", &choice) != 1){
            int ch;
            while((ch = getchar()) != '\n' && ch != EOF);
            printf("Invalid input. Please enter a number between 1 and 6. \n");
            continue;
        }

        switch(choice){
            case 1:
            handleAddPlayer(teamsarr, teamCount);
            break;
            case 2:
            handleDisplayTeamPlayers(teamsarr, teamCount);
            break;
            case 3: 
            handleDisplayTeamsSortedbySR(teamsarr, teamCount);
            break;
            case 4:
            handleTopKPlayersByRole(teamsarr, teamCount);
            break;
            case 5:
            handleDisplayAllPlayersbyRole(teamsarr, teamCount);
            break;
            case 6:
            cleanupMemory(teamsarr, teamCount);
            printf("Exiting ICC ODI Player Performance Analyzer. GoodBye!\n");
            return 0;
            default:
            printf("Invalid Choice! Please select an option from 1 to 6\n");
        }
    }
    return 0;
}