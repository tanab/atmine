#ifndef _LETTERS_H
#define	_LETTERS_H

#include <QChar>
#include <QVector>
#include "common.h"

//constantletters
static const QChar hamza=QChar(0x0621);
static const QChar ya2=QChar(0x064A);
static const QChar alef=QChar(0x0627);
static const QChar alef_madda_above= QChar(0x0622);
static const QChar alef_hamza_above= QChar(0x0623);
static const QChar alef_hamza_below= QChar(0x0625);
static const QChar alef_wasla=QChar(0x0671);
static const QChar ta2_marbouta=QChar(0x0629);
static const QChar waw=QChar(0x0648);
static const QChar waw_hamza_above=QChar(0x0624);
static const QChar shadde=QChar(0x0651);
static const QChar shadde_alef_above=QChar(0xFC63);
static const QChar fatha=QChar(0x064E);
static const QChar damma=QChar(0x064F);
static const QChar kasra=QChar(0x0650);
static const QChar sukun=QChar(0x0652);
static const QChar lam=QChar(0x0644);
static const QChar kasratayn=QChar(0x064D);
static const QChar dammatayn=QChar(0x064C);
static const QChar fathatayn=QChar(0x064B);
static const QChar kha2=QChar(0x062E);
static const QChar ba2=QChar(0x0628);
static const QChar ra2=QChar(0x0631);
static const QChar noon=QChar(0x0646);
static const QChar seen=QChar(0x0633);
static const QChar meem=QChar(0x0645);
static const QChar _3yn=QChar(0x0639);
static const QChar ta2=QChar(0x062A);
static const QChar qaf=QChar(0x0642);
static const QChar _7a2=QChar(0x062D);
static const QChar dal=QChar(0x062F);
static const QChar tha2=QChar(0x062B);
static const QChar aleft_superscript=QChar(0x0670);
static const QChar ha2 =QChar(0x0647);
static const QChar veh =QChar(0x06A4);
static const QChar feh =QChar(0x0641);
static const QChar madda =QChar(0x0653);
static const QChar zain =QChar(0x0632);
static const QChar thal =QChar(0x0630);
static const QChar alef_maksoura= QChar(0x0649);

static const QChar question_mark=QChar(0x061F);
static const QChar semicolon=QChar(0x061B);
static const QChar fasila=QChar(0x060C);
static const QChar full_stop=QChar(0x06D4);
static const QChar full_stop2=QChar(0x200F);
static const QChar full_stop3=QChar(0x002E);

static const QString delimiters=QString("[ :\\.,()-><{}\\/|'\"\n")+fasila+question_mark+semicolon+full_stop+full_stop2+full_stop3+"]";
static const QString punctuation=QString(":\\.,()-'\n\"")+fasila+question_mark+semicolon+full_stop+full_stop2+full_stop3;
static const QString alefs=QString("")+alef+alef_hamza_above+alef_hamza_below+alef_madda_above+alef_wasla;
static const QString nonconnectingLetters=QString("")+alef+alef_hamza_above+alef_hamza_below+alef_madda_above+alef_wasla+aleft_superscript+
		waw_hamza_above+waw+hamza+zain+ra2+thal+dal+ta2_marbouta+alef_maksoura;

inline bool isDelimiter(const QChar & letter)
{
	return delimiters.contains(letter);
}

inline bool isPunctuationMark(const QChar & letter)
{
	return punctuation.contains(letter);
}

inline bool isNonConnectingLetter(const QChar & letter)
{
	return nonconnectingLetters.contains(letter);
}

#endif	/* _LETTERS_H */

