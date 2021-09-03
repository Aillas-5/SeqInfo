//////////////////////////////////////////////////////////////////////////////////
//      Note: This is version 2 and only works with the "new" revision of       //
//     regina_file.  If a file error occurs, check the regina_file version.     //
//     (The new version of regina_file has no "[" at the line beginnings.)      //
//                                                                              //
// This program is designed to display cumulative data for Aliquot sequences.   //
// The program relies on a file named "regina_file" which must be retrieved and //
// unpacked from this file: regina_file.tar.lzma.  The file can be retrieved    //
// from this site: http://www.aliquotes.com/aliquote_base.htm#alibasefonda      //
// The accuracy of this program is dependent on the currency of regina_file.    //
//                                                                              //
// To retrieve cycle information, the program has to access factordb.com, so a  //
// network connection is also necessary.  If no network is available, cycle     //
// data will show as blank.  wget must also be installed, which is commonly     //
// available on most linux platforms.                                           //
//                                                                              //
// A "results.txt" file can be used to capture the output of various data sets. //
// This file is never deleted or overwritten by the program.  It only appends   //
// new data to its end.  Therefore, it is necessary to manually remove the file //
// if old data is no longer wanted.                                             //
//                                                                              //
// Most functions are self-explanatory.  When a (y/n/f/c) prompt is offered,    //
// "f" means to send data to the results.txt file as well as the screen.  The   //
// formatting is slightly different between them.  The file output is displayed //
// with one element per line, while the screen output is displayed in columns.  //
// The screen display is based on a screen width of 0 (mod 10).  Any variation  //
// of columns not equal to 0 (mod 10) will not be aligned properly.  The        //
// default for (y/n/f/c) is n.  "c" only provides the total count.              //
//                                                                              //
// Compile with "g++ <filename> -o <program name>"                              //
//////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>


static std::set<double> s_perfectNumber{ 6.0, 28.0, 496.0, 8128.0, 33550336.0, 8589869056.0, 137438691328.0, 2305843008139952128.0, 2658455991569831744654692615953842176.0, 191561942608236107294793378084303638130997321548169216.0 };

struct reginaDataLine {
    int32_t elA;  // ** Not used - index equals sequence **
    int16_t elB;  // <0=cycle, 0=open ended, 1=prime
    int32_t elC;  //  number of terms at sequence end or 10^50
    double  elD;  // for elB=<0,0,1: equals cycle entry term, smallest sequence for merges, terminating prime
    int32_t elE;  // ** Not currently used - value computed by program **
    int32_t elF;  // number of digits for largest term reached (up to 10^50 for open-ended)
    int32_t elG;  // number of relative minimums
    int32_t elH;  // number of relative maximums
    int32_t elI;  // number of parity changes
    int32_t elJ;  // record number of consecutive even abundant terms
    int32_t elK;  // record number of consecutive even deficient terms
    int32_t elL;  // record number of consecutive odd deficient terms
    int32_t elM;  // record number of consecutive odd abundant terms
    int32_t elN;  // number of down driver extracts
    double  elO;  // smallest quotient of two consecutive terms
    double  elP;  // largest quotient of two consecutive terms
    double  elQ;  // arithmetic mean of all quotients of two consecutive terms
    double  elR;  // geometric mean of all quotients of two consecutive terms
    double  elS;  // arithmetic mean of the number of digits of all the minimums of the sequence
    double  elT;  // arithmetic mean of the number of digits of all the maximums of the sequence
    bool    ulist;  // used for advanced searches of results lists

    friend std::istream& operator >> (std::istream& is, reginaDataLine& seq) {
        char comma;
        seq.ulist = false;

        return is \
            >> seq.elA >> comma \
            >> seq.elB >> comma \
            >> seq.elC >> comma \
            >> seq.elD >> comma \
            >> seq.elE >> comma \
            >> seq.elF >> comma \
            >> seq.elG >> comma \
            >> seq.elH >> comma \
            >> seq.elI >> comma \
            >> seq.elJ >> comma \
            >> seq.elK >> comma \
            >> seq.elL >> comma \
            >> seq.elM >> comma \
            >> seq.elN >> comma \
            >> seq.elO >> comma \
            >> seq.elP >> comma \
            >> seq.elQ >> comma \
            >> seq.elR >> comma \
            >> seq.elS >> comma \
            >> seq.elT;
    }
};

struct reginaData {
public:
    std::vector<reginaDataLine> regina{};
    uint32_t ccount = 0;
    uint32_t ocount = 0;
    uint32_t pcount = 0;
    int32_t lastIndex = 0;
    int32_t lastIndexSize = 0;
    uint16_t indexShift = 2;

    reginaData() = default;

    reginaData(std::size_t reserveSize) {
        std::cout << "Reserving a regina vector of " << reserveSize << " elements" << std::endl;
        regina.reserve(reserveSize);
    }

    void reserve(std::size_t vectorSize) {
        regina.reserve(vectorSize);
        std::cout << "Reserving a regina vector of  " << vectorSize << " elements (capacity: " << regina.capacity() << ")" << std::endl;
    }

    void shrink() {
        regina.shrink_to_fit();
        std::cout << "Shrink regina vector to  " << regina.capacity() << " elements" << std::endl;
    }

    friend std::istream& operator >> (std::istream& is, reginaData& i) {
        std::for_each(std::istream_iterator<reginaDataLine>(is), {}, [&](const reginaDataLine& line) {
            if (line.elA % 100000 == 0) {
                double percent = line.elA * 100.0 / i.regina.capacity();
                std::cout << "\rLoad in Progress: " << std::fixed << std::setprecision(2) << percent << std::left << std::setw(3) << "%";
                fflush(stdout);
            }

            i.regina.emplace_back(line);

            if (line.elB > 0)
                i.pcount++;
            else if (line.elB == 0)
                i.ocount++;
            else if (line.elB < 0)
                i.ccount++;
            }
        );

        std::cout << std::endl;
        return is;
    }
};

std::string dToString(double num)
{
    std::ostringstream streamDouble;

    // Set Fixed -Point Notation + Set precision to 0 digits
    streamDouble << std::fixed << std::setprecision(0) << num;

    // Get string from output string stream
    return streamDouble.str();
}

// Routine to update all open-ended sequences
// This function does not change the original regina_file, but provides an
// additional file that updates the program data when it is run.
void createupdate(const struct reginaData& /*data*/) {
    std::cout << "Update file routine not available yet!" << std::endl;
}

// Routine to display help information
void helptext() {
    std::cout << "--Available options for the following prompts--" << std::endl;
    std::cout << "(##/a/h/p/p##/q/u): prompt" << std::endl;
    std::cout << "    ## displays info for sequence ## if it is within the range." << std::endl;
    std::cout << "    a brings up an advanced input routine." << std::endl;
    std::cout << "    h provides this text block." << std::endl;
    std::cout << "    p lists counts of all primes that terminate" << std::endl;
    std::cout << "      a sequence within the limit of regina_file." << std::endl;
    std::cout << "      This will take a long time to complete." << std::endl;
    std::cout << "      Due to the large return count, the list is" << std::endl;
    std::cout << "      sent to a primescount.txt file.  This file is" << std::endl;
    std::cout << "      overwritten with each run." << std::endl;
    std::cout << "    p## searches for sequences that terminate with the prime ##." << std::endl;
    std::cout << "    u (not available yet!) run a routine to make a file of updates." << std::endl;
    std::cout << "      The file OE_3000000_C80.txt must be available." << std::endl;
    std::cout << "      This will take a long time.  If the file exists" << std::endl;
    std::cout << "      it will be overwritten with a new file." << std::endl;
    std::cout << "(y/n/c/f): prompt" << std::endl;
    std::cout << "    y performs the procedure referenced." << std::endl;
    std::cout << "    n negative response. (default if an entry is omitted, except for Advanced)" << std::endl;
    std::cout << "    c provides a count only, without a listing." << std::endl;
    std::cout << "    f provides a listing to screen and to results.txt." << std::endl;
    std::cout << "      (results.txt is never removed by the program." << std::endl;
    std::cout << "      It is only appended to.  It has to be manually deleted.)" << std::endl;
}

// Routine to list and count all prime terminations for entire regina_file range
void primeslist(reginaData& data) {
    std::ofstream pout;
    char yn;
    std::map<double, uint32_t> primeCountMap;

    std::cout << "This process will take several hours.  Continue? (y/n):";
    std::cin >> yn;

    if (yn == 'y') {
        auto start = std::chrono::system_clock::now();

        const auto& vLines = data.regina;

        std::for_each(std::cbegin(vLines), std::cend(vLines), [&](const reginaDataLine& line) {
            if (line.elA % 100000 == 0) {
                uint64_t part = line.elA;
                std::cout << "  Progress: " << (part * 100) / vLines.size() << "%\r";
                fflush(stdout);
            }

            if (line.elB == 1)
                primeCountMap[line.elD] += 1;
            }
        );

        std::cout << "  Progress: 100%" << std::endl;

        pout.open("primescount.txt");

        if (pout.is_open()) {

            for (const auto& [prime, count] : primeCountMap)
                pout << prime << ": " << count << std::endl;

            pout.close();
            pout << primeCountMap.size() << " unique primes found!" << std::endl;
        }
        else
            std::cout << "primescount.txt did not open properly!" << std::endl;

        std::cout << primeCountMap.size() << " unique primes found!" << std::endl;

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Listing took " << int(elapsed.count() + .5) << " seconds to generate." << std::endl;
    }
}

template<typename T>
std::pair<T, T> getParamPair(const std::string& message, T minFirstTerm = 0, T default2Term = 10000, T mac2Term = std::numeric_limits<T>::max(), bool oneToMin = false) {
    std::string line;
    T firstTerm = std::numeric_limits<T>::max();
    T secondTerm = std::numeric_limits<T>::max();

    std::cout << message;

    std::cin.seekg(0, std::ios::end);
    std::cin.clear();

    std::getline(std::cin, line);
    std::stringstream ss(line);

    ss >> firstTerm >> secondTerm;

    if (ss.fail()) {
        // Read 0 or 1 int on the line
        if (firstTerm != std::numeric_limits<T>::max()) {
            // Read only one int
            secondTerm = firstTerm;

            if (oneToMin)
                firstTerm = minFirstTerm;
        }
        else {
            // No int on input. Use default value
            firstTerm = minFirstTerm;
            secondTerm = default2Term;
        }
    }

    if (firstTerm < minFirstTerm)
        firstTerm = minFirstTerm;

    if ((mac2Term != std::numeric_limits<T>::max()) && (secondTerm > mac2Term))
        secondTerm = mac2Term;

    return { firstTerm, secondTerm };
}

void advanced(const struct reginaData& data, bool uselist) {
    const auto& vLines = data.regina;
    char input;
    int16_t e;

    if (uselist)
        std::cout << "Advanced Routine using previous results" << std::endl;
    else
        std::cout << "Advanced Routine" << std::endl;

    size_t reginaSize = vLines.size() - 1;
    int32_t indexMax = std::numeric_limits<int32_t>::max();

    if (reginaSize < std::numeric_limits<int32_t>::max())
        indexMax = static_cast<int32_t>(reginaSize);
    else
        assert(false);  // Time to update seqinfo.

    const auto [a, b] = getParamPair<int32_t>("Sequence filter a (mod b) (a b): ", 0, 1, indexMax, true);
    const auto [c, d] = getParamPair<int32_t>("Sequence range (begin end): ", 2, indexMax, indexMax);

    std::cout << "All, Prime, Open, Cycle (a/p/o/c): ";
    std::cin >> input;
    switch (input)
    {
    case 'p': e = 1; break;
    case 'o': e = 0; break;
    case 'c': e = -1; break;
    default:  e = 2; break;
    }

    const auto [w, x] = getParamPair<int32_t>("Range for number of iterations (min max): ", 0, 3000);

    const auto [f, g] = getParamPair<int32_t>("Range for number of relative minimums (min max): ");
    const auto [h, j] = getParamPair<int32_t>("Range for number of relative maximums (min max): ");
    const auto [k, l] = getParamPair<int32_t>("Range for number of parity changes (min max): ");
    const auto [m, n] = getParamPair<int32_t>("Range for number of consecutive even abundant terms (min max): ");
    const auto [o, p] = getParamPair<int32_t>("Range for number of consecutive even deficient terms (min max): ");
    const auto [q, r] = getParamPair<int32_t>("Range for number of consecutive odd deficient terms (min max): ");
    const auto [s, t] = getParamPair<int32_t>("Range for number of consecutive odd abundant terms (min max): ");
    const auto [u, v] = getParamPair<int32_t>("Range for number of down driver extracts (min max): ");

    const auto [da, db] = getParamPair<double>("Range for smallest quotient of two consecutive terms (min max): ");
    const auto [dc, dd] = getParamPair<double>("Range for largest quotient of two consecutive terms (min max): ");
    const auto [de, df] = getParamPair<double>("Range for arith mean of all quotients of 2 consecutive terms (min max): ");
    const auto [dg, dh] = getParamPair<double>("Range for geo mean of all quotients of 2 consecutive terms (min max): ");
    const auto [di, dj] = getParamPair<double>("Range for arith mean of # of digits of all the mins (min max): ");
    const auto [dk, dl] = getParamPair<double>("Range for arith mean of # of digits of all the maxes (min max): ");

    std::cout << "List filtered sequences for " << a << " (mod " << b << ") from " << c << " through " << d << "? (y/n/c/f): ";

    std::ofstream resultFS;
    char yn;
    std::cin >> yn;

    if ((yn == 'n') || ((yn != 'y') && (yn != 'c') && (yn != 'f')))
        return;

    bool toFile = (yn == 'f');
    bool countOnly = (yn == 'c');

    if (toFile) {
        resultFS.open("results.txt", std::ios_base::app);
        toFile = resultFS.is_open();
    }

    if (toFile)
        resultFS << "All filtered sequences for " << a << " (mod " << b << ") from " << c << " through " << d << ":" << std::endl;

    uint32_t rcount = 0;
    bool crlf = false;
    const uint16_t spacing = data.lastIndexSize + 2;

    // vLines is a vector that contains sorted 'reginaDataLine', base on the index.
    // This index is contiguous, no hole.
    // So vLines[0] == first index of the regina file, should be 2, but to be agnosstic, when loading the regina file
    // the first index is saved in data.indexShift.
    // vLines[0].elA == data.indexShift or vLine[sequenceNum - data.indexShift].elA == sequenceNum

    // To test all sequences between c & d, we are going to compute the start & end iterator.
    int32_t firstIndex = c - data.indexShift;
    int32_t lastIndex = d - data.indexShift + 1;

    std::for_each(std::next(std::cbegin(vLines), firstIndex), std::next(std::begin(vLines), lastIndex), [&](const reginaDataLine& line) {

        bool show = (a == line.elA % b);

        if (!show)
            return;

        show = !((uselist == true && line.ulist == false) ||
            ((e == -1) && (line.elB > -1)) ||
            ((e != line.elB) && (e != 2)) ||
            ((line.elC < w) || (line.elC > x)) ||
            ((line.elG < f) || (line.elG > g)) ||
            ((line.elH < h) || (line.elH > j)) ||
            ((line.elI < k) || (line.elI > l)) ||
            ((line.elJ < m) || (line.elJ > n)) ||
            ((line.elK < o) || (line.elK > p)) ||
            ((line.elL < q) || (line.elL > r)) ||
            ((line.elM < s) || (line.elM > t)) ||
            ((line.elN < u) || (line.elN > v)) ||
            ((line.elO < da) || (line.elO > db)) ||
            ((line.elP < dc) || (line.elP > dd)) ||
            ((line.elQ < de) || (line.elQ > df)) ||
            ((line.elR < dg) || (line.elR > dh)) ||
            ((line.elS < di) || (line.elS > dj)) ||
            ((line.elT < dk) || (line.elT > dl))
            );

        if (show) {
            std::string temp;

            if (toFile)
                resultFS << line.elA << std::endl;

            if (!countOnly)
                std::cout << std::right << std::setw(spacing) << line.elA;

            rcount++;
            crlf = true;
        }
        }
    );

    if (toFile)
        resultFS.close();

    if (crlf == true && !countOnly)
        std::cout << std::endl;

    if (rcount != 1)
        std::cout << rcount << " sequences found." << std::endl;
    else
        std::cout << rcount << " sequence found." << std::endl;
}

// This routine finds all sequences that terminate with the supplied prime
void primesfind(reginaData& data, double primeToSearch) {
    char yn;
    std::ofstream resultFS;
    uint32_t rcount;
    auto& vLines = data.regina;

    std::cout << "List all sequences that terminate with " << primeToSearch << "? (y/n/c/f): ";
    std::cin >> yn;

    if ((yn == 'n') || ((yn != 'y') && (yn != 'c') && (yn != 'f')))
        return;

    bool toFile = (yn == 'f');
    bool countOnly = (yn == 'c');

    if (toFile) {
        resultFS.open("results.txt", std::ios_base::app);
        toFile = resultFS.is_open();
    }

    if (toFile)
        resultFS << "All sequences that terminate with " << primeToSearch << ":" << std::endl;

    rcount = 0;
    const uint16_t spacing = data.lastIndexSize + 2;

    std::for_each(std::begin(vLines), std::end(vLines), [&](reginaDataLine& line) {
        if (line.elD == primeToSearch) {
            if (toFile)
                resultFS << line.elA << std::endl;

            if (!countOnly)
                std::cout << std::right << std::setw(spacing) << line.elA;

            line.ulist = true;
            rcount++;
        }
        else
            line.ulist = false;
        }
    );

    if (toFile)
        resultFS.close();

    if (!countOnly)
        std::cout << std::endl;

    if (rcount > 1) {
        std::cout << rcount << " sequences found." << std::endl;
        std::cout << "Perform Advanced Filtering on these results? (y/n): ";
        std::cin >> yn;

        if (yn == 'y')
            advanced(data, true);
    }
    else
        std::cout << rcount << " sequence found." << std::endl;
}

std::size_t roundUp(std::size_t numToRound, std::size_t multiple)
{
    return ((numToRound + multiple - 1) / multiple) * multiple;
}

std::size_t getEstimateVectorSizeFromFileSize(std::string filename)
{
    std::size_t vectorSize = 20000000;
    std::error_code error;

    std::uintmax_t fileSize = std::filesystem::file_size(filename, error);

    if (!error) {
        fileSize /= 110;
        vectorSize = roundUp(fileSize, 1000000);
    }

    return vectorSize;
}

void createnewhugefile(const std::string& reginaFileName) {
    std::ifstream reginaStream;
    std::ofstream hugeF;
    uint64_t globalIndex = 2;
    std::string line;

    reginaStream.open(reginaFileName);

    if (reginaStream.is_open()) {
        std::string buff;

        hugeF.open("megaregina180M.txt");

        if (hugeF.is_open()) {
            while (getline(reginaStream, line))
            {
                std::string substr;
                std::stringstream ss(line);

                // skip index;
                getline(ss, substr, ',');
                getline(ss, substr);

                for (uint32_t i = 0; i < 10; i++) {
                    hugeF << globalIndex++ << "," << substr << std::endl;

                    if (globalIndex % 100000 == 0) {
                        uint64_t part = globalIndex * 100;
                        part /= 180000000;
                        std::cout << "globalIndex %: " << part << "%\r";
                        fflush(stdout);
                    }
                }
            }

            hugeF.close();
        }

        reginaStream.close();
    }
}

bool checkRegineFileFormat(const std::string& reginaFileName) {
    std::ifstream reginaStream;

    std::cout << "Verifying " << reginaFileName << "...";
    fflush(stdout);

    reginaStream.open(reginaFileName);

    if (reginaStream.is_open()) {
        std::string buff;

        std::getline(reginaStream, buff);
        reginaStream.close();

        if (buff.find('[') != std::string::npos) {
            std::cout << " FAILED !!" << std::endl;
            std::cout << "This program does not work with the original regina_file!" << std::endl;
            std::cout << "Please use the new one. An alternate filename can be added" << std::endl;
            std::cout << "to the command vLines to call it instead of regina_file." << std::endl;
            return false;
        }
    }
    else {
        std::cout << " FAILED !!" << std::endl;
        std::cout << "Could not find file " << reginaFileName << std::endl;
        return false;
    }

    std::cout << " Done successfully" << std::endl;

    return true;
}

bool loadReginaFile(const std::string& reginaFileName, reginaData& data) {
    std::ifstream reginaStream;
    std::size_t vectorSize = getEstimateVectorSizeFromFileSize(reginaFileName);

    data.reserve(vectorSize);

    reginaStream.open(reginaFileName);

    if (reginaStream.is_open()) {
        std::cout << "Loading " << reginaFileName << "..." << std::endl;

        auto start = std::chrono::system_clock::now();

        reginaStream >> data;

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Loading regina file took " << int(elapsed.count() + .5) << " seconds." << std::endl;

        if (!reginaStream.eof()) {
            std::cout << "Loading data failed!" << std::endl;
            std::cout << "Data available for sequences 2 through " << data.regina.back().elA << std::endl;
            std::cout << "Data size: " << data.regina.size() << std::endl;

            reginaStream.close();
            return false;
        }

        std::cout << "Loading data done successfully" << std::endl;

        data.indexShift = data.regina.front().elA;
        data.lastIndex = data.regina.back().elA;
        data.lastIndexSize = static_cast<uint16_t>(floor(std::log10(data.lastIndex) + 1));

        std::cout << "Data available for sequences " << data.indexShift << " through " << data.lastIndex << std::endl;
        std::cout << "Sequence endings - prime: " << data.pcount << ", cycle: " << data.ccount << ", open: " << data.ocount << std::endl;
    }

    reginaStream.close();

    // Needed ??
    // data.shrink();

    return true;
}

void managePrimeCommand(reginaData& data, const std::string& commandStr) {
    std::string tmp;
    bool found;

    if (commandStr.length() > 1) {
        tmp.assign(commandStr.substr(1));
        double primeToSearch = std::stod(tmp);
        const auto& vLines = data.regina;

        found = std::any_of(std::cbegin(vLines), std::cend(vLines), [&](const reginaDataLine& line) {
            return ((line.elB == 1) && (line.elD == primeToSearch));
            }
        );

        if (found)
            primesfind(data, primeToSearch);
        else
            std::cout << "No sequences found that terminate with " << tmp << "." << std::endl;

    }
    else
        primeslist(data);
}

void manageOpenEndedSequence(reginaData& data, int32_t sequenceNumber, int32_t sequenceIndex) {
    char yn;
    std::ofstream resultFs;
    uint32_t rcount;
    auto& vLines = data.regina;
    double seqPrime = vLines[sequenceIndex].elD;

    std::cout << sequenceNumber << " is open ended. ";

    if (sequenceNumber != seqPrime) {
        std::cout << "It merges with " << seqPrime << "." << std::endl;
        return;
    }

    std::cout << "List any sequences that merge with " << sequenceNumber << "? (y/n/c/f): ";
    std::cin >> yn;

    if ((yn == 'n') || ((yn != 'y') && (yn != 'c') && (yn != 'f')))
        return;

    bool toFile = (yn == 'f');
    bool countOnly = (yn == 'c');

    if (toFile) {
        resultFs.open("results.txt", std::ios_base::app);
        toFile = resultFs.is_open();
    }

    if (toFile)
        resultFs << "All sequences that merge with " << seqPrime << ":" << std::endl;

    rcount = 0;
    const uint16_t spacing = data.lastIndexSize + 2;

    std::for_each(std::next(std::begin(vLines), ++sequenceIndex), std::end(vLines), [&](reginaDataLine& line) {
        if (line.elD == seqPrime) {
            if (toFile)
                resultFs << line.elA << std::endl;

            if (!countOnly)
                std::cout << std::right << std::setw(spacing) << line.elA;

            line.ulist = true;
            rcount++;
        }
        else
            line.ulist = false;
        }
    );

    if (toFile)
        resultFs.close();

    if (!countOnly)
        std::cout << std::endl;

    if (rcount > 1) {
        std::cout << rcount << " sequences found." << std::endl;
        std::cout << "Perform Advanced Filtering on these results? (y/n): ";
        std::cin >> yn;

        if (yn == 'y')
            advanced(data, true);
    }
    else if (rcount == 1)
        std::cout << " One sequence found." << std::endl;
    else
        std::cout << "No merges found." << std::endl;
}

void LoadCycleSequence(const std::string primeStr, std::unordered_set<double>& cycleSet) {
    std::ifstream cycleFs;
    std::string line;

    std::string commandStr = R"(curl -q -s -o cycle.tmp "http://www.factordb.com/elf.php?seq=)" + primeStr + R"(&type=1")";
    system(commandStr.c_str());

    cycleFs.open("cycle.tmp");

    if (cycleFs.is_open()) {
        while (std::getline(cycleFs, line)) {
            size_t foundp = line.find('.');
            size_t founde = line.find('=', foundp + 4);
            std::string composite = line.substr(foundp + 4, founde - (1 + foundp + 4));
            // std::(unordered_)set doesn't have duplicated. Ne need later to reduce index for listing without dup.
            (void)cycleSet.emplace(std::stod(composite));
        }

        cycleFs.close();
    }
    else
        std::cout << "An error was encountered trying to read cycle.tmp!" << std::endl;
}

void managexxx(reginaData& data, const uint32_t sequenceNumber, const uint32_t sequenceIndex, std::unordered_set<double>& cycleSet)
{
    std::ofstream resultFs;
    char yn;
    auto& vLines = data.regina;
    double seqPrime = vLines[sequenceIndex].elD;
    std::string seqPrimeStr = dToString(seqPrime);

    std::cin >> yn;

    if ((yn == 'n') || ((yn != 'y') && (yn != 'c') && (yn != 'f')))
        return;

    bool toFile = (yn == 'f');
    bool countOnly = (yn == 'c');

    if (toFile) {
        resultFs.open("results.txt", std::ios_base::app);
        toFile = resultFs.is_open();
    }

    if (toFile) {
        if (s_perfectNumber.contains(seqPrime))
            resultFs << "All sequences that end with the perfect number " << seqPrimeStr << ":" << std::endl;
        else
            resultFs << "All sequences that end within the same cycle as " << seqPrimeStr << ":" << std::endl;
    }

    if (cycleSet.empty())
        LoadCycleSequence(seqPrimeStr, cycleSet);

    uint32_t rcount = 0;
    const uint16_t spacing = data.lastIndexSize + 2;

    std::for_each(std::next(std::begin(vLines), data.indexShift), std::end(vLines), [&](reginaDataLine& line) {
        line.ulist = false;
        if (cycleSet.contains(line.elD))
        {
            if (toFile)
                resultFs << line.elA << std::endl;

            if (!countOnly)
                std::cout << std::right << std::setw(spacing) << line.elA;

            line.ulist = true;
            rcount++;
        }
        }
    );

    if (toFile)
        resultFs.close();

    if (!countOnly)
        std::cout << std::endl;

    if (rcount > 1) {
        std::cout << rcount << " sequences found." << std::endl;
        std::cout << "Perform Advanced Filtering on these results? (y/n): ";
        std::cin >> yn;

        if (yn == 'y')
            advanced(data, true);
    }
    else
        std::cout << rcount << " sequence found." << std::endl;
}

bool  manageCycleNumber(reginaData& data, const uint32_t sequenceNumber, const uint32_t sequenceIndex, std::unordered_set<double>& cycleSet) {
    std::ofstream resultFs;
    std::ifstream  cycleFs;
    std::string line;
    char yn;
    auto& vLines = data.regina;

    std::cout << sequenceNumber << " ends in a cycle. Display cycle? (starts at entry point) (y/n/f): ";

    std::cin >> yn;

    if ((yn == 'n') || ((yn != 'y') && (yn != 'f')))
        return false;

    bool toFile = (yn == 'f');

    if (toFile) {
        resultFs.open("results.txt", std::ios_base::app);
        toFile = resultFs.is_open();
    }

    if (toFile)
        resultFs << sequenceNumber << " ends with the following cycle:" << std::endl;

    double seqPrime = vLines[sequenceIndex].elD;
    std::string seqPrimeStr = dToString(seqPrime);

    LoadCycleSequence(seqPrimeStr, cycleSet);

    // std::set is ordered and doesn't have duplicated. No need to reduce the parsing by 2.
    std::for_each(std::cbegin(cycleSet), std::cend(cycleSet), [&](const double composite) {
        std::string compositeStr = dToString(composite);
        if (toFile)
            resultFs << compositeStr << std::endl;

        std::cout << compositeStr << std::endl;
        }
    );

    if (toFile)
        resultFs.close();

    std::cout << "Display all sequences that end in this cycle? (y/n/c/f): ";

    return true;
}


bool manageEndWithPerfectNumber(reginaData& data, const uint32_t sequenceNumber, const uint32_t sequenceIndex) {
    auto& vLines = data.regina;
    std::string primeStr = dToString(vLines[sequenceIndex].elD);

    std::cout << sequenceNumber << " ends with the perfect number " << primeStr << "," << std::endl;
    std::cout << "Display all sequences that end with " << primeStr << "? (y/n/c/f): ";

    return true;
}

bool managePerfectNumber(reginaData& data, const uint32_t sequenceNumber, const uint32_t sequenceIndex) {
    auto& vLines = data.regina;

    std::cout << sequenceNumber << " is a perfect number." << std::endl;
    std::cout << "Display all sequences that end with " << dToString(vLines[sequenceIndex].elD) << "? (y/n/c/f): ";

    return true;
}

void manageSequence(reginaData& data, const std::string& sequenceNumberStr) {
    std::unordered_set<double> cycleSet;
    auto& vLines = data.regina;

    int32_t sequenceNumber = std::stol(sequenceNumberStr);
    int32_t sequenceIndex = sequenceNumber - data.indexShift;

    if (sequenceNumber < data.indexShift || sequenceNumber > data.lastIndex) {
        std::cout << "Value entered is outside current bounds of " << data.indexShift << " through " << data.lastIndex << std::endl;
        return;
    }

    bool noError{ false };
    if (vLines[sequenceIndex].elB == 1) {
        std::cout << sequenceNumber << " terminates with prime " << dToString(vLines[sequenceIndex].elD) << "." << std::endl;
        primesfind(data, vLines[sequenceIndex].elD);
    }
    else if (vLines[sequenceIndex].elB == 0) {
        manageOpenEndedSequence(data, sequenceNumber, sequenceIndex);
    }
    else if (vLines[sequenceIndex].elB < 0) {
        if (s_perfectNumber.contains(vLines[sequenceIndex].elD)) {
            if (s_perfectNumber.contains(sequenceNumber))
                noError = managePerfectNumber(data, sequenceNumber, sequenceIndex);
            else
                noError = manageEndWithPerfectNumber(data, sequenceNumber, sequenceIndex);
        }
        else
            noError = manageCycleNumber(data, sequenceNumber, sequenceIndex, cycleSet);

        if (noError)
            managexxx(data, sequenceNumber, sequenceIndex, cycleSet);
    }
    else
        std::cout << "Sequence appears to be incomplete!" << std::endl;
}

int main(int argv, char* argc[])
{
    reginaData reginaDataFile;
    std::string infile;

    std::ofstream out;
    std::string  cycles[40], sequence, temp, yn;

    if (argv > 1)
        infile.assign(argc[1]);
    else
        infile.assign("regina_file");

    if (!checkRegineFileFormat(infile))
        return 0;

    //createnewhugefile(infile);
    //return 0;

    if (!loadReginaFile(infile, reginaDataFile))
        return 0;

    do {
        std::string commandStr;

        std::cout << "Enter sequence (##/a/h/p/p##/q/u): ";

        std::cin.seekg(0, std::ios::end);
        std::cin.clear();

        std::getline(std::cin, commandStr);

        if (commandStr.empty())
            continue;

        switch (commandStr[0]) {
        case 'q': return 0;

        case 'h': helptext(); break;

        case 'a': advanced(reginaDataFile, false); break;

        case 'u': createupdate(reginaDataFile); break;

        case 'p': managePrimeCommand(reginaDataFile, commandStr); break;

        default: manageSequence(reginaDataFile, commandStr); break;
        }

    } while (true);

    std::cout << "Program closed!" << std::endl;

    return 0;
}
