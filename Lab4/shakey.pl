% Describe the actions in STRIPS notation
act(go(X, Y),
    [on(floor), at(shakey, X), connected(X, Y)],
    [at(shakey, X)],
    [at(shakey, Y)]).

act(push(B, X, Y),
    [on(floor), box(B), at(B, X), at(shakey, X), connected(X, Y), light_on(X)],
    [at(B, X), at(shakey, X)],
    [at(B, Y), at(shakey, Y)]).

act(climb_up(B),
    [box(B), at(shakey, X), at(B, X), on(floor)],
    [on(floor)],
    [on(B)]).

act(climb_down(B),
    [box(B), on(B)],
    [on(B)],
    [on(floor)]).

act(turn_on(S),
    [on(B), box(B), at(B, S), connected(S, R), room(R), switch(S), light_off(R)],
    [light_off(R), light_off(S)],
    [light_on(R), light_on(S)]).

act(turn_off(S),
   [on(B), box(B), at(B, S), connected(S, R), room(R), switch(S), light_on(R)],
   [light_on(R), light_on(S)],
   [light_off(R), light_off(S)]).

% Goal state for Shakey
goal_state([
    at(shakey, room_3),
    light_off(room_1),
    at(box_2, room_2)
]).

% Initial state for Shakey
initial_state([
    at(shakey, room_3),
    on(floor),

    light_on(room_1),
    light_on(switch_1),
    light_off(room_2),
    light_off(switch_2),
    light_off(room_3),
    light_off(switch_3),
    light_on(room_4),
    light_on(switch_4),
    light_on(corridor),

    at(box_1, room_1),
    at(box_2, room_1),
    at(box_3, room_1),
    at(box_4, room_1),

    box(box_1),
    box(box_2),
    box(box_3),
    box(box_4),

    room(room_1),
    room(room_2),
    room(room_3),
    room(room_4),

    switch(switch_1),
    switch(switch_2),
    switch(switch_3),
    switch(switch_4),

    % place(room_1),
    % place(room_2),
    % place(room_3),
    % place(room_4),
    % place(switch_1),
    % place(switch_2),
    % place(switch_3),
    % place(switch_4),
    % place(corridor),

    connected(room_1, switch_1),
    connected(switch_1, room_1),
    connected(room_2, switch_2),
    connected(switch_2, room_2),
    connected(room_3, switch_3),
    connected(switch_3, room_3),
    connected(room_4, switch_4),
    connected(switch_4, room_4),

    connected(room_1, corridor),
    connected(switch_1, corridor),
    connected(room_2, corridor),
    connected(switch_2, corridor),
    connected(room_3, corridor),
    connected(switch_3, corridor),
    connected(room_4, corridor),
    connected(switch_4, corridor),

    connected(corridor, switch_1),
    connected(corridor, room_1),
    connected(corridor, switch_2),
    connected(corridor, room_2),
    connected(corridor, switch_3),
    connected(corridor, room_3),
    connected(corridor, switch_4),
    connected(corridor, room_4)
]).

