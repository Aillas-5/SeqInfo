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
#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;

// structure for the first elements
struct seqdata{
//    int elA;  // ** Not used - index equals sequence **
    int elB;  // <0=cycle, 0=open ended, 1=prime
    int elC;  //  number of terms at sequence end or 10^50
    string elD;  // for elB=<0,0,1: equals cycle entry term, smallest sequence for merges, terminating prime
    int elE;  // ** Not currently used - value computed by program **
    int elF;  // number of digits for largest term reached (up to 10^50 for open-ended)
    int elG;  // number of relative minimums
    int elH;  // number of relative maximums
    int elI;  // number of parity changes
    int elJ;  // record number of consecutive even abundant terms
    int elK;  // record number of consecutive even deficient terms
    int elL;  // record number of consecutive odd deficient terms
    int elM;  // record number of consecutive odd abundant terms
    int elN;  // number of downdriver extracts
    bool ulist;  // used for advanced searches of results lists
};

// structure for the precision elements - only used because the final couple of elements
// overran the other structure's size capability
struct seqdatad{
    double elO;  // smallest quotient of two consecutive terms
    double elP;  // largest quotient of two consecutive terms
    double elQ;  // arithmetic mean of all quotients of two consecutive terms
    double elR;  // geometric mean of all quotients of two consecutive terms
    double elS;  // arithmetic mean of the number of digits of all the minimums of the sequence
    double elT;  // arithmetic mean of the number of digits of all the maximums of the sequence
};

// Routine to update all open-ended sequences
// This function does not change the original regina_file, but provides an
// additional file that updates the program data when it is run.
void createupdate(struct seqdata seqd[]){
    cout << "Update file routine not available yet!" << endl;

    return;
}

// Routine to display help information
void helptext(){
    cout << "--Available options for the following prompts--" << endl;
    cout << "(##/a/h/p/p##/q/u): prompt" << endl;
    cout << "    ## displays info for sequence ## if it is within the range." << endl;
    cout << "    a brings up an advanced input routine." << endl;
    cout << "    h provides this text block." << endl;
    cout << "    p lists counts of all primes that terminate" << endl;
    cout << "      a sequence within the limit of regina_file." << endl;
    cout << "      This will take a long time to complete." << endl;
    cout << "      Due to the large return count, the list is" << endl;
    cout << "      sent to a primescount.txt file.  This file is" << endl;
    cout << "      overwritten with each run." << endl;
    cout << "    p## searches for sequences that terminate with the prime ##." << endl;
    cout << "    u (not available yet!) run a routine to make a file of updates." << endl;
    cout << "      The file OE_3000000_C80.txt must be available." << endl;
    cout << "      This will take a long time.  If the file exists" << endl;
    cout << "      it will be overwritten with a new file." << endl;
    cout << "(y/n/c/f): prompt" << endl;
    cout << "    y performs the procedure referenced." << endl;
    cout << "    n negative response. (default if an entry is omitted, except for Advanced)" << endl;
    cout << "    c provides a count only, without a listing." << endl;
    cout << "    f provides a listing to screen and to results.txt." << endl;
    cout << "      (results.txt is never removed by the program." << endl;
    cout << "      It is only appended to.  It has to be manually deleted.)" << endl;
    return;
}

// Routine to list and count all prime terminations for entire regina_file range
void primeslist(struct seqdata seqd[], int seqscount){
    ofstream pout;
    static string plist[2000000];
    static int plcount[2000000];
    string ptemp, yn;
    int check, pcount=0, i, j, ptempi;
    bool exists;

    cout << "This process will take several hours.  Continue? (y/n):";
    getline (cin, yn);
    if(yn.substr(0.1)=="y"){
        auto start = std::chrono::system_clock::now();

        for(i=2;i<seqscount;i++){
            if(i%1000==0){
                cout << "  Progress: " << (i*100)/seqscount << "%\r";
                fflush(stdout);
            }
            if(seqd[i].elB==1){
                exists=false;
                for(j=0;j<pcount;j++){
                    if(seqd[i].elD==plist[j]){
                        exists=true;
                        plcount[j]++;
                    }
                }
                if(exists==false){
                    plist[pcount]=seqd[i].elD;
                    plcount[pcount]=1;
                    pcount++;
                }
            }
        }

        for(i=0;i<pcount-1;i++){
            for(j=1;j<pcount;j++){
                check=plist[j-1].compare(plist[j]);
                if(check>0){
                    ptemp.assign(plist[j-1]);
                    plist[j-1].assign(plist[j]);
                    plist[j].assign(ptemp);
                    ptempi=plcount[j-1];
                    plcount[j-1]=plcount[j];
                    plcount[j]=ptempi;
                }
                if(plist[j-1].length()>plist[j].length()){
                    ptemp.assign(plist[j-1]);
                    plist[j-1].assign(plist[j]);
                    plist[j].assign(ptemp);
                    ptempi=plcount[j-1];
                    plcount[j-1]=plcount[j];
                    plcount[j]=ptempi;
                }
            }
        }

        pout.open("primescount.txt");
        if(pout.is_open()){
            for(i=0;i<pcount;i++){
                pout << plist[i] << ": " << plcount[i] << endl;
            }
            pout.close();
            pout << pcount << " unique primes found!" << endl;
        }
        else
            cout << "primescount.txt did not open properly!" << endl;

        cout << pcount << " unique primes found!" << endl;

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        cout << "Listing took " << int(elapsed.count()+.5) << " seconds to generate." << endl;

    }

    return;
}

// Routine to perform advanced searches of data
void advanced(struct seqdata seqd[], struct seqdatad seqdd[], int seqscount, bool uselist){
    ofstream aout;
    string sf1, sf2, sf3, temp, yn;
    char *dp;
    size_t found;
    int i, a, b, c, d, e, f, g, h, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, rcount;
    double da, db, dc, dd, de, df, dg, dh, di, dj, dk, dl;
    bool crlf, show;

    if(uselist==true)
        cout << "Advanced Routine using previous results" << endl;
    else
        cout << "Advanced Routine" << endl;
    cout << "Sequence filter a (mod b) (a b): ";
    getline(cin, temp);
    if(temp.length()>0){
        found=temp.find(" ");
        if(found!=string::npos){
            a=atoi(temp.substr(0,found).c_str());
            b=atoi(temp.substr(found+1).c_str());
        }
        else{
            a=0;
            b=atoi(temp.c_str());
        }
    }
    else{
        a=0;
        b=1;
    }
    if(a<0)
        a=0;
    if(b>seqscount-1)
        b=seqscount-1;
    cout << "Sequence range (begin end): ";
    getline(cin, temp);
    if(temp.length()>0){
        found=temp.find(" ");
        if(found!=string::npos){
            c=atoi(temp.substr(0,found).c_str());
            d=atoi(temp.substr(found+1).c_str());
        }
        else{
            c=atoi(temp.c_str());
            d=c;
        }
    }
    else{
        c=2;
        d=seqscount-1;
    }
    if(c<2)
        c=2;
    if(d>seqscount-1)
        d=seqscount-1;
    cout << "All, Prime, Open, Cylce (a/p/o/c): ";
    getline(cin, temp);
    if(temp.substr(0,1)=="p")
        e=1;
    else if(temp.substr(0,1)=="o")
        e=0;
    else if(temp.substr(0,1)=="c")
        e=-1;
    else
        e=2;
    cout << "Range for number of iterations (min max): ";
    getline(cin, temp);
    if(temp.length()>0){
        found=temp.find(" ");
        if(found!=string::npos){
            w=atoi(temp.substr(0,found).c_str());
            x=atoi(temp.substr(found+1).c_str());
        }
        else{
            w=atoi(temp.c_str());
            x=w;
        }
    }
    else{
        w=0;
        x=30000;
    }
    if(w<0)
        w=0;
    cout << "Range for number of relative minimums (min max): ";
    getline(cin, temp);
    if(temp.length()>0){
        found=temp.find(" ");
        if(found!=string::npos){
            f=atoi(temp.substr(0,found).c_str());
            g=atoi(temp.substr(found+1).c_str());
        }
        else{
            f=atoi(temp.c_str());
            g=f;
        }
    }
    else{
        f=0;
        g=10000;
    }
    if(f<0)
        f=0;
    cout << "Range for number of relative maximums (min max): ";
    getline(cin, temp);
    if(temp.length()>0){
        found=temp.find(" ");
        if(found!=string::npos){
            h=atoi(temp.substr(0,found).c_str());
            j=atoi(temp.substr(found+1).c_str());
        }
        else{
            h=atoi(temp.c_str());
            j=h;
        }
    }
    else{
        h=0;
        j=10000;
    }
    if(h<0)
        h=0;
    cout << "Range for number of parity changes (min max): ";
    getline(cin, temp);
    if(temp.length()>0){
        found=temp.find(" ");
        if(found!=string::npos){
            k=atoi(temp.substr(0,found).c_str());
            l=atoi(temp.substr(found+1).c_str());
        }
        else{
            k=atoi(temp.c_str());
            l=k;
        }
    }
    else{
        k=0;
        l=10000;
    }
    if(k<0)
        k=0;
    cout << "Range for number of consecutive even abundant terms (min max): ";
    getline(cin, temp);
    if(temp.length()>0){
        found=temp.find(" ");
        if(found!=string::npos){
            m=atoi(temp.substr(0,found).c_str());
            n=atoi(temp.substr(found+1).c_str());
        }
        else{
            m=atoi(temp.c_str());
            n=m;
        }
    }
    else{
        m=0;
        n=10000;
    }
    if(m<0)
        m=0;
    cout << "Range for number of consecutive even deficient terms (min max): ";
    getline(cin, temp);
    if(temp.length()>0){
        found=temp.find(" ");
        if(found!=string::npos){
            o=atoi(temp.substr(0,found).c_str());
            p=atoi(temp.substr(found+1).c_str());
        }
        else{
            o=atoi(temp.c_str());
            p=o;
        }
    }
    else{
        o=0;
        p=10000;
    }
    if(o<0)
        o=0;
    cout << "Range for number of consecutive odd deficient terms (min max): ";
    getline(cin, temp);
    if(temp.length()>0){
        found=temp.find(" ");
        if(found!=string::npos){
            q=atoi(temp.substr(0,found).c_str());
            r=atoi(temp.substr(found+1).c_str());
        }
        else{
            q=atoi(temp.c_str());
            r=q;
        }
    }
    else{
        q=0;
        r=10000;
    }
    if(q<0)
        q=0;
    cout << "Range for number of consecutive odd abundant terms (min max): ";
    getline(cin, temp);
    if(temp.length()>0){
        found=temp.find(" ");
        if(found!=string::npos){
            s=atoi(temp.substr(0,found).c_str());
            t=atoi(temp.substr(found+1).c_str());
        }
        else{
            s=atoi(temp.c_str());
            t=s;
        }
    }
    else{
        s=0;
        t=10000;
    }
    if(s<0)
        s=0;
    cout << "Range for number of downdriver extracts (min max): ";
    getline(cin, temp);
    if(temp.length()>0){
        found=temp.find(" ");
        if(found!=string::npos){
            u=atoi(temp.substr(0,found).c_str());
            v=atoi(temp.substr(found+1).c_str());
        }
        else{
            u=atoi(temp.c_str());
            v=s;
        }
    }
    else{
        u=0;
        v=10000;
    }
    if(u<0)
        u=0;
    cout << "Range for smallest quotient of two consecutive terms (min max): ";
    getline(cin, temp);
    if(temp.length()>0){
        found=temp.find(" ");
        if(found!=string::npos){
            da=strtod(temp.substr(0,found).c_str(), &dp);
            db=strtod(temp.substr(found+1).c_str(), &dp);
        }
        else{
            da=strtod(temp.c_str(), &dp);
            db=da;
        }
    }
    else{
        da=0;
        db=10000;
    }
    if(da<0)
        da=0;
    cout << "Range for largest quotient of two consecutive terms (min max): ";
    getline(cin, temp);
    if(temp.length()>0){
        found=temp.find(" ");
        if(found!=string::npos){
            dc=strtod(temp.substr(0,found).c_str(), &dp);
            dd=strtod(temp.substr(found+1).c_str(), &dp);
        }
        else{
            dc=strtod(temp.c_str(), &dp);
            dd=dc;
        }
    }
    else{
        dc=0;
        dd=10000;
    }
    if(dc<0)
        dc=0;
    cout << "Range for arith mean of all quotients of 2 consequtive terms (min max): ";
    getline(cin, temp);
    if(temp.length()>0){
        found=temp.find(" ");
        if(found!=string::npos){
            de=strtod(temp.substr(0,found).c_str(), &dp);
            df=strtod(temp.substr(found+1).c_str(), &dp);
        }
        else{
            de=strtod(temp.c_str(), &dp);
            df=de;
        }
    }
    else{
        de=0;
        df=10000;
    }
    if(de<0)
        de=0;
    cout << "Range for geo mean of all quotients of 2 consequtive terms (min max): ";
    getline(cin, temp);
    if(temp.length()>0){
        found=temp.find(" ");
        if(found!=string::npos){
            dg=strtod(temp.substr(0,found).c_str(), &dp);
            dh=strtod(temp.substr(found+1).c_str(), &dp);
        }
        else{
            dg=strtod(temp.c_str(), &dp);
            dh=dg;
        }
    }
    else{
        dg=0;
        dh=10000;
    }
    if(dg<0)
        dg=0;
    cout << "Range for arith mean of # of digits of all the mins (min max): ";
    getline(cin, temp);
    if(temp.length()>0){
        found=temp.find(" ");
        if(found!=string::npos){
            di=strtod(temp.substr(0,found).c_str(), &dp);
            dj=strtod(temp.substr(found+1).c_str(), &dp);
        }
        else{
            di=strtod(temp.c_str(), &dp);
            dj=di;
        }
    }
    else{
        di=0;
        dj=10000;
    }
    if(di<0)
        di=0;
    cout << "Range for arith mean of # of digits of all the maxes (min max): ";
    getline(cin, temp);
    if(temp.length()>0){
        found=temp.find(" ");
        if(found!=string::npos){
            dk=strtod(temp.substr(0,found).c_str(), &dp);
            dl=strtod(temp.substr(found+1).c_str(), &dp);
        }
        else{
            dk=strtod(temp.c_str(), &dp);
            dl=dk;
        }
    }
    else{
        dk=0;
        dl=10000;
    }
    if(dk<0)
        dk=0;

    cout << "List filtered sequences for " << a << " (mod " << b << ") from " << c << " through " << d << "? (y/n/c/f): ";
    getline(cin, yn);
    if(yn.substr(0,1)=="n")
        return;
    if(yn.substr(0,1)=="y" || yn.substr(0,1)=="f" || yn.substr(0,1)=="c"){
        if(yn.substr(0,1)=="f"){
            aout.open("results.txt", std::ios_base::app);
            aout << "All filtered sequences for " << a << " (mod " << b << ") from " << c << " through " << d << ":" << endl;
        }
    }
    rcount=0;
    crlf=false;
    for(i=c;i<=d;i++){
        show=true;
        if(a!=i%b){
            show=false;
        }
        if(show==true && uselist==true && seqd[i].ulist==false)
            show=false;
        if(show==true && e==-1 && seqd[i].elB>-1)
            show=false;
        if(show==true && e!=seqd[i].elB && e!=2)
            show=false;
        if(show==true && (seqd[i].elC<w || seqd[i].elC>x))
            show=false;
        if(show==true && (seqd[i].elG<f || seqd[i].elG>g))
            show=false;
        if(show==true && (seqd[i].elH<h || seqd[i].elH>j))
            show=false;
        if(show==true && (seqd[i].elI<k || seqd[i].elI>l))
            show=false;
        if(show==true && (seqd[i].elJ<m || seqd[i].elJ>n))
            show=false;
        if(show==true && (seqd[i].elK<o || seqd[i].elK>p))
            show=false;
        if(show==true && (seqd[i].elL<q || seqd[i].elL>r))
            show=false;
        if(show==true && (seqd[i].elM<s || seqd[i].elM>t))
            show=false;
        if(show==true && (seqd[i].elN<u || seqd[i].elN>v))
            show=false;
        if(show==true && (seqdd[i].elO<da || seqdd[i].elO>db))
            show=false;
        if(show==true && (seqdd[i].elP<dc || seqdd[i].elP>dd))
            show=false;
        if(show==true && (seqdd[i].elQ<de || seqdd[i].elQ>df))
            show=false;
        if(show==true && (seqdd[i].elR<dg || seqdd[i].elR>dh))
            show=false;
        if(show==true && (seqdd[i].elS<di || seqdd[i].elS>dj))
            show=false;
        if(show==true && (seqdd[i].elT<dk || seqdd[i].elT>dl))
            show=false;
        if(show==true){
            if(aout.is_open())
                aout << i << endl;
            temp.assign(to_string(i));
            temp.append("          ");
            temp=temp.substr(0,10);
            if(yn.substr(0,1)!="c")
                cout << temp;
            rcount++;
            crlf=true;
        }
    }
    if(aout.is_open())
        aout.close();
    if(crlf==true && yn.substr(0,1)!="c")
        cout << endl;
    if(rcount!=1)
        cout << rcount << " sequences found." << endl;
    else
        cout << rcount << " sequence found." << endl;

    return;

}

// This routine finds all sequences that terminate with the supplied prime
void primesfind(struct seqdata seqd[], struct seqdatad seqdd[], int seqscount, int seqn){
    string yn, temp;
    ofstream out;
    int j, rcount;
    bool uselist;

    cout << "List all sequences that terminate with " << seqd[seqn].elD << "? (y/n/c/f): ";
    getline(cin, yn);
    if(yn.substr(0,1)=="y" || yn.substr(0,1)=="f" || yn.substr(0,1)=="c"){
        if(yn.substr(0,1)=="f"){
            out.open("results.txt", std::ios_base::app);
            out << "All sequences that terminate with " << seqd[seqn].elD << ":" << endl;
        }
        rcount=0;
        for(j=2;j<seqscount;j++){
            if(seqd[j].elD==seqd[seqn].elD){
                if(out.is_open())
                    out << j << endl;
                temp.assign(to_string(j));
                temp.append("          ");
                temp=temp.substr(0,10);
                if(yn!="c")
                    cout << temp;
                seqd[j].ulist=true;
                rcount++;
            }
            else
                seqd[j].ulist=false;
        }
        if(out.is_open())
            out.close();
        if(yn!="c")
            cout << endl;
        if(rcount>1){
            cout << rcount << " sequences found." << endl;
            cout << "Perform Advanced Filtering on these results? (y/n): ";
            getline(cin, yn);
            if(yn.substr(0,1)=="y"){
                uselist=true;
                advanced(seqd, seqdd, seqscount, uselist);
            }
        }
        else
            cout << rcount << " sequence found." << endl;
    }
    return;
}

int main(int argv, char *argc[])
{
    ifstream in, cyc;
    ofstream out;
    static struct seqdata seqd[20000001];
    static struct seqdatad seqdd[20000001];
    string buff, cycles[40], infile, seqq, sequence, temp, yn;
    char *p;
    int c, i, j, k, ccount=0, ocount=0, pcount=0, rcount, seqn, seqscount=2;
    size_t found, founde, foundp;
    bool none, uselist;

    if(argv>1)
        infile.assign(argc[1]);
    else
        infile.assign("regina_file");

    cout << "Reading " << infile << " . . .";
    fflush(stdout);

    in.open(infile);
    if(in.is_open()){
        getline(in, buff);
        in.close();
        found=buff.find("[");
        if(found!=string::npos){
            cout << "\rThis program does not work with the original regina_file!" << endl;
            cout << "Please use the new one. An alternate filename can be added" << endl;
            cout << "to the command line to call it instead of regina_file." << endl;
            return 0;
        }
    }

    in.open(infile);
    if(in.is_open()){
        while(!in.eof() && seqscount<20000001){
            getline(in, buff);
            // elA not used
            found=buff.find(",");
            if(found!=string::npos)
                buff.assign(buff.substr(found+1));
            // Assign elB
            found=buff.find(",");
            if(found!=string::npos){
                sscanf(buff.substr(0,found).c_str(), "%d", &seqd[seqscount].elB);
                buff.assign(buff.substr(found+1));
                if(seqd[seqscount].elB>0)
                    pcount++;
                if(seqd[seqscount].elB==0)
                    ocount++;
                if(seqd[seqscount].elB<0)
                    ccount++;
            }
            // Assign elC
            found=buff.find(",");
            if(found!=string::npos){
                sscanf(buff.substr(0,found).c_str(), "%d", &seqd[seqscount].elC);
                buff.assign(buff.substr(found+1));
            }
            // Assign elD
            found=buff.find(",");
            if(found!=string::npos){
                seqd[seqscount].elD.assign(buff.substr(0,found));
                buff.assign(buff.substr(found+1));
            }
            // Assign elE
            found=buff.find(",");
            if(found!=string::npos){
                sscanf(buff.substr(0,found).c_str(), "%d", &seqd[seqscount].elE);
                buff.assign(buff.substr(found+1));
            }
            // Assign elF
            found=buff.find(",");
            if(found!=string::npos){
                sscanf(buff.substr(0,found).c_str(), "%d", &seqd[seqscount].elF);
                buff.assign(buff.substr(found+1));
            }
            // Assign elG
            found=buff.find(",");
            if(found!=string::npos){
                sscanf(buff.substr(0,found).c_str(), "%d", &seqd[seqscount].elG);
                buff.assign(buff.substr(found+1));
            }
            // Assign elH
            found=buff.find(",");
            if(found!=string::npos){
                sscanf(buff.substr(0,found).c_str(), "%d", &seqd[seqscount].elH);
                buff.assign(buff.substr(found+1));
            }
            // Assign elI
            found=buff.find(",");
            if(found!=string::npos){
                sscanf(buff.substr(0,found).c_str(), "%d", &seqd[seqscount].elI);
                buff.assign(buff.substr(found+1));
            }
            // Assign elJ
            found=buff.find(",");
            if(found!=string::npos){
                sscanf(buff.substr(0,found).c_str(), "%d", &seqd[seqscount].elJ);
                buff.assign(buff.substr(found+1));
            }
            // Assign elK
            found=buff.find(",");
            if(found!=string::npos){
                sscanf(buff.substr(0,found).c_str(), "%d", &seqd[seqscount].elK);
                buff.assign(buff.substr(found+1));
            }
            // Assign elL
            found=buff.find(",");
            if(found!=string::npos){
                sscanf(buff.substr(0,found).c_str(), "%d", &seqd[seqscount].elL);
                buff.assign(buff.substr(found+1));
            }
            // Assign elM
            found=buff.find(",");
            if(found!=string::npos){
                sscanf(buff.substr(0,found).c_str(), "%d", &seqd[seqscount].elM);
                buff.assign(buff.substr(found+1));
            }
            // Assign elN
            found=buff.find(",");
            if(found!=string::npos){
                sscanf(buff.substr(0,found).c_str(), "%d", &seqd[seqscount].elN);
                buff.assign(buff.substr(found+1));
            }
            // Assign elO
            found=buff.find(",");
            if(found!=string::npos){
                seqdd[seqscount].elO=strtod(buff.substr(0,found).c_str(), &p);
                buff.assign(buff.substr(found+1));
            }
            // Assign elP
            found=buff.find(",");
            if(found!=string::npos){
                seqdd[seqscount].elP=strtod(buff.substr(0,found).c_str(), &p);
                buff.assign(buff.substr(found+1));
            }
            // Assign elQ
            found=buff.find(",");
            if(found!=string::npos){
                seqdd[seqscount].elQ=strtod(buff.substr(0,found).c_str(), &p);
                buff.assign(buff.substr(found+1));
            }
            // Assign elR
            found=buff.find(",");
            if(found!=string::npos){
                seqdd[seqscount].elR=strtod(buff.substr(0,found).c_str(), &p);
                buff.assign(buff.substr(found+1));
            }
            // Assign elS
            found=buff.find(",");
            if(found!=string::npos){
                seqdd[seqscount].elS=strtod(buff.substr(0,found).c_str(), &p);
                buff.assign(buff.substr(found+1));
            }
            // Assign elT
            found=buff.find(",");
            if(found!=string::npos){
                seqdd[seqscount].elT=strtod(buff.substr(0,found).c_str(), &p);
                buff.assign(buff.substr(found+1));
            }
            seqd[seqscount].ulist=false;
            seqscount++;
        }
        seqscount-=1;
    }
    else{
        cout << "\rError reading " << infile << "!!" << endl;
        return 0;
    }

    cout << "\rData available for sequences 2 through " << seqscount-1 << endl;
    cout << "Sequence endings - prime: " << pcount << ", cycle: " << ccount << ", open: " << ocount << endl;

    do{
        cout << "Enter sequence (##/a/h/p/p##/q/u): ";
        getline(cin, seqq);
        if(seqq.substr(0,1)=="q")
            return 0;
        if(seqq.substr(0,1)=="h")
            helptext();
        else if(seqq.substr(0,1)=="a"){
            uselist=false;
            advanced(seqd, seqdd, seqscount, uselist);
        }
        else if(seqq.substr(0,1)=="u")
            createupdate(seqd);
        else if(seqq.substr(0,1)=="p")
            if(seqq.length()>1){
                temp.assign(seqq.substr(1));
                none=true;
                for(i=2;i<seqscount;i++){
                    if(temp==seqd[i].elD && seqd[i].elB==1){
                        none=false;
                        primesfind(seqd, seqdd, seqscount, i);
                        break;
                    }
                }
                if(none==true)
                    cout << "No sequences found that terminate with " << temp << "." << endl;
            }
            else
                primeslist(seqd, seqscount);
        else{
            seqn=atoi(seqq.c_str());
            if(seqn>1 && seqn<seqscount){
                if(seqd[seqn].elB==1){
                    cout << seqn << " terminates with prime " << seqd[seqn].elD << "." << endl;
                    primesfind(seqd, seqdd, seqscount, seqn);
                }
                else if(seqd[seqn].elB==0){
                    cout << seqn << " is open ended. ";
                    if(to_string(seqn)==seqd[seqn].elD){
                        cout << "List any sequences that merge with " << seqn << "? (y/n/c/f): ";
                        getline(cin, yn);
                        if(yn.substr(0,1)=="y" || yn.substr(0,1)=="f" || yn.substr(0,1)=="c"){
                            if(yn.substr(0,1)=="f"){
                                out.open("results.txt", std::ios_base::app);
                                out << "All sequences that merge with " << seqd[seqn].elD << ":" << endl;
                            }
                            rcount=0;
                            for(j=seqn+1;j<seqscount;j++){
                                if(seqd[j].elD==seqd[seqn].elD){
                                    if(out.is_open())
                                        out << j << endl;
                                    temp.assign(to_string(j));
                                    temp.append("          ");
                                    temp=temp.substr(0,10);
                                    if(yn!="c")
                                        cout << temp;
                                    seqd[j].ulist=true;
                                    rcount++;
                                }
                                else
                                    seqd[j].ulist=false;
                            }
                            if(out.is_open())
                                out.close();
                            if(yn!="c")
                                cout << endl;
                            if(rcount>1){
                                cout << rcount << " sequences found." << endl;
                                cout << "Perform Advanced Filtering on these results? (y/n): ";
                                getline(cin, yn);
                                if(yn.substr(0,1)=="y"){
                                    uselist=true;
                                    advanced(seqd, seqdd, seqscount, uselist);
                                }
                            }
                            else if(rcount==1)
                                cout << rcount << " sequence found." << endl;
                            else
                                cout << "No merges found." << endl;
                        }
                    }
                    else
                        cout << "It merges with " << seqd[seqn].elD << "." << endl;
                }
                else if(seqd[seqn].elB<0){
                    c=0;
                    if(seqd[seqn].elD=="6" || seqd[seqn].elD=="28" || seqd[seqn].elD=="496" || seqd[seqn].elD=="8128" || seqd[seqn].elD=="33550336" || seqd[seqn].elD=="8589869056" || seqd[seqn].elD=="137438691328" || seqd[seqn].elD=="2305843008139952128" || seqd[seqn].elD=="2658455991569831744654692615953842176" || seqd[seqn].elD=="191561942608236107294793378084303638130997321548169216"){
                        if(to_string(seqn)=="6" || to_string(seqn)=="28" || to_string(seqn)=="496" || to_string(seqn)=="8128" || to_string(seqn)=="33550336"){
                            cout << seqn << " is a perfect number." << endl;
                            cout << "Display all sequences that end with " << seqd[seqn].elD << "? (y/n/c/f): ";
                        }
                        else{
                            cout << seqn << " ends with the perfect number " << seqd[seqn].elD << "," << endl;
                            cout << "Display all sequences that end with " << seqd[seqn].elD << "? (y/n/c/f): ";
                        }
                    }
                    else{
                        cout << seqn << " ends in a cycle. Display cycle? (starts at entry point) (y/n/f): ";
                        getline(cin, yn);
                        if(yn.substr(0,1)=="y" || yn.substr(0,1)=="f"){
                            if(yn.substr(0,1)=="f"){
                                out.open("results.txt", std::ios_base::app);
                                out << seqn << " ends with the following cycle:" << endl;
                            }
                            temp.assign("wget \"http://www.factordb.com/elf.php?seq=");
                            temp.append(seqd[seqn].elD);
                            temp.append("&type=1\" -q -O cycle.tmp");
                            system(temp.c_str());
                            cyc.open("cycle.tmp");
                            if(cyc.is_open()){
                                while(!cyc.eof()){
                                    getline(cyc, buff);
                                    foundp=buff.find(".");
                                    founde=buff.find("=");
                                    if(founde!=string::npos && foundp!=string::npos){
                                        cycles[c].assign(buff.substr(foundp+4,founde-foundp-5));
                                        c++;
                                    }
                                }
                                cyc.close();
                                c-=2;
                                for(j=0;j<c;j++){
                                    if(out.is_open())
                                        out << cycles[j] << endl;
                                    cout << cycles[j] << endl;
                                }
                                if(out.is_open())
                                    out.close();
                            }
                            else
                                cout << "An error was encountered trying to read cycle.tmp!" << endl;
                        }
                        cout << "Display all sequences that end in this cycle? (y/n/c/f): ";
                    }
                    getline(cin, yn);
                    if(yn.substr(0,1)=="y" || yn.substr(0,1)=="f" || yn.substr(0,1)=="c"){
                        if(yn.substr(0,1)=="f"){
                            out.open("results.txt", std::ios_base::app);
                            if(seqd[seqn].elD=="6" || seqd[seqn].elD=="28" || seqd[seqn].elD=="496" || seqd[seqn].elD=="8128" || seqd[seqn].elD=="33550336" || seqd[seqn].elD=="8589869056" || seqd[seqn].elD=="137438691328" || seqd[seqn].elD=="2305843008139952128")
                                out << "All sequences that end with the perfect number " << seqd[seqn].elD << ":" << endl;
                            else
                                out << "All sequences that end within the same cycle as " << seqd[seqn].elD << ":" << endl;
                        }
                        rcount=0;
                        if(c==0){
                            temp.assign("wget \"http://www.factordb.com/elf.php?seq=");
                            temp.append(seqd[seqn].elD);
                            temp.append("&type=1\" -q -O cycle.tmp");
                            system(temp.c_str());
                            cyc.open("cycle.tmp");
                            if(cyc.is_open()){
                                while(!cyc.eof()){
                                    getline(cyc, buff);
                                    foundp=buff.find(".");
                                    founde=buff.find("=");
                                    if(founde!=string::npos && foundp!=string::npos){
                                        cycles[c].assign(buff.substr(foundp+4,founde-foundp-5));
                                        c++;
                                    }
                                }
                                cyc.close();
                                c-=2;
                            }
                            else
                                cout << "An error was encountered trying to read cycle.tmp!" << endl;
                        }
                        for(j=2;j<seqscount;j++){
                            seqd[j].ulist=false;
                            for(k=0;k<c;k++)
                                if(cycles[k]==seqd[j].elD){
                                    if(out.is_open())
                                        out << j << endl;
                                    temp.assign(to_string(j));
                                    temp.append("          ");
                                    temp=temp.substr(0,10);
                                    if(yn!="c")
                                        cout << temp;
                                    seqd[j].ulist=true;
                                    rcount++;
                                }
                        }
                        if(out.is_open())
                            out.close();
                        if(yn!="c")
                            cout << endl;
                        if(rcount>1 || rcount<1)
                            cout << rcount << " sequences found." << endl;
                        if(rcount>1){
                            cout << "Perform Advanced Filtering on these results? (y/n): ";
                            getline(cin, yn);
                            if(yn.substr(0,1)=="y"){
                                uselist=true;
                                advanced(seqd, seqdd, seqscount, uselist);
                            }
                        }
                        else
                            cout << rcount << " sequence found." << endl;
                    }
                }
                else
                    cout << "Sequence appears to be incomplete!" << endl;
            }
            else
                cout << "Value entered is outside current bounds of 2 through " << seqscount-1 << endl;;
        }

    }while(seqq.substr(0,1)!="q");


    cout << "Program closed!" << endl;
    return 0;
}
