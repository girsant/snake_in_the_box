#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <stddef.h>
#include <vector>
#include "def.h"
using namespace std;

// Variáveis GLOBAIS 
const int vertot = VERTOT;          // total de vértices do cubo 

vector <string> vert_bin;           // vetor com todos os vértices do cubo que representa o espaço de busca
                                    // elementos com string binário com os números dos vértices que estão disponíveis
                                    // para estender snakes e elementos com string "-1" são vértices não disponíveis

string viz_bin[SNAKE_DIMENSION];    // vetor com todos os vértices vizinhos de um determinado vértice. É utilizado pela 
                                    // função vizinhos  que coloca em viz_bin todos os vizinhos de um determinado vértice em
                                    // fmt string binário. Também é utilizado pela função vizinhos_spa que além de colocar
                                    // todos os vizinhos de um determinado vértice em viz_bin, coloca nos vizinhos que estão 
                                    // indisponíveis em  vert_bin (marcados com "-1") o mesmo valor "-1" no correspondente
                                    // vizinho. 

vector <string> vert_comp;          // vetor com todos os vértices da componente conexa alcançáveis e disponíveis no espaço
                                    // de busca vert_bin, a partir da cabeça da snake em questão. O tamanho de vert_comp,
                                    // chamado de fitness,  é a primeira e mais importante medida de adequação de uma
                                    // snake para avaliar a sua probabilidade de crescimento.

int snake_length;                   // variável que mantem o tamanho da snake corrente sendo processada

vector <int> trans;                 // vetor de inteiros c/ todas transições de uma determinada snake, da sua
                                    // cauda (índice == 0) até a sua cabeça (indice == tamanho do vetor trans - 1)

vector <string> vert_bins;          // vetor de vértices em fmt string binário com todos os vértices de uma
                                    // determinada snake da sua cauda (índice == 0) até a
                                    // sua cabeça (indice == tamanho do vetor vert_bins - 1).
                                    // O tamanho do vetor vert_bins é igual ao tamanho do vetor trans + 1.

// Função converte (vertice) do formato string binário p/ valor inteiro vint
int bin2int(const std::string& vertice) {
    int vint = 0;
    // Inicia c/ bit mais a esquerda
    int bitValue = 1 << (vertice.size() - 1); 

    for (char bit : vertice) {
        if (bit == '1') {
            vint |= bitValue; // Põe 1 no bit correspondente
        }
        bitValue >>= 1; // Vai p/ próximo bit
    }
    return vint;
}

// Função que imprime um vetor de vértices em fmt string binário
// usada somente para fins de debug 
void listbin(string tipo, vector <string> vert) {
    cout<<tipo;
    int vertex;
    for (int i=0; i < vert.size(); i++)
    {
        vertex = bin2int(vert[i]);
        if (i == vert.size()-1)
            cout<<vertex;
        else
            cout<<vertex<<", ";
    }
    cout<<" ]"<<endl;
    return;
}

// Função que imprime um vetor de vértices em fmt inteiro
// usada somente para fins de debug 
void listvint(string tipo, vector <int> vert, int vertot) {  
    cout<<tipo;
    for (int i=0; i < vertot; i++)
    {
    
        if (i == vertot-1)
            cout<<vert[i];
        else
            cout<<vert[i]<<",";
    }
    cout<<" ]\n";
    return;
}

// Função que coloca no array viz_bin em fmt string binário,
// os vizinhos de um vértice string v de entrada
void vizinhos(string v){
    string vx;
    for (int i=0; i < SNAKE_DIMENSION; i++)
    {
        vx = v;
        if (vx[i] == '0')
            vx[i] = '1';
        else 
            vx[i] = '0';
        viz_bin[i] = vx;
    }
    return;
}

// Função que transforma para fmt string binário os vértices correspondentes 
// a um vetor de transições trans em fmt inteiro, devolvendo os vértices 
// no vetor vert_bins 
void trans2vertb(const std::vector<int>& trans) {

    vert_bins.clear();
    // Inicializa vert_bins c/ '0' 
    vert_bins.push_back(std::string(SNAKE_DIMENSION, '0')); 

    for (int i = 0; i < trans.size(); i++) {
        // Guarda cópia do último vertice em ult_vertex
        std::string ult_vertex = vert_bins.back(); 
        int desl = SNAKE_DIMENSION - trans[i] - 1;
        ult_vertex[desl] = (ult_vertex[desl] == '0') ? '1' : '0';
        vert_bins.push_back(ult_vertex);
    }
}

// Cria e coloca em  vert_bin todos os vértices do cubo em
// formato string (binário) por exemplo: se dimensão do cubo ==  8,
// retorna em vert_bin = ["00000000"], ["00000001"], ... ["11111111"]
void ini_vert_bin(vector <string>& vert_bin, int vertot) {
    for (int i=0; i < vertot; i++)
        {
        string vb =  bitset<SNAKE_DIMENSION>(i).to_string();
        vert_bin.push_back(vb);
        }
    return; 
}

// Função que imprime um vetor de transições em fmt inteiro
// usada somente para fins de debug 
string listaTransitions (vector<int> vetrans, string tipo) {
int no = vetrans.size();
string texto = "";
cout<<tipo<<" length = "<<vetrans.size()<< " trans = [ ";
texto = to_string(vetrans.size()) + " " + tipo + " trans = [ ";
for(int i=0; i < no; i++)
    if (i == no-1)
    {
        cout<<vetrans[i]<< "";
        texto = texto + to_string(vetrans[i]) + "";
    }
    else
    {
        cout<<vetrans[i]<< ", ";
        texto = texto + to_string(vetrans[i]) + ", ";
    } 
    cout<<" ]"<<endl;
    texto = texto + " ]";
    return texto;
}

// Função que coloca no array viz_bin em fmt string binário,
// os vizinhos de um vértice string v de entrada, colocando
// o string "-1" quando o vértice do vizinho estiver em 
// posição indisponível em vert_bin (espaço de busca), e
// também retorna a quantidade de vizinhos disponíveis do vértice v
// no espaço de busca.
int vizinhos_spa(string v){
    int qtd_viz = 0;
    string vx;
    for (int i=0; i < SNAKE_DIMENSION; i++)
    {
        vx = v;     
        if  (vx[i] == '0')        // modifica um char de cada vez
            vx[i] = '1';          // se '0' troca para '1'
        else
            vx[i] = '0';          // se '1' troca para '0'

        // converte vértice atual vert_bin[i] para (fmt int) x
        int zin = bin2int(vx); 
        // verifica se vértice do vizinho atual está em posição proibida
        if ( vert_bin[zin] == "-1")     
            viz_bin[i] = "-1";
        else
            {
            viz_bin[i] = vx;      // guarda vizinho no array viz_bin
            qtd_viz += 1;
            }
    }
    return qtd_viz;
}

// Função que retorna o tamanho do vetor vert_comp que
// representa a quantidade de vértices alcançáveis a partir do
// vértice vorig que é o vértice da cabeça da snake sendo
// processada. Além disso esta função calcula também a quantidade de 
// vértices de pele (skin nodes) e coloca o resultado na variável
// global new_skin_fit.
int find_alcance(string vorig)
{
new_skin_fit = 0;
vert_comp.clear();
vert_comp.push_back(vorig);
int qtd_viz;

for (int k=0; k < vert_comp.size(); k++)
{
// coloca em viz_bin vértices vizinhos alcançáveis de vert_comp[k]
// coloca em qtd_viz a qtd de vértices vizinhos alcançáveis   "" 
    qtd_viz = vizinhos_spa(vert_comp[k]); 

// elimina vértices atingiveis q tem 0 vizinhos atingíveis
    if (qtd_viz == 0)                     
        {
        new_skin_fit = new_skin_fit + SNAKE_DIMENSION; 
        continue;
        }
    
    for (int i = 0; i < SNAKE_DIMENSION; i++)
    {
        if (viz_bin[i] == "-1")
            continue;
    
        int javis = 0;
        for (int j=0; j < vert_comp.size(); j++)
            {
                if (viz_bin[i] == vert_comp[j])
                    {
                        javis = 1;
                        break;
                    }
            }
        
        if (javis == 0)
            {   
        // com exceção daqueles da cabeça da snake corrente (vorig)
        // acumula em skin_fit os vértices não alcançáveis de vert_comp[k]
        // coloca vértice no vetor de vértices atingíveis 
            if (viz_bin[i] != vorig)                                       
                new_skin_fit = new_skin_fit + (SNAKE_DIMENSION - qtd_viz); 
            vert_comp.push_back(viz_bin[i]);                                
            }
    }
}
return vert_comp.size();
}

// A classe Transition define os objetos que representam as snakes.
// Cada snake é 1 caminho induzido em 1 grafo da família dos cubos.
// A primeira forma de representação das snakes poderia ser a
// sequência de seus vértices, expressos na base 10 ou na base 2. 
// Para compactar a representação das snakes encontradas, usa-se
// comumente, representar as snakes com a notação da sequência das transições
// dos vértices expressos na base 2. As transicoes são expressas
// na base 10. Vamos exemplificar, num cubo de dim 3 a snake de tamanho 3:
// Vertices em base 2:    snake_vertices   =  ["000", "001", "011", "111"]
// Transições na base 10: snake_transitions = [         "0",  "1",  "2"  ]
// A estrutura de dados escolhida para as snakes é uma lista encadeada
// de objetos da Classe Transition, representando uma arvore especial
// (grafo caminho) que contém somente vértices de grau 2 e 1.
// Em particular, essa árvore tem dois vértices terminais que tem grau 1,
// enquanto todos os outros têm grau 2. Cada  objeto Transition aponta
// somente para o seu objeto pai (father).

class Transition {
    public:
        Transition * father;
        int transition;
        int max_seen;
        int fitness;
        int skin_fit;

        Transition (Transition * father_in, int transition_in,
                    int max_seen_in, int fitness_in, int skin_fit_in) 
        {
            father     = father_in;
            transition = transition_in;
            max_seen   = max_seen_in;
            fitness    = fitness_in;
            skin_fit     = skin_fit_in;
        }

        // Sobrepõe o  operador "<" para fins de comparação
        // Min-heap: classificada em ordem ascendente de fitness
        bool operator<(const Transition& other) const {
            return fitness > other.fitness; 
        }

// Método transition_sequence ... chamada:  v->transition_sequence() 
// a partir do ponteiro para o objeto da Classe Transition que aponta
// para a última transição, retorna o vetor trans com toda a sequência
// de transições da snake
        vector<int> transition_sequence() {
            
            vector<int>result;
            result.push_back(transition);
            Transition * nextfather = father;      
            while (nextfather != nextfather->father)
            {
                result.push_back(nextfather->transition);
                nextfather = nextfather->father;   
            }

            vector<int> newvector (result.rbegin(), result.rend());           
        
        return newvector;

        }

// Método is_snake  recebe a próxima transição em transx e o vértice cabeça
// da snake em v. Devolve "true" se após o acréscimo de transx,
// o caminho continuar a  ser uma snake
// Coloca também nas variáveis globais  new_fitness  e  new_skin_fit as 
// duas medidas de adequação de cada nova snake estendida
bool is_snake(int transx, Transition * v) {
// variável que recebe valor decimal inteiro de cada vértice em fmt string binário
int verint;   // para poder marcar vértices indisponíveis no espaço de busca ( vetor vert_bin em fmt string binário)

// Prepara vetor trans c/ transições snake anterior + nova transição (a testar)
trans.clear();
trans = v->transition_sequence();
trans.push_back(transx);

// Chama função trans2vertb para transformar o vetor de transições trans
// em  vetor de vértices em fmt string binário
trans2vertb(trans);

// esvazia vector <string> vert_bin e chama ini_vert_bin para 
// colocar em  vector <string> vert_bin a lista de todos os vértices
// do cubo em fmt binario
vert_bin.clear();                    
ini_vert_bin(vert_bin, vertot);       

for (int i = 1; i < vert_bins.size(); i++)
{
    // converte vértice atual vert_bin[i] para (fmt int) verint
    verint = bin2int(vert_bins[i]);
    // verifica se vértice atual está em posição proibida
    if ( vert_bin[verint] == "-1")    
    {
        return false;
    }
    vizinhos(vert_bins[i-1]);
    for (int j=0; j < SNAKE_DIMENSION; j++)
    {
        // converte vértice vizinho atual viz_bin[i] para (fmt int) verint
        verint = bin2int(viz_bin[j]); 
        // marca indisponibilidade em todos os vértices vizinhos do
        // vértice anterior da snake inclusive aquele de transx
        vert_bin[verint] = "-1";                        
    }                                                   
}
// atualiza snake_length para o novo tamanho da snake corrente
snake_length = trans.size();                            

// coloca em  vorig o vértice da cabeça da snake corrente
string vorig = vert_bins[vert_bins.size() - 1];

// Chama a função para calcular new_fitness e new_skin_fit  (as 2 adequações).
// A atribuição na chamada coloca  em new_fitness a quantidade de vértices alcançáveis
// a partir do vértice da cabeça da snake corrente.
// Coloca na global new_skin_fit = a quantidade de vértices do tipo skin.
new_fitness = find_alcance(vorig);                                                                                                                                      
return true;
}
};




