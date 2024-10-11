// PARÂMETROS QUE PODEM SER ALTERADOS A CADA PROCESSAMENTO
#define SNAKE_DIMENSION 8
#define TXRAND1 2               // PERCENTUAL PARA TROCA DE SNAKES COM QQ
                                // FITNESS E SKIN_FIT PARA FUGIR DE MÁXIMOS LOCAIS 

#define TXRAND2 90              // PERCENTUAL PARA TROCA DE SNAKES COM             
                                // MESMA FITNESS E MESMA SKIN_FIT EM
                                // NEW_BEAM E NEW_SEQUENCES 

#define SIZE_BEAM     4000000     // TAMANHO DO beam 
#define SIZE_NEW_BEAM 4000000     // TAMANHO DO new_beam
#define DEBUGAR  4


// PARÂMETROS PARA NÃO SEREM ALTERADOS A CADA PROCESSAMENTO
#define TWO_TO_THE_N (1 << SNAKE_DIMENSION) // CALCULA O NÚMERO DE VÉRTICES DO CUBO 

// DEFINE VERTOT E IGUALA A TWO_TO_THE_N    SNAKE_DIMENSION 3   4   5   6    7    8    9    10     11
#define VERTOT  TWO_TO_THE_N             // VERTOT          8  16  32  64  128  256  512  1024   2048 

#define MAX_DIMENSION SNAKE_DIMENSION -1    // DIMENSÃO MÁXIMA (DIMENSÃO DO CUBO - 1) PORQUE COMEÇA DE 0
extern int new_fitness;         // new_fitness DEFINIDA EM  beam.cpp E USADA EXTERNAMENTE EM trasitionClass.cpp
extern int snake_length;        // snake_length DEFINIDA EM  beam.cpp E USADA EXTERNAMENTE EM trasitionClass.cpp 
extern int debugar;             // debugar DEFINIDA EM  beam.cpp E USADA EXTERNAMENTE EM trasitionClass.cpp
extern int new_skin_fit;        // new_skin_fit DEFINIDA EM  beam.cpp E USADA EXTERNAMENTE EM trasitionClass.cpp
