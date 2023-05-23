/* Autor: Uroš Jevtić Datum: 12.06.2022. */
#include "LivenesAnalysis.h"

/*Checks if there is already variable in a list*/
bool variableExists(Variable* variable, Variables variables) {
	for (Variables::iterator it = variables.begin(); it != variables.end(); it++)
	{
		if (variable->m_name == (*it)->m_name && (*it)->m_type == variable->m_type)
		{
			return true;
		}
	}

	return false;
}

/*Does livenes analysis*/
void livenessAnalysis(Instructions instructions)
{
	bool condition = true;

	while (condition) {
		condition = false;

		for (Instructions::reverse_iterator rit = instructions.rbegin(); rit != instructions.rend(); rit++) 
		{

			Variables& out = (*rit)->m_out;
			Variables& in = (*rit)->m_in;
			Variables out2;
			Variables in2;

			/* out[n] ← U sUsucc[n]in[s]*/
			Instructions& successors = (*rit)->m_succ;
			for (Instructions::iterator s_it = successors.begin(); s_it != successors.end(); s_it++) 
			{
				Variables& succIn = (*s_it)->m_in;
				out2.insert(out2.end(), succIn.begin(), succIn.end());
			}
			/*Sorts list*/
			out2.sort();
			/*Removes all duplicate elements*/
			out2.unique();

			/*in[n] ← use[n] U (out[n]-def[n])*/
			Variables& use = (*rit)->m_use;
			Variables& def = (*rit)->m_def;

			/*(out[n]-def[n])*/
			Variables outMinusDef;
			for (Variables::iterator varIt = out.begin(); varIt != out.end(); varIt++)
			{
				if (!variableExists(*varIt, def)) 
				{
					outMinusDef.push_back(*varIt);
				}
			}

			in2 = use;
			for (Variables::iterator oud_it = outMinusDef.begin(); oud_it != outMinusDef.end(); oud_it++)
			{
				in2.push_back((*oud_it));
			}
			in2.sort();
			in2.unique();

			if (in2 != in || out2 != out) {
				condition = true;
			}
			/*Sorts list*/
			in = in2;
			/*Removes all duplicate elements*/
			out = out2;
		}
	}
}