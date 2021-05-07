:- use_module(library(clpfd)).

% Move something to a block
act(move(X, From, To),
    [clear(X), clear(To)],
    [on(X, From), clear(To)],
    [on(X, To), clear(From)])
    :- object(X), block(To), X #\= From, X #\= To, From #\= To.

% Move something to a table
act(move(X, From, To),
    [clear(X)],
    [on(X, From)],
    [on(X, To), clear(From)])
    :- object(X), table(To), X #\= From, From #\= To.

% Some blocks are blocks, others are pyramids
table(0).
block(X)   :- X in 2..4 \/ 6.
pyramid(X) :- X in 1 \/ 5.
object(X)  :- block(X); pyramid(X).

% Colors
red(X)   :- X in 1 \/ 4.
green(X) :- X in 2 \/ 5.
blue(X)  :- X in 3 \/ 6.

% Blocky goal
goal_state([
    on(X, Y),
    on(Y, Z),
    on(Z, 0),   % To ensure a stack of three objects, Z needs to be on table
    clear(X)    % and X needs to be clear
]) :- green(Y), blue(Z).

% Blocky state
initial_state([ 
    on(1, 0),
    on(2, 0),
    on(3, 4),
    on(4, 0),
    on(5, 6),
    on(6, 0),
    clear(1),
    clear(2),
    clear(3),
    clear(5)
]).