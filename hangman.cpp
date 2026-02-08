#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <cctype>

using namespace std;

/* ===== COLORS ===== */
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

/* ===== ASCII ===== */
vector<string> art = {
" +---+\n     |\n     |\n     |\n    ===",
" +---+\n O   |\n     |\n     |\n    ===",
" +---+\n O   |\n |   |\n     |\n    ===",
" +---+\n O   |\n/|   |\n     |\n    ===",
" +---+\n O   |\n/|\\  |\n     |\n    ===",
" +---+\n O   |\n/|\\  |\n/    |\n    ===",
" +---+\n O   |\n/|\\  |\n/ \\  |\n    ==="
};

/* ===== TIMER ===== */
bool timeout(auto start, int sec) {
    return chrono::duration_cast<chrono::seconds>(
        chrono::steady_clock::now() - start).count() > sec;
}

/* ===== SAVE GAME ===== */
void saveGame(string w, string g, vector<char> wrong,
              int m, int s) {

    ofstream f("savegame.txt");
    f << w << "\n" << g << "\n"
      << m << "\n" << s << "\n";

    for (char c : wrong) f << c;
}

bool loadGame(string &w, string &g,
              vector<char> &wrong,
              int &m, int &s) {

    ifstream f("savegame.txt");
    if (!f) return false;

    getline(f, w);
    getline(f, g);
    f >> m >> s;

    string x;
    f >> x;
    for (char c : x) wrong.push_back(c);

    return true;
}

/* ===== SCOREBOARD ===== */
void saveScore(string name, int s) {
    ofstream f("scores.txt", ios::app);
    f << name << " " << s << endl;
}

void showLeaderboard() {

    ifstream f("scores.txt");
    vector<pair<string,int>> list;

    string n; int s;
    while (f >> n >> s)
        list.push_back({n,s});

    sort(list.begin(), list.end(),
         [](auto a, auto b) {
             return a.second > b.second;
         });

    cout << BLUE << "\n🏆 LEADERBOARD\n" << RESET;
    for (int i = 0; i < list.size() && i < 5; i++)
        cout << i+1 << ". " << list[i].first
             << " - " << list[i].second << endl;
}

/* ===== ACHIEVEMENTS ===== */
void checkAchievement(int score) {
    if (score >= 50)
        cout << YELLOW
             << "🏅 Achievement: Hangman Master!\n"
             << RESET;
    else if (score >= 20)
        cout << YELLOW
             << "🏅 Achievement: Word Hunter!\n"
             << RESET;
}

/* ===== GAME ===== */
bool playGame(string word, int &score,
              int maxMistakes,
              bool timer, int seconds) {

    transform(word.begin(), word.end(),
              word.begin(), ::tolower);

    string guess(word.size(), '_');
    vector<char> wrong;
    int mistakes = 0;
    bool hintUsed = false;

    auto start = chrono::steady_clock::now();

    while (mistakes < maxMistakes && guess != word) {

        if (timer && timeout(start, seconds)) {
            cout << RED << "\n⏰ TIME UP!\n" << RESET;
            return false;
        }

        cout << RED << art[mistakes]
             << RESET << "\n";

        cout << "\nWord: ";
        for (char c : guess) cout << c << " ";

        cout << "\nWrong: ";
        for (char c : wrong) cout << c << " ";

        cout << "\n(h=hint, s=save & exit)";
        cout << "\nGuess: ";

        char g;
        cin >> g;
        g = tolower(g);

        if (g == 's') {
            saveGame(word, guess,
                     wrong, mistakes,
                     score);
            cout << YELLOW
                 << "\nGame Saved!\n"
                 << RESET;
            exit(0);
        }

        if (g == 'h' && !hintUsed) {
            for (int i = 0; i < word.size(); i++)
                if (guess[i] == '_') {
                    guess[i] = word[i];
                    break;
                }
            hintUsed = true;
            continue;
        }

        if (!isalpha(g)) continue;

        bool ok = false;

        for (int i = 0; i < word.size(); i++)
            if (word[i] == g &&
                guess[i] == '_') {
                guess[i] = g;
                ok = true;
            }

        if (!ok &&
            find(wrong.begin(),
                 wrong.end(), g)
            == wrong.end()) {
            wrong.push_back(g);
            mistakes++;
        }

        saveGame(word, guess,
                 wrong, mistakes,
                 score);
    }

    remove("savegame.txt");

    if (guess == word) {
        cout << GREEN
             << "\n🎉 YOU WON! "
             << word << RESET << endl;
        score += 10;
        return true;
    }

    cout << RED
         << "\n💀 YOU LOST! "
         << word << RESET << endl;
    return false;
}

/* ===== MAIN ===== */
int main() {

    srand(time(0));

    int score = 0;
    string name;

    cout << "Enter name: ";
    cin >> name;

    string w, g;
    vector<char> wrong;
    int m = 0;

    if (loadGame(w, g, wrong, m, score)) {
        cout << "Resume saved game? (y/n): ";
        char c; cin >> c;
        if (c == 'y')
            playGame(w, score,
                     6, false, 0);
    }

    int ch;
    do {
        cout << BLUE
             << "\n==== HANGMAN EXTREME ====\n"
             << RESET;
        cout << "1.New Game\n";
        cout << "2.Leaderboard\n";
        cout << "3.Exit\n";
        cin >> ch;

        if (ch == 2) showLeaderboard();

        if (ch == 1) {

            cout << "Difficulty 1-Easy 2-Med 3-Hard: ";
            int d; cin >> d;

            int maxMist =
                (d==1?6:(d==2?5:4));
            bool timer = d > 1;
            int sec =
                (d==2?60:40);

            vector<string> temp =
            {"computer","internet",
             "keyboard","gaming"};

            string word =
                temp[rand()%temp.size()];

            playGame(word, score,
                     maxMist,
                     timer, sec);
        }

    } while (ch != 3);

    saveScore(name, score);
    checkAchievement(score);

    cout << "\nFinal score saved 👋\n";
}
