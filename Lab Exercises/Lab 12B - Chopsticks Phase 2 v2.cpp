//Merlin, Santiago

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#ifdef enable_debug
#define DEBUG(x) x
#else
#define DEBUG(x) 
#endif

using namespace std;

class BodyPart;
class Player;
class Team;
class Game;
class GameHandler;

// TO DO:

/// ----- PROGRAM CONSTANTS ----- ///

class Constants {
public:
    static const bool hand_rollover;
    static const bool foot_rollover;
    static const int initial_fingers;
    static const int initial_toes;

    static const string human_class;
    static const int human_hands;
    static const int human_feet;
    static const int human_fingers;
    static const int human_toes;
    static const int human_actions;
    
    static const string alien_class;
    static const int alien_hands;
    static const int alien_feet;
    static const int alien_fingers;
    static const int alien_toes;
    static const int alien_actions;

    static const string zombie_class;
    static const int zombie_hands;
    static const int zombie_feet;
    static const int zombie_fingers;
    static const int zombie_toes;
    static const int zombie_actions;

    static const string doggo_class;
    static const int doggo_hands;
    static const int doggo_feet;
    static const int doggo_fingers;
    static const int doggo_toes;
    static const int doggo_actions;

    static const int max_teams;

    static const int max_players;
};

/// BODY PART CONSTANTS

const bool Constants::hand_rollover = true;
const bool Constants::foot_rollover = false;
const int Constants::initial_fingers = 1;
const int Constants::initial_toes = 1;

/// PLAYER & PLAYER CLASS CONSTANTS

const string Constants::human_class = "human";
const int Constants::human_hands = 2;
const int Constants::human_feet = 2;
const int Constants::human_fingers = 5;
const int Constants::human_toes = 5;
const int Constants::human_actions = 1;

const string Constants::alien_class = "alien";
const int Constants::alien_hands = 4;
const int Constants::alien_feet = 2;
const int Constants::alien_fingers = 3;
const int Constants::alien_toes = 2;
const int Constants::alien_actions = 1;

const string Constants::zombie_class = "zombie";
const int Constants::zombie_hands = 1;
const int Constants::zombie_feet = 0;
const int Constants::zombie_fingers = 4;
const int Constants::zombie_toes = 0;
const int Constants::zombie_actions = 2;

const string Constants::doggo_class = "doggo";
const int Constants::doggo_hands = 0;
const int Constants::doggo_feet = 4;
const int Constants::doggo_fingers = 0;
const int Constants::doggo_toes = 4;
const int Constants::doggo_actions = 1;

/// TEAM CONSTANTS

const int Constants::max_teams = 3;

/// GAME CONSTANTS

const int Constants::max_players = 6;



/// ----- BODY PARTS ----- ///

class BodyPart {
protected:
    int value;
    int rollover_value;
    bool rollover_enabled;
    bool dead;

    Player *owner;
public:
    BodyPart(int value, bool rollover_enabled, int rollover_value) {
        this->value = value;
        this->rollover_enabled = rollover_enabled;
        this->rollover_value = rollover_value;
        this->dead = false;
    }

    virtual bool can_rollover() {
        return rollover_enabled;
    }

    virtual int get_value() {
        return value;
    }

    virtual void set_value(int x) {
        value = x;
        if(can_rollover()) roll_over();
    }

    virtual void roll_over() {
        while(value > rollover_value) {
            value -= rollover_value;
        }
    }

    virtual void set_owner(Player *player) {
        owner = player;
    }

    virtual bool is_dead() {
        return false;
    }

    virtual bool trigger_death_flag() {
        if(is_dead() && !dead) {
            dead = true;
            return true;
        }

        return false;
    }
};

class Hand: public BodyPart {
private:
public:
    Hand(int value, int rollover_value)
        : BodyPart(value, Constants::hand_rollover, rollover_value) {
    }

    bool is_dead() {
        return value == rollover_value;
    }
};

class Foot: public BodyPart {
private:
public:
    Foot(int value, int rollover_value)
        : BodyPart(value, Constants::foot_rollover, rollover_value) {
    }

    bool is_dead() {
        return value >= rollover_value;
    }
};



/// ----- PLAYER CLASSES ----- ///

class Player {
protected:
    vector<Hand> hands;
    vector<Foot> feet;

    int actions_per_turn;
    int player_id;
    string player_class;
    string player_name;
    bool is_skipped;
public:
    Player(int _hands, int _feet, int _fingers, int _toes, const string &_player_class, int _actions_per_turn, const string &_player_name, int _player_id) {
        for(int i = 0; i < _hands; i++) {
            Hand hand(Constants::initial_fingers, _fingers);
            hands.push_back(hand);
            hands[i].set_owner(this);
        }

        for(int i = 0; i < _feet; i++) {
            Foot foot(Constants::initial_toes, _toes);
            feet.push_back(foot);
            feet[i].set_owner(this);
        }

        actions_per_turn = _actions_per_turn;
        player_class = _player_class;
        player_name = _player_name;
        player_id = _player_id;
        is_skipped = false;
    }

    virtual bool is_dead() {
        for(unsigned int i = 0; i < hands.size(); i++) {
            if(!hands[i].is_dead()) return false;
        }

        for(unsigned int i = 0; i< feet.size(); i++) {
            if(!feet[i].is_dead()) return false;
        }

        return true;
    }

    virtual bool can_act() {
        return !is_skipped;
    }

    virtual void skip() {
        is_skipped = false;
    }

    virtual void skip_next_turn() {
        is_skipped = true;
    }

    virtual void add_hand(int hand_number, int value) {
        hands[hand_number].set_value(hands[hand_number].get_value() + value);
        if(hands[hand_number].trigger_death_flag()) on_hand_death();
    }

    virtual void add_toe(int toe_number, int value) {
        feet[toe_number].set_value(feet[toe_number].get_value() + value);
        if(feet[toe_number].trigger_death_flag()) on_foot_death();
    }

    virtual int get_hand_fingers(int hand_number) {
        return hands[hand_number].get_value();
    }

    virtual int get_foot_toes(int foot_number) {
        return feet[foot_number].get_value();
    }

    virtual int get_number_of_hands() {
        return hands.size();
    }

    virtual int get_number_of_feet() {
        return feet.size();
    }

    virtual void set_hand_fingers(int hand_number, int value) {
        hands[hand_number].set_value(value);
        if(hands[hand_number].trigger_death_flag()) on_hand_death();
    }

    virtual void set_foot_toes(int toe_number, int value) {
        feet[toe_number].set_value(value);
        if(feet[toe_number].trigger_death_flag()) on_foot_death();
    }

    virtual int get_actions_per_turn() {
        return actions_per_turn;
    }

    virtual string get_player_class() {
        return player_class;
    }

    virtual string get_player_name() {
        return player_name;
    }

    virtual int get_player_id() {
        return player_id;
    }

    virtual void on_hand_death() {
        
    }

    virtual void on_foot_death() {
        skip_next_turn();
    }

    virtual void on_target(Player *target) {

    }

    virtual void on_player_attack(Player *attacker) {

    }

    virtual void print_player_status() {
        cout << "(";

        for(unsigned int i = 0; i < hands.size(); i++) {
            if(hands[i].is_dead()) cout << "X";
            else cout << hands[i].get_value();
        }

        cout << ":";

        for(unsigned int i = 0; i < feet.size(); i++) {
            if(feet[i].is_dead()) cout << "X";
            else cout << feet[i].get_value();
        }

        cout << ")";
    }
};

class Human: public Player {
private:
public:
    Human(string player_name, int player_id)
        : Player(Constants::human_hands, Constants::human_feet, Constants::human_fingers, Constants::human_toes, Constants::human_class, Constants::human_actions, player_name, player_id) {

    }
};

class Alien: public Player {
private:
public:
    Alien(string player_name, int player_id)
        : Player(Constants::alien_hands, Constants::alien_feet, Constants::alien_fingers, Constants::alien_toes, Constants::alien_class, Constants::alien_actions, player_name, player_id) {

    }

    void on_foot_death() {
        
    }
};

class Zombie: public Player {
private:
    bool can_revive;
public:
    Zombie(string player_name, int player_id)
        : Player(Constants::zombie_hands, Constants::zombie_feet, Constants::zombie_fingers, Constants::zombie_toes, Constants::zombie_class, Constants::zombie_actions, player_name, player_id) {
        can_revive = true;
    }

    void on_hand_death() {
        if(can_revive) {
            Hand hand_new(Constants::initial_fingers, Constants::zombie_fingers);
            hand_new.set_owner(this);
            hands.push_back(hand_new);
            can_revive = false;
        }
    }
};

class Doggo: public Player {
private:
public:
    Doggo(string player_name, int player_id)
        : Player(Constants::doggo_hands, Constants::doggo_feet, Constants::doggo_fingers, Constants::doggo_toes, Constants::doggo_class, Constants::doggo_actions, player_name, player_id) {

    }

    void on_target(Player *target) {
        if(target->get_player_class() != Constants::doggo_class) {
            target->skip_next_turn();
        }
    }
};



/// ----- TEAM CLASS ----- ///

class Team {
private:
    vector<Player*> players;
    int current_player;
public:
    Team(): current_player(0) {

    }

    void add_player(Player *player) {
        players.push_back(player);
    }

    Player* get_current_player() {
        return players[current_player];
    }

    int get_number_of_players() {
        return players.size();
    }

    bool is_dead() {
        for(unsigned int i = 0; i < players.size(); i++) {
            if(!players[i]->is_dead()) return false;
        }

        return true;
    }

    bool can_act() {
        for(unsigned int i = 0; i < players.size(); i++) {
            if(players[i]->can_act() && !players[i]->is_dead()) return true;
        }

        return false;
    }

    void skip() {
        for(unsigned int i = 0; i < players.size(); i++) {
            players[i]->skip();
        }
    }

    void next_player() {
        current_player = (current_player + 1) % players.size();
    }

    void print_team_status() {
        for(unsigned int i = 0; i < players.size(); i++) {
            cout << "P" << players[i]->get_player_id() + 1 << players[i]->get_player_class()[0] << " ";
            players[i]->print_player_status();

            if(i != players.size() - 1) cout << " | ";
        }
    }
};



/// --- GAME CLASS --- ///

class Game {
private:
    vector<Player*> players;
    vector<Team> teams;

    int current_team;
    int winner;
public:
    Game(int max_players, int max_teams)
        : teams(max_teams), current_team(0) {
        
    }

    ~Game() {
        for(unsigned int i = 0; i < players.size(); i++) {
            delete players[i];
        }
    }

    void add_player(Player* player, int team_number) {
        players.push_back(player);
        teams[team_number].add_player(player);
    }

    Player* get_current_player() {
        return teams[current_team].get_current_player();
    }

    int get_dead_teams() {
        int dead_teams = 0;

        for(unsigned int i = 0; i < teams.size(); i++) {
            if(teams[i].is_dead()) dead_teams++;
        }

        return dead_teams;
    }

    bool is_game_over() {
        if(teams.size() - get_dead_teams() <= 1) return true;
        return false;
    }

    int get_winning_team() {
        if(!is_game_over()) return -1;

        for(unsigned int i = 0; i < teams.size(); i++) {
            if(!teams[i].is_dead()) return i;
        }

        return -1;
    }

    void next_team() {
        current_team = (current_team + 1) % teams.size();
    }

    void end_turn() {
        teams[current_team].next_player();
        next_team();

        while(teams[current_team].is_dead() || !teams[current_team].can_act()) {
            if(!teams[current_team].can_act()) {
                teams[current_team].skip();
            }

            next_team();
        }

        while(get_current_player()->is_dead() || !get_current_player()->can_act()) {
            if(!get_current_player()->can_act()) {
                get_current_player()->skip();
            }

            teams[current_team].next_player();
        }
    }

    void tap(string &player_body_part, int player_body_number, int target_id, string &target_body_part, int target_body_number) {
        int player_value, target_value;

        if(player_body_part == "hand") player_value = get_current_player()->get_hand_fingers(player_body_number);
        else if(player_body_part == "foot") player_value = get_current_player()->get_foot_toes(player_body_number);

        if(target_body_part == "hand") {
            target_value = players[target_id]->get_hand_fingers(target_body_number);
            players[target_id]->set_hand_fingers(target_body_number, player_value + target_value);
        }
        else if(target_body_part == "foot") {
            target_value = players[target_id]->get_foot_toes(target_body_number);
            players[target_id]->set_foot_toes(target_body_number, player_value + target_value);
        }

        get_current_player()->on_player_attack(players[target_id]);
        players[target_id]->on_target(get_current_player());
    }

    void disthands(vector<int> hands) {
        for(unsigned int i = 0; i < get_current_player()->get_number_of_hands(); i++) {
            get_current_player()->set_hand_fingers(i, hands[i]);
        }
    }

    void distfeet(vector<int> feet) {
        for(unsigned int i = 0; i < get_current_player()->get_number_of_feet(); i++) {
            get_current_player()->set_foot_toes(i, feet[i]);
        }
    }

    void print_game_status() {
        for(unsigned int i = 0; i < teams.size(); i++) {
            if(teams[i].get_number_of_players() > 0) {
                cout << "Team " << i + 1 << ": ";
                teams[i].print_team_status();
                cout << '\n';
            }
        }

        cout << '\n';
    }
};



/// ----- GAME HANDLER CLASS ----- ///

class GameHandler {
private:
    Game *game;
public:
    GameHandler(Game* game) {
        this->game = game;
    }

    void print_game_winner() {
        cout << "Team " << game->get_winning_team() + 1 << " wins!";
    }

    void run() {
        initialize();

        while(!game->is_game_over()) {
            DEBUG(cout << "Current Player: " << game->get_current_player()->get_player_name() << '\n';)
            game->print_game_status();
            input();
            game->end_turn();
        }

        DEBUG(cout << "Current Player: " << game->get_current_player()->get_player_name() << '\n';)
        game->print_game_status();
        print_game_winner();
    }

    void initialize() {
        int number_of_players, number_of_teams;
        cin >> number_of_players >> number_of_teams;

        for(int i = 0; i < number_of_players; i++) {
            Player *player;
            string player_class, player_name = "Player " + to_string(i + 1);
            int team_number;

            cin >> player_class >> team_number;

            if(player_class == "human") player = new Human(player_name, i);
            if(player_class == "alien") player = new Alien(player_name, i);
            if(player_class == "zombie") player = new Zombie(player_name, i);
            if(player_class == "doggo") player = new Doggo(player_name, i);

            game->add_player(player, team_number - 1);
        }
    }

    void input() {
        for(int i = 0; i < game->get_current_player()->get_actions_per_turn(); i++) {
            string command;
            cin >> command;
            run_command(command);
        }
    }

    void run_command(string &command) {
        if(command == "tap") {
            string player_command, target_command, player_body_part, target_body_part;
            int target_number, target_id, player_body_number, target_body_number;
            cin >> player_command >> target_number >> target_command;

            DEBUG(cout << "tap " << player_command << " " << target_number << " " << target_command << '\n';)

            target_id = target_number - 1;

            if(player_command[0] == 'H') player_body_part = "hand";
            else if(player_command[0] == 'F') player_body_part = "foot";

            if(target_command[0] == 'H') target_body_part = "hand";
            else if(target_command[0] == 'F') target_body_part = "foot";

            player_body_number = player_command[1] - 'A';
            target_body_number = target_command[1] - 'A';

            game->tap(player_body_part, player_body_number, target_id, target_body_part, target_body_number);
        } else if(command == "disthands") {
            vector<int> hands(game->get_current_player()->get_number_of_hands());

            DEBUG(cout << "disthands ";)

            for(unsigned int i = 0; i < hands.size(); i++) {
                cin >> hands[i];
                DEBUG(cout << hands[i] << " ";)
            }

            DEBUG(cout << '\n';)

            game->disthands(hands);
        } else if(command == "distfeet") {
            vector<int> feet(game->get_current_player()->get_number_of_feet());

            DEBUG(cout << "distfeet ";)

            for(unsigned int i = 0; i < feet.size(); i++) {
                cin >> feet[i];
                DEBUG(cout << feet[i] << " ";)
            }

            DEBUG(cout << '\n';)

            game->distfeet(feet);
        }
    }
};

void execute_game() {
    Game *game = new Game(Constants::max_players, Constants::max_teams);
    GameHandler gamehandler(game);
    gamehandler.run();
    delete game;
}

int main() {
    execute_game();
}