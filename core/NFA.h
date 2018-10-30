/*
ʹ���ڽ��������洢NFA����Ϣ����ģ�����У������洢��Ϣ���ɡ�
vector<pair<int, char>> G[]
*/

#pragma once
#include "stdafx.h"
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include "DFA.h"
using namespace std;
class NFA
{
public:
	NFA();
	NFA(int stateCount);
	~NFA();

	int stateCount;
	int startState;
	vector<int> acceptStates;
	vector<pair<int, char>> *G;

	void setStartState(int x);
	void addAcceptState(int state);
	void addEdge(int state1, int state2, char c);

	/*
	epsilon Closure
	*/
	void epsilonClosure(int state, vector<int> &reachableStates);
	void epsilonClosure(vector<int> states, vector<int> &reachableStates);

	/*
	input a character, return reachable states
	*/
	void alphabetClosure(char x, int state, vector<int> &reachableStates);
	void alphabetClosure(char x, vector<int> states, vector<int> &reachableStates);

	DFA convertToDFA();

	void generate_DOT(const char *name);

private:
	bool hasSameVector(vector<int> &a, vector<int> &b);
};

NFA nfa_concatenate(NFA a, NFA b);
NFA nfa_union(NFA a, NFA b);
NFA nfa_star(NFA a);



NFA::NFA()
{
}


NFA::NFA(int stateCount)
{
	G = new vector<pair<int, char>>[stateCount];
	this->stateCount = stateCount;
}


NFA::~NFA()
{
}


void NFA::setStartState(int x)
{
	startState = x;
}


void NFA::addAcceptState(int state)
{
	acceptStates.push_back(state);
}


void NFA::addEdge(int state1, int state2, char c)
{
	G[state1].push_back(pair<int, char>(state2, c));
}


void NFA::epsilonClosure(int state, vector<int> &reachableStates)
{
	for (int i = 0; i < G[state].size(); i++)
	{
		if (G[state][i].second == '\0' && G[state][i].first != state) {
			reachableStates.push_back(G[state][i].first);
			epsilonClosure(G[state][i].first, reachableStates);
		}
	}
}


void NFA::epsilonClosure(vector<int> states, vector<int> &reachableStates)
{
	for (int i = 0; i < states.size(); i++)
	{
		epsilonClosure(states[i], reachableStates);
	}
}


void NFA::alphabetClosure(char x, int state, vector<int> &reachableStates)
{
	for (int i = 0; i < G[state].size(); i++)
	{
		if (G[state][i].second == x) {
			if (count(reachableStates.begin(), reachableStates.end(), G[state][i].first)) {
				continue;
			}
			else {
				reachableStates.push_back(G[state][i].first);
				epsilonClosure(G[state][i].first, reachableStates);
			}
		}
	}
}


void NFA::alphabetClosure(char x, vector<int> states, vector<int> &reachableStates)
{
	for (int i = 0; i < states.size(); i++)
	{
		alphabetClosure(x, states[i], reachableStates);
	}
}


bool NFA::hasSameVector(vector<int> &a, vector<int> &b)
{
	sort(a.begin(), a.end());
	sort(b.begin(), b.end());

	if (a.size() != b.size()) return false;
	for (int i = 0; i < a.size(); i++)
	{
		if (a[i] != b[i])
		{
			return false;
		}
	}
	return true;
}


DFA NFA::convertToDFA()
{
	//map<int, vector<int>> M;
	vector<vector<int>> M;
	vector<int> reachableStates;
	reachableStates.push_back(startState);
	epsilonClosure(startState, reachableStates);

	M.push_back(vector<int>());
	M.push_back(reachableStates);
	//M[0] = vector<int>();
	//M[1] = reachableStates;

	DFA dfa;
	dfa.addState(translate()); // state 0
	dfa.addState(translate()); // state 1
	dfa.setStartState(1);
	bool flag = false;
	for (int i = 0; i < reachableStates.size(); i++) {
		for (int j = 0; j < acceptStates.size(); j++) {
			if (reachableStates[i] == acceptStates[j]) {
				dfa.addAcceptState(1);
				flag = true;
				break;
			}
		}
		if (flag) break;
	}

	int j = 0, p = 1;
	while (j <= p)
	{
		vector<int> temp;
		for (int i = 1; i < 128; i++)
		{
			temp.clear();
			alphabetClosure(char(i), M[j], temp);
			bool find = false;
			if (temp.size() == 0)
			{
				find = true;
				dfa.states[0].table[i] = '\0';
			}
			else
			{
				for (int k = 1; k <= p; k++)
				{
					if (temp.size() != M[k].size()) continue;
					else if (hasSameVector(temp, M[k]))
					{
						dfa.states[j].table[i] = k;
						find = true;
						break;
					}
				}
			}
			if (!find)
			{
				p = p + 1;
				for (int ii = 0; ii < temp.size(); ii++)
				{
					for (int jj = 0; jj < acceptStates.size(); jj++)
					{
						if (temp[ii] == acceptStates[jj])
						{
							dfa.addAcceptState(p);
							break;
						}
					}
				}
				M.push_back(temp);
				dfa.addState(translate());
				dfa.states[j].table[i] = p;
			}
		}
		j++;
	}

	return dfa;
}


NFA nfa_concatenate(NFA a, NFA b)
{
	//if (b.stateCount == 2 && b.acceptStates[0] == 1)
	//{
	//	NFA c(a.stateCount + 1);
	//	c.setStartState(a.startState);
	//	c.addAcceptState(a.stateCount);
	//	for (int i = 0; i < a.stateCount; i++) {
	//		c.G[i].insert(c.G[i].end(), a.G[i].begin(), a.G[i].end());
	//	}
	//	for (int i = 0; i < a.acceptStates.size(); i++) {
	//		for (int j = 0; j < b.G[0].size(); j++) {
	//			c.addEdge(a.acceptStates[i], a.stateCount, b.G[0][j].second);
	//		}
	//	}
	//	return c;
	//}

	//NFA temp = a;
	//a = b;
	//b = temp;

	//if (b.stateCount == 2 && b.acceptStates[0] == 1)
	//{
	//	NFA c(a.stateCount + 1);
	//	c.setStartState(a.startState);
	//	c.addAcceptState(a.stateCount);
	//	for (int i = 0; i < a.stateCount; i++) {
	//		c.G[i].insert(c.G[i].end(), a.G[i].begin(), a.G[i].end());
	//	}
	//	for (int i = 0; i < a.acceptStates.size(); i++) {
	//		for (int j = 0; j < b.G[0].size(); j++) {
	//			c.addEdge(a.acceptStates[i], a.stateCount, b.G[0][j].second);
	//		}
	//	}
	//	return c;
	//}

	//temp = a;
	//a = b;
	//b = temp;

	// ���ȣ���b������״̬��ID����a��ƫ������
	int bias = a.stateCount;
	for (int i = 0; i < b.stateCount; i++)
	{
		for (int j = 0; j < b.G[i].size(); j++)
		{
			b.G[i][j].first += bias;
		}
	}

	// ��b��ͼ�ӵ�a�ĺ���
	NFA c(a.stateCount + b.stateCount);
	for (int i = 0; i < a.stateCount; i++)
	{
		c.G[i].insert(c.G[i].end(), a.G[i].begin(), a.G[i].end());
	}
	for (int i = a.stateCount; i < a.stateCount + b.stateCount; i++)
	{
		c.G[i].insert(c.G[i].end(), b.G[i - a.stateCount].begin(), b.G[i - a.stateCount].end());
	}

	// ����һ��epsilonת�ƣ���a��accept states��b��start states
	for (int i = 0; i < a.acceptStates.size(); i++)
	{
		c.G[a.acceptStates[i]].push_back(pair<int, char>(b.startState + a.stateCount, '\0'));
	}

	// ����b��accept state Ϊ c��accept state
	for (int i = 0; i < b.acceptStates.size(); i++)
	{
		c.addAcceptState(b.acceptStates[i] + a.stateCount);
	}

	c.setStartState(a.startState);

	return c;
}


NFA nfa_union(NFA a, NFA b)
{
	if (a.stateCount == 2 && b.stateCount == 2 && a.acceptStates[0] == 1 && b.acceptStates[0] == 1) {
		NFA c(2);
		c.setStartState(0);
		c.addAcceptState(1);
		for (int i = 0; i < a.G[0].size(); i++) {
			c.addEdge(0, 1, a.G[0][i].second);
		}
		for (int i = 0; i < b.G[0].size(); i++) {
			c.addEdge(0, 1, b.G[0][i].second);
		}
		return c;
	}

	// ���ȣ���b������״̬��ID����a��ƫ������
	int bias = a.stateCount;
	for (int i = 0; i < b.stateCount; i++)
	{
		for (int j = 0; j < b.G[i].size(); j++)
		{
			b.G[i][j].first += bias;
		}
	}

	// ��b��ͼ�ӵ�a�ĺ���
	NFA c(a.stateCount + b.stateCount + 1);
	for (int i = 0; i < a.stateCount; i++)
	{
		c.G[i].insert(c.G[i].end(), a.G[i].begin(), a.G[i].end());
	}
	for (int i = a.stateCount; i < a.stateCount + b.stateCount; i++)
	{
		c.G[i].insert(c.G[i].end(), b.G[i - a.stateCount].begin(), b.G[i - a.stateCount].end());
	}

	// ������һ��start state, ���start state �� ת�Ƶ� ��������start state, ��0��ʼ�������������һ��״̬������count�ĺ͡�
	c.setStartState(a.stateCount + b.stateCount);
	c.addEdge(a.stateCount + b.stateCount, a.startState, '\0');
	c.addEdge(a.stateCount + b.stateCount, b.startState + a.stateCount, '\0');

	// c��accept state
	for (int i = 0; i < a.acceptStates.size(); i++)
	{
		c.addAcceptState(a.acceptStates[i]);
	}
	for (int i = 0; i < b.acceptStates.size(); i++)
	{
		c.addAcceptState(b.acceptStates[i] + a.stateCount);
	}

	return c;
}


/*
ITOC P62
*/
NFA nfa_star(NFA a)
{
	NFA c(a.stateCount + 1);
	for (int i = 0; i < a.stateCount; i++)
	{
		c.G[i].insert(c.G[i].end(), a.G[i].begin(), a.G[i].end());
	}
	c.setStartState(a.stateCount);
	c.addAcceptState(a.stateCount);
	c.addEdge(a.stateCount, a.startState, '\0');

	for (int i = 0; i < a.acceptStates.size(); i++)
	{
		c.addEdge(a.acceptStates[i], a.startState, '\0');
		c.addAcceptState(a.acceptStates[i]);
	}

	return c;
}


void NFA::generate_DOT(const char *name)
{
	ofstream outFile(name);
	string shapeCircle = "[shape=circle]";
	string shapeDoubleCircle = "[shape=doublecircle]";

	outFile << "digraph graphname {\n";
	outFile << "\trankdir=LR;\n";

	// define shape
	outFile << "\t\"\" [shape=none]\n";
	set<int> finalStates;
	set<int> allStates;
	set<int> otherStates;

	for (int i = 0; i < acceptStates.size(); i++)
	{
		finalStates.insert(acceptStates[i]);
	}
	for (int i = 0; i < stateCount; i++)
	{
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
	string epsilon = "epsilon";

	outFile << "\t" << "\"\"" << arrow << startState << endl;

	for (int i = 0; i <stateCount; i++)
	{
		for (int j = 0; j < G[i].size(); j++)
		{
			if (G[i][j].second != '\0') {
				outFile << "\t" << i << arrow << G[i][j].first << " " << labelStart << G[i][j].second << labelEnd << endl;
			}
			else {
				outFile << "\t" << i << arrow << G[i][j].first << " " << labelStart << epsilon << labelEnd << endl;
			}
		}
	}

	outFile << "}";
}