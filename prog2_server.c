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

#define MAXWORDSIZE = 255;
#define QLEN 6 /* size of request queue */
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

/*
#define MAXWORDSIZE 254

//simple check if client has won
int isWon(char* board) {
  int i = 0;
    for(i = 0; i < strlen(board); i++) {
        if(board[i] == '_') {
            return 0;
        }
    }
    return 1;
}

//check if guess has been guessed or if it isnt in the word
int check_guess(char guess, char* board, const char*  word) {
    int guessed = 0;
    int i = 0;
    for(i = 0; i < strlen(board);i++) {
        if (guess == (board[i])) {
            guessed = -1;
        } else if(guess == word[i] && guessed == 0) {
            guessed = 1;
            board[i] = word[i];
        }
    }
    if(guessed == -1) {
      guessed = 0;
    }
    return guessed;
}

 /*main game loop for server*/
 /*
 void play_game(char* word,int c_sd){
     //max word size is 254 so indecies should be 0-253
     char boardbuffer[MAXWORDSIZE+1];
     int wordlength = (int)strlen(word);
     //the number of guesses based on the length of the word
     uint8_t numguesses = (uint8_t)wordlength;
     //used to "hide" the word -> using _ instead of the characters
     char* displayword = (char*) malloc((strlen(word)+1) * sizeof(char));
     char guess; //guess recieved from server
     int isCorrect = 0; //flag for if the guess is correct
     int i; //used in for loop

     //initialize empty board and fill with '_'
     memset(boardbuffer,0,sizeof(boardbuffer));
     for (i = 0; i < wordlength; ++i) {
         displayword[i] = '_';
     }
     //null terminate the board
     displayword[wordlength] = 0;
     while(numguesses > 0) {
         //send N --> seems to work
         send(c_sd,&numguesses,sizeof(numguesses),0);

         //prepare to send the board
         sprintf(boardbuffer,"%s",displayword);
         send(c_sd,boardbuffer,(size_t)wordlength,0);

         //recieve the guess
         recv(c_sd,&guess,1,0);

         //checking guess and updating the board, if wrong, decrement guesses left
         // if guess is correct, check if won
         isCorrect = check_guess(guess,displayword,word);
         if(!isCorrect) {
             numguesses--;
         } else if (isWon(displayword)) {
             numguesses = 255;
             send(c_sd,&numguesses,sizeof(numguesses),0);
             sprintf(boardbuffer,"%s",displayword);
             send(c_sd,boardbuffer,(size_t)wordlength,0);
         }
     }
     send(c_sd,&numguesses,sizeof(numguesses),0);
     sprintf(boardbuffer,"%s",displayword);
     send(c_sd,boardbuffer,(size_t)wordlength,0);
 }
 */

char* makeBoard(uint8_t board_size) {
  char board[board_size];
  char vowels[5] = {'a','e','i','o','u'};
  char randChar;
  int vowel = 0;
  srand(time(NULL));
  for (int i = 0; i < board_size; i++) {

    if(i != board_size-1 || vowel)
    randChar = (rand() %(122-97+1))+97;
    else
    randChar = vowels[rand()%(5-0+1)];

    if(randChar == 'a' || randChar == 'e' || randChar == 'i' || randChar == 'o' || randChar == 'u')
    vowel = 1;

    board[i] = randChar;

    fprintf(stderr, "%c", board[i]);
  }
}

void play_game(uint8_t board_size, uint8_t turn_time, int sd2, int sd3) {

  char* board = makeBoard(board_size);
  char yourTurn = 'Y';
  char notYourTurn = 'N';
  char boardbuffer[MAXWORDSIZE+1];
  char guess; //guess recieved from server
  int isCorrect = 0; //flag for if the guess is correct
  int i; //used in for loop

  while(1) {
    //TODO
    send(sd2,&yourTurn,sizeof(yourTurn),0);
    send(sd3,&notYourTurn,sizeof(notYourTurn),0);

      //prepare to send the board
      sprintf(boardbuffer,"%s",displayword);
      send(c_sd,boardbuffer,(size_t)wordlength,0);

      //recieve the guess
      recv(c_sd,&guess,1,0);

      //checking guess and updating the board, if wrong, decrement guesses left
      // if guess is correct, check if won
      isCorrect = check_guess(guess,displayword,word);
      if(!isCorrect) {
          numguesses--;
      } else if (isWon(displayword)) {
          numguesses = 255;
          send(c_sd,&numguesses,sizeof(numguesses),0);
          sprintf(boardbuffer,"%s",displayword);
          send(c_sd,boardbuffer,(size_t)wordlength,0);
      }
  }
  send(c_sd,&numguesses,sizeof(numguesses),0);
  sprintf(boardbuffer,"%s",displayword);
  send(c_sd,boardbuffer,(size_t)wordlength,0);
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

#define DICTIONARYPATH "./twl06.txt"

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
