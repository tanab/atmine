#ifndef MERFTAG_H
#define MERFTAG_H

#include <QVector>
#include "match.h"
#include "tag.h"
#include "msformula.h"

class MERFTag : public Match
{
public:
    MERFTag();
    MERFTag(MSFormula* formula, Source source=sarf);
    bool setMatch(Match* match);
    bool isUnaryM();
    bool isBinaryM();
    bool isSequentialM();
    bool isKeyM();
    bool isMERFTag();
    int getPOS();
    int getLength();
    QString getText();
    int getMatchCount();
    void buildMatchTree(Agraph_t* G,Agnode_t* node,Agedge_t* edge,QMap<Agnode_t *,Agnode_t *>* parentNodeMap,QTreeWidgetItem* parentItem, int& id);
    void executeActions(NFA* nfa);
    QString getParam(QString msfName,QString param);
    QVariantMap getJSON();
    MSFormula* formula;
    int pos;
    int length;
    Source source;
    Match* match;
    ~MERFTag();
};

#endif // MERFTAG_H