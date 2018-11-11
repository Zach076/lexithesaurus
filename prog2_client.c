/* CSCI 367 Lexithesaurus: prog2_client.c
*
* 31 OCT 2018, Zach Richardson and Mitch Kimball
*/

//TODO: ALTER CODE FROM FIRST PROJECT
#include<netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXWORDSIZE 255
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

/*
#define MAXWORDSIZE 254

char display_board(uint8_t guesses, char* board){
  char guess=0;
  printf("\nBoard: %s (%d guesses left) \n", board, guesses);
  printf("Enter guess: ");
  scanf("%c",&guess);
  if(guess == '\n'){
    scanf("%c",&guess);
  }
  return guess;
}
*/

/*main game loop for clients*/
/*
void play_game(int sd) {
  char guess;
  uint8_t numguesses=0;
  int wordlength;
  ssize_t n;

  //create a word buffer with size numguesses+1 to store the word + a null terminator
  char wordbuf[MAXWORDSIZE+1];
  memset(wordbuf,0,sizeof(wordbuf)); //this essentially adds the null  terminator to the board for us.

  //read the number of guesses
  n = read(sd,&numguesses,sizeof(numguesses));
  if(n != sizeof(numguesses)){
    perror("Read Error: numguesses not read properly");
    exit(EXIT_FAILURE);
  }

  wordlength = numguesses;

  //read the board, wait for all characters of the secret word
  n = recv(sd,wordbuf,numguesses,0);
  if(n != wordlength){
    perror("Recv Error: board not read properly");
    exit(EXIT_FAILURE);
  }

  //while client still has guesses left
  while (numguesses > 0){
    if(numguesses == 255) {
      //print board, you win, close socket, and exit
      printf("\nBoard: %s\n",wordbuf);
      printf("You win\n");
      close(sd);
      exit(EXIT_SUCCESS);
    }

    guess = display_board(numguesses,wordbuf);
    //send the character guess to the server
    send(sd,&guess,1,0);
    //get server response
    memset(wordbuf,0,sizeof(wordbuf));
    n = read(sd,&numguesses,sizeof(numguesses));
    if(n != sizeof(numguesses)){
      perror("Read Error: numguesses not read properly");
      exit(EXIT_FAILURE);
    }
    n = recv(sd,wordbuf,wordlength,0);
    if(n != wordlength){
      perror("Recv Error: board not read properly");
      exit(EXIT_FAILURE);
    }
  }

  //print board, you lose, close socket, and exit
  printf("Board: %s\n",wordbuf);
  printf("You lost\n");
  close(sd);
  exit(EXIT_SUCCESS);
}
*/

void play_game(int sd, char playerNum, uint8_t board_size, uint8_t turn_time) {
  char guessBuffer[MAXWORDSIZE+1];
  char board[board_size+1];
  uint8_t player1Score;
  uint8_t player2Score;
  uint8_t roundNum;
  ssize_t n;
  int i; //for loop

  memset(guessBuffer,0,sizeof(guessBuffer)); //this essentially adds the null  terminator to the board for us.
  memset(board,0,sizeof(board));

  //while(1) {
    //R.1
    n = read(sd,&player1Score,sizeof(player1Score));
    if(n != sizeof(player1Score)){
      perror("Read Error: Player1 Score not read properly");
      exit(EXIT_FAILURE);
    }

    n = read(sd,&player2Score,sizeof(player2Score));
    if(n != sizeof(player2Score)){
      perror("Read Error: Player2 Score not read properly");
      exit(EXIT_FAILURE);
    }

    //R.2
    n = read(sd,&roundNum,sizeof(roundNum));
    if(n != sizeof(roundNum)){
      perror("Read Error: Round number not read properly");
      exit(EXIT_FAILURE);
    }

    //R.4
    n = recv(sd,board,board_size,0);
    if(n != board_size){
      perror("Read Error: Board not read properly");
      exit(EXIT_FAILURE);
    }

    printf("Round %d... \n",roundNum);
    if(playerNum == '1') {
      printf("Score is %d-%d\n",player1Score,player2Score);
    } else {
      printf("Score is %d-%d\n",player2Score,player1Score);
    }
    //R.4
    printf("Board:");
    for(i = 0; i < board_size; i++) {
      printf(" %c", board[i]);
    }
    printf("\n");

/*
  //read the number of guesses
  n = read(sd,&numguesses,sizeof(numguesses));
  if(n != sizeof(numguesses)){
    perror("Read Error: numguesses not read properly");
    exit(EXIT_FAILURE);
  }

  wordlength = numguesses;

  //read the board, wait for all characters of the secret word
  n = recv(sd,wordbuf,numguesses,0);
  if(n != wordlength){
    perror("Recv Error: board not read properly");
    exit(EXIT_FAILURE);
  }

  //while client still has guesses left
  while (numguesses > 0){
    if(numguesses == 255) {
      //print board, you win, close socket, and exit
      printf("\nBoard: %s\n",wordbuf);
      printf("You win\n");
      close(sd);
      exit(EXIT_SUCCESS);
    }

    guess = display_board(numguesses,wordbuf);
    //send the character guess to the server
    send(sd,&guess,1,0);
    //get server response
    memset(wordbuf,0,sizeof(wordbuf));
    n = read(sd,&numguesses,sizeof(numguesses));
    if(n != sizeof(numguesses)){
      perror("Read Error: numguesses not read properly");
      exit(EXIT_FAILURE);
    }
    n = recv(sd,wordbuf,wordlength,0);
    if(n != wordlength){
      perror("Recv Error: board not read properly");
      exit(EXIT_FAILURE);
    }
  }

  //print board, you lose, close socket, and exit
  printf("Board: %s\n",wordbuf);
  printf("You lost\n");
  close(sd);
  exit(EXIT_SUCCESS);
  */
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
  uint8_t board_size;
  uint8_t turn_time;
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
    perror("Read Error: playerNum not read properly");
    exit(EXIT_FAILURE);
  }
  if(playerNum == '1') {
    fprintf(stderr,"You are Player 1... the game will begin when Player 2 joins...\n");
  } else {
    fprintf(stderr,"You are Player 2... \n");
  }

  n = read(sd,&board_size,sizeof(board_size));
  if(n != sizeof(board_size)){
    perror("Read Error: board_size not read properly");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr,"Board size : %d \n",board_size);

  n = read(sd,&turn_time,sizeof(turn_time));
  if(n != sizeof(turn_time)){
    perror("Read Error: turn_time not read properly");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr,"Seconds per turn : %d \n",turn_time);

  /* Game Logic function */
  play_game(sd, playerNum, board_size, turn_time);

  //in case real bad things happen still close the socket and exit nicely
  close(sd);

  exit(EXIT_SUCCESS);
}
