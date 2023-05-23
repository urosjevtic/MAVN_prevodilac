/* Autor: Uroš Jevtić Datum: 16.06.2022. */
#include <iostream>
#include <exception>

#include "LexicalAnalysis.h"
#include "SyntaxAnalysis.h"
#include "LivenesAnalysis.h"
#include "IR.h"
#include "InterferenceGraph.h"

using namespace std;


int main()
{
	try
	{
		string fileName = ".\\..\\examples\\mavn_files\\";
		string file = "";
		cout << "Enter the full name of the file that you want to load: ";
		cin >> file;
		fileName += file;
		bool retVal = false;

		LexicalAnalysis lex;

		if (!lex.readInputFile(fileName))
			throw runtime_error("\nException! Failed to open input file!\n");

		lex.initialize();

		retVal = lex.Do();

		if (retVal)
		{
			cout << "Lexical analysis finished successfully!" << endl;
			lex.printTokens();
		}
		else
		{
			lex.printLexError();
			throw runtime_error("\nException! Lexical analysis failed!\n");
		}

		/*Start of syntax analysis*/
		cout << "--------------------------------------------------" << endl;
		cout << "Syntax analysis start:" << endl;

		
		Instructions instructions;		//List of instructions

		SyntaxAnalysis syntax(lex, instructions);
		
		if (syntax.Do())
		{
			cout << "Syntax analysis finished successfully!" << endl;
		}
		else
		{
			throw runtime_error("Syntax analysis failed!");
			cout << "Syntax analysis failed!" << endl;
		}
		
		/*Does liveness analysis*/
		livenessAnalysis(instructions);

		/*Create interference graph*/
		InterferenceGraph ig(syntax.getRegVariables());
		ig.buildInterferenceGraph(instructions);
		ig.buildVariableStack();

		/* Print interference matrix*/
		cout << "Interference matrix:" << endl;
		ig.printInterferenceMatrix();

		//syntax.printAlocated();

		/*Does resource allocation*/
		if (ig.resourceAllocation())
			cout << "Resource allocation successful!" << endl;
		else
			throw runtime_error("\nResource allocation failed!!\n");


		syntax.createMIPS();
	}



	catch (runtime_error e)
	{
		cout << e.what() << endl;
		return 1;
	}

	return 0;
}
