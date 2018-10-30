/*
�ο� Modern Compiler Implementation in C P24 figure 2.5
��������״̬������һ����ʾ��һ�ν���accept stateʱ��state�ϵı�š�һ����ʾ��ǰ�����״̬�ı�š�
ɨ��Ĺ��̣���������λ�ñ�����һ�������ڲ��ϵ�ǰ����һ������������һ��ɨ�迪ʼʱ��λ�ã�
����һ������������һ����һ��ǰ���ɹ���λ�á��������ǰ����Ҫ���µ�����������
�������ǰ������ô����ڶ����������������������λ�÷�Χ���ַ������������һ�ν���accpet state��״̬�ı�š�
Ȼ���������λ�ñ���Ϊ�ڶ�λ�ñ�������ֵ��

ʹ�õ�ʱ��Ҫע�����ȼ��ߵĹ���Ҫ����루addPattern)��
*/

#pragma once
#include "stdafx.h"
#include "DFA.h"
#include "RegularExpression.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;
class Lexer
{
public:
	Lexer();
	~Lexer();

	vector<string> patterns;
	vector<NFA> NFAs;
	NFA mixNFA;;
	DFA dfa;
	string text;

	void readText(const char *fileName);
	void addPattern(string pattern);
	void convertToDFA();
	void inputSample(string x, string y);
	vector<pair<string, string>> scan();
};



Lexer::Lexer()
{
}


Lexer::~Lexer()
{
}


/*
Reference:
https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
https://www.tutorialspoint.com/Read-whole-ASCII-file-into-Cplusplus-std-string
*/
void Lexer::readText(const char *fileName)
{
	ifstream t(fileName);
	stringstream buffer;
	buffer << t.rdbuf();
	text = buffer.str();
}


void Lexer::addPattern(string pattern)
{
	patterns.push_back(pattern);
}


void Lexer::convertToDFA()
{
	for (int i = 0; i < patterns.size(); i++) {
		NFAs.push_back(re_to_NFA(patterns[i]));
	}
	NFA nfa = NFAs[0];
	for (int i = 1; i < NFAs.size(); i++) {
		nfa = nfa_union(nfa, NFAs[i]);
	}
	mixNFA = nfa;
	dfa = nfa.convertToDFA();
}


/*
input: x
stateID: y
*/
void Lexer::inputSample(string x, string y)
{
	dfa.reset();
	bool accepted;
	int acceptedID;
	for (int i = 0; i < x.length(); i++)
	{
		dfa.runOneStep(x[i], accepted, acceptedID);
	}
	dfa.addStateID(acceptedID, y);
	//dfa.stateIDs[acceptedID] = y;
}


vector<pair<string, string>> Lexer::scan()
{
	int startIndex = 0, lastAcceptIndex = 0, indexExplorer = 0;
	bool accepted;
	int currentState;
	vector<pair<string, string>> result;
	dfa.reset();
	bool haveAccepted = false;
	int lastAcceptState = 0;
	while (indexExplorer < text.length())
	{
		dfa.runOneStep(text[indexExplorer], accepted, currentState);
		if (accepted)
		{
			lastAcceptState = currentState;
			haveAccepted = true;
			lastAcceptIndex = indexExplorer;
			indexExplorer++;
			if (indexExplorer == text.length()) {
				string input = text.substr(startIndex, lastAcceptIndex - startIndex + 1);
				string stateID = dfa.stateIDs[dfa.getLastAcceptState()];
				result.push_back(pair<string, string>(input, stateID));
				break;
			}
		}
		else if (!accepted && (currentState == -1) && haveAccepted)
		{
			string input = text.substr(startIndex, lastAcceptIndex - startIndex + 1);
			string stateID = dfa.stateIDs[lastAcceptState];
			startIndex = lastAcceptIndex + 1;
			indexExplorer = lastAcceptIndex + 1;
			lastAcceptIndex++;
			result.push_back(pair<string, string>(input, stateID));

			dfa.reset();
			haveAccepted = false;
		}
		else {
			indexExplorer++;
			if (indexExplorer == text.length()) cerr << "EOF" << endl;
		}
	}
	return result;
}
