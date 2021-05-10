%====================================================================
% SHOPPING PROBLEM

% CONTROL PARAMETERS
% ordering(linear).
ordering(partial).
derivedPlans(one).

% ACTIONS:    action(Name,Prec,Del,Add)

% Agent A buys item X at the Store
action(buy(A,W,Store),
	[store(Store),at(A,Store),sells(Store,W)],
	[],
	[has(A,W), obj_at(W, Store)]).

% Agent A goes from location X to location Y
action(go(A,X,Y),
	[location(X),location(Y),X\=Y,at(A,X)],
	[at(A,X)],
	[at(A,Y)]).

% Agent A carries an item W from location X to location Y
action(carry(A, W, X, Y),
	[location(X), location(Y), obj_at(W, X), X\=Y, at(A, X), has(A, W)],
	[obj_at(W, X), at(A, X)],
	[obj_at(W, Y), at(A, Y)]).

% PARALLELISMS
parallel(X, Y) :- X =.. [F|_], Y =.. [F|_], F = buy, !.
parallel(X, Y) :- X =.. [F, A, W1, From, To], Y =.. [F, A, W2, From, To], F = carry, W1 \= W2, !.

% FLUENT
fluent(at(_,_)).
fluent(has(_,_)).
fluent(obj_at(_,_)).

% DOMAIN KNOWLEDGE
store(ica) <- [].
store(clasohlson) <- [].
sells(ica,banana) <- [].
sells(ica,bread) <- [].
sells(ica,cheese) <- [].
sells(clasohlson,drill) <- [].
location(home) <- [].
location(office) <- [].
location(X) <- [store(X)].

% INITIAL SITUATION
holds(at(chris,home),init).

%% plan :- solve([has(chris,drill)],2).
%% plan :- solve([has(chris,drill), has(chris, banana)],4).
%% plan :- solve([has(chris,bread), has(chris, cheese)],3).
%% plan :- solve([objat(bread,home)],3).
% plan :- solve([obj_at(bread,home), obj_at(cheese, home)],6).
