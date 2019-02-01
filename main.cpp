#pragma warning( disable : 4996 )


int main()
{
	printf("In main\n\n\n");

	cout << "Data Type Sizes:" << "\n";
	cout << "================" << "\n";
	cout << "bool........." << sizeof(bool) << " bytes" << "\n";
	cout << "char........." << sizeof(char) << " bytes" << "\n";
	cout << "int.........." << sizeof(int) << " bytes" << "\n";
	cout << "long........." << sizeof(long) << " bytes" << "\n";
	cout << "long long...." << sizeof(long long) << " bytes" << "\n";
	cout << "float........" << sizeof(float) << " bytes" << "\n";
	cout << "double......." << sizeof(double) << " bytes" << "\n";
	cout << "\n" << "\n";


	try
	{
		stock_report rep("input.csv", "output.csv");

		//update the max time gap
		rep.add_calculator([](std::vector<std::string>& tokens, sorted_vector<stock_report::stock, std::string, stock_report::Compare>::iterator it)
		{
			if (it->ts == 0) return;

			long long ts_now = stoll(tokens[0]);
			if (ts_now - it->ts > it->ts_max_gap)
				it->ts_max_gap = ts_now - it->ts;
		});

		//update the min time gap
		rep.add_calculator([](std::vector<std::string>& tokens, sorted_vector<stock_report::stock, std::string, stock_report::Compare>::iterator it)
		{
			if (it->ts == 0) return;

			long long ts_now = stoll(tokens[0]);
			if (ts_now - it->ts < it->ts_min_gap)
				it->ts_min_gap = ts_now - it->ts;
		});

		//update the volume & value
		rep.add_calculator([](std::vector<std::string>& tokens, sorted_vector<stock_report::stock, std::string, stock_report::Compare>::iterator it)
		{
			int quantity = atoi(tokens[2].c_str());
			int price = atoi(tokens[3].c_str());

			it->total_value += (quantity * price);
			it->volume += quantity;
		});

		//update the max price
		rep.add_calculator([](std::vector<std::string>& tokens, sorted_vector<stock_report::stock, std::string, stock_report::Compare>::iterator it)
		{
			int price = atoi(tokens[3].c_str());

			if (price > it->max_price)
				it->max_price = price;
		});

		//update the largest trade
		rep.add_calculator([](std::vector<std::string>& tokens, sorted_vector<stock_report::stock, std::string, stock_report::Compare>::iterator it)
		{
			int quantity = atoi(tokens[2].c_str());

			if (quantity > it->largest_trade)
				it->largest_trade = quantity;
		});

		rep.run_test();
	}
	catch (...)
	{
		cout << "Exception thrown\n";
	}


 	return 0;
}
