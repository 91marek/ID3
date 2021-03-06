/*
 * main.cpp
 */

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <id3lib/DecisionTree.hpp>
#include <id3lib/ZPRDecisionTree.hpp>
#include <id3lib/UMDecisionTree.hpp>
#include <stdexcept>
#include <boost/tokenizer.hpp>
#include <limits>

using std::vector;
using std::string;
using std::invalid_argument;
using boost::shared_array;
using std::numeric_limits;
using std::istream;
using boost::shared_ptr;
using boost::tokenizer;
using boost::char_separator;
using std::logic_error;
using std::ostream;
using std::endl;
using std::cout;
using std::cerr;
using std::ifstream;
using namespace id3lib;

const int UNKNOWN_MODE = -1;
const int ZPR_MODE = 0;
const int UM_MODE = 1;

const char zpr[] = "zpr";
const char um[] = "um";

void help() {
	cerr << "Args:" << endl
	<< string(zpr) + " building_set_file pruning_set_file to_classify_set_file category_index"
	<< endl << "or" << endl
	<< string(um) + " building_set_file to_classify_set_file category_index mep_parameter"
	<< endl
	<< endl
	<< "category_index - Index of column category counted from 0."
	<< endl
	<< "Important! All files must have column names in first line."
	<< endl
	<< string(zpr) + " uses REP pruning method."
	<< endl
	<< string(um) + " uses MEP pruning method."
	<< endl;
}

int main(int argc, char* argv[]) {
	// check parameters number
	if (argc != 6) {
		help();
		return EXIT_FAILURE;
	}
	
	// choose mode
	int mode = UNKNOWN_MODE;
	if (!strcmp(argv[1], zpr)) {
		mode = ZPR_MODE;
	} else if (!strcmp(argv[1], um)) {
		mode = UM_MODE;
	}
	
	// quit app if mode is unknown
	if (UNKNOWN_MODE == mode) {
		help();
		return EXIT_FAILURE;
	}
	
	// read category_index
	int readed_category_index = 0;
	if (ZPR_MODE == mode) {
		readed_category_index = atoi(argv[5]);
	} else if (UM_MODE == mode) {
		readed_category_index = atoi(argv[4]);
	}
	if (readed_category_index < 0) {
		cerr << "category_index must be > 0" << endl;
		return EXIT_FAILURE;
	}
	unsigned category_index = static_cast<unsigned>(readed_category_index);

	// read mep_parameter - only in UM mode
	unsigned mep_parameter = 0;
	if (UM_MODE == mode) {
		int readed_mep_parameter = atoi(argv[5]);
		if (readed_mep_parameter < 0) {
			cerr << "mep_parameter must be > 0" << endl;
			return EXIT_FAILURE;
		}
		mep_parameter = static_cast<unsigned>(readed_mep_parameter);
	}

	// this values could be app parameters
	char separator[] = " ,";
	char missing_value_mark[] = "?";

	try {
		/* Building tree */
		vector<string> attr = vector<string>();
		string line = "";
		ifstream building_set_file(argv[2]);
		if (building_set_file.is_open()) {
			getline(building_set_file, line);
		} else
			throw string("Unable to open file: " + string(argv[2]));

		typedef tokenizer<char_separator<char> > tokenizer;
		char_separator<char> sep(separator);
		tokenizer tok(line, sep);
		for (tokenizer::iterator tok_iter = tok.begin(); tok_iter != tok.end();
				++tok_iter)
			attr.push_back(string(*tok_iter));

		cout << "Category name: " << attr[category_index] << endl;

		TrainingSet building_table = TrainingSet(attr, category_index,
				missing_value_mark);
		size_t examplesCount = building_table.readFromStream(building_set_file,
				separator);
		cout << "Number of readed examples to build: "
				<< examplesCount << endl;
		building_set_file.close();
		DecisionTree* dt = NULL;
		if (ZPR_MODE == mode)
			dt = new ZPRDecisionTree();
		else if (UM_MODE == mode)
			dt = new UMDecisionTree();
		dt->build(building_table);
		cout << "Built tree:" << endl;
		cout << *dt;

		/* Classifying */
		attr = vector<string>();
		char* p_to_classify_set_file = NULL;
		if (ZPR_MODE == mode) {
			p_to_classify_set_file = argv[4];
		} else if (UM_MODE == mode) {
			p_to_classify_set_file = argv[3];
		}
		line = "";
		ifstream to_classify_set_file(argv[3]);
		if (to_classify_set_file.is_open()) {
			getline(to_classify_set_file, line);
		} else
			throw string("Unable to open file: " + string(argv[3]));

		tok = tokenizer(line, sep);
		for (tokenizer::iterator tok_iter = tok.begin(); tok_iter != tok.end();
				++tok_iter)
			attr.push_back(string(*tok_iter));

		TrainingSet to_classify_table = TrainingSet(attr, category_index,
				missing_value_mark);
		examplesCount = to_classify_table.readFromStream(to_classify_set_file,
				separator);
		cout << "Number of readed examples to classify:"
				<< examplesCount << endl;
		to_classify_set_file.close();
		shared_ptr<vector<string> > result(dt->classify(to_classify_table));
		ErrorRate er = ErrorRate();
		er.count(result, to_classify_table);
		cout << "Error rate before pruning: " << er.get() << endl;

		/* Pruning tree */
		if (ZPR_MODE == mode) {
			attr = vector<string>();
			line = "";
			ifstream pruning_set_file(argv[3]);
			if (pruning_set_file.is_open()) {
				getline(pruning_set_file, line);
			} else
				throw string("Unable to open file: " + string(argv[3]));

			tok = tokenizer(line, sep);
			for (tokenizer::iterator tok_iter = tok.begin(); tok_iter != tok.end();
					++tok_iter)
				attr.push_back(string(*tok_iter));

			TrainingSet pruning_table = TrainingSet(attr, category_index,
					missing_value_mark);
			examplesCount = pruning_table.readFromStream(pruning_set_file,
					separator);
			cout << "Number of readed examples to prune:"
					<< examplesCount << endl;
			pruning_set_file.close();
			static_cast<ZPRDecisionTree*>(dt)->reducedErrorPruning(pruning_table);
		} else if (UM_MODE == mode) {
			cout << "MEP parameter = " << mep_parameter << endl;
			static_cast<UMDecisionTree*>(dt)->minimumErrorPruning(mep_parameter);
		}
		
		cout << "Pruned tree:" << endl;
		cout << *dt;

		shared_ptr<vector<string> > result2(dt->classify(to_classify_table));
		ErrorRate er2 = ErrorRate();
		er2.count(result2, to_classify_table);
		cout << "Error rate after pruning: " << er2.get() << endl;
		delete dt;
	} catch (std::string& err) {
		cerr << err << endl;
		return EXIT_FAILURE;
	} catch (std::exception& e) {
		cerr << e.what() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

