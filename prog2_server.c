/* CSCI 367 Lexithesaurus: prog2_server.c
 *
 * 31 OCT 2018, Zach Richardson and Mitch Kimball
 */

 //TODO: ALTER CODE FROM FIRST PROJECT
 #include <time.h>
 #include <netdb.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include "trie.h"

#define MAXWORDSIZE 255
#define DICTIONARYPATH "twl06.txt"
#define QLEN 6 /* size of request queue */
#define TRUE 1
#define FALSE 0
 int visits = 0; /* counts client connections */
 Trie* dictionary;

 /*------------------------------------------------------------------------
 *  Program: demo_server
 *
 *  Purpose: allocate a socket and then repeatedly execute the following:
 *  (1) wait for the next connection from a client
 *  (2) send a short message to the client
 *  (3) close the connection
 *  (4) go back to step (1)
 *
 *  Syntax: ./demo_server port
 *
 *  port - protocol port number to use
 *
 *------------------------------------------------------------------------
 */


void makeBoard(char* board, uint8_t board_size) {
  char vowels[5] = {'a','e','i','o','u'};
  char randChar;
  int vowel = 0;

  srand(time(NULL));
  for (int i = 0; i < board_size; i++) {

    if(i != board_size-1 || vowel)
    randChar = (rand() %(122-97+1))+97;
    else
    randChar = vowels[rand()%(4-0+1)];

    if(randChar == 'a' || randChar == 'e' || randChar == 'i' || randChar == 'o' || randChar == 'u')
    vowel = 1;

    board[i] = randChar;
    //TODO remove printing
    fprintf(stderr, "%c", board[i]);
  }
  fprintf(stderr, "\n");
}

int check_guess(char* guess,char* board){
    int letterCount[26];
    int i;
    int valid =TRUE;

    for(i=0;i <strlen(board);i++){
        letterCount[board[i] - 97]++;
    }

    for(i=0;i <strlen(guess);i++){
        letterCount[guess[i]-97]--;
        if(letterCount[guess[i]-97] ==-1){
            valid = FALSE;
        }
    }
    return valid;

}

void turn_handler(int p1,int p2,char* board,uint8_t *p1Score,uint8_t* p2Score){
    char yourTurn = 'Y';
    char notYourTurn = 'N';
    int isRoundOver = FALSE;
    ssize_t n;
    uint8_t wordlength;
    char guessbuffer[MAXWORDSIZE];
    Trie* guessedWords = trie_new();
    uint8_t validguess = TRUE;

    memset(guessbuffer,0,sizeof(guessbuffer));
    int ap = p1;
    int iap = p2;

    while(!isRoundOver){
        //T.1
        send(ap,&yourTurn,sizeof(yourTurn),0);
        send(iap,&notYourTurn,sizeof(notYourTurn),0);
       //TODO: set flag for timeouts

       // recieve players guess
       n = recv(ap,&wordlength,sizeof(wordlength),0);
       if(n != sizeof(wordlength)){
           fprintf(stderr,"recv error: wordlength not read properly\n");
           exit(EXIT_FAILURE);
       }

        n = recv(ap,guessbuffer,wordlength,0);
        if(n != wordlength){
            fprintf(stderr,"recv error: word not read properly\n");
            exit(EXIT_FAILURE);
        }

        if(trie_lookup(dictionary,guessbuffer) == TRIE_NULL){
            //not valid word
            //round over logic
            validguess = FALSE;
            send(ap,&validguess,sizeof(validguess),0);
            send(iap,&validguess,sizeof(validguess),0);

            isRoundOver = TRUE;
            if(ap == p1) {
                (*p2Score)++;
            } else {
                (*p1Score)++;
            }
        }
        // T.3.1 check if the word is in the trie already
        else if(trie_lookup(guessedWords, guessbuffer) == TRIE_NULL){
            trie_insert(guessedWords,guessbuffer,(TrieValue)1);

            if(check_guess(guessbuffer,board)){
                //guess is valid
                send(ap,&validguess,sizeof(validguess),0);
                //send length to inactive player
                send(iap,&wordlength,sizeof(wordlength),0);
                //send the guessed word
                send(iap,guessbuffer,sizeof(guessbuffer),0);
            }

            else{
                //round over
                validguess = FALSE;
                send(ap,&validguess,sizeof(validguess),0);
                send(iap,&validguess,sizeof(validguess),0);

                isRoundOver = TRUE;
                if(ap == p1) {
                    (*p2Score)++;
                } else {
                    (*p1Score)++;
                }
            }
        }

        else{
            //round over
            validguess = FALSE;
            send(ap,&validguess,sizeof(validguess),0);
            send(iap,&validguess,sizeof(validguess),0);

            isRoundOver = TRUE;
            if(ap == p1) {
                (*p2Score)++;
            } else {
                (*p1Score)++;
            }
        }

        //changing active player
        if(ap == p1) {
            ap = p2;
            iap = p1;
        } else {
            ap = p1;
            iap = p2;
        }
    }
}

void play_game(uint8_t board_size, uint8_t turn_time, int sd2, int sd3) {

  char board[board_size+1];

  char boardbuffer[MAXWORDSIZE+1];
  char guess; //guess recieved from server
  int isCorrect = 0; //flag for if the guess is correct
  int i; //used in for loop
  uint8_t roundNum = 1;
  uint8_t player1Score = 0;
  uint8_t player2Score = 0;

  memset(board,0,sizeof(board));

  while(player1Score < 3 && player2Score < 3) {
    //R.1
    send(sd2,&player1Score,sizeof(player1Score),0);
    send(sd3,&player1Score,sizeof(player1Score),0);
    send(sd2,&player2Score,sizeof(player2Score),0);
    send(sd3,&player2Score,sizeof(player2Score),0);
    //R.2
    send(sd2,&roundNum,sizeof(roundNum),0);
    send(sd3,&roundNum,sizeof(roundNum),0);
    //R.3
    makeBoard(board, board_size);
    //R.4
    send(sd2,&board,(size_t)board_size,0);
    send(sd3,&board,(size_t)board_size,0);

    //R.5+R.6
    if(roundNum%2 == 0){
        turn_handler(sd3,sd2, board, &player2Score, &player1Score);
    }
    else{
        turn_handler(sd2,sd3, board, &player1Score, &player2Score);
    }

    //increment round number
    roundNum++;
  }

}

void populateTrie(char* dictionaryPath) {
  FILE *fp;
  char fileBuffer[1000];
  memset(fileBuffer,0,sizeof(fileBuffer));
  int secretVal = 1;
  int* value = &secretVal;

  fp = fopen(dictionaryPath, "r");
  while(fgets(fileBuffer,1000,(FILE*)fp)) {
    if(!trie_insert(dictionary, fileBuffer, (TrieValue)value)) {
      fprintf(stderr,"trie_insert didn't insert correctly \n");
    }
  }
}

int main(int argc, char **argv) {
  struct protoent *ptrp; /* pointer to a protocol table entry */
  struct sockaddr_in sad; /* structure to hold server's address */
  struct sockaddr_in cad; /* structure to hold client's address */

  int sd, sd2, sd3; /* socket descriptors */
  int port; /* protocol port number */
  int alen; /* length of address */
  int optval = 1; /* boolean value when we set socket option */
  char buf[1000]; /* buffer for string the server sends */
  char player1 = '1';
  char player2 = '2';
  uint8_t board_size;
  uint8_t turn_time;
  //TODO
  //char* dictionary_path;
  port = atoi(argv[1]); /* convert argument to binary */

  dictionary = trie_new();


  if( argc != 4 ) {
    fprintf(stderr,"Error: Wrong number of arguments\n");
    fprintf(stderr,"usage:\n");
    fprintf(stderr,"./prog2_server server_port board_size seconds_per_round dictionary_path\n");
    exit(EXIT_FAILURE);
  }

  board_size = atoi(argv[2]);
  turn_time = atoi(argv[3]);
  //TODO
  //dictionary_path = argv[4];

  memset((char *)&sad,0,sizeof(sad)); /* clear sockaddr structure */

  //: Set socket family to AF_INET
  sad.sin_family = AF_INET;

  // Set local IP address to listen to all IP addresses this server can assume. You can do it by using INADDR_ANY
  sad.sin_addr.s_addr = INADDR_ANY;


  if (port > 0) { /* test for illegal value */
    // set port number. The data type is u_short
    sad.sin_port = htons(port);
  } else { /* print error message and exit */
    fprintf(stderr,"Error: Bad port number %s\n",argv[1]);
    exit(EXIT_FAILURE);
  }

  /* Map TCP transport protocol name to protocol number */
  if ( ((long int)(ptrp = getprotobyname("tcp"))) == 0) {
    fprintf(stderr, "Error: Cannot map \"tcp\" to protocol number");
    exit(EXIT_FAILURE);
  }

  /*  Create a socket with AF_INET as domain, protocol type as
  SOCK_STREAM, and protocol as ptrp->p_proto. This call returns a socket
  descriptor named sd. */
  sd = socket(AF_INET, SOCK_STREAM, ptrp->p_proto);
  if (sd < 0) {
    fprintf(stderr, "Error: Socket creation failed\n");
    exit(EXIT_FAILURE);
  }

  /* Allow reuse of port - avoid "Bind failed" issues */
  if( setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0 ) {
    fprintf(stderr, "Error Setting socket option failed\n");
    exit(EXIT_FAILURE);
  }

  /*  Bind a local address to the socket. For this, you need to
  pass correct parameters to the bind function. */
  if (bind(sd, (struct sockaddr*) &sad, sizeof(sad)) < 0) {
    fprintf(stderr,"Error: Bind failed\n");
    exit(EXIT_FAILURE);
  }

  /*  Specify size of request queue. Listen take 2 parameters --
  socket descriptor and QLEN, which has been set at the top of this code. */
  if (listen(sd, QLEN) < 0) {
    fprintf(stderr,"Error: Listen failed\n");
    exit(EXIT_FAILURE);
  }
     pid_t pid; //process id of the child processes.
  /* Main server loop - accept and handle requests */

  populateTrie(DICTIONARYPATH);

  while (1) {
    alen = sizeof(cad);
    if ((sd2 = accept(sd, (struct sockaddr *)&cad, (socklen_t*)&alen)) < 0) {
      fprintf(stderr, "Error: Accept failed\n");
      exit(EXIT_FAILURE);
    }

    send(sd2,&player1,sizeof(player1),0);
    send(sd2,&board_size,sizeof(board_size),0);
    send(sd2,&turn_time,sizeof(turn_time),0);

    if ((sd3 = accept(sd, (struct sockaddr *)&cad, (socklen_t*)&alen)) < 0) {
      fprintf(stderr, "Error: Accept failed\n");
      exit(EXIT_FAILURE);
    }

    send(sd3,&player2,sizeof(player2),0);
    send(sd3,&board_size,sizeof(board_size),0);
    send(sd3,&turn_time,sizeof(turn_time),0);

    // fork here and implement logic
    pid = fork();
    if (pid < 0) {
      perror("Error Fork() failure");
    }
    //we are in the child process.
    else if (pid == 0) {

      //play Lexithesaurus
      play_game(board_size, turn_time,sd2,sd3);

      //at end of game close the socket and exit
      close(sd2);
      close(sd3);
      exit(EXIT_SUCCESS);
    }
  }
}
