#include <iostream>
#include <vector>
#include <stack>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>

using namespace std;

void write_board_csv(const vector<vector<int>>& board, bool first) {
    ios_base::openmode mode = ios::app;
    if (first) mode = ios::trunc;
    ofstream fout("game_output.csv", mode);
    if (!fout) return;
    for (int r=0;r<4;r++){
        for (int c=0;c<4;c++){
            fout<<board[r][c];
            if (!(r==3 && c==3)) fout<<",";
        }
    }
    fout<<"\n";
}

void print_board(const vector<vector<int>>& board, bool first) {
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            std::cout << board[i][j] << "\t";
        }
        std::cout << std::endl;
    }
    write_board_csv(board, first);
}

// TODO: use algorithms to spawn properly
void spawn_tile(std::vector<std::vector<int>>& board) {
    std::vector<std::pair<int, int>> empty_positions;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (board[r][c] == 0) {
                empty_positions.emplace_back(r, c);
            }
        }
    }
    if (empty_positions.empty()) return;
    std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<> pos_dist(0, empty_positions.size() - 1);
    auto [row, col] = empty_positions[pos_dist(gen)];
    std::uniform_int_distribution<> tile_dist(1, 10);
    int new_tile = (tile_dist(gen) == 10) ? 4 : 2;
    board[row][col] = new_tile;
}


// TODO: Compress a row, remove zeroes, and then pad with zeroes at the end
std::vector<int> compress_row(const std::vector<int>& row) {
    vector<int> compressed;
    int zeroes = 0;
    for(int i = 0; i < row.size(); i++){
        if(row[i] != 0){
            compressed.push_back(row[i]);
        }
        else {zeroes ++;}
    }
    for(int i = 0; i < zeroes; i++){
        compressed.push_back(0);
    }
    return compressed;
}
// TODO: Merge a row (assumes the row is already compressed)
std::vector<int> merge_row(std::vector<int> row) {
    for(int i = 0; i < row.size() - 1; i++){
        if(row[i] == row[i + 1] && row[i] != 0){
            row[i] *= 2;
            row[i + 1] = 0;
            i++;
        }
    }
    return row;
}

// TODO: use copy_if and iterators
bool move_left(vector<vector<int>>& board){
    bool moved = false;
    for(int i = 0; i < board.size(); i++){
        vector<int> row_copy = board[i];
        row_copy = merge_row(compress_row(row_copy));
        if (row_copy != board[i])
        {
            moved = true;
            copy(row_copy.begin(), row_copy.end(), board[i].begin());
        }
    }
    return moved;
}
// TODO: use reverse iterators
bool move_right(vector<vector<int>>& board){
    bool moved = false;
    for(int i = 0; i < board.size(); i++){
        vector<int> row_copy = board[i];
        reverse(row_copy.begin(), row_copy.end());
        row_copy = merge_row(compress_row(row_copy));
        reverse(row_copy.begin(), row_copy.end());
        if (row_copy != board[i])
        {
            moved = true;
            copy(row_copy.begin(), row_copy.end(), board[i].begin());
        }
    }
    return moved;
}
// TODO: use column traversal
bool move_up(vector<vector<int>>& board){
    bool moved = false;
    for(int i = 0; i < board.size(); i++){
        vector<int> col;
        for (int j = 0; j < board[i].size(); j++) col.push_back(board[j][i]);
        vector<int> old_col = col;
        col = merge_row(col);
        for (int j = 0; j < board[i].size(); j++) board[j][i] = col[j];
        if (col != old_col) moved = true;
    }
    return moved;
}
// TODO: use column traversal with reverse
bool move_down(vector<vector<int>>& board){
    bool moved = false;
    for(int i = 0; i < board.size(); i++){
        vector<int> col;
        for (int j = 0; j < board[i].size(); j++) col.push_back(board[j][i]);
        vector<int> old_col = col;
        reverse(col.begin(), col.end());
        col = merge_row(col);
        reverse(col.begin(), col.end());
        for (int j = 0; j < board[i].size(); j++) board[j][i] = col[j];
        if (col != old_col) moved = true;
    }
    return moved;
}

int main(){
    srand(time(nullptr));
    vector<vector<int>> board(4, vector<int>(4,0));
    spawn_tile(board);
    spawn_tile(board);

    stack<vector<vector<int>>> history;
    bool first=true;

    while(true){
        print_board(board, first);
        first=false;
        cout<<"Move (w=up, a=left, s=down, d=right), u=undo, q=quit: ";
        char cmd;
        if (!(cin>>cmd)) break;
        if (cmd=='q') break;

        if (cmd=='u') {
            // TODO: get the history and print the board and continue
            if(history.empty()){
                std::cout << "No history to undo";
            }
            else{
                board = history.top();
                history.pop();
            }
            print_board(board,false);
            continue;
        }

        vector<vector<int>> prev = board;
        bool moved=false;
        if (cmd=='a') moved=move_left(board);
        else if (cmd=='d') moved=move_right(board);
        else if (cmd=='w') moved=move_up(board);
        else if (cmd=='s') moved=move_down(board);

        if (moved) {
            // TODO: Store the previous state here!
            history.push(prev);
            spawn_tile(board);
        }
    }
    return 0;
}
