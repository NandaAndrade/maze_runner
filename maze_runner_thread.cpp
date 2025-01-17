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
int num_threads = 0;
bool saidaa = 0;

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
	printf("\n-----------------------------------------\n");
	this_thread::sleep_for(chrono::milliseconds(55));
}


pos_t load_maze(const char *file_name)
{
	pos_t initial_pos;

	FILE *mazeFile;
	mazeFile = fopen(file_name, "r");
	if(mazeFile == NULL){
		throw std::runtime_error("Arquivo não encontrado");
	}


	fscanf(mazeFile, "%d %d", &num_rows, &num_cols);

	cout<<num_rows<<" "<<num_cols<<endl;

	maze = (char **)malloc(num_rows * sizeof(char *));

	for (int i = 0; i < num_rows; i++)
	{
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
	pos_t pos_esquerda, pos_direita, pos_cima, pos_baixo;
	bool saida = 0;
	int i = 0;
	int j = 0;
	int id = num_threads;
	num_threads++;
    pos_t current_position;
    std::stack<pos_t> valid_positions;
    valid_positions.push(initial_pos);

	while (!saida && !valid_positions.empty())
	{
		// printf("\n-----------------------------------------\n");
		// cout<<id<< ": "<<"current_positions: "<<valid_positions.top().get_i()<<" "<<valid_positions.top().get_j()<<endl;

		current_position = valid_positions.top();
		// cout<<id<< ": "<<"apagando: "<< current_position.get_i()<<" "<<current_position.get_j()<<endl;
		valid_positions.pop();
		
        // cout<<id<< ": "<<"depois de pop valid_positions.size(): "<<valid_positions.size()<<endl;
        // if(valid_positions.size() > 0){
        //     cout<<id<< ": "<<valid_positions.top().get_i()<<" "<<valid_positions.top().get_j()<<endl;
        // }

        if(valid_positions.size() > 0) {
            // cout <<id<< ": "<< "Criando thread" << endl;
            // cout<<id<< ": "<<"valid_positions: "<<endl;
			// cout<<id<< ": "<<valid_positions.top().get_i()<<" "<<valid_positions.top().get_j()<<endl;
            thread new_thread(walk, valid_positions.top());
            new_thread.detach();
			// cout<<id<< ": "<<"trhead criada, dando pop"<<endl;
			valid_positions.pop();
        }

		saida = (*current_position == IND_SAIDA);

		i = current_position.get_i();
		j = current_position.get_j();

		pos_esquerda.atualiza_posicao(i, j + 1);
		pos_direita.atualiza_posicao(i, j - 1);
		pos_cima.atualiza_posicao(i - 1, j);
		pos_baixo.atualiza_posicao(i + 1, j);

		// cout <<id<< ": "<< "pos_esquerda: " << pos_esquerda.get_i()<< " "<< pos_esquerda.get_j() << endl;
		// cout <<id<< ": "<< "pos_direita: " << pos_direita.get_i() << " "<< pos_direita.get_j() << endl;
		// cout <<id<< ": "<< "pos_cima: " << pos_cima.get_i() <<" "<< pos_cima.get_j() << endl;
		// cout <<id<< ": "<< "pos_baixo: " << pos_baixo.get_i()<< " " << pos_baixo.get_j() << endl;

		// Marcar a posição atual com o símbolo '.'
		atualizar_maze('.', current_position);

		if(pos_esquerda.get_j() < num_cols){
			if (*pos_esquerda == 'x' || *pos_esquerda == IND_SAIDA)
			{            
				valid_positions.push(pos_esquerda);
				// cout<<id<< ": "<<"pos_esquerda: "<< ": "<<pos_esquerda.get_i()<<" "<<pos_esquerda.get_j()<<endl;
			}
		}
		else{
			// cout<<id<< ": "<<"else pos_esquerda: "<< pos_esquerda.get_i()<<" "<<pos_esquerda.get_j()<<endl;
		}
		if(pos_direita.get_j() >= 0){
			if ( *pos_direita == 'x' || *pos_direita == IND_SAIDA)
			{
				valid_positions.push(pos_direita);
				// cout<<id<< ": "<<"pos_direita: "<<pos_direita.get_i()<<" "<<pos_direita.get_j()<<endl;
			}
		}
		else{
			// cout<<id<< ": "<<"else pos_direita: "<<pos_direita.get_i()<<" "<<pos_direita.get_j()<<endl;
		}
		if(pos_cima.get_i() >= 0){
			if (*pos_cima == 'x' || *pos_cima == IND_SAIDA)
			{
				valid_positions.push(pos_cima);
				// cout<<id<< ": "<<"pos_cima: "<<pos_cima.get_i()<<" "<<pos_cima.get_j()<<endl;
			}
		}
		else{
			// cout<<id<< ": "<<"else pos_cima: "<<pos_cima.get_i()<<" "<<pos_cima.get_j()<<endl;
		}
		if(pos_baixo.get_i() < num_rows ){
			if (*pos_baixo == 'x' || *pos_baixo == IND_SAIDA)
			{
				valid_positions.push(pos_baixo);
				// cout<<id<< ": "<<"pos_baixo: "<<pos_baixo.get_i()<<" "<<pos_baixo.get_j()<<endl;
			}
		}
		else{
			// cout<<id<< ": "<<"else pos_baixo: "<<pos_baixo.get_i()<<" "<<pos_baixo.get_j()<<endl;
		}

		atualizar_maze(' ', current_position);
		this_thread::sleep_for(chrono::milliseconds(100));
		
	}

	if(saida == 1)
		saidaa = saida;

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
	if (argc != 2)
	{
		throw std::runtime_error("Número de parâmetros incorreto, passe o nome do arquivo texto como parâmetro do programa");
		return -1;
	}

	char *file_name = argv[1];

	pos_t initial_pos = load_maze(file_name);
	print_maze();

	printf("\n INICIO \n");
	thread new_thread(walk, initial_pos);
	
	while(!saidaa){
		cout<<"saidaa: "<<saidaa<<endl;
		print_maze();

	}
	new_thread.detach();
	free_maze();

	return 0;
}
