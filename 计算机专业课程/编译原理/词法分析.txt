#include<bits/stdc++.h>
using namespace std;
#define MAX 1005
#define DFANUM 8

//正规式的操作符
set<char> operators;
//操作数
set<char> operands;

//NFA的开始状态和结束状态
typedef struct State{
    int begin;
    int end;
}State;

//NFA节点
typedef struct NFA{
    int from;//状态1 
    int to;//状态2
    char c;//字符c
}NFA;

//存储一个NFA
NFA nfa[MAX];
int point = 0;

//DFA存储结构
typedef struct DFA{
	int id;
	map<set<int>, int> dfa_id;
	bool vn[MAX];
    int tran[MAX][MAX];
}DFA;

//存储多个DFA
DFA* dfas[DFANUM];
int point_dfa = 0;

//正规式对应的类型名称
string names[DFANUM];

//运算符优先级
int priority(char ch);

//预处理，插入连接符
void insert_behind_n(string& s, int n, char ch);

void pre_process(string& s);

//中缀转后缀
string infix_Suffix(string s);

//正规式转NFA
void CFG_NFA(string cfg);

//closure()
set<int> empty_transfer(int id);

//closure(move())
set<int> empty_move(int id,char c);

//NFA转DFA
void NFA_DFA();

//DFA最小化
void minimize_DFA();

//初始化操作符，NFA，DFA
void init();
void init_DFA(string cfg);

//通过DFA判断单词是否是正规式的句子
int distinguish(string s);
//source 单词;floor 行号
void myLex(string soucre,int floor);

void input();
int main(){
    init();
    //实现简易的Lex，即 .l 文件中的 
    //正则表达式 {fun()}
    input();
    return 0;
}

void init(){
    //初始化正规式运算符
    operators.insert('*');
    operators.insert('&');
    operators.insert('|');
    operators.insert('(');
    operators.insert(')');

    //正规式对应的类型名称
	names[0] = "保留字";
	names[1] = "界符";
    names[2] = "标识符";
    names[3] = "无符号整数";
    names[4] = "运算符";
    //正规式
	init_DFA("const|var|procedure|begin|end|odd|if|then|call|while|do|read|write");
    init_DFA("\\(|\\)|,|;|.");
    init_DFA("(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|0|1|2|3|4|5|6|7|8|9)*");
    init_DFA("0|(1|2|3|4|5|6|7|8|9)(0|1|2|3|4|5|6|7|8|9)*");
    init_DFA("+|-|\\*|/|<|<=|>|>=|#|=|:=");
}

void input(){//扫描字符串
    char ch;
    string word;
    int floor = 1;//行号
    ch = getchar();
    do{
        //跳过空格
        while(ch == ' '){
            ch = getchar();
        }
        //跳过注释
        if (ch == '/') {
            ch = getchar();
            if(ch=='/'){
                while(ch !='\n'){
                	ch = getchar();
                }
            }else if(ch == '*'){
                while(1){
                	ch = getchar();
                	if(ch == EOF){
                            cout<<"error";
                			break;
                		}
                	if(ch == '*'){
                		ch = getchar();
                		if(ch == '/'){
                			ch = getchar();
                			break;
                		}
                	}
                    if(ch == '\n'){
                        floor++;
                    }
                }
            }
        }

        //识别符号
        while(ch != ' ' ){
            if(ch == '\n'){
                floor++;
                if(word!=""){
                    myLex(word,floor);
                }
                word = "";
                ch = getchar();
                break;
            }
            string b(1,ch);
            int ret = distinguish(b);
            if(ret == 1){
            	
                myLex(word,floor);
                myLex(b,floor);
                word = "";
                ch = getchar();
                break;
            }
            word += ch;
            ch = getchar();
        }
        if(word != "")
            myLex(word,floor);
        word = "";
    }while(ch != EOF);
    
}

int priority(char ch)
{

	if (ch == '*')
	{
		return 3;
	}

	if (ch == '&')
	{
		return 2;
	}

	if (ch == '|')
	{
		return 1;
	}

	if (ch == '(')
	{
		return 0;
	}
}

void insert_behind_n(string& s, int n, char ch)
{

	s += '#';

	for (int i = s.size() - 1; i > n; i--)
		s[i] = s[i - 1];

	s[n] = ch;
}

void pre_process(string& s)
{

	int i = 0, length = s.size()-1;
	while (i < length)
	{
		int in_all_1 = 0, in_all_2 = 0;
        //注意转义
        if(operators.count(s[i])==0 && s[i]!='\\'){
            in_all_1 = 1;
        }
        if(operators.count(s[i+1])==0 ){
            in_all_2 = 1;
        }

		if (in_all_1 || (s[i] == '*') || (s[i] == ')')){
            if (in_all_2 || s[i + 1] == '(')
			{
				insert_behind_n(s, i + 1, '&');
				length++;
			}
        }
		i++;
	}
}

string infix_Suffix(string s)
{

	pre_process(s);	
	string str;				
	stack<char> oper;		

	for (unsigned int i = 0; i < s.size(); i++)
	{
		int in_all = 0;
        if(s[i] == '\\'){
            str += s[i];
            i++;
            str += s[i];
            operands.insert(s[i]); 
        }
		if(operators.count(s[i])==0){
            in_all = 1;
            operands.insert(s[i]);
        }

		if (in_all)	
		{
            str += s[i];
		}
		else							
		{

			if (s[i] == '(')			
			{
				oper.push(s[i]);
			}

			else if (s[i] == ')')	
			{

				char ch = oper.top();
				while (ch != '(')		
				{
					str += ch;
					oper.pop();
					ch = oper.top();
				}

				oper.pop();				
			}
			else				
			{

				if (!oper.empty())		
				{

					char ch = oper.top();
					while (priority(ch) >= priority(s[i]))
					{

						str += ch;
						oper.pop();

						if (oper.empty())	
						{
							break;
						}
						else ch = oper.top();
					}

					oper.push(s[i]);		
				}

				else				
				{
					oper.push(s[i]);
				}
			}
		}
	}

	while (!oper.empty())
	{

		char ch = oper.top();
		oper.pop();

		str += ch;
	}
    //cout<<str<<endl;
	return str;
}
//DFA的状态栈，记录起始状态和终结状态
State stateStack[MAX];
int top;
//获取ID
int stateID = 1;
void CFG_NFA(string cfg){
    
    stateID = 1;
    top = 0;
    point = 0;
    for (int i=0;i<cfg.length();i++){
        State a;
        State b;
        State c;
        switch (cfg[i])
        {
        case '&': 
            //从状态栈中取出两个状态，并更新起始和终结状态
            a = stateStack[top--];
            b = stateStack[top--];
            c.begin = b.begin;
            c.end = a.end;
            stateStack[++top] = c;
            //新建一个状态转移，a状态遇到#转移到b，将a和b状态连接起来
            NFA nfa0;
            nfa0.from = b.end;
            nfa0.to = a.begin;
            nfa0.c = '#';
            nfa[point++] = nfa0;
            break;
        case '|':
            //规则2
            c.begin = stateID++;
            c.end = stateID++;
            a = stateStack[top--];
            b = stateStack[top--];
            stateStack[++top] = c;
            
            NFA nfa1;
            nfa1.from = c.begin;
            nfa1.to = a.begin;
            nfa1.c = '#';
            nfa[point++] = nfa1;
            
            NFA nfa2;
            nfa2.from = c.begin;
            nfa2.to = b.begin;
            nfa2.c = '#';
            nfa[point++] = nfa2;

            NFA nfa3;
            nfa3.from = a.end;
            nfa3.to = c.end;
            nfa3.c = '#';
            nfa[point++] = nfa3;

            NFA nfa4;
            nfa4.from = b.end;
            nfa4.to = c.end;
            nfa4.c = '#';
            nfa[point++] = nfa4;
            break;
        case '*':
            //规则3
            c.begin = stateID++;
            c.end = stateID++;
            a = stateStack[top--];
            stateStack[++top] = c;

            NFA nfa5;
            nfa5.from = c.begin;
            nfa5.to = c.end;
            nfa5.c = '#';
            nfa[point++] = nfa5;
            
            NFA nfa6;
            nfa6.from = c.begin;
            nfa6.to = a.begin;
            nfa6.c = '#';
            nfa[point++] = nfa6;

            NFA nfa7;
            nfa7.from = a.end;
            nfa7.to = a.begin;
            nfa7.c = '#';
            nfa[point++] = nfa7;

            NFA nfa8;
            nfa8.from = a.end;
            nfa8.to = c.end;
            nfa8.c = '#';
            nfa[point++] = nfa8;
            break;
        case '\\':
            //遇到转义字符
            i++;
            c.begin = stateID++;
            c.end = stateID++;
            stateStack[++top] = c;

            NFA nfa10;
            nfa10.from = c.begin;
            nfa10.to = c.end;
            nfa10.c = cfg[i];
            nfa[point++] = nfa10;
            break;
        default:
            //遇到操作数
            c.begin = stateID++;
            c.end = stateID++;
            stateStack[++top] = c;

            NFA nfa9;
            nfa9.from = c.begin;
            nfa9.to = c.end;
            nfa9.c = cfg[i];
            nfa[point++] = nfa9;
            break;
        }
    }
    //输出NFA
    // for(int i=0;i<point;i++){
    //     cout<<nfa[i].from<<" "<<nfa[i].c<<" "<<nfa[i].to<<endl;
    // }
}

set<int> empty_transfer(int id){
    set<int> set1;
    set<int> set2;
    set1.insert(id);
    //遍历NFA，将到达的状态集合加入set1和set2
    for(int i=0;i<point;i++){
        if(nfa[i].from==id && nfa[i].c == '#'){
            set1.insert(nfa[i].to);
            set2.insert(nfa[i].to);
        } 
    }
    //遍历上述得到的set2，继续求空转移，直到set2为空，得到的set1就是状态的空转移
    while(!set2.empty()){
        int temp = *set2.begin();
        set2.erase(set2.begin());
        for(int i=0;i<point;i++){
            if(nfa[i].from==temp && nfa[i].c == '#'){
                set2.insert(nfa[i].to);
                set1.insert(nfa[i].to);
            } 
        }
    }
    return set1;
}

set<int> empty_move(int id,char c){
    //先求move（c）
	set<int> set1;
    for(int i=0;i<point;i++){
        if(nfa[i].from==id && nfa[i].c == c){
            set1.insert(nfa[i].to);
        } 
    }
    //得到move之后求空转移
    set<int> set2;
    set2.insert(set1.begin(),set1.end());
    set<int>::iterator it1;
    for (it1 = set1.begin(); it1 != set1.end(); it1++){
        set<int> set3 = empty_transfer(*it1);
        set2.insert(set3.begin(),set3.end());
    }
    return set2;
}

void NFA_DFA(){

	DFA* dfa = new DFA();
	
    //获取开始状态和终结状态
    State ss = stateStack[top];
    int begin = ss.begin;
    int end = ss.end;
    //初始状态的空转移
    set<int> initial = empty_transfer(begin);
    //状态集合
    set< set<int> > states;
    set< set<int> > states_temp;
    states.insert(initial);
    //重命名DFA
    dfa->dfa_id[initial] = ++dfa->id;
    //遍历状态集合
    while(!states.empty()){
        set<int> temp = *states.begin();
        states.erase(states.begin());
        set<int>::iterator it1;
        set<char>::iterator it2;
        //遍历操作数求closeuure（move）
        for (it2 = operands.begin(); it2 != operands.end(); it2++){
            set<int> new_set;
            for (it1 = temp.begin(); it1 != temp.end(); it1++){
                set<int> set_temp = empty_move(*it1,*it2);
                new_set.insert(set_temp.begin(),set_temp.end());
            }
            if(states_temp.count(new_set) == 0 && !new_set.empty()){
               
                states.insert(new_set);
                states_temp.insert(new_set);
                dfa->dfa_id[new_set] = ++dfa->id;
            }
			//cout<<dfa->dfa_id[temp]<<" "<<*it2<<" "<<dfa->dfa_id[new_set]<<endl;
            dfa->tran[dfa->dfa_id[temp]][*it2-33] = dfa->dfa_id[new_set];
            if(new_set.count(end)){
                dfa->vn[dfa->dfa_id[new_set]] = true;
            }else{
                dfa->vn[dfa->dfa_id[new_set]] = false;
            }
        }
    }
	dfas[point_dfa++] = dfa;
}

void init_DFA(string cfg){
    operands.clear();
	cfg = infix_Suffix(cfg);
	CFG_NFA(cfg);
	NFA_DFA();
    minimize_DFA();
}

void minimize_DFA(){
    set<set<int> > sets;
    DFA* dfa = dfas[point_dfa-1];
    //初态和终态区分
    //cout<<dfa->id<<endl;
    int len = dfa->id;
    set<int> set1,set2;
    for(int i=1;i<=len;i++){
        if(dfa->vn[i]){
            set1.insert(i);
        }else{
            set2.insert(i);
        }
    }
    sets.insert(set1);
    sets.insert(set2);

    set<set<int> > begin_set;
    do{
        begin_set = sets;
        set<set<int> >::iterator it1;
        set<int> temp;
        for (it1 = begin_set.begin(); it1 != begin_set.end(); it1++){
            temp= *it1;
            // if(temp.size() == 1){
            //     break;
            // }
            set<char>::iterator it2;
            for(it2 = operands.begin();it2 != operands.end();it2++){
                map<set<int>,set<int> > map_set;
                set<int>::iterator it3;
                for(it3 = temp.begin();it3 != temp.end();it3++){
                    int d = dfa->tran[*it3][*it2-33];
                    //判断属于哪个集合
                    //cout<<*it3<<" "<<*it2<<" "<<d<<endl;
                    set<set<int> >::iterator it4;
                    int flag = 0;
                    set<int>temp2,temp3;
                    for(it4 = begin_set.begin();it4 != begin_set.end();it4++){
                        temp2 = *it4;
                        if(temp2.count(d)>0){
                            temp3 = map_set[temp2];
                            flag = 1;
                        }
                    }
                    if(flag == 1){
                        sets.erase(temp);
                        temp3.insert(*it3);
                        map_set[temp2] = temp3;
                    }
                }
                
                map<set<int>,set<int> >::iterator it5;
                for(it5 = map_set.begin();it5 != map_set.end();it5++){
                //     set<int > ::iterator it;
                //      for(it = it5->second.begin();it!=it5->second.end();it++ ){
                //         cout<<*it;
                // }
                    sets.insert(it5->second);
                }
                
            }
            // set<set<int> >::iterator itr1;
            //         for(itr1 = sets.begin();itr1 != sets.end();itr1++){
            //             set<int> tempy = *itr1;
            //             set<int>::iterator ity1;
            //             for(ity1 = tempy.begin();ity1 !=tempy.end();ity1++){
            //                 cout<<*ity1;
            //             }
            //                cout<<endl;
            //         }
        }
        
    }while(begin_set != sets);
    DFA* min_dfa = new DFA();
    set<set<int> >::iterator it6;
    set<set<int> > sets_temp;

    for(it6 = sets.begin();it6 != sets.end();it6++){

        set<int> t_set = *it6;
        set<char>::iterator it7;
        int f = *t_set.begin();
        int id=0,to=0;
        if(sets_temp.count(t_set) == 0){
            sets_temp.insert(t_set);
            min_dfa->dfa_id[t_set] = ++min_dfa->id;  
        }
        id = min_dfa->dfa_id[t_set];
        for(it7 = operands.begin();it7 != operands.end();it7++){
            to = 0;
            int d = dfa->tran[f][*it7 -33];
            //cout<<f<<*it7<<d<<endl;
            set<set<int> >::iterator it8;
            for(it8 = sets.begin();it8 != sets.end();it8++){
                set<int> t_set2 = *it8;
                if(t_set2.count(d)>0){
                    if(sets_temp.count(t_set2) == 0){
                        min_dfa->dfa_id[t_set2] = ++min_dfa->id;
                        sets_temp.insert(t_set2);
                    }
                    to = min_dfa->dfa_id[t_set2];
                    if(min_dfa->vn[d]){
                        min_dfa->vn[to] = true;
                    }
                }
            }
            
            // min_dfa->tran[min_dfa->id][*it7-33] = to;
            //cout<<id<<" "<<*it7<<" "<<to<<endl;
            min_dfa->tran[id][*it7-33] = d;
        }
    }

    dfas[point_dfa] = min_dfa;
}

int distinguish(string s){
   for(int i = 0;i<point_dfa;i++){
        int cur = 1,flag = 1;
        DFA* dfa  = dfas[i];
        int to = 0;
		for(int j = 0;j<s.length();j++){
			to = dfa->tran[cur][s[j]-33];
			if(to == 0){
				flag = 0;
			}else{
				cur = to;
			}
		}
		if(flag ==1 && dfa->vn[to]){
			return i;
		}
   }
   return -1;
}

void myLex(string source,int floor){
	if(source == ""){return;
	}

    //ecucoder 的问题，本地可以运行，educoder上有Tab
    string s_new;
    for(int i=0;i<source.length();i++){
        if(source[i] != '\t'){
            s_new+=source[i];
        }
    }
	int ans = distinguish(s_new);
	if(ans !=-1){
        if(ans == 2 &&s_new.length()>8){
            cout<<"("<<"标识符长度超长"<<","<<s_new<<","<<"行号:"<<floor<<")"<<endl;
            return ;
        }else if(ans == 3 && s_new.length()>8){
            cout<<"("<<"无符号整数越界"<<","<<s_new<<","<<"行号:"<<floor<<")"<<endl;
            return ;
        }
		cout<<"("<<names[ans]<<","<<s_new<<")"<<endl;
	}else{
        cout<<"("<<"非法字符(串)"<<","<<s_new<<","<<"行号:"<<floor<<")"<<endl;
    }
}
