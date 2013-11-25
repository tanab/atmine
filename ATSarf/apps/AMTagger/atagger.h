#ifndef ATAGGER_H
#define ATAGGER_H

#include <QVector>
#include <QByteArray>
#include <QtAlgorithms>
#include <QMultiHash>
#include "tag.h"
#include "merftag.h"
#include "tagtype.h"
#include "sarftagtype.h"
#include "msformula.h"
#include "nfa.h"
#include "binarym.h"
#include "unarym.h"
#include "sequentialm.h"
#include "keym.h"
#include "commonS.h"

class ATagger;

class ATagger {
public:
    ATagger();
    ~ATagger();
    bool insertTag(const TagType*, int, int, int, Source, Dest);
    bool insertTagType(QString, QString, QString, QString, int, bool, bool, bool, Source, Dest);
    bool insertSarfTagType(QString, QVector < Quadruple< QString , QString , QString , QString > > , QString, QString, QString, int, bool, bool, bool, Source, Dest);
    QByteArray dataInJsonFormat(Data _data);
    bool buildNFA();
    bool buildActionFile();
    bool runSimulator();
    Match* simulateNFA(NFA* nfa, QString state, int wordIndex);
    void executeActions(NFA* nfa);
    void drawNFA();
    void updateMatch(Match* match,NFA* nfa, QString state, const Tag* tag=NULL);
    QMultiHash<int,Tag> tagHash;
    QMultiHash<int,Tag> compareToTagHash;
    QVector<Match*> simulationVector;
    QVector<TagType*> *tagTypeVector;
    QVector<TagType*> *compareToTagTypeVector;
    QVector<MSFormula*> *msfVector;
    QVector<MSFormula*> *tempMSFVector;
    QVector<NFA*> *nfaVector;
    QString text;
    QString textFile;
    QString tagFile;
    QString compareToTagFile;
    QString tagtypeFile;
    QString compareToTagTypeFile;
    bool isSarf;
    bool compareToIsSarf;
    /// This boolean keeps track of whether the current tags are MBF based or MSF based
    bool isTagMBF;
    /// Hash to keep track of word index based on position
    QHash<int,int> wordIndexMap;
    /// Counter to save word count in text
    int wordCount;
};

#endif // ATAGGER_H
