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

struct Node
{
    Puzzle p;
    int cost;
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

Puzzle make_move(const Move &m, Puzzle p)
{
    auto empty_slot = std::find(p.begin(), p.end(), 0);
    switch (m)
    {
    case Move::UP:
        std::swap(*empty_slot, *(empty_slot - 3));
        break;
    case Move::DOWN:
        std::swap(*empty_slot, *(empty_slot + 3));
        break;
    case Move::LEFT:
        std::swap(*empty_slot, *(empty_slot - 1));
        break;
    case Move::RIGHT:
        std::swap(*empty_slot, *(empty_slot + 1));
    default:
        break;
    }
    return p;
}

std::vector<Node> expand_node(const Node &node)
{
    // Maybe there should be a struct holding empty_slot index/iterator
    auto empty_slot = std::find(node.p.begin(), node.p.end(), 0);
    int index = empty_slot - node.p.begin();
    auto legal_moves = LEGAL_MOVES[index];

    std::vector<Node> result;

    std::transform(legal_moves.begin(), legal_moves.end(),
                   std::back_inserter(result),
                   [&node](const Move &m) { return Node{make_move(m, node.p), node.cost + 1}; });
    return result;
}

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

    // Puzzle dummy = {1, 1, 1, 1, 0, 1, 1, 1, 1}; // Simple test puzzle
    // std::cout << "Move up:\n";
    // print_puzzle(make_move(Move::UP, puzzle));
    // std::cout << "Move down:\n";
    // print_puzzle(make_move(Move::DOWN, puzzle));
    // std::cout << "Move left:\n";
    // print_puzzle(make_move(Move::LEFT, puzzle));
    // std::cout << "Move right:\n";
    // print_puzzle(make_move(Move::RIGHT, puzzle));

    Node n{puzzle, 0};
    std::vector<Node> nodes = expand_node(n);
    std::cout << "Expand node:\n";
    std::for_each(nodes.begin(), nodes.end(), [](const Node &n) { print_puzzle(n.p); std::cout << "Cost: " << n.cost << '\n'; });
    std::vector<Node> more_nodes = expand_node(nodes[0]);
    std::cout << "Expand first expanded node:\n";
    std::for_each(more_nodes.begin(), more_nodes.end(), [](const Node &n) { print_puzzle(n.p); std::cout << "Cost: " << n.cost << '\n'; });

    return 0;
}