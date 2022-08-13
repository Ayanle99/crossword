#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include <fstream>
#include <unordered_map>
using namespace std;


string ToUpper(string s){
    string temp;
    for(char ch : s){
        temp += toupper(ch);
    }
    return temp;
};


struct Point {
    Point(){}
    Point(int r, int c) : row(r), col(c) {}
    
    friend ostream& operator<<(ostream& os, const Point& p);
    
    int row = 0;
    int col = 0;
};

ostream& operator<<(ostream& os, const Point& p){
    os << "(" << p.row << ',' << p.col << ')';
    return os;
}

struct Span {
  
    Span(Point p, int l, bool v) : point(p), len(l), vert(v) {}
    
    Point GetPoint(int i ) const {
        assert(i >= 0 && i < len);
        if (vert) {
            return Point(point.row+i, point.col);
        }else{
            return  Point(point.row, point.col+i);
        }
    }
    friend ostream& operator<<(ostream& os, const Span& s);
    
    Point point;
    int len;
    bool vert;
};

typedef vector<Span> Spans;

ostream& operator<<(ostream& os, const Span& s){
    
    os << "[" << s.point << "len=" << s.len << "vert="<<s.vert << "]";
    
    return os;
}

struct Word {
    Word(){}
    Word(string w): word(w){}
    
    int len() const {
        return word.length();
    }
    string word;
};

typedef vector<Word*> Words;
typedef unordered_map<string, Words> WordMap;

class Library {
    
public:
    ~Library(){
        for(Word* w : words_){
            delete w;
        }
    }
    
    string GetWord(int i ) const {
        assert(i >= 0 && i < words_.size());
        return words_[i]->word;
    }
    
    const Words* FindWord(const string& s) const{
        auto it = word_map_.find(s);
        
        if (it != word_map_.end()) {
            
            return &it->second;
            
       
        }else{
            return NULL;
        }
        
    }
        
    bool IsWord(string s) const {
        auto it = word_map_.find(s);
        
        if (it == word_map_.end()) {
            return false;
        }else{
            return true;
        }
        
    }
    void Print() const{
        for(const Word* s : words_){
            cout << s->word << "\n";
        }
    }
    void ComputeStats(){
        
        assert(counts_.empty());
        counts_.resize(18);
        
        for(const Word* s : words_){
            int len = s->word.length();
            if (len < 18) {
                counts_[len]++;
            }
            
        }
        
    }
    void PrintStats() const {
        
        for (int i =1; i<counts_.size(); i++) {
            cout << "[" << i << "] " << counts_[i] << "\n";
        }
    }
    
    void CreatePatternHash(Word* w){
       
        int len = w->len();
        if (len > 7) {
            return;
        }
        int num_patterns = 1 << len;
        //cout << "Pattern hash on: " << w->word << "\n";
       

        for (int i = 0; i < num_patterns; i++) {
           
            string temp = w->word;
            
            for(int j=0;j<temp.length();j++){
                if((i >>j)&1){
                    temp[j]='-';
                    
                }
            }
            
            //cout << temp << "\n";
            
            word_map_[temp].push_back(w);
            
        }
        
        
        
    }
    void ReadFromFille(string filename, int max_size){
        ifstream f;
        f.open(filename);
        
        while (f.is_open() && !f.eof()) {
            string line;
            getline(f, line);
            line = ToUpper(line);
            
            int len = line.length();
            
        
            if (len <= max_size) {
                Word* w = (new Word(line));
                words_.push_back(w);
                CreatePatternHash(w);
            }
           
            
        
        }
        
        cout << "Read: " << words_.size() << " words." << "\n";
    }
   
    
private:
    Words words_;
    WordMap word_map_;
    vector<int> counts_;
};

Library lib;


struct Attr {
    
    bool is_empty() const {
        return has_blanks && !has_letters;
    }
    bool is_full() const {
        return !has_blanks && has_letters;
    }
    bool is_partial() const {
        return has_letters && has_blanks;
    }
    
    bool has_letters = false;
    bool has_blanks = false;
    
};


struct Grid {
  
    Grid(string n) : name(n) {}
    
    int rows() const {
        return lines.size();
    }
    int cols() const {
        
        if (lines.size() == 0) {
            return 0;
        }else{
            return lines[0].size();
        }
    }
    char box(const Point& p) const {
        assert(in_bounds(p));
        return lines[p.row][p.col];
    }
    void write_box(const Point& p, char c) {
        assert(in_bounds(p));
        lines[p.row][p.col] = c;
    }
    
    bool is_block(const Point& p) const {
        
        assert(in_bounds(p));
        
        return box(p) == '.';
        
    }
    bool is_letter(const Point& p) const {
        assert(in_bounds(p));
        
        return box(p) >= 'A' && box(p) <= 'Z';
        
    }
    bool is_blank(const Point& p) const {
        assert(in_bounds(p));
        
        return box(p) == '-';
    }
    
    bool in_bounds(const Point& p) const{
        return (p.row >= 0 && p.row < rows() && p.col >= 0 && p.col < cols());
    }
    
    string GetString(const Span& s, Attr& attr)const {
       
        int len = s.len;
        
        string temp;
        temp.resize(len);
        
        for(int i = 0; i < len;i++){
            
            Point p = s.GetPoint(i);
            char c = box(p);
            if (c == '-') {
                attr.has_blanks = true;
            }else if (c >= 'A' && c <= 'Z'){
                attr.has_letters = true;
            }
            temp[i] = c;
        }
        return temp;
    }
    void WriteString(const Span& s, const string t)  {
        int len = s.len;
        asin(t.length() == len);
        
        for(int i = 0; i < len;i++){
            
            Point p = s.GetPoint(i);
            write_box(p, t[i]);
            
            
            
        }
    }
    void FillSpans(bool vert){
       
        Point p;
        
        while (in_bounds(p)) {
            
            while (in_bounds(p) && is_block(p)) {
                Next(p, vert);
                
            }
            if (!in_bounds(p)) {
                return;
            }
            Point startp = p;
            
            //cout << "SPAN START -> " << p << "\n";
            
            int len = 0;
            

            do{
                
                Next(p, vert);
                len++;
                
        
            }while(in_bounds(p) && !is_block(p));


           // cout << "End of span.. len = " << len << "\n";
            
            spans.push_back(Span(startp, len, vert));
            
        }
        
 

    }
    void FillSpans(){
        // DON'T DUPLICATE THE SPAN,
        // MAKE SURE TO CALL THIS METHOOD
        // ONLY ONCE
        assert(spans.empty());
        FillSpans(false);
        FillSpans(true);
    }
    
    void PrintSpans() const {
        cout << "Spans:\n";
        for(const Span& s : spans){
            Attr attr;
            cout << s << " " << GetString(s, attr) << "\n";
        }
    }
    
    bool Next(Point& p, bool v){
        
       
        if (v) {
            p.row++;
            if (p.row >= rows()) {
                p.row=0;
                p.col++;
            }
        }else{
            p.col++;
            if (p.col >= cols()) {
                p.col=0;
                p.row++;
            }
        }
        
        return in_bounds(p);
    }
    
 
    void LoadFromFile(string filename){
        ifstream f;
        f.open(filename);
        
        while (f.is_open() && !f.eof()) {
            string line;
            getline(f, line);
            lines.push_back(line);
        }
    }
    
    void Check() const {
       
        for (int i = 0; i<lines.size(); i++) {
            assert(lines[i].size() == cols());
            
        }
        
    }
    
    int max_size() const {
        return max(rows(), cols());
    }
    
    void Print() const {
        
//        cout << "Rows: " << rows() << "\n";
//        cout << "Cols: " << cols() << "\n";
//        cout << "Max size: " << max_size() << "\n";
//
        for(string s : lines){
            cout << s << "\n";
        }
        
    }
    
    string name;
    vector<string> lines;
    Spans spans;
  
    
};

struct Slot {
  
    Slot(const Span& s, const string& p) : span(s), pattern(p) {}
    
    friend ostream& operator<<(ostream& os, const Slot& s);
    
    Span span;
    string pattern;
};

ostream& operator<<(ostream& os, const Slot& s){
    os << s.span << " '" << s.pattern << "'";
    return os;
}


typedef vector<Slot> Slots;
class Solver  {
public:
    Solver(){}
    void Solve(const Grid& grid){
//        cout << "solving this grid." << "\n";
//        grid.Print();
//
        Loop(grid, 0);
    }
    
private:
    void Loop(Grid grid, int depth){
      
        depth++;
        
        
        Slots full_slots;
        Slots partial_slots;
        Slots empty_slots;
       
        for(const Span& s : grid.spans){
            Attr attr;
            string temp = grid.GetString(s, attr);
            
            if (attr.is_empty()) {
                empty_slots.push_back(Slot(s, temp));
            }
            else if(attr.is_partial()){
                partial_slots.push_back(Slot(s, temp));
            }
            else if(attr.is_full()){
                full_slots.push_back(Slot(s, temp));
            }
        }
        
        int num_empty = empty_slots.size();
        int num_partial = partial_slots.size();
        int num_full = full_slots.size();
        
//        cout << "num empty: " << num_empty << "\n";
//        cout << "num partial: " << num_partial << "\n";
//        cout << "num full: " << num_full << "\n";
//
        // CHECK IF WORDS ARE VALID BEFORE MOVING ON
        
        for(const Slot& s : full_slots){
            
            if (!lib.IsWord(s.pattern)) {
                return;
            }
            
        } 
        if (num_partial == 0 && num_empty==0) {
            cout << "SOLUTION\n";
            grid.Print();
            return;
            
        }
        
        assert(num_partial > 0);
        
        CommitSlot(grid, partial_slots[0], depth);
    }
    void CommitSlot(Grid& grid,const Slot slot, int depth){

        const Words* words = lib.FindWord(slot.pattern);        
        
        if (words) {
            for(const Word* w : *words){
                assert(!words->empty());
                grid.WriteString(slot.span, w->word);
                Loop(grid, depth);
            }
        }else{
           // cout << "NO MATCHES FOUND" << "\n";
        }
        
        
    }
    
};


int main(int argc, const char * argv[]) {
    
    string path  =  "assets/grid.txt";
    string path2 = "assets/words.txt";
  
    
    
    Grid grid("My grid");
    grid.LoadFromFile(path);
    grid.Check();
    grid.Print();
    grid.FillSpans();
    grid.PrintSpans();

   
    lib.ReadFromFille(path2, grid.max_size());
    Solver solver;  
    solver.Solve(grid);
    
 
    
 
    
    
}
