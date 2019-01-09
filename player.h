/* Player class provide the information of its accumulated payoffs in the game and their strategy selection
 *
 */

#include "strategy.h"

class Player
{
	private:
		int m_acc_payoffs;

	public:
		Info m_info;
		int index;
		Strategy current_strategy;
		
		Player(Strategy s, int ind) : m_acc_payoffs(0)
		{
			current_strategy = s;
			index = ind;
		}

		void exec();
		int get_acc_payoffs(){return m_acc_payoffs;}
		void print_payoffs(){cout << "Acculated Payoff:" << get_acc_payoffs() << endl;}
};

void Player::exec()
{
	int action = current_strategy.exec(m_info);
	if(action == 0)
	{
		m_acc_payoffs += 5;
		m_info.my_history.push_back(0);
	}
	else
	{
		m_acc_payoffs += 1;
		m_info.my_history.push_back(1);
	}
}

