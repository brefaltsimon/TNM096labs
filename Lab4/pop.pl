% PARTIAL ORDER PLANNER WITH SEQUENTIAL AND PARALLEL ACTIONS
% by Pierangelo Dell'Acqua 2016-03-04'

% =============================================================================
% goal(A,P) is a goal
%       A is the index of the goal (an integer)
%       P is a proposition
% act(A,P) is an action instance
%       A is the index (needed if there are multiple instances of the same action)
%   P is an action (a proposition)
% cl(A0,P,A1) is a causal link
%       A0 and A1 are action instance indexes
%       P is a proposition meaning: A0 is making P true for A1
% [goal(A0,P0),...,goal(An,Pn)] is an agenda
%       a list of goals
% plan(As,Ls) is a plan
%   As is a list of actions instances
%   Ls is a list of causal links
% Conjunctions are represented as lists
% actions MUST have different names     
% =============================================================================

:- use_module(library(clpfd)).

% \= is the object level not equal
:- op(700,xfx, \=).
:- op(700,fx, -). % negation
:- op(1200,xfx,[<-]).
:- op(500,xfx, ?).    % disjunction

constraint(P) :- \+ ground(P),!,fail.
constraint(-_) :- !.
constraint(_?_) :- !.


% =============================================================================
% pop(Plan0, Ag0, Vars, Plan1) is true if
%   Plan0 is the current plan, Ag0 is the current agenda
%   and Vars is the list of free variables that can be used to index action instances
%   (the length of Vars gives the depth-bound)
%   Plan1 is the new plan
        
pop(Plan,[],_,Plan,W,W).
pop(Plan0,Ag0,Vars0,Plan,CW,W) :-
   select(G,Ag0,Ag1),
   solve_goal(G,Plan0,Ag1,Vars0,Plan1,Ag2,Vars1),
   G=goal(_,P),
   ( constraint(P) -> CW2=[G|CW] ; CW2=CW ),
   pop(Plan1,Ag2,Vars1,Plan,CW2,W).

% select(Goal,Ag,Ag2) is true if Goal is selected from Ag with Ag2 the remaining elements
select(goal(A,P),[goal(A,P)|Ag],Ag) :-
        constraint(P),
        !.
select(goal(A,P),[goal(A,P)|Ag],Ag) :-
   fluent(P),
   !.
select(G,[goal(A,P)|Ag0],Ag2) :-
   (P <- B),
   makeintogoal(B,A,Ag0,Ag1),
   ( Ag1=[] -> ( Ag2=[], G=goal(A,true) ) ; select(G,Ag1,Ag2) ).

makeintogoal([],_,Ag,Ag) :-
        !.
makeintogoal([X \= Y|R],A,Ag0,Ag1) :-
   !,
   dif(X,Y),
   makeintogoal(R,A,Ag0,Ag1).
makeintogoal([ground(X)|R],A,Ag0,Ag1) :-
   !,
   ground(X),
   makeintogoal(R,A,Ag0,Ag1).
makeintogoal([P|R],A,Ag0,[goal(A,P)|Ag1]) :-
   makeintogoal(R,A,Ag0,Ag1).

% =============================================================================
% solve_goal(Goal,Plan0,Ag0,Vars0,Plan1,Ag1,Vars1) chooses an action to
%    solve Goal, updating Plan0 to Plan1, agenda Ag0 to Ag1,
%    and updating the list of free variables Vars0 to Vars1

solve_goal(goal(_,true),plan(As,Ls),Ag,Vars,plan(As,Ls),Ag,Vars) :-
   !.
solve_goal(goal(_,P),plan(As,Ls),Ag,Vars,plan(As,Ls),Ag,Vars) :-
        constraint(P),
        !.

% CASE 1: use existing action
solve_goal(goal(A1,P),plan(As,Ls),Ag,Vars,plan(As,[cl(A0,P,A1)|Ls]),Ag,Vars) :-
   member(act(A0,Act0),As),
   achieves(Act0,P),
   A0 #< A1,
   incorporate_causal_link(cl(A0,P,A1),As).

% CASE 2: add new action. A0 acts as the unique index of the new action instance
solve_goal(
goal(A1,P),plan(As,Ls),Ag0,Vars,plan([act(A0,Act0)|As],[cl(A0,P,A1)|Ls]),Ag1,Vars2) :-
   achieves(Act0,P),
   Vars=[A0|Vars2],
   A0 #< A1,
   incorporate_action(act(A0,Act0),Ls),
   incorporate_causal_link(cl(A0,P,A1),As),
   add_preconds(act(A0,Act0),Ag0,Ag1).
   
% =============================================================================
% incorporate_causal_link(CL, As) incorporates causal links in CL
   
incorporate_causal_link(_,[]) :-
        !.
incorporate_causal_link(CL,[A|R]) :-
   protect(CL,A),
   incorporate_causal_link(CL,R).
incorporate_action(_,[]) :-
        !.
incorporate_action(A,[CL|R]) :-
   protect(CL,A),
   incorporate_action(A,R).

% protect(Cl,Action) protects causal link CL from Action if necessary
protect(cl(A0,_,_),act(A,_)) :- A0==A,!.
protect(cl(A0,_,_),act(A,_)) :- it_holds(A<A0),!.
protect(cl(_,_,A1),act(A,_)) :- A1==A,!.
protect(cl(_,_,A1),act(A,_)) :- it_holds(A1<A),!.

protect(cl(_,P,_),act(_,Act)) :-
        when(ground((Act,P)), \+ deletes(Act,P)).  % NO cut here!

protect(cl(A0,P,A1),act(A,Act)) :-
   deletes(Act,P),
   enforce_order(A0,A,A1).

enforce_order(_,A,A1) :-
%   A1 #< A.   original
   A1 #=< A.
enforce_order(A0,A,_) :-
   A #< A0.

add_preconds(act(A,P),Ag0,Ag2) :-
   preconditions(P,Ps),
   makeintogoal(Ps,A,[],Ag1),
   append(Ag0,Ag1,Ag2),
   !.

it_holds(X<Y) :-
        \+ X #>= Y.

% =============================================================================
solve(Goals,T) :-
   S=0,
   F is T+1,
   S #< F,
   create_variables(T,Vars),
   makeintogoal(Goals,F,[],Ag),
   !,
   pop(plan([act(S,init),act(F,end)],[]),Ag,Vars,Plan,[],W),
   writePlan(Plan,Vars,W).

writePlan(Plan,Vars,W) :-
        duplicateVariables(Plan,L2),
        label(Vars),
        checkConstraints(W,Plan),
        ( (ordering(partial);ordering(linear)) -> true; (write('Wrong ordering'),!,fail) ),
        ( ordering(partial) -> check_parallel_actions(Plan);true ),
        writePOP(L2),
        write_plan_actions(Plan).

create_variables(T,Vars) :-
        create_var_list(T,Vars),
        Vars ins 1..T,
        (ordering(linear) -> all_different(Vars);true),   % makes actions sequential
        !.

create_var_list(0,[]) :-
        !.
create_var_list(T,[_|L]) :-
        T1 is T-1,
        create_var_list(T1,L).  

duplicateVariables(plan(As,Ls),L2) :- 
        extractVars(Ls,Lista),
        computeVarLessOrder(Lista,[],OrderList),
        L=(Ls,OrderList,As),
        copy_term(L,L2,_),
        numbervars(L2,0,_),
        !.
 
checkConstraints([],_) :- !.
checkConstraints(W,plan(As,_)) :-
        sort(W,W2),
        sort(As,[act(0,init)|As2]),
        findall(X,holds(X,init),S0),
        checkCons2(0,S0,W2,As2),
        !.

checkCons2(_,_,[],_) :- !.
checkCons2(L,S,W,As) :-
        A is L+1,
        W=[goal(A,P)|W2],
        !,
        prove(P,S),
        % ( prove(P,S) -> print([\n,'proved: ', P,\n,'from: ',\s,S,\n,'L: ',L,\n]) ; ( print([\n,'failed: ', P,\n,from,\s,S,\n,'L: ',L,\n]), fail) ),
        !,
        checkCons2(L,S,W2,As).

checkCons2(L,S,W,As) :-
        L2 is L+1,
        calculateNextState(L2,S,As,S2,As2),
        !,
        checkCons2(L2,S2,W,As2).
        
        
calculateNextState(L2,S1,As1,S,As) :-
        As1=[act(L2,P)|As2],
        !,
        action(P,_,DeleteL,AddList),
        removeElements(DeleteL,S1,S2),
        addElements(AddList,S2,S3),
        calculateNextState(L2,S3,As2,S,As),!.
calculateNextState(_,S1,As1,S1,As1).

removeElements([],L,L) :- !.
removeElements([P|Ps],L1,L) :-
        ( remove(P,L1,L2) ; L2=L1),
        !,
        removeElements(Ps,L2,L).

addElements([],L,L) :- !.
addElements([P|Ps],L1,L) :-
        insert(P,L1,L2),                        % NO duplicates
        !,
        addElements(Ps,L2,L).
        

prove([],_) :- !.
prove([P|Ps],S) :-
        !,
        prove(P,S),
        prove(Ps,S).

prove(-P,S) :-
        !,
        \+ prove(P,S).

prove(X?Y,S) :-
        !,
        ( prove(X,S) ; prove(Y,S) ).

prove(X\=Y,_) :-
        !,
        X\=Y.

prove(P,S) :-
        fluent(P),
        !,
        member(P,S).

prove(P,S) :-
        (P<-B),
        prove(B,S).
        

% Test the actions that can be executed in the same layer
check_parallel_actions(plan(As,_)) :-
        parallel_actions(As).

parallel_actions([]) :- !.
parallel_actions([act(A,P)|T]) :-
        member(act(A,P2),T),
        \+ parallel2(P,P2),
        !,
        fail.
parallel_actions([_|T]) :-
        parallel_actions(T),
        !.

parallel2(X,Y) :-
        ( parallel(X,Y) ; parallel(Y,X) ),
        !.
   
        
% =============================================================================
% BASICS

append([],L,L) :-
        !.
append([H|X],Y,[H|Z]) :-
   append(X,Y,Z).

% member(X,L) is true if X is a member of list L / MUST be backtrakable
member(X,[X|_]).
member(X,[_|L]) :-
   member(X,L).

% remove(X,L,R) is true if X is a member of list L with remaining elements R
remove(X,[X|R],R) :-
        !.
remove(X,[A|L],[A|R]) :-
   remove(X,L,R).

% insert(E,L,L1) inserts E into L producing L1; NOT added if E is already in L
insert(A,[],[A]) :-
        !.
insert(A,[B|R],[B|R]) :-
        A == B,
        !.
insert(A,[B|R],[B|R1]) :-
   A \== B,
   !,
   insert(A,R,R1).

print([]) :- !.
print([H|T]) :-
        printA(H),
        print(T),
        !.
printA(X) :- var(X),!,write(X).
printA(\n) :- nl,!.
printA(\s) :- write(' '),!.
printA(\t) :- write('\t'),!.
printA(X) :- write(X),!.

writeListH([]) :- !,nl.
writeListH([H|T]) :- print([H,\t]),writeListH(T),!.

writeListV([]) :- !,nl.
writeListV([H|T]) :- print([H,\n]),writeListV(T),!.

% =============================================================================
% Write plans

% write_plan_actions(Plan) prints the actions sorted from Plan
write_plan_actions(plan(As,_)) :-
        nl,
        (ordering(linear) -> write('T\tLinear plan:');write('L\tParallel plan')),
        print([\n,'-----------------------------------',\n]),
        sort(As,Sorted),
        writeActions(Sorted,0,0,0),
        !.

writeActions([act(A,end)],_,NL,NA) :-
        !,
        NA2 is NA-1,
        print([A,'\t',end,\n,'-----------------------------------',\n,'N. of actions: ',
               NA2,\n,'N. of layers:  ',NL,\n,\n]).

writeActions([act(A,P)|T],PrevLayer,NL,NA) :-
   !,
   ( A > PrevLayer -> NL2 is NL+1; NL2 = NL ),
   print([A,\t,P,\n]),
   NA2 is NA+1,
   writeActions(T,A,NL2,NA2).

   
writePOP((Ls,OrderList,As)) :- 
        sort(As,As2),
        sort(Ls,Ls2),
        sort(OrderList,OL2),
        print([\n,\n,'      PARTIAL ORDER PLAN - POP',\n,
                   '-----------------------------------------------',\n]),
        print([\n,'Actions',\n]),
        writeListV(As2),
        print([\n,'Causal links',\n]),
        writeListV(Ls2),
        (       OL2 == [],!
                ;
                print([\n,'Variable order',\n]),
                writeListH(OL2),
                print([\n,\n])
        ),
        !.

computeVarLessOrder([],L,L) :- !.
computeVarLessOrder([X|T],L,L3) :-
        extract_order(X,T,L,L2),
        !,
        computeVarLessOrder(T,L2,L3).
        
extract_order(_,[],L,L) :- !.
extract_order(X,[Y|T],L,L2) :-
        ground((X,Y)),
        !,
        extract_order(X,T,L,L2).
extract_order(X,[Y|T],L,L3) :-
        it_holds(X<Y),
        !,
        insert(X<Y,L,L2),
        extract_order(X,T,L2,L3).
extract_order(X,[Y|T],L,L3) :-
        it_holds(Y<X),
        !,
        insert(Y<X,L,L2),
        extract_order(X,T,L2,L3).
extract_order(X,[_|T],L,L2) :-
        extract_order(X,T,L,L2).
        
extractVars(L,L2) :-
                extractVars2(L,[],L2).
extractVars2([],L,L) :- !.
extractVars2([cl(A,_,B)|T],L,L4) :-
        insert(A,L,L2),
        insert(B,L2,L3),
        !,
        extractVars2(T,L3,L4).

        
% ============================================================================= 
% INTERFACE TO FORMAT:  action(Name, Preconditions, Delete, Add)
% Actions must have DIFFERENT names

preconditions(Name,PrecList) :-
        action(Name,PrecList,_,_),
        !.

achieves(init,X) :-
        holds(X,init).
achieves(Name,P) :-
        action(Name,_,_,AddList),   % NO cut here!
        member(P,AddList).
 
deletes(Name,P) :-
        action(Name,_,DeleteList,_),
        member(P,DeleteList).
% =============================================================================