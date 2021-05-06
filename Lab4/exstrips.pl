% STRIPS planner with iterative deepening extended with operators
% diff for difference, ? disjunction, - explicit negation, -> conditional effect

:-op(500,xfx, ?).    % disjunction; only in Precondition

:-op(500,fx,-).      % explicit negation; only in Precondition
                     % -A in Pre is true if A does not belong to Pre

:-op(500,xfx,->).    % conditional effect world(C)->H
                     % only in Del and Add list
					 
					 % diff only in Precondition

plan :-
     initial_state(IS),
     goal_state(GS),
     increase(Level),
     write('Level '),Level1 is Level+1,writeNLNL(Level1),
     solve(IS,GS,[],Plan,0,Level),
     printPlan(Plan).


% Planner with a max depth level
plan(MaxL) :-
     initial_state(IS),
     goal_state(GS),
     increase(Level),
     ( Level = MaxL,
       !,
       writeNL('Solution not found')
     ;
       write('Level '),Level1 is Level+1,writeNLNL(Level1),
       solve(IS,GS,[],Plan,0,Level),
       printPlan(Plan)
     ).

% if Goal is a subset of State, then return Plan
solve(State, Goal, Plan, Plan, _, _):-
     is_subset(Goal, State), !.


% otherwise, select next action and increase Counter
solve(State, Goal, Sofar, Plan, Counter, Level):-
     Counter =< Level,
     act(Action, Precons, Delete, Add),
     is_subset(Precons, State),
     % \+ member(Action, Sofar),     NOT needed since we have MAX level. Having it may block the construction of plans that require an action to be repeated twice
     delete_list(Delete, State, Remainder),
     add_list(Add, Remainder, NewState),
     NewCounter is Counter+1,
     solve(NewState, Goal, [Action|Sofar], Plan, NewCounter, Level).



% AUXILIARY

is_subset([], _).
is_subset([(-A)|T], Set):- !, \+ member(A, Set), !, is_subset(T, Set).
is_subset([A ? B|T], Set):- !, (member(A, Set) ; member(B, Set)), is_subset(T, Set).
is_subset([diff(A,B)|T], Set):- !, \+ A=B, is_subset(T, Set).
is_subset([H|T], Set):- member(H, Set), is_subset(T, Set).


add_list(Add, Remainder, NewState) :-  append2(Add, Remainder, NewState).
append2([], L, L):- !.
append2([world(C)->H|T], L1, L2):-
           !,
           (   world(C),
               !,
               append2(T, L1, L3),
               L2 = [H|L3]
           ;
               append2(T, L1, L3),
               L2 = L3
           ),
           !.
append2([H|T], L1, [H|L2]):- append2(T, L1, L2).


% Remove all elements of 1st list from second to create third.
delete_list([], State, State) :- !.
delete_list([world(C)->H|T], State, Newstate):-
           !,
           (   world(C),
               !,
               remove(H, State, Remainder),
               delete_list(T, Remainder, Newstate)
           ;
               delete_list(T, State, Newstate)
           ),
           !.
delete_list([H|T], State, Newstate):-
           remove(H, State, Remainder),
       delete_list(T, Remainder, Newstate).

remove(X, [X|T], T) :- !.
remove(X, [H|T], [H|R]):- remove(X, T, R).

increase(0).
increase(X) :- increase(Y), X is Y+1.

writeNL(X) :- write(X), nl.
writeNLNL(X) :- write(X), nl, nl.

printPlan(Plan) :-
     length(Plan, Length),
     write('----- A Plan (length '), write(Length), writeNL(')'),
     printPlan2(Plan),
     writeNL('-------------------------').

printPlan2([]).
printPlan2([H|T]):- printPlan2(T), writeNL(H).

