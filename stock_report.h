#ifndef STOCK_REPORT_H
#define STOCK_REPORT_H

#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include "sorted_vector.h"
#include "raw_algorithms.h"


#undef SHOW_TEST_OUTPUT


class stock_report
{
public:
	stock_report();
	stock_report(std::string in_file_name, std::string out_file_name);
	~stock_report();

	void run_test();
	bool process_record(std::string record);

	struct stock
	{
		std::string symbol;		//aaa, aab, aac...
		long long ts;			//time stamp from last event
		long long ts_max_gap;	//largest gap between the two events
		long long ts_min_gap;	//smallest gap between the two events
		int total_value;		//running sum of (price * quantity)
		int volume;				//running sum of quantities
		int num_trades;			//number of trades
		int largest_trade;		//largest single trade
		int max_price;			//max stock price seen since start  of run
	};

	//compare function allows object-2-object and object-2-key comparisons
	struct Compare
	{
		bool operator()(const stock& lhs, const stock& rhs) const
		{
			return lhs.symbol < rhs.symbol;
		}

		template<typename T>
		bool operator()(const stock& lhs, const T& symbol) const
		{
			return lhs.symbol < symbol;
		}

		template<typename T>
		bool operator()(const T& symbol, const stock& rhs) const
		{
			return symbol < rhs.symbol;
		}

		using is_transparent = int;
	};

	using calculation_functor = std::function<void(std::vector<std::string>& tokens, sorted_vector<stock, std::string, Compare>::iterator it)>;
	void add_calculator(calculation_functor func);


private:

	//accessor functions for updating the stock table
	void add(std::vector<std::string>& tokens);
	void update(std::vector<std::string>& tokens, sorted_vector<stock, std::string, Compare>::iterator it);

	//stock table
	sorted_vector<stock, std::string, Compare> m_stocks;
	std::vector<calculation_functor> m_calculators;

	//Stream file in/out objects
	std::ifstream m_in_file;
	std::ofstream m_out_file;
	bool m_in_test_mode;
};

stock_report::stock_report() : 
	m_stocks(), 
	m_calculators(), 
	m_in_test_mode(false) 
{}

//ctor requires an input & output file, nothing happens without them
//throws if either can't be accessed (permissions, missing, et al)
stock_report::stock_report(std::string in_file_name, std::string out_file_name) : 
	m_stocks(), 
	m_calculators(), 
	m_in_test_mode(true)
{
	//a cheat, the current test run had 350 individual stocks so start there
	m_stocks.reserve(350);

	m_in_file.open(in_file_name);
	if (!m_in_file.good())
	{
		m_in_test_mode = false;
		return;
	}

	m_out_file.open(out_file_name);
	if (!m_out_file.good())
	{
		m_in_file.close();
		m_in_test_mode = false;
	}

	if (m_in_test_mode)
		std::cout << "Entering test mode\n";
}


//dtor uses RAII to close up the files on loss of scope
stock_report::~stock_report()
{
	if (m_in_test_mode)
	{
		m_in_file.close();
		m_out_file.close();
	}
}

void stock_report::add_calculator(calculation_functor func)
{
	m_calculators.push_back(func);
}

//adds new stock to the table
void stock_report::add(std::vector<std::string>& tokens)
{
	stock_report::stock obj;
	obj.symbol = tokens[1];
	obj.ts = 0;
	obj.ts_max_gap = 0;
	obj.ts_min_gap = INT64_MAX;
	obj.total_value = 0;
	obj.volume = 0;
	obj.num_trades = 0;
	obj.largest_trade = atoi(tokens[2].c_str());
	obj.max_price = atoi(tokens[3].c_str());

	auto it = m_stocks.insert(obj);
	update(tokens, it);
}

//updates an existing stock
void stock_report::update(std::vector<std::string>& tokens, sorted_vector<stock, std::string, Compare>::iterator it)
{
	for (const auto& functor : m_calculators)
		functor(tokens, it);

	it->ts = stoll(tokens[0]);
	++it->num_trades;
}

//main handler reads, compiles & writes
void stock_report::run_test()
{
	if (!m_in_test_mode)
	{
		std::cout << "Test mode disabled\n";
		return;
	}

	//Sample Input :
	//<TimeStamp>,<Symbol>,<Quantity>,<Price>
	//52924702,aaa,13,1136
	//52924702,aac,20,477
	//52925641,aab,31,907
	//52927350,aab,29,724
	//52927783,aac,21,638
	//52930489,aaa,18,1222
	//52931654,aaa,9,1077
	//52933453,aab,9,756

	std::string line;
	while (getline(m_in_file, line))
		process_record(line);

	for (const auto& n : m_stocks)
		m_out_file << n.symbol << "," << n.ts_max_gap << "," << n.volume << "," << (n.volume > 0 ? n.total_value / n.volume : 0) << "," << n.max_price << "," << n.ts_min_gap << "," << n.largest_trade << "," << n.num_trades << "\n";

	//Sample Output :
	//<symbol>,<MaxTimeGap>,<Volume>,<WeightedAveragePrice>,<MaxPrice>
	//aaa,5787,40,1161,1222
	//aab,6103,69,810,907
	//aac,3081,41,559,638

	#ifdef SHOW_TEST_OUTPUT
	for (const auto& n : m_stocks)
		std::cout << n.symbol << "," << n.ts_max_gap << "," << n.volume << "," << (n.volume > 0 ? n.total_value / n.volume : 0) << "," << n.max_price << "," << n.ts_min_gap << "," << n.largest_trade << "," << n.num_trades << "\n";
	#endif
}

//public process method for use when data is being fed from an alternate source
//such as a ring buffer being populated through a network source, et al
bool stock_report::process_record(std::string record)
{
	auto tokens = raw_algorithms::tokenizer(record, ',');
	if (tokens.size() != 4)
		return false;

	auto it = m_stocks.find(tokens[1]);
	if (it != m_stocks.end())
		update(tokens, it);
	else
		add(tokens);

	return true;
}

#endif //STOCK_REPORT_H