#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <utility>
#include <chrono>
#include <functional>
#include <unordered_set>
#include <queue>
#include <memory>
#include <numeric>

/*
Indices in puzzle:
0, 1, 2
3, 4, 5
6, 7, 8
*/

typedef std::array<int, 9> Puzzle;
typedef std::function<int(const Puzzle &, const Puzzle &)> Heuristic;

enum Move
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
};

const std::array<std::vector<Move>, 9> LEGAL_MOVES = {
    std::vector<Move>{Move::DOWN, Move::RIGHT},                       // 0, 0
    std::vector<Move>{Move::LEFT, Move::DOWN, Move::RIGHT},           // 0, 1
    std::vector<Move>{Move::DOWN, Move::LEFT},                        // 0, 2
    std::vector<Move>{Move::UP, Move::DOWN, Move::RIGHT},             // 1, 0
    std::vector<Move>{Move::UP, Move::DOWN, Move::LEFT, Move::RIGHT}, // 1, 1
    std::vector<Move>{Move::UP, Move::DOWN, Move::LEFT},              // 1, 2
    std::vector<Move>{Move::UP, Move::RIGHT},                         // 2, 0
    std::vector<Move>{Move::LEFT, Move::UP, Move::RIGHT},             // 2, 1
    std::vector<Move>{Move::UP, Move::LEFT}                           // 2, 2
};

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
    case Move::NONE:
        break;
    default:
        break;
    }
    return p;
}

struct Node
{
    Puzzle p;
    int cost;

    // Link back to previous node to find the solution path
    std::shared_ptr<Node> parent = nullptr;

    // Save the Move that led to this Node
    Move m = Move::NONE;

    // Nodes are equal if their puzzle configuration are equal
    bool operator==(const Node &other) const
    {
        return std::equal(p.begin(), p.end(), other.p.begin());
    }
};

// Implement hash function for Node
namespace std
{
    template <>
    struct hash<Node>
    {
        std::size_t operator()(const Node &n) const
        {
            unsigned int polynomial = 0;
            for (int i = 0; i < 9; ++i)
            {
                polynomial += (unsigned int)std::pow(n.p[i], i);
            }

            return std::size_t(std::hash<unsigned int>()(
                polynomial));
        }
    };
}

// {4, 1, 3, 7, 2, 6, 0, 5, 8}    ##### easy with 6 moves
// {7, 2, 4, 5, 0, 6, 8, 3, 1}     ##### medium 20 moves
// {6, 4, 7, 8, 5, 0, 3, 2, 1}    ##### difficult 31 moves

// 31 moves (most difficult) {8, 6, 7, 2, 5, 4, 3, 0, 1};

Puzzle create_new_puzzle(int num_shuffle_moves = 100)
{
    // Puzzle p = {4, 1, 3, 7, 2, 6, 0, 5, 8}; // Easy
    // Puzzle p = {7, 2, 4, 5, 0, 6, 8, 3, 1}; // Medium
    // Puzzle p = {6, 4, 7, 8, 5, 0, 3, 2, 1}; // Hard
    // Puzzle p = {8, 6, 7, 2, 5, 4, 3, 0, 1}; // Hardest

    Puzzle p = {1, 2, 3, 4, 5, 6, 7, 8, 0};

    for (int i = 0; i < num_shuffle_moves; ++i) {
        auto empty_slot = std::find(p.begin(), p.end(), 0);
        int index = empty_slot - p.begin();
        auto legal_moves = LEGAL_MOVES[index];

        // Select a random move to make
        int selection = rand() % legal_moves.size();
        p = make_move(legal_moves[selection], p);
    }

    // Old, possibly degenerate solution for shuffling
    // std::random_shuffle(p.begin(), p.end());
    return p;
}

void print_puzzle(const Puzzle &p)
{
    for (auto i = 0; i < 9; ++i)
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
        count += !(p[i] == s[i]);
    }

    return count;
}

int manhattan_distance(const Puzzle &p, const Puzzle &s)
{
    int index = 0;
    int total = std::accumulate(
        p.begin(), p.end(), 0,
        [&s, &index](int acc, int cell) {
            auto correct_pos = std::find(s.begin(), s.end(), cell) - s.begin();

            int x_diff = abs(correct_pos % 3 - index % 3);
            int y_diff = abs(floor(correct_pos / 3) - floor(index / 3));

            ++index;
            return acc + x_diff + y_diff;
        });
    return total;
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
                   [&node](Move m) { return Node{make_move(m, node.p), node.cost + 1, std::make_shared<Node>(node), m}; });
    return result;
}

std::vector<Puzzle> solve_puzzle(const Puzzle &p, const Puzzle &s, Heuristic heuristic)
{
    // Rank nodes by lowest estimated cost
    auto compare_nodes = [&s, heuristic](const Node &a, const Node &b) -> bool {
        return a.cost + heuristic(a.p, s) > b.cost + heuristic(b.p, s);
    };

    std::priority_queue<Node, std::vector<Node>, decltype(compare_nodes)> open(compare_nodes);
    std::unordered_set<Node> closed;

    // Initial node is pushed into the open prio queue
    open.push({p, 0});

    // Run A* search
    for (Node current = open.top();                          // Get a copy of the current best node
         !std::equal(s.begin(), s.end(), current.p.begin()); // Stop when the current puzzle is the solution
         current = open.top())                               // Update current node
    {
        // Print current puzzle state for debugging
        // print_puzzle(current.p);
        // std::cout << '\n';

        open.pop();

        // Expand current node
        std::vector<Node> children = expand_node(current);
        // std::copy_if(
        //     children.begin(),
        //     children.end(),
        //     std::back_inserter(open),
        //     [&closed](const Node &n) { return closed.find(n) == closed.end(); });
        for (auto child : children)
        {
            if (closed.find(child) == closed.end())
                open.push(child);
        }

        // Make sure the current node is moved to the closed list
        closed.emplace(current);
        // Re-heapify and remove last element
        // std::pop_heap(open.begin(), open.end(), compare_nodes);
        // std::swap(open.front(), open.back());
        // open.pop();
        // std::make_heap(open.begin(), open.end(), compare_nodes);
    }

    // The node corresponding to the solution is now on top of the heap
    std::vector<Puzzle> moves;
    moves.push_back(open.top().p);
    for (auto n = open.top().parent; n != nullptr; n = n->parent)
    {
        moves.push_back(n->p);
    }

    return moves;
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
    // std::cout << '\n';

    // Sanity checks
    std::cout << "\nInitial number of cells out of place: " << cells_out_of_place(puzzle, solved) << '\n';

    // Solve the puzzle
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<Puzzle> road_to_victory = solve_puzzle(puzzle, solved, manhattan_distance);
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "\nTook " << time.count() / 1000.0f << " seconds to solve\n";

    // Print the solution
    std::cout << std::endl
              << "The steps taken: (" << road_to_victory.size() - 1 << " steps)" << std::endl;
    std::for_each(road_to_victory.rbegin(), road_to_victory.rend(), [](const Puzzle &p) { print_puzzle(p); std::cout << '\n'; });

    // // Puzzle dummy = {1, 1, 1, 1, 0, 1, 1, 1, 1}; // Simple test puzzle
    // // std::cout << "Move up:\n";
    // // print_puzzle(make_move(Move::UP, puzzle));
    // // std::cout << "Move down:\n";
    // // print_puzzle(make_move(Move::DOWN, puzzle));
    // // std::cout << "Move left:\n";
    // // print_puzzle(make_move(Move::LEFT, puzzle));
    // // std::cout << "Move right:\n";
    // // print_puzzle(make_move(Move::RIGHT, puzzle));

    // Node n{puzzle, 0};
    // std::vector<Node> nodes = expand_node(n);
    // std::cout << "Expand node:\n";
    // std::for_each(nodes.begin(), nodes.end(), [](const Node &n) { print_puzzle(n.p); std::cout << "Cost: " << n.cost << '\n'; });
    // std::vector<Node> more_nodes = expand_node(nodes[0]);
    // std::cout << "Expand first expanded node:\n";
    // std::for_each(more_nodes.begin(), more_nodes.end(), [](const Node &n) { print_puzzle(n.p); std::cout << "Cost: " << n.cost << '\n'; });

    return 0;
}