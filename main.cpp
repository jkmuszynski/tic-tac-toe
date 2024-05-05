#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

using namespace std;

class Player {
public:
    Player() {}
    Player(int idArg, char signArg) : id(idArg), sign(signArg) {}
    int id;
    char sign;
    int wins = 0;
    int draws = 0;
    int loses = 0;
    int points = 0;
};

struct Game {
    Game() {}
    char board[3][3];
    Player player1;
    Player player2;
};

vector<Player> readPlayersFromFile(const string& filename) {
    vector<Player> players;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open file " << filename << endl;
        return players;
    }
    int id;
    char sign;
    while (file >> id >> sign) {
        players.push_back(Player(id, sign));
    }
    file.close();
    return players;
}

vector<Game> readGamesFromFile(const string& filename) {
    vector<Game> games;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open file " << filename << endl;
        return games;
    }
    char symbol;
    vector<char> symbols;
    int count = 0;
    while (file.get(symbol)) {
        if (symbol != '\n') {
            symbols.push_back(symbol);
            ++count;
            if (count == 9) {
                Game game;
                for (int i = 0; i < 3; ++i) {
                    for (int j = 0; j < 3; ++j) {
                        int index = i * 3 + j;
                        game.board[i][j] = symbols[index];
                    }
                }
                games.push_back(game);
                count = 0;
                symbols.clear();
            }
        }
    }
    file.close();
    return games;
}

void printGames(const vector<Game>& games) {
    for (const auto& game : games) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                cout << game.board[i][j];
            }
            cout << endl;
        }
        cout << endl;
    }
}

void printPlayers(const vector<Player>& players) {
    for (const auto& player : players) {
        cout << player.id << " " << player.sign << endl;
    }
}

void recognizePlayers(vector<Game>& games, const vector<Player>& players) {
    for (auto& game : games) {
        char sign1 = '.', sign2 = '.';
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                char sign = game.board[i][j];
                if (sign != '.') {
                    if (sign1 == '.') {
                        sign1 = sign;
                        game.player1.sign = sign1;
                    } else if (sign2 == '.' && sign != sign1) {
                        sign2 = sign;
                        game.player2.sign = sign2;
                    }
                }
            }
        }
        for (const auto& player : players) {
            if (player.sign == sign1) {
                game.player1 = player;
            } else if (player.sign == sign2) {
                game.player2 = player;
            }
        }
    }
}

char determineGameResult(const Game& game) {
    for (int i = 0; i < 3; ++i) {
        if (game.board[i][0] == game.board[i][1] && game.board[i][1] == game.board[i][2] && game.board[i][0] != '.') {
            return game.board[i][0];
        }
    }

    for (int j = 0; j < 3; ++j) {
        if (game.board[0][j] == game.board[1][j] && game.board[1][j] == game.board[2][j] && game.board[0][j] != '.') {
            return game.board[0][j];
        }
    }

    if (game.board[0][0] == game.board[1][1] && game.board[1][1] == game.board[2][2] && game.board[0][0] != '.') {
        return game.board[0][0];
    }
    if (game.board[0][2] == game.board[1][1] && game.board[1][1] == game.board[2][0] && game.board[0][2] != '.') {
        return game.board[0][2];
    }
    return '.';
}

void updatePlayerStats(vector<Player>& players, const Game& game, char result) {
    if (result == '.') {
        for (auto& player : players) {
            if (player.sign == game.player1.sign || player.sign == game.player2.sign) {
                player.draws++;
                player.points += 1;
            }
        }
    } else {
        const Player& winner = (result == game.player1.sign) ? game.player1 : game.player2;
        const Player& loser = (result == game.player1.sign) ? game.player2 : game.player1;
        for (auto& player : players) {
            if (player.id == winner.id) {
                player.wins++;
                player.points += 3;
            } else if (player.id == loser.id) {
                player.loses++;
            }
        }
    }
}


void recognizeGameResults(vector<Game>& games, vector<Player>& players) {
    for (auto& game : games) {
        char result = determineGameResult(game);
        updatePlayerStats(players, game, result);
    }
}

void playersTable(vector<Player>& players) {
    cout << "Id   Sign Wins Draws Loses Points" << endl;
    for (const auto& player : players) {
        cout << player.id << "   " << player.sign << "  " << player.wins << "    " << player.draws << "  " << player.loses << "  " << player.points << endl;
    }
}

void displayPlayerStats(const vector<Player>& players, int id) {
    for (const auto& player : players) {
        if (player.id == id) {
            cout << id << " " << player.sign << " " << player.wins << " " << player.draws << " " << player.loses << " " << player.points << endl;
            return;
        }
    }
    cout << "Player not found" << endl;
}

void displayMenu() {
    cout << "1 - show table" << endl;
    cout << "2 - show results" << endl;
    cout << "3 - show player stats" << endl;
    cout << "4 - exit" << endl;
    cout << "Choose option: ";
}

bool comparePlayers(const Player& player1, const Player& player2) {
    if (player1.points != player2.points) {
        return player1.points > player2.points;
    } else if (player1.wins != player2.wins) {
        return player1.wins > player2.wins;
    } else {
        return player1.draws > player2.draws;
    }
}

void sortPlayers(vector<Player>& players) {
    std::sort(players.begin(), players.end(), comparePlayers);
}

int main() {
    vector<Player> players = readPlayersFromFile("players.txt");
    vector<Game> games = readGamesFromFile("tictactoe.txt");
    recognizePlayers(games, players);
    recognizeGameResults(games, players);
    int choice;
    do {
        displayMenu();
        cin >> choice;
        switch (choice) {
            case 1:
                sortPlayers(players);
                playersTable(players);
                break;
            case 2:
                printGames(games);
                break;
            case 3:
                int playerId;
                cout << "Enter player id: ";
                cin >> playerId;
                displayPlayerStats(players, playerId);
                break;
            case 4:
                cout << "Exiting program." << endl;
                break;
            default:
                cout << "Invalid choice. Please choose again." << endl;
        }
    } while (choice != 4);

    return 0;
}
