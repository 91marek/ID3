/*
 * main.cpp
 */

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <id3lib/DecisionTree.hpp>
#include <id3lib/ZPRDecisionTree.hpp>
#include <id3lib/UMDecisionTree.hpp>
#include <stdexcept>
#include <boost/tokenizer.hpp>

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

const int ZPR_MODE = 0;
const int UM_MODE = 1;

int main(int argc, char* argv[]) {
	if (argc != 5 && argc != 6) {
		cout << "ZPR usage:" << endl
				<< "building_set_file prunning_set_file to_classify_set_file category_index"
				<< endl << "UM usage:" << endl
				<< "building_set_file prunning_set_file to_classify_set_file category_index mep_parameter"
				<< endl;
		return EXIT_FAILURE;
	}

	int readed_category_index = atoi(argv[4]);
	if (readed_category_index < 0) {
		cerr << "category_index must be > 0" << endl;
		return EXIT_FAILURE;
	}
	unsigned category_index = static_cast<unsigned>(readed_category_index);

	int mode = ZPR_MODE;

	// UM mode
	unsigned mep_parameter = 0;
	if (6 == argc) {
		mode = UM_MODE;
		int readed_mep_parameter = atoi(argv[5]);
		if (readed_mep_parameter < 0) {
			cerr << "mep_parameter must be > 0" << endl;
			return EXIT_FAILURE;
		}
		mep_parameter = static_cast<unsigned>(readed_mep_parameter);
	}

	// domyslne wartosci
	char separator[] = " ,";
	char missing_mark[] = "?";

	try {
		/* Building tree */
		vector<string> attr = vector<string>();
		string line = "";
		ifstream building_set_file(argv[1]);
		if (building_set_file.is_open()) {
			getline(building_set_file, line);
		} else
			cout << "Unable to open file: " << argv[1] << endl;

		typedef tokenizer<char_separator<char> > tokenizer;
		char_separator<char> sep(separator);
		tokenizer tok(line, sep);
		for (tokenizer::iterator tok_iter = tok.begin(); tok_iter != tok.end();
				++tok_iter)
			attr.push_back(string(*tok_iter));

		TrainingSet building_table = TrainingSet(attr, category_index,
				missing_mark);
		size_t examplesCount = building_table.readFromStream(building_set_file,
				separator);
		cout << "Liczba wczytanych przykladow do budowy drzewa: "
				<< examplesCount << endl;
		building_set_file.close();
		DecisionTree* dt = NULL;
		if (ZPR_MODE == mode)
			dt = new ZPRDecisionTree();
		else
			dt = new UMDecisionTree();
		dt->build(building_table);
		cout << "Zbudowane drzewo:" << endl;
		cout << *dt;

		/* Classifying */
		attr = vector<string>();
		line = "";
		ifstream to_classify_set_file(argv[3]);
		if (to_classify_set_file.is_open()) {
			getline(to_classify_set_file, line);
		} else
			cout << "Unable to open file: " << argv[3] << endl;

		tok = tokenizer(line, sep);
		for (tokenizer::iterator tok_iter = tok.begin(); tok_iter != tok.end();
				++tok_iter)
			attr.push_back(string(*tok_iter));

		TrainingSet to_classify_table = TrainingSet(attr, category_index,
				missing_mark);
		examplesCount = to_classify_table.readFromStream(to_classify_set_file,
				separator);
		cout << "Liczba wczytanych przykladow do klasyfikacji: "
				<< examplesCount << endl;
		to_classify_set_file.close();
		shared_ptr<vector<string> > result(dt->classify(to_classify_table));
		ErrorRate er = ErrorRate();
		er.count(result, to_classify_table);
		cout << "Error rate: " << er.get() << endl;

		/* Prunning tree */
		attr = vector<string>();
		line = "";
		ifstream prunning_set_file(argv[2]);
		if (prunning_set_file.is_open()) {
			getline(prunning_set_file, line);
		} else
			cout << "Unable to open file: " << argv[2] << endl;

		tok = tokenizer(line, sep);
		for (tokenizer::iterator tok_iter = tok.begin(); tok_iter != tok.end();
				++tok_iter)
			attr.push_back(string(*tok_iter));

		TrainingSet prunning_table = TrainingSet(attr, category_index,
				missing_mark);
		examplesCount = prunning_table.readFromStream(prunning_set_file,
				separator);
		cout << "Liczba wczytanych przykladow do przyciecia drzewa: "
				<< examplesCount << endl;
		prunning_set_file.close();
		if (ZPR_MODE == mode)
			static_cast<ZPRDecisionTree*>(dt)->reducedErrorPruning(prunning_table);
		else
			static_cast<UMDecisionTree*>(dt)->minimumErrorPruning(mep_parameter);
		cout << "Przyciete drzewo:" << endl;
		cout << *dt;

		shared_ptr<vector<string> > result2(dt->classify(to_classify_table));
		ErrorRate er2 = ErrorRate();
		er2.count(result2, to_classify_table);
		cout << "Error rate: " << er2.get() << endl;
		delete dt;
	} catch (std::exception& e) {
		cerr << e.what() << endl;
	}

	return EXIT_SUCCESS;
}
