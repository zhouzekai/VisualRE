/*
ʹ�÷�����
��ʼ����translate��Ȼ����˳��ѹ�뵽DFA�С�DFAҪ���ÿ�ʼ�ͽ��ܡ�
����һ��string��string�ĳ��ȣ�����Ƿ���ܡ�
translate a0, a1, a2;
for (int i = 0; i < 256; i++) {
a0.table[i] = -1;
a1.table[i] = -1;
a2.table[i] = -1;
}

a0.table[int('a')] = 1;
a0.table[int('c')] = 2;
a1.table[int('a')] = 2;
char x = 'b';
do {
a1.table[int(x)] = 1;
x = x + 1;
} while (x != 'z');
a2.table[int('c')] = 2;

DFA dfa;
dfa.setAcceptState(2);
dfa.setStartState(0);
dfa.addState(a0);
dfa.addState(a1);
dfa.addState(a2);

string str;
while (cin >> str) {
bool x = dfa.IsAccepted(str, str.length());
cout << str << (x ? " accept" : " reject") << endl;
}*/

#pragma once
#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iterator>
#include <queue>
#include <fstream>
using namespace std;

struct translate {
	int table[128];

	translate()
	{
		for (int i = 0; i < 128; i++)
		{
			table[i] = -1;
		}
	}
};

class DFA
{
public:
	DFA();
	~DFA();

	/*
	���������char��x��translate[int(x)]�õ�����ת�ƹ�ȥ��״̬�����Ϊ-1����ʾ������������ת�ƺ�����
	�����Ҫ���һ���µ�״̬����ôֻ��Ҫ��ʼ����translate֮����states pushһ��translate��
	*/
	map<int, string> stateIDs;
	//vector<translate> states;
	map<int, translate> states;
	int index = 0;

	int startState;
	vector<int> acceptStates;

	void setStartState(int x);
	void addAcceptState(int x);

	/*
	���һ��״̬
	��ʼ��state��state[int(x)] = ĳһ��״̬��ID
	*/
	void addState(translate state);
	void addState(translate state, string stateID);
	void addStateID(int x, string y);

	/*
	ͨ��stateID�����Ҷ�Ӧvector<translate>�е�λ��
	����-1��ʾû���ҵ���
	*/
	int statesPos(string stateID);

	/*
	����һ���ַ���str,����length��Ȼ�󷵻����DFA�Ƿ���������
	*/
	bool IsAccepted(const char *str, int length);
	bool IsAccepted(string str, int length);

	/*
	����һ���ַ���һ��ת��
	��Ҫ�����״̬����ǰ��state����һ��accept��state
	*/
	void runOneStep(char x, bool &accepted, int &currentStateID);
	void reset();
	int getLastAcceptState();

	void removeUnreachableStates();

	void mergeNondistinguishableStates();

	void minized();

	void generate_DOT(const char *name);

private:
	int lastAcceptState;
	int currentState;

	bool seperate(set<int> input, char x, set<int> &A, set<int> &B);
	//set<int> translateToOneState(char c, set<int> theStates);
	int findOwner(vector<set<int>> P, int x);
};



DFA::DFA()
{
}


DFA::~DFA()
{
}


void DFA::setStartState(int x)
{
	startState = x;
}


void DFA::addAcceptState(int x)
{
	acceptStates.push_back(x);
}


void DFA::addState(translate state)
{
	//states.push_back(state);
	states[index] = state;
	index++;
}


void DFA::addState(translate state, string stateID)
{
	//states.push_back(state);
	states[index] = state;
	index++;
	stateIDs[states.size() - 1] = stateID;
	//stateIDs.push_back(stateID);
}

void DFA::addStateID(int x, string y)
{
	stateIDs[x] = y;
}


int DFA::statesPos(string stateID)
{
	for (int i = 0; i < stateIDs.size(); i++) {
		if (stateIDs[i] == stateID) {
			return i;
		}
	}
	return -1;
}


bool DFA::IsAccepted(const char *str, int length)
{
	int nextState = startState;
	for (int i = 0; i < length; i++) {
		nextState = states[nextState].table[int(str[i])];
		if (nextState == -1) {
			return false;
		}
	}
	for (int i = 0; i < acceptStates.size(); i++)
	{
		if (nextState == acceptStates[i])
		{
			return true;
		}
	}
	return false;
}


bool DFA::IsAccepted(string str, int length)
{
	int nextState = startState;
	for (int i = 0; i < length; i++) {
		nextState = states[nextState].table[int(str[i])];
		if (nextState == -1) {
			return false;
		}
	}
	for (int i = 0; i < acceptStates.size(); i++)
	{
		if (nextState == acceptStates[i])
		{
			return true;
		}
	}
	return false;
}


void DFA::runOneStep(char x, bool &accepted, int &currentStateID)
{
	int nextState = states[currentState].table[int(x)];
	currentStateID = nextState;
	if (nextState == -1) {
		accepted = false;
	}
	else {
		currentState = nextState;
		accepted = false;
		for (unsigned int i = 0; i < acceptStates.size(); i++) {
			if (acceptStates[i] == currentState) {
				accepted = true;
				lastAcceptState = currentState;
			}
		}
	}
}


void DFA::reset()
{
	currentState = startState;
}


int DFA::getLastAcceptState()
{
	return lastAcceptState;
}


void DFA::removeUnreachableStates()
{
	set<int> reachableStates = { startState };
	set<int> newStates = { startState };
	set <int> difference;

	do
	{
		set<int> temp;
		for (int x : newStates)
		{
			for (int y = 0; y < 128; y++)
			{
				int nextState = states[x].table[y];
				if (nextState != -1) {
					temp.insert(nextState);
				}
			}
		}
		newStates.clear();
		set_difference(temp.begin(), temp.end(), reachableStates.begin(), reachableStates.end(), inserter(newStates, newStates.begin()));
		set_union(temp.begin(), temp.end(), reachableStates.begin(), reachableStates.end(), inserter(reachableStates, reachableStates.begin()));

	} while (newStates.size() != 0);

	set<int> allStates;
	for (int i = 0; i<states.size(); i++) {
		allStates.insert(i);
	}

	set<int> unreachableStates;
	set_difference(allStates.begin(), allStates.end(), reachableStates.begin(), reachableStates.end(), inserter(unreachableStates, unreachableStates.begin()));

	if (unreachableStates.size() == 0) return;
	for (auto x : unreachableStates)
	{
		states.erase(x);
		index--;
	}
}


void DFA::mergeNondistinguishableStates()
{
	set<int> finalStates;
	set<int> otherStates;
	set<int> allStates;
	for (int i = 0; i < acceptStates.size(); i++)
	{
		finalStates.insert(acceptStates[i]);
	}
	for (int i = 0; i < states.size(); i++)
	{
		allStates.insert(i);
	}
	set_difference(allStates.begin(), allStates.end(), finalStates.begin(), finalStates.end(), inserter(otherStates, otherStates.begin()));

	queue<set<int>> newAllStates;
	newAllStates.push(otherStates);
	newAllStates.push(finalStates);

	vector<set<int>> newStates;

	while (!newAllStates.empty())
	{
		set<int> stateSet = newAllStates.front();
		newAllStates.pop();
		if (stateSet.size() == 1) {
			newStates.push_back(stateSet);
			continue;
		}
		set<int> A, B;
		bool seperatable = false;
		for (int i = 0; i < 128; i++)
		{
			if (seperate(stateSet, char(i), A, B))
			{
				newAllStates.push(A);
				newAllStates.push(B);
				seperatable = true;
				break;
			}
		}
		if (!seperatable) {
			newStates.push_back(stateSet);
		}
	}

	DFA newDFA;

	for (int i = 0; i < newStates.size(); i++)
	{
		set<int> X = newStates[i];
		set<int>::iterator it = X.find(startState);
		if (it != X.end()) newDFA.setStartState(i);
		newDFA.addState(translate());
	}

	set<int> newAcceptStates;
	for (int i = 0; i < newStates.size(); i++)
	{
		for (int x : newStates[i])
		{
			for (int j = 0; j < 128; j++)
			{
				bool isAcceptState = false;
				for (int state_i = 0; state_i < acceptStates.size(); state_i++)
				{
					if (acceptStates[state_i] == x) {
						isAcceptState = true;
						break;
					}
				}
				int newCurrentState = findOwner(newStates, x);

				if (isAcceptState) {
					//newDFA.addAcceptState(newCurrentState);
					newAcceptStates.insert(newCurrentState);
				}

				int nextState = states[x].table[j];
				if (nextState == -1) continue;
				int newNextState = findOwner(newStates, nextState);
				newDFA.states[newCurrentState].table[j] = newNextState;
			}
		}
	}
	for (auto x : newAcceptStates)
	{
		newDFA.addAcceptState(x);
	}

	*this = newDFA;
}


void DFA::minized()
{
	mergeNondistinguishableStates();
	removeUnreachableStates();
}


void DFA::generate_DOT(const char *name)
{
	ofstream outFile(name);
	string shapeCircle = "[shape=circle]";
	string shapeDoubleCircle = "[shape=doublecircle]";

	outFile << "digraph graphname {\n";
	outFile << "\trankdir=LR;\n";

	// define shape
	outFile << "\t\"\" [shape=none]\n";
	set<int> finalStates;
	set<int> otherStates;
	set<int> allStates;
	for (int i = 0; i < acceptStates.size(); i++)
	{
		finalStates.insert(acceptStates[i]);
	}
	for (int i = 0; i < states.size(); i++)
	{
		map<int, translate>::iterator it = states.find(i);
		if (it != states.end())
			allStates.insert(i);
	}
	set_difference(allStates.begin(), allStates.end(), finalStates.begin(), finalStates.end(), inserter(otherStates, otherStates.begin()));
	for (auto x : finalStates)
	{
		outFile << "\t" << x << " " << shapeDoubleCircle << endl;
	}
	for (auto x : otherStates)
	{
		outFile << "\t" << x << " " << shapeCircle << endl;
	}

	// define translate
	// sample a->b[label = "0"]
	string labelStart = "[label =\"";
	string labelEnd = "\"]";
	string arrow = "->";

	outFile << "\t" << "\"\"" << arrow << startState << endl;

	for (auto x : states)
	{
		for (int i = 0; i < 128; i++)
		{
			int nextState = x.second.table[i];
			if (nextState != -1)
			{
				outFile << "\t" << x.first << arrow << nextState << " " << labelStart << char(i) << labelEnd << endl;
			}
		}
	}


	outFile << "}";
}


bool DFA::seperate(set<int> input, char x, set<int>& A, set<int>& B)
{
	A.clear();
	B.clear();
	bool seperatable = false;;
	for (auto state : input)
	{
		int nextState = states[state].table[x];
		if (nextState == -1) continue;

		set<int>::iterator it = input.find(nextState);
		if (it == input.end())
		{
			B.insert(state);
			seperatable = true;
		}
	}

	set_difference(input.begin(), input.end(), B.begin(), B.end(), inserter(A, A.begin()));
	if (A.size() == 0)  seperatable = false;
	return seperatable;
}


//set<int> DFA::translateToOneState(char c, set<int> theStates)
//{
//	set<int> reachableStates;
//	set<int> result;
//	for (auto x : theStates)
//	{
//		int nextStates = states[x].table[c];
//		if (nextStates != -1) {
//			reachableStates.insert(nextStates);
//			result.insert(x);
//		}
//	}
//
//	if (reachableStates.size() == 0) return set<int>();
//	set<int>::iterator it = theStates.find(*reachableStates.begin());
//	if (reachableStates.size() == 1 && it != theStates.end())
//	{
//		return result;
//	}
//	return set<int>();
//}


int DFA::findOwner(vector<set<int>> P, int x)
{
	for (int i = 0; i < P.size(); i++)
	{
		if (P[i].find(x) != P[i].end()) return i;
	}
}
