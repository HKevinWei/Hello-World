#include <iostream>
#include <string>

using namespace std;

int main()
{
	// acquire minutes
		int minutes;
		cout << "Minutes used: ";
		cin >> minutes;

	// acquire texts
		int texts;
		cout << "Text messages: ";
		cin >> texts;
		cin.ignore(10000, '\n');

	// acquire name
		cout << "Customer name: "; 
		string name;
		getline(cin, name);

	// acquire month
		int month;
		cout << "Month number (1=Jan, 2=Feb, etc.): ";
		cin >> month;

	// calculate bill
		double R; //R is rate
		if (month <= 5 || month >= 10) 
			R = .03;
		else 
			R = .02;

		double cost=40.00;

		if (minutes > 500) //if minutes max out
			cost = cost + (minutes - 500)*.45;


		if (texts <= 200) //if texts stay below 200, dont need to pay extra 
			cost = cost;

		else if (texts <= 400) //if texts exceed, need to pay extra
			cost = cost + (texts - 200)*R;

		else if (texts > 400) //if texts exceed 400, need to pay .11 per extra text
			cost = cost + 200 * R + (texts - 400)*.11;

		cout << "---" << endl;

		//errors

		if (minutes < 0)  
			cout << "The number of minutes used must be nonnegative.";
		else if (texts < 0)
			cout << "The number of text messages must be nonnegative.";
		else if (name == "")
			cout << "You must enter a customer name.";
		else if (month >= 13 || month <= 0)
			cout << "The month number must be in the range 1 through 12.";
		else
		{
			cout.setf(ios::fixed);
			cout.precision(2);
			cout << "The bill for " << name << " is $" << cost;
		}
}