#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <utility>

/*
Indices in puzzle:
0, 1, 2
3, 4, 5
6, 7, 8
*/

typedef std::array<int, 9> Puzzle;

enum Move
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

const std::array<std::vector<Move>, 9> LEGAL_MOVES = {
    std::vector<Move>{Move::DOWN, Move::RIGHT},                       // 0, 0
    std::vector<Move>{Move::LEFT, Move::DOWN, Move::RIGHT},           // 0, 1
    std::vector<Move>{Move::DOWN, Move::LEFT},                        // 0, 2
    std::vector<Move>{Move::UP, Move::DOWN, Move::LEFT},              // 1, 0
    std::vector<Move>{Move::UP, Move::DOWN, Move::LEFT, Move::RIGHT}, // 1, 1
    std::vector<Move>{Move::UP, Move::DOWN, Move::LEFT},              // 1, 2
    std::vector<Move>{Move::UP, Move::RIGHT},                         // 2, 0
    std::vector<Move>{Move::LEFT, Move::RIGHT, Move::RIGHT},          // 2, 1
    std::vector<Move>{Move::UP, Move::LEFT}                           // 2, 2
};

Puzzle create_new_puzzle()
{
    std::array<int, 9> p = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    std::random_shuffle(p.begin(), p.end());
    return p;
}

void print_puzzle(const Puzzle &p)
{
    for (int i = 0; i < 9; ++i)
    {
        if (i != 0 && i % 3 == 0)
        {
            std::cout << '\n';
        }
        std::cout << p[i] << " ";
    }
    std::cout << '\n';
}

int cells_out_of_place(const Puzzle &p, const Puzzle &s)
{
    int count = 0;

    for (int i = 0; i < 9; ++i)
    {
        if (p[i] == 0)
            continue;
        
        count += !(p[i] == s[i]);
    }

    return count;
}

// Puzzle make_move(const Move &m, const Puzzle &p)
// {
//     auto row = std::find(p.begin(), p.end(), 0);
//     switch ()
//     {
//     }
//     return
// }

// std::vector<Puzzle> expand_node(const Puzzle &puzzle)
// {

// }

void solve_puzzle(Puzzle &p, const Puzzle &s)
{
    int evalutation = 0;
}

int main()
{
    // Seed the RNG for new randomizations every time
    srand(time(NULL));

    // Solution to compare with
    Puzzle solved = {1, 2, 3, 4, 5, 6, 7, 8, 0};
    std::cout << "Solution\n";
    print_puzzle(solved);

    // The random puzzle to solve
    Puzzle puzzle = create_new_puzzle();
    std::cout << "Puzzle\n";
    print_puzzle(puzzle);

    // Sanity checks
    std::cout << "Number of cells out of place: " << cells_out_of_place(puzzle, solved) << '\n';

    // Puzzle dummy = {1, 1, 1, 1, 0, 1, 1, 1, 1};
    // std::cout << "Move up:\n";
    // print_puzzle(make_move(Move::UP, dummy));
    // std::cout << "Move down:\n";
    // print_puzzle(make_move(Move::DOWN, dummy));
    // std::cout << "Move left:\n";
    // print_puzzle(make_move(Move::LEFT, dummy));
    // std::cout << "Move right:\n";
    // print_puzzle(make_move(Move::RIGHT, dummy));

    return 0;
}