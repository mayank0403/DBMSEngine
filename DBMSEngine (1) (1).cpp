#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <math.h>
#include <algorithm>

using namespace std;

vector<string> tabnames;
// This is the structure of input type. All input is taken in form of a string and is then converted to entrytype when inserted in table.
struct inputtype{
    string str;
    int flag=0; // flag =0 means string, 1 means int and 2 means float.
    string name[3]= {"string", "int", "float"};
    // Here 0 means string, 1 means int and 2 means float.
};

// This is the structure of a table cell. Flag denotes the type of table cell. Flag = 0 means string, 1 means int and 2 is float.
struct entrytype{
    string str; // this value holds the value if the cell type is string
    int val = -100; // this value holds the value if the cell type is int
    float fval; // this value holds the value if the cell type is float
    int flag = -100.0;
    // flag = 0 means string, 1 means integer and 2 means float
};
// This is the table data structure.
struct table{
    vector <vector <entrytype*> > intable; // A table is represented as a vector of vector of entrytype(data structure for a cell). It is a vector of column vectors(which is a table column).
    map <string, int> maptoplace; // This map if given attribute name, it gives the index of the column corresponding to this attribute.
    map <string, int> maptotype; // This map if given attribute name, it gives the data type(string, int or float).
    int numberofatts;
};

map <string, table*> tablemap; // This map if given table name gives pointer to the table.
// This function converts a string value to integer number.
int stringtoint(string input){
    int ans=0, res=1;
    for(int i=0; i<input.size(); i++){
        ans += res*(input[input.size()-1-i]-'0');
        res *= 10;
    }
    return ans;
}
// This function converts a string to float number. These functions are used to go from inputtype to entrytype.
float stringtofloat(string input){
    int k=0;
    for(k=0; k<input.size(); k++){
        if(input[k]=='.')
            break;
    }
    string temp="";
    for(int i=0; i<input.size(); i++){
        if(input[i]=='.')
            continue;
        temp += input[i];
    }
    int a = stringtoint(temp);
    int div = 1;
    for(int i=0; i<input.size()-k-1; i++){
        div *= 10;
    }
    return (float)a/(float)div;
}
// This function converts integer number to its string representation.
string inttostring(int input){
    int r, q;
    q = input;
    string ret="";
    while(q){
        r = q%10;
        q = q/10;
        ret += (char)(r+48);
    }
    string retf="";
    for(int i=0; i<ret.size(); i++){
        retf += ret[ret.size()-1-i];
    }
    return retf;
}
// This function converts floating point value to string representation.
string floattostring(float input){
    std::ostringstream buff;
    buff<<input;
    return buff.str();
}
// This function converts strings inputs (as received from user) and fills up input type data structure from it.
inputtype* getinputtype(string input){
    inputtype * ret = new inputtype;
    if(input[0]=='\"' && input[input.size()-1]=='\"'){
        ret->flag=0;
        ret->str = input.substr(1, input.size()-2);
    }
    else{
        int i=0, pointcnt=0;
        for(i=0; i<input.size(); i++){
            if(input[i]=='.')
                pointcnt++;
            if(((int)input[i] < 48 || (int)input[i] > 57) && ((int)input[i] !=46 || pointcnt>1)){
                break;
            }
        }
        if(i<input.size()){
            ret->flag=0;
            ret->str = input;
            return ret;
        }
        bool flag = false;
        for(i=0; i<input.size(); i++){
            if(input[i]=='.'){
                flag = true;
                break;
            }
        }
        if(flag){
            ret->flag=2;
            ret->str = input;
        }
        else{
            ret->flag=1;
            ret->str = input;
        }
    }
    return ret;
}
// This function removes extra spaces from strings. It converts them into query tokens.
string removewhitespacesfromends(string input){
    char c1 = input[0];
    while(c1==' '){
        input = input.substr(1);
        c1 = input[0];
    }
    int i = input.size()-1;
    c1 = input[i--];
    while(c1==' '){
        c1 = input[i--];
    }
    input = input.substr(0, i+2);
    return input;
}
// This function splits strings separated from commas. It helps in tokenizing the query.
vector<string> splitcomma(string input){
    char c;
    string temp="";
    vector <string> buffer;
    for(int i=0; i<input.size(); i++){
        c = input[i];
        temp="";
        while(c != ',' && c != '\n' && i<input.size()){
            temp += c;
            i++;
            c = input[i];

        }
        if(temp.compare("")!=0){
            char c1 = temp[0];
            while(c1==' '){
                temp = temp.substr(1);
                c1 = temp[0];
            }
            buffer.push_back(temp);
        }
    }
    return buffer;
}
// This is the master split function, it checks if commas, whitespaces, etc. are present and calls the above specific split functions if necessary.
vector<string> split(string input){
    char c;
    string temp="";
    vector <string> buffer;
    for(int i=0; i<input.size(); i++){
        c = input[i];
        temp="";
        bool flag = false;
        if(c=='('){
            flag = true;
            i++;
            c = input[i];
           }
        while((c != ' ' || flag) && c != '\n' && i<input.size() && c!= ')'){
            if(c=='('){
                flag = true;
                i++;
                c = input[i];
                if(temp.compare("")!=0)
                    buffer.push_back(temp);
                temp = "";
                }

            temp += c;
            i++;
            c = input[i];

        }
        if(flag){
            vector <string> commabuf = splitcomma(temp);
            for(int j=0; j<commabuf.size(); j++){
                buffer.push_back(commabuf[j]);
            }
        }
        if(temp.compare("")!=0 && !flag)
            buffer.push_back(temp);
        if(c==')'){
            flag = false;
        }
    }
    for(int i=0; i<buffer.size(); i++){
        buffer[i] = removewhitespacesfromends(buffer[i]);
    }
    return buffer;
}
// This function splits query items separated by tabs, it is used while loading from file, as items there are separated by tabs. This also checks and splits about commas and whitespaces also.
vector<string> splittabs(string input){
    char c;
    string temp="";
    vector <string> buffer;
    for(int i=0; i<input.size(); i++){
        c = input[i];
        temp="";
        bool flag = false;
        if(c=='('){
            flag = true;
            i++;
            c = input[i];
           }
        while((c != '\t' || flag) && c != '\n' && i<input.size() && c!= ')'){
            if(c=='('){
                flag = true;
                i++;
                c = input[i];
                if(temp.compare("")!=0)
                    buffer.push_back(temp);
                temp = "";
                }

            temp += c;
            i++;
            c = input[i];

        }
        if(flag){
            vector <string> commabuf = splitcomma(temp);
            for(int j=0; j<commabuf.size(); j++){
                buffer.push_back(commabuf[j]);
            }
        }
        if(temp.compare("")!=0 && !flag)
            buffer.push_back(temp);
        if(c==')'){
            flag = false;
        }
    }
    for(int i=0; i<buffer.size(); i++){
        buffer[i] = removewhitespacesfromends(buffer[i]);
    }
    return buffer;
}
// This function splits tokens for nested queries.
vector<string> splitnested(string input){
    char c;
    string temp="";
    vector <string> buffer;
    for(int i=0; i<input.size(); i++){
        c = input[i];
        temp="";
        while(c != '[' && c != ']'&& c != '\n' && i<input.size()){
            temp += c;
            i++;
            c = input[i];

        }
        if(temp.compare("")!=0){
            char c1 = temp[0];
            while(c1==' '){
                temp = temp.substr(1);
                c1 = temp[0];
            }
            buffer.push_back(temp);
        }
    }
    return buffer;
}
// This is the main function to insert a table row in a table.
void insertintable(table* t, vector<entrytype*> params){
    int i=0;
    // For the first row vector should be initialized.
    if(t->intable.size()==0){
        for(i=0; i<params.size(); i++){
            vector<entrytype*> temp;
            temp.push_back(params[i]);
            t->intable.push_back(temp);
        }
        return;
    }
    // For rest of the rows, we can just add the new entries directly after the already present entries.
    for(i=0; i<params.size(); i++){
        t->intable[i].push_back(params[i]);
    }
}
// This function converts the string tokens to fill the table cell data type.
vector<entrytype*> genparams(vector<string> input){
    // IMPORTANT : provide it the params only to convert them into entrytype
    vector<entrytype*> ret;
    for(int i=0; i<input.size(); i++){
        inputtype *rec = getinputtype(input[i]);
        input[i] = rec->str;
        entrytype * temp = new entrytype;
        temp->flag = rec->flag;
        if(rec->flag==0){
            temp->str = rec->str;
        }
        else if(rec->flag==1){// convert string to int to save for int value.
            temp->val = stringtoint(rec->str);
        }
        else{// convert string to float to save for float value.
            temp->fval = stringtofloat(rec->str);
        }
        ret.push_back(temp);
    }
    return ret;
}
// This is the function to create a table, i.e. it initializes the table data structure from given data types and attributes.
table* createtable(vector<int> types, vector<string> atts){
    table * ret = new table;
    for(int i=0; i<atts.size(); i++){
        ret->maptoplace[atts[i]]=i;
        ret->maptotype[atts[i]]=types[i];
    }
    ret->numberofatts = atts.size();
    return ret;
}
// This function is used to project some columns out of the table and returns a new table from them.
table* project(table* t, vector<string> atts){
    // Here atts is attribute names
    if(atts.size()>t->intable.size()){
        cout<<"Error! More attributes given than in table!\n\n";
        return nullptr;
    }
    table * ret = new table; // Table to be returned.
    map<string, bool> duplicatesmap;
    for(int i=0; i<atts.size(); i++){
        if(t->maptoplace.find(atts[i]) != t->maptoplace.end()){
        }
        else{// This is entered if the attribute is not present in table after finding for the attribute name key in the maptoplace map.
            cout<<"\nAttribute "<<atts[i]<<" not present in table\n\n";
            return nullptr;
        }// The maps for the resulting table are also added to it by just considering the given attributes from the original table.
        vector<entrytype*> temp = t->intable[t->maptoplace[atts[i]]];
        ret->intable.push_back(temp);
        ret->maptoplace[atts[i]]=i;
        ret->maptotype[atts[i]]=t->maptotype[atts[i]];
    }
    int cnt=0;// This counts the rows that will be deleted because of duplicate tuples.
    for(int i=0; i<ret->intable[0].size(); i++){
        string temp="";// To check for duplicate tuples, I just make a string from the row by appending row items together and add it to a map to check that it does not repeat again.
        for(int j=0; j<atts.size(); j++){
            if(ret->intable[j][i]->flag==0)
                temp += ret->intable[j][i]->str;
            else if(ret->intable[j][i]->flag==1){
                temp += (char)(ret->intable[j][i]->val+48);}//cout<<"NP"<<endl;}
            else{
                temp += floattostring(ret->intable[j][i]->fval);
            }
        }
        temp+=',';
        if(duplicatesmap[temp]==true){
                cnt++;
                for(int j=0; j<atts.size(); j++){
                    ret->intable[j].erase(ret->intable[j].begin()+i);// If a tuple is repeated, it is deleted from the return table.
                }
                i--;
        }
        else
            duplicatesmap[temp]=true; // else the row is added to map so that it is not repeated ever.
    }
    return ret;
}
// This function check if 2 tables are compatible to the union operation.
bool unioncompatible(table* t1, table* t2){
    if(t1->intable.size()!=t2->intable.size()){
        cout<<"a\n";
        return false;
    }
    for(int j=0; j<t1->intable.size(); j++){
        string t1att, t2att;
        for (auto &i : t1->maptoplace) {
                if (i.second == j) {// find all the attributes of table and get their types.
                    t1att = i.first;
                    break;
                }
        }
        for (auto &i : t2->maptoplace) {
                if (i.second == j) {
                    t2att = i.first;
                    break;
                }
        }// If types of corresponding tables do not match, then the tables are incompatible.
        if(t1->maptotype[t1att]!=t2->maptotype[t2att]){
            cout<<"b\n"<<t1att<<" "<<t1->maptotype[t1att]<<" "<<t2att<<" "<<t2->maptotype[t2att]<<endl;
            return false;
        }
    }
    return true;
}
// This function checks if a string has hyphen in it. If it has hyphen, then the substring before hyphen is the table name.
bool containshyphen(string input){
    int k=0;
    while(input[k]!='-' && k<input.size()){
        k++;
    }
    if(k==input.size())
        return false;
    return true;
}
table* setunion(table*, table*, int);// This is skeleton of the joint function for union, intersection and set difference.
// This is the function used for selection. Here andor = 1 means that all the conditions are ORed else all conditions are ANDed.
table* select(table* t, table *uniteto, vector<string> atts, string op, vector<string> value, int andor){
    // Here atts is attribute names
    table * ret = nullptr;
    if(t==nullptr || t->intable.size()==0)
        return nullptr;
    map<string, bool> duplicatesmap;// This map is first initialized and then used to check if a repeated row comes.
    for(int i=0; i<value.size(); i++){
        if(containshyphen(value[i])){// If comparison is done with a column rather than a constant, then we have to check if the column is present or not.
            if(t->maptoplace.find(value[i]) != t->maptoplace.end()){
                ;
            }
            else{
                cout<<"\nEnter correct RHS attributes for constraint "<<i+1<<" (try using tablename-attribute)! RHS attributes not available in table!\n\n";
                return nullptr;
            }
        }
    }
    if(atts.size()!=op.size() || atts.size()!=value.size() || op.size()!=value.size()){
        cout<<"\nSyntax Error\n\n";
        return nullptr;
    }
    // This function is a recursive function so, it first evaluates for the 1st condition and then recursively calls for other conditions.
    if(t->maptoplace.find(atts[0]) != t->maptoplace.end()){
            ;
    }
    else{
        cout<<"\nAttribute "<<atts[0]<<" not present in table\n\n";
        return nullptr;
    }
    vector<entrytype*> x = t->intable[t->maptoplace[atts[0]]];
    if(x.size()==0){
        cout<<"\nAttribute "<<atts[0]<<" not present in table\n\n";
        return nullptr;
    }

    table* t1 = new table;
    // The column name and type maps will remain same as the table structure is not altered.
    t1->maptoplace = t->maptoplace;
    t1->maptotype = t->maptotype;
    int cn=0;
    for(int j=0; j<x.size(); j++){
        if(op[0]=='='){ // if condition operator is =

                if(t->maptotype[atts[0]]==1){// if attribute type is int
                    int val = stringtoint(value[0]);
                    string atr = value[0];
                    if(containshyphen(value[0])){
                        val = t->intable[t->maptoplace[atr]][j]->val;
                    }
                    if(x[j]->val == val){
                    cn++;// As condition is met here, we initialize the result vector with the first row that matches the condition
                    if(cn==1){
                        for(int k=0; k<t->intable.size(); k++){
                            vector<entrytype*> v;
                            entrytype* e = new entrytype;
                            for (auto &i1 : t->maptoplace) { // To find the name of attributes and hence their types.
                                if (i1.second == k) {
                                    e->flag = t->maptotype[i1.first];
                                    break; // to stop searching
                                }
                            }
                            if(e->flag==0)
                                e->str = t->intable[k][j]->str;
                            else if(e->flag==1)
                                e->val = t->intable[k][j]->val;
                            else
                                e->fval = t->intable[k][j]->fval;
                            v.push_back(e);
                            t1->intable.push_back(v);
                        }
                    } // If the result table has already been initialized, now for other promising rows we just push them at the back of existing rows.
                    else{
                        for(int k=0; k<t->intable.size(); k++){
                            entrytype* e = new entrytype;
                            for (auto &i1 : t->maptoplace) {
                                if (i1.second == k) {
                                    e->flag = t->maptotype[i1.first];
                                    break; // to stop searching
                                }
                            }
                            if(e->flag==0)
                                e->str = t->intable[k][j]->str;
                            else if(e->flag==1)
                                e->val = t->intable[k][j]->val;
                            else
                                e->fval = t->intable[k][j]->fval;
                            t1->intable[k].push_back(e);
                        }
                    }
                }
                }
        else if (t->maptotype[atts[0]]==0){// If datatype is string
                string val = value[0];
                string atr = value[0];
                    if(containshyphen(value[0])){
                        val = t->intable[t->maptoplace[atr]][j]->str;
                    }
                if(x[j]->str.compare(val)==0){
                    cn++;
                    if(cn==1){
                        for(int k=0; k<t->intable.size(); k++){
                            vector<entrytype*> v;
                            entrytype* e = new entrytype;
                            for (auto &i1 : t->maptoplace) {
                                if (i1.second == k) {
                                    e->flag = t->maptotype[i1.first];
                                    break; // to stop searching
                                }
                            }
                            if(e->flag==0)
                                e->str = t->intable[k][j]->str;
                            else if(e->flag==1)
                                e->val = t->intable[k][j]->val;
                            else
                                e->fval = t->intable[k][j]->fval;
                            v.push_back(e);
                            t1->intable.push_back(v);
                        }
                    }
                    else{
                        for(int k=0; k<t->intable.size(); k++){
                            entrytype* e = new entrytype;
                            for (auto &i1 : t->maptoplace) {
                                if (i1.second == k) {
                                    e->flag = t->maptotype[i1.first];
                                    break; // to stop searching
                                }
                            }
                            if(e->flag==0)
                                e->str = t->intable[k][j]->str;
                            else if(e->flag==1)
                                e->val = t->intable[k][j]->val;
                            else
                                e->fval = t->intable[k][j]->fval;
                            t1->intable[k].push_back(e);
                        }
                    }
                }
        }

        else{// if data type is float
                    float val;
                    string ar = value[0];
                    if(containshyphen(value[0])){
                        val = t->intable[t->maptoplace[ar]][j]->fval;
                    }
                    else{
                        val = stringtofloat(value[0]);
                    }
                    if(abs(x[j]->fval - val)<0.000001){// floats are compared using this method rather than ==.
                    cn++;
                    if(cn==1){
                        for(int k=0; k<t->intable.size(); k++){
                            vector<entrytype*> v;
                            entrytype* e = new entrytype;
                            for (auto &i1 : t->maptoplace) {
                                if (i1.second == k) {
                                    e->flag = t->maptotype[i1.first];
                                    break; // to stop searching
                                }
                            }
                            if(e->flag==0)
                                e->str = t->intable[k][j]->str;
                            else if(e->flag==1)
                                e->val = t->intable[k][j]->val;
                            else
                                e->fval = t->intable[k][j]->fval;
                            v.push_back(e);
                            t1->intable.push_back(v);
                        }
                    }
                    else{
                        for(int k=0; k<t->intable.size(); k++){
                            entrytype* e = new entrytype;
                            for (auto &i1 : t->maptoplace) {
                                if (i1.second == k) {
                                    e->flag = t->maptotype[i1.first];
                                    break; // to stop searching
                                }
                            }
                            if(e->flag==0)
                                e->str = t->intable[k][j]->str;
                            else if(e->flag==1)
                                e->val = t->intable[k][j]->val;
                            else
                                e->fval = t->intable[k][j]->fval;
                            t1->intable[k].push_back(e);
                        }
                    }
                }
                }


        }// The same follows for condition operator = >
        else if(op[0]=='>'){
                if(t->maptotype[atts[0]]==1){
                    int val = stringtoint(value[0]);
                    string ar = value[0];
                    if(containshyphen(value[0])){
                        val = t->intable[t->maptoplace[ar]][j]->val;
                    }
                    if(x[j]->val > val){
                    cn++;
                    if(cn==1){
                        for(int k=0; k<t->intable.size(); k++){
                            vector<entrytype*> v;
                            entrytype* e = new entrytype;
                            for (auto &i1 : t->maptoplace) {
                                if (i1.second == k) {
                                    e->flag = t->maptotype[i1.first];
                                    break; // to stop searching
                                }
                            }
                            if(e->flag==0)
                                e->str = t->intable[k][j]->str;
                            else if(e->flag==1)
                                e->val = t->intable[k][j]->val;
                            else
                                e->fval = t->intable[k][j]->fval;
                            v.push_back(e);
                            t1->intable.push_back(v);
                        }
                    }
                    else{
                        for(int k=0; k<t->intable.size(); k++){
                            entrytype* e = new entrytype;
                            for (auto &i1 : t->maptoplace) {
                                if (i1.second == k) {
                                    e->flag = t->maptotype[i1.first];
                                    break; // to stop searching
                                }
                            }
                            if(e->flag==0)
                                e->str = t->intable[k][j]->str;
                            else if(e->flag==1)
                                e->val = t->intable[k][j]->val;
                            else
                                e->fval = t->intable[k][j]->fval;
                            t1->intable[k].push_back(e);
                        }
                    }
                }
                }
                else if(t->maptotype[atts[0]]==2){
                    float val;
                    string ar = value[0];
                    if(containshyphen(value[0])){
                        val = t->intable[t->maptoplace[ar]][j]->fval;
                    }
                    else{
                        val = stringtofloat(value[0]);
                    }
                    if(x[j]->fval > val){
                    cn++;
                    if(cn==1){
                        for(int k=0; k<t->intable.size(); k++){
                            vector<entrytype*> v;
                            entrytype* e = new entrytype;
                            for (auto &i1 : t->maptoplace) {
                                if (i1.second == k) {
                                    e->flag = t->maptotype[i1.first];
                                    break; // to stop searching
                                }
                            }
                            if(e->flag==0)
                                e->str = t->intable[k][j]->str;
                            else if(e->flag==1)
                                e->val = t->intable[k][j]->val;
                            else
                                e->fval = t->intable[k][j]->fval;
                            v.push_back(e);
                            t1->intable.push_back(v);
                        }
                    }
                    else{
                        for(int k=0; k<t->intable.size(); k++){
                            entrytype* e = new entrytype;
                            for (auto &i1 : t->maptoplace) {
                                if (i1.second == k) {
                                    e->flag = t->maptotype[i1.first];
                                    break; // to stop searching
                                }
                            }
                            if(e->flag==0)
                                e->str = t->intable[k][j]->str;
                            else if(e->flag==1)
                                e->val = t->intable[k][j]->val;
                            else
                                e->fval = t->intable[k][j]->fval;
                            t1->intable[k].push_back(e);
                        }
                    }
                }
                }

        }
        else{ // The same follows for condition operator = <
            if(t->maptotype[atts[0]]==1){
                    int val = stringtoint(value[0]);
                    string ar = value[0];
                    if(containshyphen(value[0])){
                        val = t->intable[t->maptoplace[ar]][j]->val;
                    }
                    if(x[j]->val < val){
                    cn++;
                    if(cn==1){
                        for(int k=0; k<t->intable.size(); k++){
                            vector<entrytype*> v;
                            entrytype* e = new entrytype;
                            for (auto &i1 : t->maptoplace) {
                                if (i1.second == k) {
                                    e->flag = t->maptotype[i1.first];
                                    break; // to stop searching
                                }
                            }
                            if(e->flag==0)
                                e->str = t->intable[k][j]->str;
                            else if(e->flag==1)
                                e->val = t->intable[k][j]->val;
                            else
                                e->fval = t->intable[k][j]->fval;
                            v.push_back(e);
                            t1->intable.push_back(v);
                        }
                    }
                    else{
                        for(int k=0; k<t->intable.size(); k++){
                            entrytype* e = new entrytype;
                            for (auto &i1 : t->maptoplace) {
                                if (i1.second == k) {
                                    e->flag = t->maptotype[i1.first];
                                    break; // to stop searching
                                }
                            }
                            if(e->flag==0)
                                e->str = t->intable[k][j]->str;
                            else if(e->flag==1)
                                e->val = t->intable[k][j]->val;
                            else
                                e->fval = t->intable[k][j]->fval;
                            t1->intable[k].push_back(e);
                        }
                    }
                }
                }

                else if(t->maptotype[atts[0]]==2){
                    float val;
                    string ar = value[0];
                    if(containshyphen(value[0])){
                        val = t->intable[t->maptoplace[ar]][j]->fval;
                    }
                    else{
                        val = stringtofloat(value[0]);
                    }
                    if(x[j]->fval < val){
                    cn++;
                    if(cn==1){
                        for(int k=0; k<t->intable.size(); k++){
                            vector<entrytype*> v;
                            entrytype* e = new entrytype;
                            for (auto &i1 : t->maptoplace) {
                                if (i1.second == k) {
                                    e->flag = t->maptotype[i1.first];
                                    break; // to stop searching
                                }
                            }
                            if(e->flag==0)
                                e->str = t->intable[k][j]->str;
                            else if(e->flag==1)
                                e->val = t->intable[k][j]->val;
                            else
                                e->fval = t->intable[k][j]->fval;
                            v.push_back(e);
                            t1->intable.push_back(v);
                        }
                    }
                    else{
                        for(int k=0; k<t->intable.size(); k++){
                            entrytype* e = new entrytype;
                            for (auto &i1 : t->maptoplace) {
                                if (i1.second == k) {
                                    e->flag = t->maptotype[i1.first];
                                    break; // to stop searching
                                }
                            }
                            if(e->flag==0)
                                e->str = t->intable[k][j]->str;
                            else if(e->flag==1)
                                e->val = t->intable[k][j]->val;
                            else
                                e->fval = t->intable[k][j]->fval;
                            t1->intable[k].push_back(e);
                        }
                    }
                }
                }
        }
    }
    ret = t1; // We copy the temp table pointer to the return table pointer variable.
    if(ret->intable.size()>0){// If there are entries in the return table i.e. table is not empty, we remove the duplicate tuples
    for(int i=0; i<ret->intable[0].size(); i++){
        string temp="";
        for(int j=0; j<ret->intable.size(); j++){
            if(ret->intable[j][i]->flag==0)
                temp += ret->intable[j][i]->str;
            else if(ret->intable[j][i]->flag==1)
                temp += (char)(ret->intable[j][i]->val+48);
            else{
                temp += floattostring(ret->intable[j][i]->fval);
            }
        }
        temp+=',';
        if(duplicatesmap[temp]==true){
                for(int j=0; j<atts.size(); j++){
                    ret->intable[j].erase(ret->intable[j].begin()+i);
                }
                i--;
        }
        else
            duplicatesmap[temp]=true;
    }}
    if(op.size()>1){// when there are more than 1 condition to be fulfilled, we have to send the remaining conditions to the recursive call.
        op = op.substr(1);
        vector<string>::const_iterator first = atts.begin() + 1;
        vector<string>::const_iterator last = atts.end();
        vector<string> atts1(first, last);
        vector<string>::const_iterator first1 = value.begin() + 1;
        vector<string>::const_iterator last1 = value.end();
        vector<string> value1(first1, last1);
        if(andor==1){// If we want to have conditions ORed and this is the 1st call, we will do union of the current solved table with the table with rest of the conditions.
            return select(t, ret, atts1, op, value1, 0);
        }
        else{
        if(uniteto==nullptr){ // If we want to have conditions ANDed, we pass null to uniteto
            return select(ret, nullptr, atts1, op, value1, 0);
        }
        else // This is the call for OR, but not the 1st call.
            return select(t, ret, atts1, op, value1, 0);}
    }
    if(uniteto!=nullptr){
        if(uniteto->intable.size()==0 && ret->intable.size()==0)
            return nullptr;
        return setunion(uniteto, ret, 0); // we do union of the 2 tables as the conditions are ORed.
    }
    else{
        if(ret->intable.size()==0)
            return nullptr;
        return ret;
    }
}
// This function renames the table, it adds the new table name in the tablemap
bool rename(table* t, string name){
    if(tablemap[name])
        return false;
    tablemap[name] = t;
    return true;
}
// This function renames attributes of the table with the name as well.
map<string, int> renameatts(table* t, string name, vector<string> newatts){
    map <string, int> newmap, temp;
    temp = t->maptoplace;
    if(newatts.size()!=t->intable.size()){ // Checks if all attributes are given the new name or not.
        cout<<"\nEnter the names for all the new attributes in order\n\n";
        temp["error"] = -1;
        return temp;
    }
    if(tablemap[name]){ // We cannot use the same table name in rename as the name of some original table.
        cout<<"Error : Name already assigned to another table"<<endl;
        return temp;
    }
    tablemap[name] = t;
    if(newatts.size()!=t->intable.size()){
        cout<<"Enter a new name for all the attributes!\n"<<endl;
        temp["error"] = -1;
        return temp;
    }
    for(int i=0; i<newatts.size(); i++){
        newmap[newatts[i]] = i; // A new attribute name to index map is created for the renamed table with new att. names.
    }
    for(int j=0; j<newatts.size(); j++){
        string attname;
        for (auto &i : t->maptoplace) {
                if (i.second == j) {
                    attname = i.first;
                    break; // to stop searching
                }
        }
        t->maptotype[newatts[j]] = t->maptotype[attname];
    }
    t->maptoplace = newmap;
    return temp;
}
// This is a utility function that prints the table in a tabular form.
void printtable(table* t, string tabname){
    if(t == nullptr){
        cout<<"\nEmpty Set\n\n";
        return;
    }
    inputtype x;
    string temp;
    temp = tabname + '\n';
    cout<<"Table Name : "<<temp<<endl; // Table name is printed first.
    temp = "";
    int c = 0, flag=0;;
    for(int j=0; j<t->intable.size()+c; j++){
        flag = 0; // Now, all attribute names are gathered from the map and they are printed tab separated.
        for (auto &i : t->maptoplace) {
                if (i.second == j) {
                    flag = 1;
                    if(j!=t->intable.size()-1+c)
                        temp += i.first + '\t';
                    else{
                        temp += i.first;
                    }
                    break; // to stop searching
                }
        }
        if(flag==1)
            c++;
    }
    for(int i=0; i<t->intable[0].size(); i++){
        for(int j=0; j<t->intable.size(); j++){ // NULL values have already been dealt with, so they are ignored here.
            if(t->intable[j][i]->flag==1 && t->intable[j][i]->str.compare("NULL")!=0){//cout<<t->intable[j][i]->val<<" s\n";
                t->intable[j][i]->str = inttostring(t->intable[j][i]->val);}
            if(t->intable[j][i]->flag==2 && t->intable[j][i]->str.compare("NULL")!=0){//cout<<t->intable[j][i]->fval<<" d\n";
                t->intable[j][i]->str = floattostring(t->intable[j][i]->fval);}
        }
    }
    cout<<temp<<endl<<endl; // prints the attribute names.
    // At last the cell values are printed tab separated.
    for(int i=0; i<t->intable[0].size(); i++){
        for(int j=0; j<t->intable.size(); j++){
            if(j!=t->intable.size()-1){
                cout<<t->intable[j][i]->str<<'\t';
            }
            else{
                cout<<t->intable[j][i]->str<<endl;
            }
        }
    }
    cout<<endl;
}
// This function is used for setunion if f = 0, setdifference if f = 1 and intersection if f = 2.
table* setunion(table* t1, table* t2, int f){
    if(t1==nullptr || t2==nullptr){
        cout<<"Syntax Error or Tables missing!\n\n";
        return nullptr;
    }// Check if any table is empty, then what will be the answer.
    if(t1->intable.size()==0){
        if(f==0)
            return t2;
        else
            return nullptr;
    }
    if(t2->intable.size()==0){
        if(f==2)
            return nullptr;
        else
            return t1;
    }// Check if the tables are union compatible.
    if(!unioncompatible(t1, t2)){
        cout<<"Union incompatible!\n";
        return nullptr;
    }
    table* res = new table; // This table will hold the result.
    // Initially, the first table is completely copied to the result table.
    for(int i=0; i<t1->intable.size(); i++){
        vector<entrytype*> temp;
        for(int j=0; j<t1->intable[i].size(); j++)
            temp.push_back(t1->intable[i][j]);
        res->intable.push_back(temp);
    }
    res->maptoplace = t1->maptoplace;
    res->maptotype = t1->maptotype;
    map<string, bool> duplicatesmap;
    if(f == 0){ // perform set union.  store all the rows of first table in a map.
            for(int i=0; i<t1->intable[0].size(); i++){
        string temp="";
        for(int j=0; j<t1->intable.size(); j++){
            if(t1->intable[j][i]->flag==0)
                temp += t1->intable[j][i]->str;
            else if(t1->intable[j][i]->flag==1){
                temp += (char)(t1->intable[j][i]->val+48);}//cout<<"NP"<<endl;}
            else{
                temp += floattostring(t1->intable[j][i]->fval);
            }
        }
        duplicatesmap[temp]=true;
    }// check for each row of second table, if the row is present in t1 (or map), then leave it else add it to the result table.
    for(int i=0; i<t2->intable[0].size(); i++){
        string temp="";
        for(int j=0; j<t2->intable.size(); j++){
            if(t2->intable[j][i]->flag==0)
                temp += t2->intable[j][i]->str;
            else if(t2->intable[j][i]->flag==1){
                temp += (char)(t2->intable[j][i]->val+48);}
            else{
                temp += floattostring(t2->intable[j][i]->fval);
            }
        }

            if(duplicatesmap[temp]==true){
        }
        else{
            duplicatesmap[temp]=true;
            for(int j=0; j<t2->intable.size(); j++){
                res->intable[j].push_back(t2->intable[j][i]);
            }
        }

    }}
    else{ // performing setdiffernce or intersection. Add all rows of t2 in the map.
        for(int i=0; i<t2->intable[0].size(); i++){
        string temp="";
        for(int j=0; j<t2->intable.size(); j++){
            if(t2->intable[j][i]->flag==0)
                temp += t2->intable[j][i]->str;
            else if(t2->intable[j][i]->flag==1){
                temp += (char)(t2->intable[j][i]->val+48);}//cout<<"NP"<<endl;}
            else{
                temp += floattostring(t2->intable[j][i]->fval);
            }
        }
        duplicatesmap[temp]=true;
    }

        for(int i=0; i<res->intable[0].size(); i++){
        string temp="";
        for(int j=0; j<res->intable.size(); j++){
            if(res->intable[j][i]->flag==0)
                temp += res->intable[j][i]->str;
            else if(res->intable[j][i]->flag==1){
                temp += (char)(res->intable[j][i]->val+48);}
            else{
                temp += floattostring(res->intable[j][i]->fval);
            }
        }
        if(f==2){// check for each row of second table, if the row is present in t2 (or map), then keep the row in result table, else remove it.
            if(duplicatesmap[temp]==false){
                for(int j=0; j<res->intable.size(); j++){
                    res->intable[j].erase(res->intable[j].begin()+i);
                }
                i--;
                duplicatesmap[temp]=true;
        }
        }
        else{// check for each row of second table, if the row is present in t2 (or map), then remove the row from result table, else keep it.
            if(duplicatesmap[temp]==true){
                for(int j=0; j<res->intable.size(); j++){
                    res->intable[j].erase(res->intable[j].begin()+i);
                }
                i--;
        }
        else
            duplicatesmap[temp]=true;
        }

    }
    }
    return res;
}
// This function takes 2 tables and return their cartesian product.
table* product(table* t1, table* t2){
    if(t1==nullptr || t2==nullptr){
        cout<<"Syntax Error or Tables missing!\n\n";
        return nullptr;
    }
    table* res = new table;
    // The first row is copied to initialize the table.
    for(int i=0; i<t1->intable.size()+t2->intable.size(); i++){
        vector <entrytype*> temp;
        if(i<t1->intable.size())
            temp.push_back(t1->intable[i][0]);
        else
            temp.push_back(t2->intable[i-t1->intable.size()][0]);
        res->intable.push_back(temp);
    } // Get names of table in tname1 and tname2.
    string tname1="", tname2="";
    int cnt=0;
    for(auto &i : tablemap){
        if(cnt>=2)
        break;
        if(i.second==t1){
            tname1 = i.first;
            cnt++;
        }
        else if(i.second==t2){
            tname2 = i.first;
            cnt++;
        }
    }
    // if table names are not available we use 1 for first and 2 for second.
    if(tname1==tname2){
        tname1 = "1";
        tname2 = "2";
    }
    // make attributes from table 2 as tname2.att
    for(int j=0; j<t2->intable.size(); j++){
        for (auto &i : t2->maptoplace) {
                if (i.second == j) {
                    if(tname2.size()==0){
                        res->maptoplace[i.first]=j+t1->intable.size();
                        res->maptotype[i.first]=t2->maptotype[i.first];
                        break;
                    }
                    res->maptoplace[tname2+"-"+i.first]=j+t1->intable.size();
                    res->maptotype[tname2+"-"+i.first]=t2->maptotype[i.first];
                    break; // to stop searching
                }
        }
    }
    // make attributes from table 1 as tname1.att
    for(int j=0; j<t1->intable.size(); j++){
        for (auto &i : t1->maptoplace) {
                if (i.second == j) {
                    if(tname1.size()==0){
                        res->maptoplace[i.first]=j+t1->intable.size();
                        res->maptotype[i.first]=t2->maptotype[i.first];
                        break;
                    }
                    res->maptoplace[tname1+"-"+i.first]=j;
                    res->maptotype[tname1+"-"+i.first]=t1->maptotype[i.first];
                    break; // to stop searching
                }
        }
    }
    // Take a row from table 1 and add one by one rows from table 2 to it to make a row of result table.
    for(int i=0; i<t1->intable[0].size(); i++){
        for(int k=0; k<t2->intable[0].size(); k++){
            for(int j=0; j<t1->intable.size()+t2->intable.size(); j++){
                if(k+i==0)
                    break;
                if(j<t1->intable.size())
                    res->intable[j].push_back(t1->intable[j][i]);
                else
                    res->intable[j].push_back(t2->intable[j-t1->intable.size()][k]);
            }
        }
    }
    return res;
}
// This function returns a natural(rather theta) join of 2 tables.
table* naturaljoin(table* t1, table* t2, vector<string> atts){
    if(t1==nullptr || t2==nullptr){
        cout<<"Syntax Error or Tables missing!\n\n";
        return nullptr;
    }
    // atts are the attributes that will be used to join, as it is a theta-join.
    map <int, bool> attsmap;
    for(int i=0; i<atts.size(); i++){
        attsmap[t2->maptoplace[atts[i]]] = true;
    }
    // The process is same as cartesian product, but the common attributes from table 2 are removed.
    table* res = new table;
    for(int i=0; i<t1->intable.size()+t2->intable.size(); i++){
        vector <entrytype*> temp;
        if(i<t1->intable.size())
            temp.push_back(t1->intable[i][0]);
        else{
            if(attsmap[i-t1->intable.size()]==false)
                temp.push_back(t2->intable[i-t1->intable.size()][0]);
            else
                continue;
        }
        res->intable.push_back(temp);
    }
    string tname1="1", tname2="2";
    int cnt=0;
    for(auto &i : tablemap){
        if(cnt>=2)
        break;
        if(i.second==t1){
            tname1 = i.first;
            cnt++;
        }
        else if(i.second==t2){
            tname2 = i.first;
            cnt++;
        }
    }
    int k=0;
    for(int j=0; j<t2->intable.size(); j++){
        for (auto &i : t2->maptoplace) {
                if (i.second == j && attsmap[j]==false) {
                    //cout<<i.first<<endl;
                    res->maptoplace[tname2+"-"+i.first]=k+t1->intable.size();k++;
                    res->maptotype[tname2+"-"+i.first]=t2->maptotype[i.first];
                    break; // to stop searching
                }
        }
    }

    for(int j=0; j<t1->intable.size(); j++){
        for (auto &i : t1->maptoplace) {
                if (i.second == j) {
                    res->maptoplace[tname1+"-"+i.first]=j;
                    res->maptotype[tname1+"-"+i.first]=t1->maptotype[i.first];
                    break; // to stop searching
                }
        }
    }// The only difference here from cartesian product is that, before combining rows from 2 tables, it is checked if they agree on the value of atts.(This is done by attsmap)
    for(int i=0; i<t1->intable[0].size(); i++){
        for(int k=0; k<t2->intable[0].size(); k++){
            string temp1="", temp2="";
            for(int a=0; a<atts.size(); a++){
                if(t1->maptotype[atts[a]]==0){
                    temp1+=t1->intable[t1->maptoplace[atts[a]]][i]->str;
                    temp2+=t2->intable[t2->maptoplace[atts[a]]][k]->str;
                }
                else if(t1->maptotype[atts[a]]==1){
                    temp1+=t1->intable[t1->maptoplace[atts[a]]][i]->val;
                    temp2+=t2->intable[t2->maptoplace[atts[a]]][k]->val;
                }
                else{
                    temp1+=t1->intable[t1->maptoplace[atts[a]]][i]->fval;
                    temp2+=t2->intable[t2->maptoplace[atts[a]]][k]->fval;
                }
            }
            if(temp1.compare(temp2)!=0)
                {continue;}
            int flg=0;
            for(int j=0; j<t1->intable.size()+t2->intable.size(); j++){
                if(k+i==0)
                    break;
                if(attsmap[j-t1->intable.size()]==true){
                    flg++;continue;}
                if(j<t1->intable.size())
                    res->intable[j].push_back(t1->intable[j][i]);
                else{//cout<<"a"<<endl;
                    res->intable[j-flg].push_back(t2->intable[j-t1->intable.size()][k]);}//;cout<<"b"<<endl;};
            }
        }
    }// as the first row was added without checking the condition to initialize the table, it is now checked.
    for(int i=0; i<atts.size(); i++){
        if(t1->maptotype[atts[i]]==0){
        if(t1->intable[t1->maptoplace[atts[i]]][0]->str.compare(t2->intable[t2->maptoplace[atts[i]]][0]->str)!=0){
            for(int j=0; j<res->intable.size(); j++)
                res->intable[j].erase(res->intable[j].begin());
            break;
        }}
        else if(t1->maptotype[atts[i]]==1){
        if(t1->intable[t1->maptoplace[atts[i]]][0]->val != (t2->intable[t2->maptoplace[atts[i]]][0]->val)){
            for(int j=0; j<res->intable.size(); j++)
                res->intable[j].erase(res->intable[j].begin());
            break;
        }}
        else{
        if(abs(t1->intable[t1->maptoplace[atts[i]]][0]->fval-t2->intable[t2->maptoplace[atts[i]]][0]->fval)<0.000001){
            for(int j=0; j<res->intable.size(); j++)
                res->intable[j].erase(res->intable[j].begin());
            break;
        }}
    }
    return res;
}
// This is similar to the natural-join, but here the common attributes are removed from table1. This is used in right outer join.
table* naturaljoin2(table* t1, table* t2, vector<string> atts){
    if(t1==nullptr || t2==nullptr){
        cout<<"Syntax Error or Tables missing!\n\n";
        return nullptr;
    }
    map <int, bool> attsmap;
    for(int i=0; i<atts.size(); i++){
        attsmap[t1->maptoplace[atts[i]]] = true;
    }
    table* res = new table;
    for(int i=0; i<t1->intable.size()+t2->intable.size(); i++){
        vector <entrytype*> temp;
        if(i>=t1->intable.size())
            temp.push_back(t2->intable[i-t1->intable.size()][0]);
        else{
            if(attsmap[i]==false)
                temp.push_back(t1->intable[i][0]);
            else
                continue;
        }
        res->intable.push_back(temp);
    }
    string tname1="1", tname2="2";
    int cnt=0;
    for(auto &i : tablemap){
        if(cnt>=2)
        break;
        if(i.second==t1){
            tname1 = i.first;
            cnt++;
        }
        else if(i.second==t2){
            tname2 = i.first;
            cnt++;
        }
    }
    int k=0;
    for(int j=0; j<t1->intable.size(); j++){
        for (auto &i : t1->maptoplace) {
                if (i.second == j && attsmap[j]==false) {
                    //cout<<i.first<<endl;
                    res->maptoplace[tname1+"-"+i.first]=k++;
                    res->maptotype[tname1+"-"+i.first]=t1->maptotype[i.first];
                    break; // to stop searching
                }
        }
    }

    for(int j=0; j<t2->intable.size(); j++){
        for (auto &i : t2->maptoplace) {
                if (i.second == j) {
                    res->maptoplace[tname2+"-"+i.first]=k++;
                    res->maptotype[tname2+"-"+i.first]=t2->maptotype[i.first];
                    break; // to stop searching
                }
        }
    }
    for(int i=0; i<t1->intable[0].size(); i++){
        for(int k=0; k<t2->intable[0].size(); k++){
            string temp1="", temp2="";
            for(int a=0; a<atts.size(); a++){
                if(t1->maptotype[atts[a]]==0){
                    temp1+=t1->intable[t1->maptoplace[atts[a]]][i]->str;
                    temp2+=t2->intable[t2->maptoplace[atts[a]]][k]->str;
                }
                else if(t1->maptotype[atts[a]]==1){
                    temp1+=t1->intable[t1->maptoplace[atts[a]]][i]->val;
                    temp2+=t2->intable[t2->maptoplace[atts[a]]][k]->val;
                }
                else{
                    temp1+=t1->intable[t1->maptoplace[atts[a]]][i]->fval;
                    temp2+=t2->intable[t2->maptoplace[atts[a]]][k]->fval;
                }
            }
            if(temp1.compare(temp2)!=0)
                {continue;}
            int flg=0;
            for(int j=0; j<t1->intable.size()+t2->intable.size(); j++){
                if(k+i==0)
                    break;
                if(attsmap[j]==true && j<t1->intable.size()){
                    flg++;continue;}
                if(j>=t1->intable.size())
                    res->intable[j-flg].push_back(t2->intable[j-t1->intable.size()][k]);
                else{//cout<<"a"<<endl;
                    res->intable[j-flg].push_back(t1->intable[j][i]);}//;cout<<"b"<<endl;};
            }
        }
    }
    // As the first row was added to initialize the vectors without checking the join condition. We check it now.
    for(int i=0; i<atts.size(); i++){
        if(t1->maptotype[atts[i]]==0){
        if(t1->intable[t1->maptoplace[atts[i]]][0]->str.compare(t2->intable[t2->maptoplace[atts[i]]][0]->str)!=0){
            for(int j=0; j<res->intable.size(); j++)
                res->intable[j].erase(res->intable[j].begin());
            break;
        }}
        else if(t1->maptotype[atts[i]]==1){
        if(t1->intable[t1->maptoplace[atts[i]]][0]->val != (t2->intable[t2->maptoplace[atts[i]]][0]->val)){
            for(int j=0; j<res->intable.size(); j++)
                res->intable[j].erase(res->intable[j].begin());
            break;
        }}
        else{
        if(abs(t1->intable[t1->maptoplace[atts[i]]][0]->fval-t2->intable[t2->maptoplace[atts[i]]][0]->fval)<0.000001){
            for(int j=0; j<res->intable.size(); j++)
                res->intable[j].erase(res->intable[j].begin());
            break;
        }}
    }
    return res;
}
// This function given the left outer join of tables.
table* louterjoin(table* t1, table* t2, vector<string> atts){
    if(t1==nullptr || t2==nullptr){
        cout<<"Syntax Error or Tables missing!\n\n";
        return nullptr;
    }
    map <int, bool> attsmap;
    table* res = new table;
    entrytype* dummy = new entrytype; // This is the NULL entry.
    dummy->str="NULL";
    dummy->val=0;
    dummy->fval=0.0;
    table* ts = naturaljoin(t1, t2, atts); // The tables are first joined.
    for(int i=0; i<ts->intable[0].size(); i++){
        for(int j=0; j<ts->intable.size(); j++){
            if(ts->intable[j][i]->flag==1 && ts->intable[j][i]->str.compare("NULL")!=0){//cout<<t->intable[j][i]->val<<" s\n";
                ts->intable[j][i]->str = inttostring(ts->intable[j][i]->val);}
            if(ts->intable[j][i]->flag==2 && ts->intable[j][i]->str.compare("NULL")!=0){//cout<<t->intable[j][i]->fval<<" d\n";
                ts->intable[j][i]->str = floattostring(ts->intable[j][i]->fval);}
        }
    }
    map <string, bool> present;
    for(int i=0; i<ts->intable[0].size(); i++){
        string temp = "";
        for(int j=0; j<t1->intable.size(); j++){
            temp+=ts->intable[j][i]->str;
        }
        present[temp]=true;
    }
    //After that, the entries if table 1 that remain are added with NULL for the table 2 columns.
    for(int i=0; i<t1->intable[0].size(); i++){
        string temp = "";
        for(int j=0; j<t1->intable.size(); j++){
            temp+=t1->intable[j][i]->str;
        }
        if(present[temp]==true){
        }
        else{
            for(int j=0; j<ts->intable.size(); j++){
                if(j<t1->intable.size()){
                    ts->intable[j].push_back(t1->intable[j][i]);
                }
                else
                    ts->intable[j].push_back(dummy);
            }
        }
    }
    return ts;
}
// This function is similar to the leftouter join. It is the right outer join.
table* routerjoin(table* t1, table* t2, vector<string> atts){
    if(t1==nullptr || t2==nullptr){
        cout<<"Syntax Error or Tables missing!\n\n";
        return nullptr;
    }
    map <int, bool> attsmap;
    table* res = new table;
    entrytype* dummy = new entrytype;
    dummy->str="NULL";
    dummy->val=0;
    dummy->fval=0.0;
    table* ts = naturaljoin2(t1, t2, atts);
    //printtable(ts, "");
    for(int i=0; i<ts->intable[0].size(); i++){
        for(int j=0; j<ts->intable.size(); j++){
            if(ts->intable[j][i]->flag==1 && ts->intable[j][i]->str.compare("NULL")!=0){//cout<<t->intable[j][i]->val<<" s\n";
                ts->intable[j][i]->str = inttostring(ts->intable[j][i]->val);}
            if(ts->intable[j][i]->flag==2 && ts->intable[j][i]->str.compare("NULL")!=0){//cout<<t->intable[j][i]->fval<<" d\n";
                ts->intable[j][i]->str = floattostring(ts->intable[j][i]->fval);}
        }
    }
    map <string, bool> present;
    for(int i=0; i<ts->intable[0].size(); i++){
        string temp = "";
        for(int j=0; j<t2->intable.size(); j++){
            temp+=ts->intable[j+t1->intable.size()-atts.size()][i]->str;
        }
        present[temp]=true;
    }
    string tname1, tname2;
    int cnt=0;
    for(auto &i : tablemap){
        if(cnt>=2)
        break;
        if(i.second==t1){
            tname1 = i.first;
            cnt++;
        }
        else if(i.second==t2){
            tname2 = i.first;
            cnt++;
        }
    }

    for(int i=0; i<t2->intable[0].size(); i++){
        string temp = "";
        for(int j=0; j<t2->intable.size(); j++){
            temp+=t2->intable[j][i]->str;
        }
        if(present[temp]==true){
        }
        else{
            for(int j=0; j<ts->intable.size(); j++){
                if(j<t1->intable.size()-atts.size()){
                    ts->intable[j].push_back(dummy);
                }
                else
                    ts->intable[j].push_back(t2->intable[j-(t1->intable.size()-atts.size())][i]);
            }
        }
    }
    //printtable(ts, "");
    // This is the only difference from left outer join, that the attributes finally should appear in correct order, i.e. first table1 atts then table 2 atts.
    vector<string> v;
    map <string, bool> attsgiven;
    for(int i=0; i<atts.size(); i++){
        attsgiven[atts[i]] = true;
    }
    // The given attributes a rearranged in correct order, by adding them order wise in v vector and then projecting in accordance with v.
    for(int j=0; j<t1->intable.size(); j++){
                for (auto &i : t1->maptoplace) {
                    if (i.second == j) {
                        if(attsgiven[i.first]==true){
                            v.push_back(tname2+"-"+i.first);
                            attsgiven[tname2+"-"+i.first] = true;
                        }
                        else{
                            v.push_back(tname1+"-"+i.first);
                        }
                        break; // to stop searching
                    }
                }
            }
            for(int j=0; j<t2->intable.size(); j++){
                for (auto &i : t2->maptoplace) {
                    if (i.second == j) {
                        if(attsgiven[i.first]==true){
                        }
                        else{
                            v.push_back(tname2+"-"+i.first);
                        }
                        break; // to stop searching
                    }
                }
            }
            //cout<<"a\n";
    return project(ts, v); // the projected table has attributes in correct order.
}

string setpoint (string);
map<string, int> gmap; int rflag; // gmap is used so that, if an attribute has been renamed, we get the original name attributes back. so they are stored in gmap.  rflag = 1 means the attributes have to be put to their original names back.
// This is a simple function that gives maximum, minimum and average(rounded down) of an integer column of a table.
int mxmnavg(vector<entrytype*> v, int flg){ // flg = 0 is max, 1 is min and 2 is average.
    int ans;
    if(flg == 0){
        ans = -1000000;
    }
    else if(flg == 1)
        ans = 1000000;
    if(flg<=1){
        for(int i=0; i<v.size(); i++){
            if(flg==0){
                if(v[i]->val>ans)
                    ans = v[i]->val;
            }
            if(flg==1){
                if(v[i]->val<ans)
                    ans = v[i]->val;
            }
        }
    }
    else{
        ans = 0;
        for(int i=0; i<v.size(); i++){
            ans+=v[i]->val;
        }
        ans = ans/v.size();
    }
    return ans;
}
// This is the master function that handles the main control of the program. It decides how different operators are to be applied and it is recursive. It handles nested queries easily.
table* parsenested(vector<string> tokens){
// this function checks for presence of an opeartion in the query and recursively calls the query and returns a pointer to table.
        if(tokens[0].compare("count")==0){
            vector<string>::const_iterator first = tokens.begin() + 1;
            vector<string>::const_iterator last = tokens.end();
            vector<string> tok(first, last);
            table* t1 = parsenested(tok);
            rflag = 2;
            cout<<t1->intable[0].size()<<endl;
            return nullptr; // As aggregate functions do not return a table here.
        }
        else if(tokens[0].compare("max")==0 || tokens[0].compare("min")==0 || tokens[0].compare("avg")==0){
            vector<string>::const_iterator first = tokens.begin() + 1;
            vector<string>::const_iterator last = tokens.end();
            vector<string> tok(first, last);
            table* t1 = parsenested(tok);
            rflag = 2;
            if(t1->intable[0][0]->flag == 1){// corresponding functions are called for each of max, min and avg.
                if(tokens[0].compare("max")==0)
                    cout<<mxmnavg(t1->intable[0], 0)<<endl;
                else if(tokens[0].compare("min")==0)
                    cout<<mxmnavg(t1->intable[0], 1)<<endl;
                else
                    cout<<mxmnavg(t1->intable[0], 2)<<endl;
            }
            else{
                cout<<"The first attribute of table is not INT type!\n\n";
            }
            return nullptr; // aggregate functions here do not return a table.
        }
        else if(tokens[0].compare("theta-join")==0 || tokens[0].compare("natural-join")==0 || tokens[0].compare("ljoin")==0 || tokens[0].compare("rjoin")==0){
            rflag = 0;
            table* t1;
            table* t2;
            int k=0, j=0, e=0;
            vector<string> atts;// These loops capture the attributes to the operations.
            while(k<tokens.size()-1 && !(tokens[k+1].compare("ljoin")==0 || tokens[k+1].compare("rjoin")==0 || tokens[k+1].compare("rename")==0 || tokens[k+1].compare("theta-join")==0 || tokens[k+1].compare("select-or")==0 || tokens[k+1].compare("product")==0 || tokens[k+1].compare("setdiff")==0 || tokens[k+1].compare("intersect")==0 || tokens[k+1].compare("union")==0 || tokens[k+1].compare("project")==0 || tokens[k+1].compare("select")==0 || tokens[k+1].compare("table")==0 || tokens[k+1].compare("project-s")==0)){
                atts.push_back(tokens[k+1]);
                k++;
            }
            j=k;
            k++;// The 2 tables for these binary operations are separated by |
            while(k<tokens.size()-1 && (!tokens[k].compare("|")==0 || e>0)){
                if(tokens[k].compare("ljoin")==0 || tokens[k].compare("rjoin")==0 || tokens[k].compare("theta-join")==0 || tokens[k].compare("product")==0 || tokens[k].compare("setdiff")==0 || tokens[k].compare("intersect")==0 || tokens[k].compare("union")==0)
                    {e++;}
                if(tokens[k].compare("|")==0){
                    e--;}
                k++;
            }
            while(k<tokens.size()-1 && !(tokens[k+1].compare("ljoin")==0 || tokens[k+1].compare("rjoin")==0 || tokens[k+1].compare("rename")==0 || tokens[k+1].compare("theta-join")==0 || tokens[k+1].compare("select-or")==0 || tokens[k+1].compare("product")==0 || tokens[k+1].compare("setdiff")==0 || tokens[k+1].compare("intersect")==0 || tokens[k+1].compare("union")==0 || tokens[k+1].compare("project")==0 || tokens[k+1].compare("select")==0 || tokens[k+1].compare("table")==0 || tokens[k+1].compare("project-s")==0)){
                k++;
            }
            vector<string>::const_iterator first = tokens.begin() + j + 1;
            vector<string>::const_iterator last = tokens.begin()+k;
            vector<string>::const_iterator first2 = tokens.begin() + k + 1;
            vector<string>::const_iterator last2 = tokens.end();
            vector<string> tok(first, last2);
            vector<string> tok2(first2, last2);
            if(true)
                t1 = parsenested(tok);
            t2 = parsenested(tok2);
            if(tokens[0].compare("natural-join")==0 || tokens[0].compare("ljoin")==0 || tokens[0].compare("rjoin")==0){ // if the operation is natural or inner join, then we first find all the common attributes and call theta-join with them.
                map<string, bool> attsoft1;
                for(int j=0; j<t1->intable.size(); j++){
                    for (auto &i : t1->maptoplace) {
                        if (i.second == j) {
                            attsoft1[i.first] = true;
                            break; // to stop searching
                        }
                    }
                }
                for(int j=0; j<t2->intable.size(); j++){
                    for (auto &i : t2->maptoplace) {
                        if (i.second == j) {
                            if(attsoft1[i.first]==true)
                                atts.push_back(i.first);
                            break; // to stop searching
                        }
                    }
                }
            }
            rflag = 0; // it is made 0 every time in an operation because we keep it = 1 only if we want to return the prev att names for table. This is on;u required if rename is the outer most operation.
            table* table1;
            if(t1==nullptr || t2==nullptr){
                return nullptr;
            }
            if(tokens[0].compare("ljoin")==0){
                table1 = louterjoin(t1, t2, atts);
            }
            else if(tokens[0].compare("rjoin")==0){
                table1 = routerjoin(t1, t2, atts);
            }
            else{
                table1 = naturaljoin(t1, t2, atts);
            }
            return table1;

        }

        else if(tokens[0].compare("union")==0 || tokens[0].compare("intersect")==0 || tokens[0].compare("setdiff")==0 || tokens[0].compare("product")==0){
            rflag = 0;
            int flagger = 0;
            if(tokens[0].compare("intersect")==0)
                flagger = 1;
            else if(tokens[0].compare("setdiff")==0)
                flagger = 2;
            else if(tokens[0].compare("product")==0)
                flagger = 3;
            table* t1;
            table* t2;
            int k=0, j=0, e=0;
            vector<string> tables;
            while(k<tokens.size()-1 && !(tokens[k+1].compare("ljoin")==0 || tokens[k+1].compare("rjoin")==0 || tokens[k+1].compare("rename")==0 || tokens[k+1].compare("theta-join")==0 || tokens[k+1].compare("select-or")==0 || tokens[k+1].compare("product")==0 || tokens[k+1].compare("setdiff")==0 || tokens[k+1].compare("intersect")==0 || tokens[k+1].compare("union")==0 || tokens[k+1].compare("project")==0 || tokens[k+1].compare("select")==0 || tokens[k+1].compare("table")==0 || tokens[k+1].compare("project-s")==0)){
                tables.push_back(tokens[k+1]);
                k++;
            }
            j=k;
            k++;
            while(k<tokens.size()-1 && (!tokens[k].compare("|")==0 || e>0)){
                if(tokens[k].compare("ljoin")==0 || tokens[k].compare("rjoin")==0 || tokens[k].compare("theta-join")==0 || tokens[k].compare("product")==0 || tokens[k].compare("setdiff")==0 || tokens[k].compare("intersect")==0 || tokens[k].compare("union")==0)
                    {e++;}
                if(tokens[k].compare("|")==0){
                    e--;}
                k++;
            }
            while(k<tokens.size()-1 && !(tokens[k+1].compare("ljoin")==0 || tokens[k+1].compare("rjoin")==0 || tokens[k+1].compare("rename")==0 || tokens[k+1].compare("theta-join")==0 || tokens[k+1].compare("select-or")==0 || tokens[k+1].compare("product")==0 || tokens[k+1].compare("setdiff")==0 || tokens[k+1].compare("intersect")==0 || tokens[k+1].compare("union")==0 || tokens[k+1].compare("project")==0 || tokens[k+1].compare("select")==0 || tokens[k+1].compare("table")==0 || tokens[k+1].compare("project-s")==0)){
                tables.push_back(tokens[k+1]);
                k++;
            }
            vector<string>::const_iterator first = tokens.begin() + j + 1;
            vector<string>::const_iterator last = tokens.begin()+k;
            vector<string>::const_iterator first2 = tokens.begin() + k + 1;
            vector<string>::const_iterator last2 = tokens.end();
            vector<string> tok(first, last2);
            vector<string> tok2(first2, last2);
            if(true)
                t1 = parsenested(tok);
            t2 = parsenested(tok2);
            if(t1==nullptr || t2==nullptr){
                return nullptr;
            }
            rflag = 0;
            table* table1;
            if(flagger == 0) // this is for union
                table1 = setunion(t1, t2, 0);
            else if(flagger == 1) // this is for intersection
                table1 = setunion(t1, t2, 2);
            else if(flagger == 2) // this is for setdiff.
                table1 = setunion(t1, t2, 1);
            else // this is for cartesian product
                table1 = product(t1, t2);
            return table1;

        }
        else if(tokens[0].compare("select")==0 || tokens[0].compare("select-or")==0){
            // select means AND select and select-or means ORed select.
            rflag = 0;
            int andor = 0; // this variable specifies if we want to do AND(=0) select or OR(=1) select.
            if(tokens[0].compare("select-or")==0){
                andor = 1;
            }
            table* t1;
            int k=0;
            vector<string> atts, value;
            string op="";
            while(k<tokens.size()-1 && !(tokens[k+1].compare("ljoin")==0 || tokens[k+1].compare("rjoin")==0 || tokens[k+1].compare("rename")==0 || tokens[k+1].compare("theta-join")==0 || tokens[k+1].compare("select-or")==0 || tokens[k+1].compare("product")==0 || tokens[k+1].compare("setdiff")==0 || tokens[k+1].compare("intersect")==0 || tokens[k+1].compare("union")==0 || tokens[k+1].compare("project")==0 || tokens[k+1].compare("select")==0 || tokens[k+1].compare("table")==0 || tokens[k+1].compare("project-s")==0)){
                atts.push_back(tokens[k+1]);
                if(k+2<tokens.size()){
                    if(!(tokens[k+2].compare("ljoin")==0 || tokens[k+2].compare("rjoin")==0 || tokens[k+2].compare("select")==0 || tokens[k+2].compare("theta-join")==0 || tokens[k+2].compare("select-or")==0 || tokens[k+2].compare("product")==0 || tokens[k+2].compare("setdiff")==0 || tokens[k+2].compare("intersect")==0 || tokens[k+2].compare("union")==0 || tokens[k+2].compare("project")==0 || tokens[k+2].compare("select")==0 || tokens[k+2].compare("table")==0 || tokens[k+2].compare("project-s")==0))
                        op += tokens[k+2];
                    else
                        break;
                }
                else
                    break;
                if(k+3<tokens.size()){
                    if(!(tokens[k+3].compare("ljoin")==0 || tokens[k+3].compare("rjoin")==0 || tokens[k+3].compare("select")==0 || tokens[k+3].compare("theta-join")==0 || tokens[k+3].compare("select-or")==0 || tokens[k+3].compare("product")==0 || tokens[k+3].compare("setdiff")==0 || tokens[k+3].compare("intersect")==0 || tokens[k+3].compare("union")==0 || tokens[k+3].compare("project")==0 || tokens[k+3].compare("select")==0 || tokens[k+3].compare("table")==0 || tokens[k+3].compare("project-s")==0))
                        value.push_back(tokens[k+3]);
                    else
                        break;
                }
                else
                    break;
                k += 3;
            }
            if(atts.size()!=op.size() || atts.size()!= value.size()){
                cout<<"\nSyntax Error\n\n";
                return nullptr;
            }

            vector<string>::const_iterator first = tokens.begin() + k + 1;
            vector<string>::const_iterator last = tokens.end();
            vector<string> tok(first, last);
            if(true)
                t1 = parsenested(tok);
            if(t1==nullptr){
                return nullptr;
            }
            vector<string> val;
            for(int i=0; i<atts.size(); i++){
                if(t1->maptotype[atts[i]]==2){
                    if(!containshyphen(value[i])){//cout<<"b";
                        val.push_back(setpoint(value[i]));}//cout<<"c";}
                    else{//cout<<"d"<<endl;
                        val.push_back(value[i]);}
                }
                else
                    val.push_back(value[i]);
            }
            rflag = 0;
            table* seltable = select(t1, nullptr, atts, op, val, andor);
            return seltable;
        }
        else if(tokens[0].compare("project")==0){
            rflag = 0;
            bool flag=false;
            table* t1;
            int k=0;
            vector <string> atts;
            while(k<tokens.size()-1 && !(tokens[k+1].compare("ljoin")==0 || tokens[k+1].compare("rjoin")==0 || tokens[k+1].compare("rename")==0 || tokens[k+1].compare("theta-join")==0 || tokens[k+1].compare("select-or")==0 || tokens[k+1].compare("product")==0 || tokens[k+1].compare("setdiff")==0 || tokens[k+1].compare("intersect")==0 || tokens[k+1].compare("union")==0 || tokens[k+1].compare("project")==0 || tokens[k+1].compare("select")==0 || tokens[k+1].compare("table")==0 || tokens[k+1].compare("project-s")==0)){
                atts.push_back(tokens[k+1]);
                k++;
            }
            if(k==0){ // means no attributes given to project.
                cout<<"\nProblem in syntax\n\n";
                return nullptr;
            }
            vector<string>::const_iterator first = tokens.begin() + k + 1;
            vector<string>::const_iterator last = tokens.end();
            vector<string> tok(first, last);
            if(true)
                t1 = parsenested(tok);
            if(t1==nullptr){
                return nullptr;
            }
            rflag = 0;
            table* projtable = project(t1, atts);
            return projtable;
        }
        else if(tokens[0].compare("rename")==0){
            table* t;
            string name = tokens[1];
            //cout<<name<<endl;
            int k=0;
            vector <string> atts;
            k++;
            while(k<tokens.size()-1 && !(tokens[k+1].compare("ljoin")==0 || tokens[k+1].compare("rjoin")==0 || tokens[k+1].compare("rename")==0 || tokens[k+1].compare("theta-join")==0 || tokens[k+1].compare("select-or")==0 || tokens[k+1].compare("product")==0 || tokens[k+1].compare("setdiff")==0 || tokens[k+1].compare("intersect")==0 || tokens[k+1].compare("union")==0 || tokens[k+1].compare("project")==0 || tokens[k+1].compare("select")==0 || tokens[k+1].compare("table")==0 || tokens[k+1].compare("project-s")==0)){
                atts.push_back(tokens[k+1]);
                k++;
            }
            vector<string>::const_iterator first = tokens.begin() + k + 1;
            vector<string>::const_iterator last = tokens.end();
            vector<string> tok(first, last);
            t = parsenested(tok);
            //printtable(t, "t");
            map <string, int> mp;
            if(atts.size()==0){ // this means just renaming the table.
                if(!rename(t, name)){
                    cout<<"Error\n\n";
                    return nullptr;
                }
            }
            else{ // this means renaming the table as well as attributes.
                mp = renameatts(t, name, atts);
                if(mp["error"]==-1){
                    cout<<"Less number of attributes given!\n\n";
                    t->maptoplace = mp;
                    return nullptr;
                }
                //t->maptoplace = mp;
            }
            rflag = 1;
            if(atts.size()==0){
                rflag = 2; // this specifies that the attributes need not be replcedback to their original names.
                return t;
            }
            printtable(t, name);
            gmap = mp;
            return t;
        }
        else if(tokens[0].compare("table")==0){
            if(tablemap.find(tokens[1]) != tablemap.end()){
                ;
            }
            else{
                cout<<"\nTable does not exist\n\n";
                return nullptr;
            }
                return tablemap[tokens[1]];
        }
        else{
            cout<<"\nEnter a valid query!\n\n";
        }

        return nullptr;
}
// this function is used to set a .00 point to strings, so that they are easily converted to strings.
string setpoint(string input){
    bool flag = false;
    string str;
    for(int i=0; i<input.size(); i++){
        if(input[i]=='.'){
            flag = true;
            break;
        }
    }
    if(!flag){
        str = input + ".0";
    }
    else
        str = input;
    return str;
}


int main()
{   // Structure of queries : everything can be space separated or values will come in () or any comma separation will come in () and nested queries will come in []
    string inquery;
    cout<<"DBMS-Engine>>";
    getline(cin, inquery);
    while(1){
        rflag = 0;
        if(inquery.size()==0){
            cout<<"DBMS-Engine>>";
            getline(cin, inquery);
            continue;
        } // if we have a empty query, loop again
        if(inquery.compare("\n")==0 || inquery.compare(" ")==0 || inquery.compare("\t")==0){
            cout<<"DBMS-Engine>>";
            getline(cin, inquery);
            continue;
        }// if exit or close given, then break the loop
        if(inquery.compare("exit")==0 || inquery.compare("close")==0)
            break;
        vector<string> tokens; // tokenize the query.
        vector<string> chunks= splitnested(inquery); // tokenize in between brackets
        for(int i=0; i < chunks.size(); i++){
            vector<string> chunks1= split(chunks[i]); // tokenize completely.
            for(int j=0; j<chunks1.size(); j++){
                /*cout<<((*/chunks1[j]=getinputtype(chunks1[j])->str;//)+" "<<(getinputtype(chunks1[j])->name[((getinputtype(chunks1[j]))->flag)])<<endl;
                tokens.push_back(chunks1[j]);
            }
        }// these vectors are constructed and passed to functions.
        vector<int> types;
        vector <string> atts;
        vector<string> param;

        if(tokens[0].compare("ra")==0){ // for nested and complex queries.
            vector<string>::const_iterator first = tokens.begin() + 1;
            vector<string>::const_iterator last = tokens.end();
            vector<string> tok(first, last);
            table* ans = parsenested(tok);
            if(rflag==1){
                ans->maptoplace = gmap;
            }
            else if(rflag == 0){
                printtable(ans, "Answer table"); // the final table.
            }
        }
        else if(tokens[0].compare("create")==0){ // this is used to create a table using create command.
            int k=0;
            while(k<tokens.size()-2){ // find and store the data types for each column
                atts.push_back(tokens[k+2]);
                if(tokens[k+3].compare("string")==0){
                    types.push_back(0);
                }
                else if(tokens[k+3].compare("int")==0){
                    types.push_back(1);
                }
                else if(tokens[k+3].compare("float")==0){
                    types.push_back(2);
                }
                else{ // data type does not match string, int or float.
                    cout<<"\nDefinitions not defined correctly\n\n";
                    cout<<"DBMS-Engine>>";
                    getline(cin, inquery);
                    continue;
                }
                k+=2;
            }
            table* table1 = createtable(types, atts); // table is created.
            tablemap[tokens[1]] = table1; // table pointer added to the table map for future retreival.
            tabnames.push_back(tokens[1]);
            k=0;
            while(k<tokens.size()-2){
                atts.pop_back();
                types.pop_back();
                k+=2;
            }
        }

        // INSERT INTO TABLE using insert command
        else if(tokens[0].compare("insert")==0){
            if(tablemap.find(tokens[1]) != tablemap.end()){
                ;
            }
            else{
                cout<<"\nTable does not exist\n\n";
                cout<<"DBMS-Engine>>";
                getline(cin, inquery);
                continue;
            }// get pointer to the table from tablemap using its name.
            table* table1 = tablemap[tokens[1]];
            int k=0;
            while(k<tokens.size()-2){
                param.push_back(tokens[k+2]);
                k++;
            }
            // input values not given to some attributes.
            if(param.size()!=table1->numberofatts){
                cout<<"\nInput values to all attributes\n\n";
                cout<<"DBMS-Engine>>";
                getline(cin, inquery);
                continue;
            }
            vector <entrytype*> params = genparams(param); // generate entrytype variable for each cell.
            insertintable(table1, params); // add them to the table.
            k=0;
            while(k<tokens.size()-2){
                param.pop_back();
                k++;
            }
        }
        else if(tokens[0].compare("load")==0){ // to load the table directly from a file.
            ifstream input; // file name retreived.
            if(tokens.size()==1){
                cout<<"Enter a file to load!\n\n";
                cout<<"DBMS-Engine>>";
                getline(cin, inquery);
                continue;
            } // try to load the file.
            input.open(tokens[1].c_str());
            string temp;
            getline(input, temp);
            if(temp.size()==0){
                cout<<"File not present! Trying .txt extension!\n\n";
                input.close();
                input.open((tokens[1]+".txt").c_str());
                getline(input, temp);
                if(temp.size()!=0)
                    cout<<"Successfully found the file!\n\n";
                else{
                    cout<<"\nFile not present\n\n";
                    cout<<"DBMS-Engine>>";
                    getline(cin, inquery);
                    continue;
                }
            }
            // tokenize the input from file as it is tabs seperated.
            // the rest is same as create and insert in table.
            vector<string> tok = splittabs(temp);
            int k=0, flg=0;
            while(k<tok.size()-1){
                atts.push_back(tok[k+1]);
                if(tok[k+2].compare("string")==0){
                    types.push_back(0);
                }
                else if(tok[k+2].compare("int")==0){
                    types.push_back(1);
                }
                else if(tok[k+2].compare("float")==0){
                    types.push_back(2);
                }
                else{
                    cout<<"\nDefinitions not defined correctly\n\n";
                    cout<<"DBMS-Engine>>";
                    getline(cin, inquery);
                    flg=1;
                    break;
                }
                k+=2;
            }
	    if(flg==1)
		continue;
            table* table1 = createtable(types, atts);
            tablemap[tok[0]] = table1;
            k=0;
            while(k<tok.size()-1){
                atts.pop_back();
                types.pop_back();
                k+=2;
            }
            while(!input.eof()){
                getline(input, temp);
                tok = splittabs(temp);
                int k=0;
		if(tok.size()==0)
		    break;
                while(k<tok.size()){
                    param.push_back(tok[k]);
                    k++;
                }
                if(param.size()!=table1->numberofatts){
                    cout<<"\nInput values to all attributes\n\n";
                    cout<<"DBMS-Engine>>";
                    getline(cin, inquery);
                    continue;
                }
                vector <entrytype*> params = genparams(param);
                insertintable(table1, params);
                k=0;
                while(k<tok.size()){
                    param.pop_back();
                    k++;
                }
            }
            input.close();
        }
        // this is used to dump a table that is in memory to an output file with name tablename.txt.
        else if(tokens[0].compare("dump")==0){
            string filename = tokens[1]+".txt";
            ofstream output(filename.c_str());
            table* t = tablemap[tokens[1]];
            string temp="";
            inputtype* x = new inputtype;
            temp += tokens[1] + '\t';
            for(int j=0; j<t->intable.size(); j++){
                for (auto &i : t->maptoplace) {
                    if (i.second == j) {
                        if(j!=t->intable.size()-1)
                            temp += i.first + '\t' + x->name[t->maptotype[i.first]] + '\t';
                        else{
                            temp += i.first + '\t' + x->name[t->maptotype[i.first]];
                        }
                        break; // to stop searching
                    }
                }
            }
            temp += '\n';
            output.write(temp.c_str(), temp.size());
            temp="";
            for(int i=0; i<t->intable[0].size(); i++){
                for(int j=0; j<t->intable.size(); j++){
                    if(j==t->intable.size()-1)
                        temp += t->intable[j][i]->str;
                    else
                        temp += t->intable[j][i]->str+'\t';
                }
                if(i!=t->intable[0].size()-1)
                    temp += '\n';
                output.write(temp.c_str(), temp.size());
                temp="";
            }
            output.close();
        }
        // this is used to print a table, it uses the printtable function.
        else if(tokens[0].compare("show")==0){
            if(tablemap.find(tokens[1]) != tablemap.end()){
                ;
            }
            else{
                cout<<"\nTable does not exist\n\n";
                cout<<"DBMS-Engine>>";
                getline(cin, inquery);
                continue;
            }
            table* t = tablemap[tokens[1]];
            printtable(t, tokens[1]);
        }
        else{
            cout<<"\nEnter a valid query\n\n";
        }cout<<"DBMS-Engine>>";
        getline(cin, inquery);
    }
    // Dump all the tables created by create table command to the file system
    // this is just iteratively using the dump command for each table in memory with a name indexed in table map.
    for(int i=0; i<tabnames.size(); i++){
            if(tablemap[tabnames[i]]->intable.size()==0)
                continue;
            string filename = tabnames[i]+".txt";
            ofstream output(filename.c_str());
            table* t = tablemap[tabnames[i]];
            string temp="";
            inputtype* x = new inputtype;
            temp += tabnames[i] + '\t';
            for(int j=0; j<t->intable.size(); j++){
                for (auto &i : t->maptoplace) {
                    if (i.second == j) {
                        if(j!=t->intable.size()-1)
                            temp += i.first + '\t' + x->name[t->maptotype[i.first]] + '\t';
                        else{
                            temp += i.first + '\t' + x->name[t->maptotype[i.first]];
                        }
                        break; // to stop searching
                    }
                }
            }
            temp += '\n';
            output.write(temp.c_str(), temp.size());
            temp="";
            for(int i=0; i<t->intable[0].size(); i++){
                for(int j=0; j<t->intable.size(); j++){
                    if(j==t->intable.size()-1)
                        temp += t->intable[j][i]->str;
                    else
                        temp += t->intable[j][i]->str+'\t';
                }
                if(i!=t->intable[0].size()-1)
                    temp += '\n';
                output.write(temp.c_str(), temp.size());
                temp="";
            }
            output.close();
    }
    return 0;
}
