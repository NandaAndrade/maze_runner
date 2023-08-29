#include <stdio.h>
#include <stack>
#include <stdexcept>
#include <iostream>
#include <string>
#include <thread>

#define IND_ENTRADA 'e'
#define IND_SAIDA 's'

using namespace std;
// Matriz de char representnado o labirinto
char** maze; // Voce também pode representar o labirinto como um vetor de vetores de char (vector<vector<char>>)

// Numero de linhas e colunas do labirinto
int num_rows;
int num_cols;

// Representação de uma posição
// Posição a ser explorada
class pos_t
{
	int i;
	int j;

public:
	pos_t()
	{
		this->i = 0;
		this->j = 0;
	}

	pos_t(int i, int j)
	{
		this->i = i;
		this->j = j;
	}

	int get_i()
	{
		return i;
	}

	int get_j()
	{
		return j;
	}

	void atualiza_posicao(int i, int j)
	{
		this->i = i;
		this->j = j;
	}

	char &operator*()
	{
		return maze[i][j];
	}

	pos_t &operator=(pos_t &pos)
	{
		this->i = pos.get_i();
		this->j = pos.get_j();
		return *this;
	}
};

pos_t next_position;
// Estrutura de dados contendo as próximas
// posicões a serem exploradas no labirinto
std::stack<pos_t> valid_positions;
/* Inserir elemento: 

	 pos_t pos;
	 pos.i = 1;
	 pos.j = 3;
	 valid_positions.push(pos)
 */
// Retornar o numero de elementos: 
//    valid_positions.size();
// 
// Retornar o elemento no topo: 
//  valid_positions.top(); 
// 
// Remover o primeiro elemento do vetor: 
//    valid_positions.pop();

void print_maze()
{
	system("clear");
	for (int i = 0; i < num_rows; i++)
	{
		for (int j = 0; j < num_cols; j++)
		{
			printf("%c", maze[i][j]);
		}
		printf("\n");
	}
	this_thread::sleep_for(chrono::milliseconds(50));
}

// Função que le o labirinto de um arquivo texto, carrega em 
// memória e retorna a posição inicial
pos_t load_maze(const char *file_name)
{
	pos_t initial_pos;
	// Abre o arquivo para leitura (fopen)
	FILE *mazeFile;
	mazeFile = fopen(file_name, "r");
	if(mazeFile == NULL){
		throw std::runtime_error("Arquivo não encontrado");
	}
	// Le o numero de linhas e colunas (fscanf)
	// e salva em num_rows e num_cols
	fscanf(mazeFile, "%d %d", &num_rows, &num_cols);

	cout<<num_rows<<" "<<num_cols<<endl;

	// Aloca a matriz maze (malloc)
	maze = (char **)malloc(num_rows * sizeof(char *));

	for (int i = 0; i < num_rows; i++)
	{
		// Aloca cada linha da matriz
		maze[i] = (char *)malloc((num_cols) * sizeof(char));
	}

	for (int i = 0; i < num_rows; i++)
	{
		for (int j = 0; j < num_cols; j++)
		{
			fscanf(mazeFile, " %c", &maze[i][j]);

			if (maze[i][j] == IND_ENTRADA)
			{
				initial_pos.atualiza_posicao(i, j);
			}
		}
	}

	cout<<initial_pos.get_i()<<" "<<initial_pos.get_j()<<endl;

	try
	{
		fclose(mazeFile);
	}
	catch (std::runtime_error &e)
	{
		// throw std::runtime_error("fclose error");"
		std::cout << "fclose error" << std::endl;
	}

	return initial_pos;
}

void atualizar_maze(char c, pos_t pos)
{
	maze[pos.get_i()][pos.get_j()] = c;
}

// Função responsável pela navegação.
// Recebe como entrada a posição initial e retorna um booleando indicando se a saída foi encontrada
void walk(pos_t initial_pos)
{
	cout<<initial_pos.get_i()<<" "<<initial_pos.get_j()<<endl;

	pos_t pos_esquerda, pos_direita, pos_cima, pos_baixo;
	valid_positions.push(initial_pos);
	bool saida = 0;
	int i = 0;
	int j = 0;

	while (!saida && !valid_positions.empty())
	{
		cout<<"valid_positions: "<<endl;

		// for(int i = 0; i< valid_positions.size(); i++){
		// 	cout<<valid_positions.top().get_i()<<" "<<valid_positions.top().get_j()<<endl;
		// }

		next_position = valid_positions.top();
		valid_positions.pop();

		saida = (*next_position == IND_SAIDA);

		i = next_position.get_i();
		j = next_position.get_j();

		pos_esquerda.atualiza_posicao(i, j + 1);
		pos_direita.atualiza_posicao(i, j - 1);
		pos_cima.atualiza_posicao(i - 1, j);
		pos_baixo.atualiza_posicao(i + 1, j);

		// cout << "pos_esquerda: " << *pos_esquerda << endl;
		// cout << "pos_direita: " << *pos_direita << endl;
		// cout << "pos_cima: " << *pos_cima << endl;
		// cout << "pos_baixo: " << *pos_baixo << endl;

		// Marcar a posição atual com o símbolo '.'
		atualizar_maze('.', next_position);

		if (*pos_esquerda == 'x' || *pos_esquerda == IND_SAIDA)
		{
			valid_positions.push(pos_esquerda);
		}
		if (*pos_direita == 'x' || *pos_direita == IND_SAIDA)
		{
			valid_positions.push(pos_direita);
		}
		if (*pos_cima == 'x' || *pos_cima == IND_SAIDA)
		{
			valid_positions.push(pos_cima);
		}
		if (*pos_baixo == 'x' || *pos_baixo == IND_SAIDA)
		{
			valid_positions.push(pos_baixo);
		}

		print_maze();
		atualizar_maze(' ', next_position);
		printf("\n-----------------------------------------\n");
		
	}

}

void free_maze()
{
	for (int i = 0; i < num_rows; i++)
	{
		free(maze[i]);
	}

	free(maze);
}

int main(int argc, char *argv[])
{
	//carregar o labirinto com o nome do arquivo recebido como argumento (argv[])
	if (argc != 2)
	{
		throw std::runtime_error("Número de parâmetros incorreto, passe o nome do arquivo texto como parâmetro do programa");
		return -1;
	}

	char *file_name = argv[1];

	pos_t initial_pos = load_maze(file_name);
	print_maze();

	printf("\n INICIO \n");
	walk(initial_pos);

	free_maze();

	return 0;
}
