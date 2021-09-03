# SeqInfo
This program is designed to display cumulative data for Aliquot sequences.

This program is designed to read and works on data provided by Jean-Luc Garambois on his website [here](http://www.aliquotes.com/aliquote_base.htm#alibasefonda).
You can download the file by clicking here: [regina file](http://www.aliquotes.com/regina_file.tar.lzma).

Each line of the file represents a sequence and is written like this :

[A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T]

|Col|Description|
|--|--|
|**A**|starting integer of the aliquot sequence.|
|**B**|relative integer which is 1 if the aliquot sequence from start A ends up at 1, which is 0 if the sequence is open-end, which is negative if the sequence falls on a cycle of length |B|, with |B| being the absolute value of B.|
|**C**|number of iterations at the end of the computation of the sequence. We stop the calculation if we end up with 1, if we find a cycle or if the terms are greater than 10^50 and we consider it as an open-end sequence (or if the sequence meets a term of 25 digits of an open-end sequence already calculated previously, to decrease the calculation time).|
|**D**|if B is 1, D is the prime number on which the starting sequence A fell. If B is 0, D is the smallest starting integer of the open-end sequence merged by the starting sequence A. If B is negative, D is the integer belonging to the cycle by which the starting sequence A entered this cycle.|
|**E**|if B is 1, E tells for how many other sequences one has already fallen on the prime number D, including the starting sequence A. If B is 0, E tells for how many other sequences one has already fallen on the open-end beginning with D, including the starting sequence A. If B is negative, E tells for how many other sequences one has already fallen on the same cycle, including the starting sequence A.|
|**F**|number of digits of the largest term of the starting sequence A.|
|**G**|number of relative minimums of the starting sequence A.|
|**H**|number of relative maximums encountered in the starting sequence A.|
|**I**|number of parity changes encountered in the starting sequence A.|
|**J**|record number of consecutive even abundant terms in the starting sequence A.|
|**K**|record number of consecutive even deficient terms in the starting sequence A.|
|**L**|record number of consecutive odd deficient terms in the starting sequence A.|
|**M**|record number of consecutive odd abundant terms in the starting sequence A.|
|**N**|number of downdriver extracts in the sequence : terms of the form 2^1 * cofactor with 3 which does not divide cofactor.|
|**O**|smallest quotient of two consecutive terms su+1(A)/su(A) found in the whole starting sequence A, except the last quotient if B=1 and A not prime (if A is prime, the only quotient is the last quotient and it has no sense, because the last but one term of the aliquot sequence is prime and so the last quotient can be very small if this prime number is large).|
|**P**|largest quotient of two consecutive terms su+1(A)/su(A) found in the whole starting sequence A.|
|**Q**|arithmetic mean of su+1(A)/su(A), that is to say arithmetic mean of all the quotients of two consecutive terms of the starting sequence A, except for the last quotient if B=1 (this last quotient is the unique one if A is prime and in this last case, we count it anyway). We make the sum of all su+1(A)/su(A), 0<=u<=C, and we divide it by the number C; or by C-1 if B=1 and if A is not prime.|
|**R**|geometric mean of su+1(A)/su(A), i.e. geometric mean of all the quotients of two consecutive terms of the starting sequence A, except for the last quotient if B=1 (this last quotient is the only one if A is prime and in this last case, it is counted anyway). We make the product of all su+1(A)/su(A) with 0<=u<=C, that comes back to consider finally only the quotient of the last term of the sequence by the first term, and we raise this exponent to the power 1/C ; or by C-1 if B=1 and if A is not prime. Be careful with the entry in the cycles : here, we only count all the members of the cycle once. But if we want another geometric mean, we can find it with the other parameters of the line. For example, we can want the geometric mean of the sequence without the cycle, or by counting the number of entries in the cycle twice...|
|**S**|arithmetic mean of the number of digits of all the minimums of the sequence.|
|**T**|arithmetic mean of the number of digits of all the maximums of the sequence.|
