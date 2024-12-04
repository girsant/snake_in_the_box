// Variáveis GLOBAIS 
const int vertot = VERTOT;          // total de vértices do cubo 

vector <string> vert_bin;           // vetor com todos os vértices do cubo em fmt string binário
                                    // vértices com string "-1" são vértices não disponíveis

vector <string> vert_bin_fixo;      // vetor com todos os vértices do cubo usado para 
                                    // inicializar o vert_bin com todos os vértices disponíveis

string viz_bin[SNAKE_DIMENSION];    // vetor com todos os vértices vizinhos de um determinado vértice. 

vector <string> viz_bin_ind;        // vetor com todos os vértices vizinhos indisponíveis de um determinado vértice 
                                    // alcançável a partir da cabeça da snake e que não estão na própria snake

vector <string> vert_comp;          // vetor com todos os vértices da componente conexa alcançáveis e disponíveis
                                    // no hipercubo, a partir da cabeça da snake em questão. 

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
// em fmt string binário no vetor vert_bins 
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
// usada somente para fins de debug e na finalização
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
// no hipercubo.
int vizinhos_spa(string v){
    string vx;
    viz_bin_ind.clear();
    int qtd_viz = 0;
    for (int i = 0; i < SNAKE_DIMENSION; i++)
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
            {
            // se vértice indisponível e não faz parte dos vértices da snake
            // então é um vértice candidato a ser um nó de pele (skin node)   
            if (std::count(vert_bins.begin(), vert_bins.end(), vx) == 0)  
                viz_bin_ind.push_back(vx);                                 
            viz_bin[i] = "-1";
            }
        else
            {
            viz_bin[i] = vx;   // guarda vizinho alcançável no array viz_bin
            qtd_viz += 1;      // conta vizinhos alcançáveis em qtd_viz
            }
    }
    return qtd_viz;
}

// Função retorna o tamanho do vetor vert_comp que representa a qtd
// de vértices alcançáveis a partir de vorig que é o vértice da cabeça 
// da snake sendo processada. Calcula também a qtd de vértices de pele 
// (skin nodes) e coloca o resultado na variável global new_skin_fit.
int find_alcance(std::string vorig)
{
    int qtd_viz;  // qtd vértices vizinhos do vértice sendo processado
    int ac_viz_zero = 0;         // contador de vértices Dead End Nodes
    new_skin_fit = 0; // global onde se calcula a qtd total de vértices skin
    vert_comp.clear();          // global (tamanho final é a medida de fitness
    vert_comp.push_back(vorig); // inicializa com vértice cabeça da snake
    // Cria um unordered set (vazio) p/checar se vértice já está em vert_comp
    std::unordered_set<std::string> visit_vert_comp;
    visit_vert_comp.insert(vorig);
    // Cria um unordered set (vazio) p/checar se vértice já computado em skin_fit
    std::unordered_set<std::string> visit_skin_fit;
    // LOOP PRINCIPAL - enquanto houverem vértices alcançáveis
    for (int k = 0; k < vert_comp.size(); k++) 
    {   // Processando vert_comp[k]
        // acha vizinhos da cabeça ou de qq outro vértice alcançável
        qtd_viz = vizinhos_spa(vert_comp[k]); 
        // Rotina para apurar a fitness
        // loop para processar os vizinhos de vert_comp[k]
        for (int i = 0; i < SNAKE_DIMENSION; i++) 
        {   // verifica se viz_bin[i] é um  Dead End Node e não conta para fitness
            if (qtd_viz == 0)       
                {   ac_viz_zero += 1;  // contador de vértices Dead End Nodes
                    break; }
            if (viz_bin[i] == "-1")
                continue; // se vizinho  é  inalcançável -> avança para o próximo
            // Verifica se vértice alcançável em viz_bin[i] já foi visitado 
            if (visit_vert_comp.find(viz_bin[i]) == visit_vert_comp.end()) 
            {   // se viz_bin[i] não está em vert_comp[]
                vert_comp.push_back(viz_bin[i]);    // insere viz_bin[i] em vert_comp[]
                visit_vert_comp.insert(viz_bin[i]); // marca como visitado
            }
        }
        // Rotina para apurar a new_skin_fit
        for (int j = 0; j < viz_bin_ind.size(); j++)
        {   if (vert_comp[k] == vorig)
                break;       
            // verifica se viz_bin_ind[j] ainda não foi visitado e computado como nó de pele
            if (visit_skin_fit.find(viz_bin_ind[j]) == visit_skin_fit.end())   
            {   new_skin_fit += 1; // se ainda não visitado soma 1 a skin_fit
                visit_skin_fit.insert(viz_bin_ind[j]);} // marca viz_bin_ind[j] como visitado    
        }  
    }       // FIM DO LOOP PRINCIPAL         

    // PREPARA PARA RETORNO   
        // Cálculo do fitness: se tamano_fitness for negativo retorna ZERO
        int tamanho_fitness =  vert_comp.size() - 1 - ac_viz_zero;
        if (tamanho_fitness < 0)
            tamanho_fitness = 0;    // Garante que o valor não seja negativo
        return tamanho_fitness;  // Retorna o valor de fitness ou 0, caso seja negativo
        // retorna a quantidade de vértices alcançáveis (fitness)
        // menos a cabeça da snake e menos os vértices DEAD END NODES
}       // e também os vértices de skin na global new_skin_fit 
                     

// A classe Transition define os objetos que representam as snakes.
// Cada snake é um caminho induzido num grafo da família dos cubos.
// Para compactar a representação das snakes encontradas, usa-se
// comumente, representar as snakes com a notação da sequência das transições
// dos vértices expressos na base 2. As transicoes são expressas na base 10. 
// A estrutura de dados escolhida para as snakes é uma lista encadeada
// de objetos da Classe Transition, representado uma arvore especial
// Cada snake é um caminho da árvore. Em particular, todos os caminhos nessa
// árvore tem a mesma origem comum no vértice 0 e têm dois vértices terminais
// que tem grau 1, enquanto todos os outros vértices internos têm grau 2. 
// Cada  objeto Transition aponta para o seu objeto pai (father).

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
        // Min-heap: classificada em ordem ascendente de fitness e de skin_fit
        bool operator<(const Transition& other) const {
            if (fitness != other.fitness) {
                return fitness > other.fitness;  // Ordem crescente de fitness
            }
            return skin_fit > other.skin_fit;    // Ordem crescente de skin_fit se fitness for igual 
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

// colocar em  vector <string> vert_bin a lista de todos os vértices
// do cubo em fmt binario
vert_bin = vert_bin_fixo;      

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

// Só usar estas instruções ao substituir a find_alcance com a find_alcance_debug
/*    if (snake_length > 3)
        listaTransitions(trans, ""); */
// coloca em  vorig o vértice da cabeça da snake corrente
string vorig = vert_bins[vert_bins.size() - 1];

// Chama a função para calcular new_fitness e new_skin_fit  (as 2 adequações).
// A atribuição na chamada coloca  em new_fitness a quantidade de vértices alcançáveis
// a partir do vértice da cabeça da snake corrente.
// Coloca na global new_skin_fit = a quantidade de vértices do tipo skin.
new_fitness = find_alcance(vorig);                                                                                                                                  
return true;
}
};  // FIM DA CLASSE Transition


