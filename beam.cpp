#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <stddef.h>
#include <vector>
#include <queue>
#include <algorithm>
#include <time.h>
#include <chrono>
#include <unistd.h>
using namespace std;
#include "transitionClass.cpp"  // inclui a Classe Transition e Funções Auxiliares

// VARIÁVEIS E ESTRUTURAS GLOBAIS
int new_fitness;  // total de vértices alcançáveis de uma nova subsolução de snake
int new_skin_fit; // total de vértices de pele (skin nodes) de uma nova subsolução de snake
// valor máximo e valor mínimo de fitness a cada extensão do beam ( tamanho t para t+1)
int max_fit;     
int min_fit;
// valor máximo e valor mínimo de skin_fit a cada extensão do beam ( tamanho t para t+1)
int min_skinfit;
int max_skinfit;
// Inteiros gerados aleatóriamente entre 1 e 100 para nossa rotina de randomicidade
int irand1;
int irand2;
// Parâmetros de randomização variáveis ajustados a cada corrida definidos em def.h
int def_irand1 = TXRAND1 + 1;
int def_irand2 = TXRAND2 + 1;
// Totais de substituições feitas ao acaso (irand1 x def_irand1) ou  (irand2 x def_irand2)
int tot_rand1 = 0;
int tot_rand2 = 0;

// variáveis auxiliares
Transition *snake;      // variável para guardar o ponteiro para um node snake
int debugar = DEBUGAR;  // variável para controle de debug (executar ou não instruções de cout's<< ...)
string nomeArq = "";    // variável para colocar o nome do arquivo onde serão gravados os resultados do processamento

// variáveis de tamanho para controle do beam e definição do array beam
int ind_beam = 0;   // ind_beam é o tamanho do beam na iteração atual (pode ser menor do que o máximo)
int max_beam         =      SIZE_BEAM;
int ind_beam_anter; // ind_beam_anter é o tamanho do beam na iteração anterior (pode ser menor do que o máximo)
int ind_beam1;      // ind_beam1 é onde salvo temporariamente o tamanho do beam na iteração atual
Transition * beam     [SIZE_BEAM]; // definição do array beam


// variáveis de tamanho para controle do new_beam e definição da min-heap new_beam
int ind_new_beam;   // ind_new_beam é o tamanho do new_beam da iteração atual (pode ser menor do que o máximo)
int max_new_beam     =      SIZE_NEW_BEAM; // tamanho máximo do new_beam

// Define vetor new_sequences onde serão colocados os ponteiros para as extensões de uma das snakes do beam
vector<Transition *> new_sequences;
const size_t MAX_NEW_SEQUENCES_SIZE = SNAKE_DIMENSION - 2;

// Define vetor vetortran onde serão colocados os ponteiros para as extensões de uma das snakes do beam
vector<int> vetortran;
const size_t MAX_VETORTRAN_SIZE = SNAKE_DIMENSION - 2;

// Define new_beam a priority queue de ponteiros para objetos da Classe Transition onde vamos tentar colocar todas as 
// extensões das snakes de todas as new_sequences geradas
struct TransitionPtrComparator {
    bool operator()(const Transition* p1, const Transition* p2) const {
        return *p1 < *p2;    //  menor fitness no topo de new_beam                                              
    }
};
priority_queue<Transition*, vector<Transition*>, TransitionPtrComparator> new_beam;
// ver na Classe Transition a sobreposição do operador para transformar a max-heap em min-heap 

// variáveis para controlar as quantidades de nodes gerados e deletados
int del_newseq_newbeam = 0; // nodes deletados por não aproveitamento ao passar de newsequ para new_beam
// nodes deletados ao reduzir tamanho de new_beam antes colocar as subsoluções de new_beam em beam 
int del_newbeam_beam = 0;   
int nodes_del;              // total gearl de nodes deletados
int pool_nodes = 4;         // total de nodes gerados



// FUNÇÕES DO PROGRAMA ***********************************************************

// Reserva o espaço máximo do vetor new_sequences e do vetor vetortran para evitar
// realocações visto que estes vetores têm tamanhos dinâmicos
void init_new_sequences_global(){
new_sequences.reserve(MAX_NEW_SEQUENCES_SIZE);
vetortran.reserve(MAX_NEW_SEQUENCES_SIZE);
}

// Gera número inteiro aleatório a cada chamada
int numeroAleatorio(int menor, int maior) {
       return rand()%(maior-menor+1) + menor;
}

// A função grava_arq abre um arquivo no modo de append e adiciona o string "texto" ao final desse arquivo.
// Se o arquivo não existir, ele será criado. Se ele já existir, o texto será simplesmente adicionado ao final
// sem alterar o conteúdo anterior.
void grava_arq(string nomeArq, string texto) {
ofstream  fout;
fout.open(nomeArq, ios::app);
fout<<texto<<endl;
fout.close();
}

// Ao final do processamento, esta função imprime na console e grava as principais informações do processamento
// usando a cada linha a função grava_arq
void tempo (int secs) {
    int hours = 0;
    int mins  = 0;
    if (secs >= 3600)
        {
            hours = secs / 3600;
            secs = secs % 3600;               
        }
    if (secs >= 60)
        {
            mins = secs / 60;
            secs = secs % 60;
        }   
    char out_string [200];
    sprintf(out_string, "tempo decorrido ->>> %i hora(s) %i minuto(s) %i segundo(s)", hours,mins, secs);
    grava_arq (nomeArq, out_string);
    cout<<out_string<<endl;
    sprintf(out_string, "dimensão = %i  SIZE_BEAM = %i SIZE_NEW_BEAM = %i TXRAND1 = %i TXRAND2 = %i", SNAKE_DIMENSION, SIZE_BEAM, SIZE_NEW_BEAM, TXRAND1, TXRAND2);
    grava_arq (nomeArq, out_string);
    cout<<out_string<<endl;
    nodes_del =  del_newseq_newbeam + del_newbeam_beam;
    sprintf(out_string, "DUAS FITNESSES - NODES CRIADOS = %i  NODES ELIMINADOS = %i  NODES OCUPADOS = %i  NODES SUBST ACASO %i  NODES SUBST EQ FITS %i ", pool_nodes, nodes_del, pool_nodes-nodes_del, tot_rand1, tot_rand2);
    grava_arq(nomeArq, out_string);
    cout << out_string << endl;
    grava_arq(nomeArq, "***************************************************************************************************************************************"); 
}

// Ao final de cada ciclo do  processamento, estendendo as subsoluções de tamanho "t" em beam para tamanho "t+1" en new_beam, 
// esta função imprime na console e grava as principais informações do processamento // usando a cada linha a função grava_arq
void tempo1 (int secs) {
    int hours = 0;
    int mins  = 0;
    if (secs >= 3600)
        {
            hours = secs / 3600;
            secs = secs % 3600;               
        }
    if (secs >= 60)
        {
            mins = secs / 60;
            secs = secs % 60;
        }   

    char out_string [200];
    sprintf(out_string, "tempo decorrido ->>> %i hora(s) %i minuto(s) %i segundo(s)", hours,mins, secs);
    cout<<out_string<<endl;
    sprintf(out_string, "dimensão = %i  SIZE_BEAM = %i SIZE_NEW_BEAM = %i TXRAND1 = %i TXRAND2 = %i", SNAKE_DIMENSION, SIZE_BEAM, SIZE_NEW_BEAM, TXRAND1, TXRAND2);
    cout<<out_string<<endl;
    nodes_del =  del_newseq_newbeam + del_newbeam_beam;
    sprintf(out_string, "DUAS FITNESSES - NODES CRIADOS = %i  NODES ELIMINADOS = %i  NODES OCUPADOS = %i", pool_nodes, nodes_del, pool_nodes-nodes_del);
    cout << out_string << endl;
}

// Esta função imprime na console e grava os maiores resultados obtidos no  processamento
// usando a cada linha a função grava_arq
void finaliza(Transition * v, int cont_atual, int cont_anter, string nomeArq){
    trans.clear();
    trans = v->transition_sequence();
    string texto;
    int i = 0;
    string s_cont_atual = to_string(cont_atual);
    string s_cont_anter = to_string(cont_anter);
    texto = listaTransitions(trans, "LONGEST SNAKE " + s_cont_atual + " OF " + s_cont_anter);
    grava_arq (nomeArq, texto);
    }

// Esta função recebe uma variável ponteiro v de um node, prepara e
// retorna um vetor de ponteiros para nodes criados para expandir o
// beam. Cada ponteiro no vetor de retorno representa uma expansão
// da snake anterior, adicionando uma nova aresta à snake anterior,
// com a criação de um novo objeto do tipo node Transition.
// Ao tentar criar o node da extensão, é obedecida a regra para
// criar snakes na forma canônica: a transição do node sendo criado
// só pode ser uma unidade maior do que as transições anteriores dos
//  nodes precedentes na snake. Os próximos nodes criados não podem
// ter também transições iguais às 2 antecedentes ( current ou previous)
void increase(Transition * ptrans) {
new_sequences.clear();                  // limpa vetor new_sequences a ser usado 
vetortran.clear();                      // limpa vetor vetortran     a ser usado 
int vmaxseen =       ptrans->max_seen;  // coloca o maior valor de transição até então 
                                        // na snake base a ser estendida para garantir
                                        // que as novas transições irão ser maiores
                                        // do que a maior das anteriores 
int trans_current  = ptrans->transition; // coloca em trans_current a atual transição da subsolução
int trans_previous = ptrans->father->transition;    // coloca em trans_previous a transição anterior da subsolução 
int next_dimension = min(vmaxseen+1, MAX_DIMENSION);  // calcula a próxima dimensão a ser acrescentada para extensão         

// Rotina que cria as próximas transições possíveis para estender a subsolução apontada em ptrans
for (int i = 0; i <= next_dimension; i++)
    {
            if ( i != trans_current and i != trans_previous)             
                vetortran.push_back(i);
    }

// Rotina que pega cada nova transição possível em vetortran, chama o método issnake da Classe Transition 
// para verificar se é uma snake, e se for cria um novo node da Classe Transition para apontar para a 
// nova snake recém criada
for (int i=0; i < vetortran.size(); i++)
    {  
        int transx = vetortran[i];      // coloca na variável auxiliar transx a nova transição a ser verificada
        bool issnake = ptrans->is_snake(transx, ptrans);    // chama método isssnake p/ verificar se é uma snake

        // se nova transx for uma snake então, atualiza a maior transição até esta nova snake em next_seen, 
        //  cria um novo node da Classe Transition e adiciona o ponteiro (u) para ele no vetor new_sequences
        if (issnake)
            {
            int next_seen = max(vetortran[i], vmaxseen); // atualiza em next_seen a maior transição até esta nova snake
            // cria novo node    Transition (&father, transition, max_seen, fitness,     skin_fit)
            Transition * u = new Transition( ptrans,  transx,     next_seen,new_fitness, new_skin_fit);
            new_sequences.push_back(u);
            pool_nodes += 1;   
            }
    }

}

// Função para estender todas as subsoluções snake no array beam[], chamando
// a função increase para estender cada uma das snakes (de tamanho t) e que
// retorna as snakes estendidas (de tamanho t+1) no vetor new_sequences. Para cada
// vetor new_sequences recebido a increase_beam pode:
// 1- se todas as novas snakes em new_sequences couberem em new_beam adiciona-as a
// new_beam e torna a chamar a a função increase.
// 2- se a quantidade máxima de snakes no min-heap new_beam  já tiver sido atingida,
//  esta rotina faz a seleção e decide se vai usar a nova snake estendida em new_sequences 
// para substituir o topo da min-heap new_beam ou se vai descartar a nova snake criada em
// new_sequences. Ao decidir descartar uma das 2 snakes ( a de new_seuqnces ou anterior do
// topo da min-heap new_beam), a função atualiza os contadores de nodes da forma adequada.
void increase_beam() 
{
    srand((unsigned)time(NULL));       // para gerar números aleatórios reais
    for (int i = 0; i < max_beam; i++) // VARRE TODO O BEAM PARA TENTAR O CRESCIMENTO DAS SNAKES
    {
        if (beam[i] == 0) // SE BEAM INCOMPLETO ( FOI ATINGIDO BEAM[I] = ZERO) INTERROMPE A INCREASE_BEAM
            break;

        // chama a função increase   
        increase(beam[i]);

        // retorna da função increase e processa as snakes de tamanho t+1 recebidas em new_sequences
        if (new_sequences.size() == 0)  // se não houve nenhuma snake de tamanho t+1 volta ao loop 
            continue;                   // do for para chamar a increase para estender a próxima snake 
                                        // do array beam[]                                                  

            for (int j=0; j < new_sequences.size();j++) // loop para processar cada nova snake de tamanho t+1
                                                        // em new_sequences
            {   
                if (ind_new_beam < max_new_beam)        // se ainda não foi atingido o tamanho máximo de
                                                        // new_beam vamos anexar a nova snake no min-heap
                                                        // new_beam e atualizar as variáveis necessárias
                {   
                new_beam.emplace(new_sequences[j]);     // insere nova snake de new_sequences corrente em new_beam
                                                        // mantendo a snake de menor fitness no topo de new_beam.
                ind_new_beam +=1;                       // atualiza o índice que mostra o tamanho atual de
                                                        // new_beam
                // atualiza as variáveis com as informações de máximos e mínimos do ciclo da increase_beam
                max_fit = max(max_fit, new_sequences[j]->fitness);
                min_fit = min(min_fit, new_sequences[j]->fitness);
                max_skinfit = max(max_skinfit, new_sequences[j]->skin_fit);
                min_skinfit = min(min_skinfit, new_sequences[j]->skin_fit);  
                } 
                // ind_new_beam >= max_new_beam  portanto não consegui
                // colocar em new_beam todas as snakes criadas na increase
                else                                        
                
                { 
                    // obtendo em px o topo atual do min-heap para
                    // poder decidir se o topo atual será, mais adiante
                    // substituído pela snake apontada p/new_sequences[j]
                    Transition* px = new_beam.top(); 

                    // Gera número aleatório irand1 entre 1 e 100 para ver se vamos
                    // trocar o topo da min-heap por uma snake qualquer, fazemos isso 
                    // para fugir de snakes máximas locais 
                    irand1 = numeroAleatorio(1, 100);
                    // se vamos trocar o topo por uma snake qq ao acaso soma 1 a tot_rand1                
                    if (irand1 < def_irand1)
                        tot_rand1 += 1;

                    // Gera número aleatório irand2 entre 1 e 100 para ver se vamos
                    // trocar o topo da min-heap por uma snake nova que tem as 
                    // mesmas medidas de adequação da snake que está no topo  
                    irand2= numeroAleatorio(1, 100);

                    // verifica se vamos trocar o topo por uma snake qq ao acaso     
                    if  ( (irand1 < def_irand1) or 
                    // ou se vamos trocar o topo por uma snake c/ mesmas fitnesses
                        ( (irand2 < def_irand2)   and  (new_sequences[j]->fitness == px->fitness and new_sequences[j]->skin_fit == px->skin_fit) )   or
                    // ou se vamos trocar o topo por uma snake com melhor fitness                    
                        (new_sequences[j]->fitness > px->fitness) or
                    // ou se vamos trocar o topo por uma snake com igual fitness
                    // mas que tem melhor skin_fit do que a snake no topo do min-heap                    
                        (new_sequences[j]->fitness == px->fitness and new_sequences[j]->skin_fit > px->skin_fit) 
                    )
                    // caso uma das 4 condições seja verdadeira, vamos descartar a snake do topo de new_beam e vamos 
                    // substituí-la pela nova snake em new_sequences
                    {
                    new_beam.pop();
                    delete(px);
                    if ( (irand2 < def_irand2)   and  (new_sequences[j]->fitness == px->fitness and new_sequences[j]->skin_fit == px->skin_fit and irand1 >= def_irand1 ) )
                        tot_rand2 += 1; 
                    del_newseq_newbeam += 1;
                    new_beam.emplace(new_sequences[j]);     // insere nova snake de new_sequences corrente em new_beam
                                                            // mantendo a snake de menor fitness no topo de new_beam.
                    // atualiza as variáveis com as informações de máximos e mínimos do ciclo da increase_beam
                    max_fit = max(max_fit, new_sequences[j]->fitness);
                    min_fit = min(min_fit, new_sequences[j]->fitness);
                    max_skinfit = max(max_skinfit, new_sequences[j]->skin_fit);
                    min_skinfit = min(min_skinfit, new_sequences[j]->skin_fit);
                    } 

                    else
                    // se nenhuma das 4 condições for verdadeira então descarta a nova snake em new_sequences 
                    // e atualiza o contador de novas snakes em new_sequences deletadas
                    { 
                    delete(new_sequences[j]);
                    del_newseq_newbeam += 1; 
                    }
                }                  
            }
    } 
}
// PROGRAMA PRINCIPAL
int main(int argc, char * argv[]) {

// obtem dimensão do cubo e se for < 4  encerra
// com as soluções triviais
int snake_dimension = int(SNAKE_DIMENSION);
if (snake_dimension == 1)
    {
    cout << "LONGEST SNAKE 1 OF 1 length = 1 trans = [ 0 ]" << endl;
    exit(0);
    }
if (snake_dimension == 2)
    {
    cout << "LONGEST SNAKE 1 OF 1 length = 2 trans = [ 0, 1]" << endl;
    exit(0);
    }  
if (snake_dimension == 3)
    {
    cout << "LONGEST SNAKE 1 OF 1 length = 3 trans = [ 0, 1, 2]" << endl;
    exit(0);
    } 

// Obtem variáveis para contabilizar o tempo de execução
auto start_time = chrono::steady_clock::now();
auto current = chrono::system_clock::now();
auto now_timet = chrono::system_clock::to_time_t(current);
auto now_local = localtime(&now_timet);

// exibe na console a data e hora de início do programa beam.cpp
cout << "DATA E HORA DE INÍCIO DO PROGRAMA =  " << put_time(now_local, "%c") << endl;

// Monta em nomeArq o nome do arquivo onde serão armazenados os resultados
// do processamento do programa beam obtendo um string com a data e hora atual
// em segundos + um numero randomico gerado no arquivo  beam.sh a partir dos
// argumentos recebidos pela função main(), que lhes foram passados na 
// chamada do programa beam executável
for (int n = 0; n < argc; n++) 
       nomeArq += argv[n];
nomeArq += ".txt";

// reserva espaço máximo para vetores new_sequences e vetortran
init_new_sequences_global(); 

// Cria o primeiro node uz cujo pai (father) aponta para ele mesmo e depois
// cria os 3 primeiros nodes de uma snake, que representam a primeira snake de
// tamanho 3 de um cubo (de qualquer dimensão > do que 3), na forma canônica snake = [0, 1, 2]
Transition uz=Transition(&uz, 0,0,0,0);   // uz nó fantasma NULL que não é processado
Transition u0=Transition(&uz, 0,0,1,0);   // u0 (root) aponta para nó fantasma z (NULL)
Transition u1=Transition(&u0, 1,1,1,0);   // u1 aponta para o u0 (root)
Transition u2=Transition(&u1, 2,2,1,0);   // u2 aponta para o u1   Transition (&father, transition, max_seen, fitness, skin_fit)

// cria o array beam (de pointers para as cabeças das snakes) com apenas uma snake,  cabeça da primeira snake = []
// este array começa com somente um ponteiro para um objeto do tipo Transition que é o node terminal (cabeça da snake = [0, 1, 2]) e vai crescendo 
// através das chamadas das funções increase_beam ( que por sua vez chama a função increase). Quando a função increase retorna um array new_beam vazio, 
// isso significa que o beam não pode mais crescer, sendo as snakes do último beam as maiores possíves 
// encontradas no processamento do programa

beam[0] = &u2;

// Loop eterno tentando estender todas as snakes de tamanho t no 
// array beam[] para tamanho t+1, chmando a função increase_beam
while (true){
    // inicializa as variáveis de controle do loop
                                    // max_new_beam é tamanho máximo do new_beam
    ind_new_beam = 0;               // ind_new_beam é o tamanho do new_beam da iteração atual
    max_fit = 0;
    min_fit = 999999999;
    max_skinfit = 0;
    min_skinfit = 999999999;

    // se variável debugar for > 2 exibe na console a data e hora de início de cada 
    // ciclo de crescimento das snakes de tamanho t para snakes de tamanho t+1
    if (debugar > 2)
        {
        cout<<"chamei o increase_beam "<<" - max_new_beam = "<<max_new_beam<<endl;
        auto start_time = chrono::steady_clock::now();
        auto current = chrono::system_clock::now();
        auto now_timet = chrono::system_clock::to_time_t(current);
        now_local = localtime(&now_timet);
        cout << "DATA E HORA DE INÍCIO DESTE CICLO =  " << put_time(now_local, "%c") << endl;
        }
    
    // Chama a função increase_beam que vai processar as snakes no array global beam[]
    increase_beam();

//  ao retornar da chamada da increase_beam, se variável debugar > e exibe na console
//  as informações de controle do ciclo 
    if (debugar > 2)   
     cout<<"voltei do increase_beam max_fit = "<<max_fit<<"   min_fit = "<<min_fit<<" max_skinfit = "<<max_skinfit<<"  min_skinfit = "<<min_skinfit<<
           "  ind_new_beam = "<<ind_new_beam<<" tot_rand subst acaso = "<<tot_rand1<<"  tot_rand subst eq fits = "<<tot_rand2<<endl;
     
// ##### FINALIZA AQUI O PROGRAMA SE NEW_BEAM ESTIVER VAZIO POIS
//  NÃO HOUVE CRESCIMENTO DE NENHUMA SNAKE DO BEAM ANTERIOR !!! ##### 
    if (ind_new_beam == 0)                         // se não houve nenhum crescimento -> finaliza com a snake em beam[0]
    {
        if (debugar > 2)    // se debugar > 2 exibe e grava até no máximo as 4 maiores snakes encontradas 
            {
                cout<<"finalizei logo depois do increase_beam sem crescimento ind_new_beam = 0 e ind_beam = "<<ind_beam<<endl;
                for (int j=0; j<ind_beam; j++)
                {
                if (j > 3)  break;
                snake = beam[j];
                finaliza(snake, j+1, ind_beam, nomeArq);
                }
            }
        else                // se debugar for <= 2 exibe e grava somente uma dentre as maiores snakes encontradas  
            {
            cout<<"finalizei logo depois do increase_beam sem crescimento ind_new_beam = 0"<<endl;
            snake = beam[0];
            finaliza(snake, 1, ind_beam, nomeArq);
            }

        // se ind_new_beam == 0 exibe na console e grava as informações de duração do processamento do programa beam
        // e ENCERRA O PROGRAMA. 
        auto end = chrono::steady_clock::now();
        int secs = chrono::duration_cast<chrono::seconds>(end - start_time).count();
        tempo(secs);                                          // 1
        nodes_del = del_newbeam_beam + del_newseq_newbeam;
       
        cout << "**************************************************************************************************" << endl;
        cout<<endl;
        exit(0);
    }
   
// Caso new_beam não esteja vazio, houve algum crescimento, então
// executa a rotina para copiar snakes de new_beam para o próximo beam
    
    if (debugar > 2)
        cout<<"antes da cópia rotina beam = new_beam ... ind_new_beam = "<<ind_new_beam<<"  max_beam = "<<max_beam<<endl;
 
    ind_beam = 0;   //  inicializa ind_beam com 0 (o indice do beam)                                   
  
// Rotina que copia todas as snakes de new_beam para beam até o limite máximo = max_beam

// Primeiramente se o tamanho de new_beam for maior do que o máximo possível do array beam[]
// no loop while vamos eliminar todas as snakes de menor fitness de new_beam de forma a ficarem
// em new_beam somente as snakes de maiores fitnesses que couberem no array beam.
// No corpo do while vamos eliminando o topo de new_beam e deletando os nodes correspondentes
// para limpar e conseguir otimizar a memória principal.    
    if (new_beam.size() > max_beam)
        {
            while (new_beam.size() != max_beam)
                {
                Transition* px = new_beam.top(); 
                new_beam.pop();
                delete(px);
                del_newbeam_beam += 1;  
                }
        }

// No corpo do loop while abaixo vamos copiar as snakes de new_beam para beam[]
// e atualizamos o ind_beam somando 1 a cada nova snake copiada   
    while (!new_beam.empty())
        {
        Transition* px = new_beam.top();
        beam[ind_beam] = px;
        new_beam.pop();
        ind_beam += 1;
        }

// Nesta rotina verificamos se o beam ficou incompleto pois existiam 
// menos snakes em new_beam do que a capacidade máxima de beam.
// Neste caso temos que zerar os elementos restantes do array beam
// pois podem estar com snakes do ciclo anterior
    ind_beam1 = ind_beam;
    while (ind_beam1 < max_beam)            
        {
            if (beam[ind_beam1] == 0)           // zera o restante do beam (até o final)
                break;
            beam[ind_beam1] = 0;
            ind_beam1++;
        }

    if (debugar > 2)
        cout<<"passei pela cópia beam = new_beam com  ind_new_beam = "<<ind_new_beam<<" ind_beam = "<<ind_beam<<endl;     

// Fim da Rotina beam = new_beam

// Rotina para imprimir as 4 primeiras snakes intermediarias de cada ciclo que já estão em beam
    for (int k=0; k<ind_beam; k++)
        if (k < 4)
        {
        trans.clear();
        trans = beam[k]->transition_sequence();
        cout<<"Snake # "<<k+1<<" de "<<ind_beam<<" ";
        listaTransitions(trans, "");  
        }

    auto end = chrono::steady_clock::now();
    int secs = chrono::duration_cast<chrono::seconds>(end - start_time).count();
    tempo1(secs);     //  não grava, só imprime
    cout<<"**************************************************************************************************"<<endl;

}           // final do while (true)

return 0;
}           // final do main()
