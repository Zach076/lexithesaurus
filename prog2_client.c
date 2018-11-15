/* CSCI 367 Lexithesaurus: prog2_client.c
*
* 31 OCT 2018, Zach Richardson and Mitch Kimball
*/

#include<netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXWORDSIZE 255
#define TRUE 1
#define FALSE 0

/*------------------------------------------------------------------------
* Program: demo_client
*
* Purpose: allocate a socket, connect to a server, and

*------------------------------------------------------------------------
* Program: demo_client
*
* Purpose: allocate a socket, connect to a server, and print all output
*
* Syntax: ./demo_client server_address server_port
*
* server_address - name of a computer on which server is executing
* server_port    - protocol port number server is using
*
*------------------------------------------------------------------------
*/
int reader(char* guess, uint8_t sec) {
  fd_set set;
  struct timeval timeout = {sec,0}; //timeout of 10 secs.
  int n;
  FD_ZERO(&set);
  FD_SET(0,&set);
  n =  select(1,&set,NULL,NULL,&timeout);
  if(n == 0){
    //timeout
    n = 0;
  } else if(n == -1) {
    //error
    n = 0;
  } else {
    fgets(guess,MAXWORDSIZE,stdin);
    n = 1;
  }
  return n;
}

void turnHandler(int sd, uint8_t turnTime) {
  char turnFlag;
  ssize_t n;
  uint8_t guessSize;
  char guess[MAXWORDSIZE];
  uint8_t isCorrect;
  uint8_t broken = FALSE;
  int done = FALSE;

  while(!done) {

    memset(guess,0,sizeof(guess));

    n = recv(sd, &turnFlag, sizeof(turnFlag), 0);
    if (n != sizeof(turnFlag)) {
      fprintf(stderr,"Read Error: Turn flag not read properly\n");
      exit(EXIT_FAILURE);
    }

    if(turnFlag == 'Y') {
      //apfprintf(stderr,"Read Error: Guess size not read properly");
      fprintf(stderr, "Your turn, enter a word: ");
      //fgets(guess,MAXWORDSIZE,stdin);
      if (!reader(guess, turnTime)) {
        broken = TRUE;
        send(sd,&broken,sizeof(broken),0);
      }
      guess[strlen(guess)-1]=0;

      guessSize = (uint8_t)strlen(guess);
      send(sd,&broken,sizeof(broken),0);
      send(sd,&guessSize,sizeof(guessSize),0);
      send(sd,guess,guessSize,0);
      recv(sd,&isCorrect,sizeof(isCorrect),0);

      if(isCorrect == 1){
        printf("Valid word\n");
      } else if(broken){
        printf("\nTimed out\n");

        done = TRUE;
      } else {
        printf("Invalid word\n");

        done = TRUE;
      }

    } else if(turnFlag == 'N') {
      //iap
      printf("Please wait for opponent to enter word... \n");

      recv(sd,&isCorrect,sizeof(isCorrect),0);

      if(isCorrect == 1){
        n = recv(sd, &guessSize, sizeof(guessSize), 0);
        if (n != sizeof(guessSize)) {
          fprintf(stderr,"Read Error: Guess size not read properly");
          exit(EXIT_FAILURE);
        }

        n = recv(sd, guess, guessSize, 0);
        if (n != guessSize) {
          fprintf(stderr,"Read Error: Guess not read properly");
          exit(EXIT_FAILURE);
        }

        printf("Opponent entered \"%s\" \n",guess);

      } else {
        printf("Opponent lost the round!\n");

        done = TRUE;
      }
    }
  }
}

void betterRead(int sd, void* buf, size_t len, char* error) {
  ssize_t n;
  n = read(sd, buf, len);
  if (n != len) {
    fprintf(stderr,"Read Error: %s Score not read properly\n", error);
    exit(EXIT_FAILURE);
  }
}

void recieve(int sd, void* buf, size_t len, int flags, char* error) {
  ssize_t n;
  n = recv(sd, buf, len, flags);
  if (n != len) {
    fprintf(stderr,"Read Error: %s Score not read properly\n", error);
    exit(EXIT_FAILURE);
  }
}

void playGame(int sd, char playerNum, uint8_t boardSize, uint8_t turnTime) {
  char guessBuffer[MAXWORDSIZE+1];
  char board[boardSize+1];
  uint8_t player1Score = 0;
  uint8_t player2Score = 0;
  uint8_t roundNum = 0;
  size_t n;
  uint8_t lastRound = 0;
  int i; //for loop
  int done = FALSE;

  memset(guessBuffer,0,sizeof(guessBuffer)); //this essentially adds the null  terminator to the board for us.
  memset(board,0,sizeof(board));

  while(!done) {
    //R.1
    recieve(sd, &player1Score, sizeof(player1Score), 0, "Player1 Score");
    recieve(sd, &player2Score, sizeof(player2Score), 0, "Player2 Score");
    //R.2
    recieve(sd, &roundNum, sizeof(roundNum), 0, "Round number");
    //R.4
    recieve(sd, board, boardSize, 0, "Board");

    if(player1Score < 3 && player2Score < 3) {
      printf("\nRound %d... \n", roundNum);
      if (playerNum == '1') {
        printf("Score is %d-%d\n", player1Score, player2Score);
      } else {
        printf("Score is %d-%d\n", player2Score, player1Score);
      }
      //R.4
      printf("Board:");
      for (i = 0; i < boardSize; i++) {
        printf(" %c", board[i]);
      }
      printf("\n");

      turnHandler(sd, turnTime);
    } else {
      done = TRUE;
    }
  }

  if (player1Score == '3') {
    if (playerNum == '1') {
      printf("You won\n");
    } else {
      printf("You lost\n");
    }
  } else {
    if (playerNum == '1') {
      printf("You won\n");
    } else {
      printf("You lost\n");
    }
  }
}

//main function, mostly connection logic
int main( int argc, char **argv) {
  struct hostent *ptrh; /* pointer to a host table entry */
  struct protoent *ptrp; /* pointer to a protocol table entry */
  struct sockaddr_in sad; /* structure to hold an IP address */
  int sd; /* socket descriptor */
  int port; /* protocol port number */
  char *host; /* pointer to host name */
  int n; /* number of characters read */
  char buf[1000]; /* buffer for data from the server */
  uint8_t boardSize;
  uint8_t turnTime;
  char playerNum;
  memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure */
  sad.sin_family = AF_INET; /* set family to Internet */

  if( argc != 3 ) {
    fprintf(stderr,"Error: Wrong number of arguments\n");
    fprintf(stderr,"usage:\n");
    fprintf(stderr,"./prog1_client server_address server_port\n");
    exit(EXIT_FAILURE);
  }

  port = atoi(argv[2]); /* convert to binary */
  if (port > 0) { /* test for legal value */
    sad.sin_port = htons((u_short)port);
  } else {
    fprintf(stderr,"Error: bad port number %s\n",argv[2]);
    exit(EXIT_FAILURE);
  }

  host = argv[1]; /* if host argument specified */

  /* Convert host name to equivalent IP address and copy to sad. */
  ptrh = gethostbyname(host);
  if ( ptrh == NULL ) {
    fprintf(stderr,"Error: Invalid host: %s\n", host);
    exit(EXIT_FAILURE);
  }

  memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);

  /* Map TCP transport protocol name to protocol number. */
  if ( ((long int)(ptrp = getprotobyname("tcp"))) == 0) {
    fprintf(stderr, "Error: Cannot map \"tcp\" to protocol number");
    exit(EXIT_FAILURE);
  }

  /* Create a socket. */
  sd = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
  if (sd < 0) {
    fprintf(stderr, "Error: Socket creation failed\n");
    exit(EXIT_FAILURE);
  }

  /* Connect the socket to the specified server. You have to pass correct parameters to the connect function.*/
  if (connect(sd, (struct sockaddr*) &sad, sizeof(sad)) < 0) {
    fprintf(stderr,"connect failed\n");
    exit(EXIT_FAILURE);
  }

  n = read(sd,&playerNum,sizeof(playerNum));
  if(n != sizeof(playerNum)){
    fprintf(stderr,"Read Error: playerNum not read properly");
    exit(EXIT_FAILURE);
  }
  if(playerNum == '1') {
    fprintf(stderr,"You are Player 1... the game will begin when Player 2 joins...\n");
  } else {
    fprintf(stderr,"You are Player 2... \n");
  }

  n = read(sd,&boardSize,sizeof(boardSize));
  if(n != sizeof(boardSize)){
    fprintf(stderr,"Read Error: boardSize not read properly");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr,"Board size : %d \n",boardSize);

  n = read(sd,&turnTime,sizeof(turnTime));
  if(n != sizeof(turnTime)){
    fprintf(stderr,"Read Error: turnTime not read properly");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr,"Seconds per turn : %d \n",turnTime);

  /* Game Logic function */
  playGame(sd, playerNum, boardSize, turnTime);

  //in case real bad things happen still close the socket and exit nicely
  close(sd);

  exit(EXIT_SUCCESS);
}
