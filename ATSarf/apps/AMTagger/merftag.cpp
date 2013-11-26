#include "merftag.h"

MERFTag::MERFTag() : Match(NONE,NULL) {
    match = NULL;
}

MERFTag::MERFTag(MSFormula* formula, Source source) : Match(NONE, NULL) {
    match = NULL;
    this->formula = formula;
    this->source = source;
}

bool MERFTag::setMatch(Match *match) {
    this->match = match;
    return true;
}

bool MERFTag::isUnaryM() {
    return false;
}

bool MERFTag::isBinaryM() {
    return false;
}

bool MERFTag::isSequentialM() {
    return false;
}

bool MERFTag::isKeyM() {
    return false;
}

bool MERFTag::isMERFTag() {
    return true;
}

int MERFTag::getPOS() {
    return match->getPOS();
}

int MERFTag::getLength() {
    return match->getLength();
}

QString MERFTag::getText() {
    return match->getText();
}

int MERFTag::getMatchCount() {
    return match->getMatchCount();
}

void MERFTag::buildMatchTree(Agraph_t* G,Agnode_t* node,Agedge_t* edge,QMap<Agnode_t *,Agnode_t *>* parentNodeMap,QTreeWidgetItem* parentItem, int& id) {
    QStringList data;
    data << "Formula" << formula->name;
    QTreeWidgetItem* newItem = new QTreeWidgetItem(parentItem,data);
    parentItem = newItem;

    char * writable = strdup(formula->name.toStdString().c_str());
    if(node == NULL) {
        stringstream strs;
        strs << id;
        string temp_str = strs.str();
        char* nodeID = strdup(temp_str.c_str());
        Agnode_t * newNode = agnode(G,nodeID, 1);
        agset(G,const_cast<char *>("root"),nodeID);
        id = id+1;
        agset(newNode,const_cast<char *>("label"),writable);
        parentNodeMap->insert(newNode,NULL);
        node = newNode;
    }
    else {
        stringstream strs;
        strs << id;
        string temp_str = strs.str();
        char* nodeID = strdup(temp_str.c_str());
        Agnode_t * newNode = agnode(G,nodeID, 1);
        id = id+1;
        agset(newNode,const_cast<char *>("label"),writable);
        edge = agedge(G, node, newNode, 0, 1);
        parentNodeMap->insert(newNode, node);
        node = newNode;
    }
    match->buildMatchTree(G,node,edge,parentNodeMap,parentItem,id);
}

void MERFTag::executeActions(NFA* nfa) {
    MSFormula* formula = (MSFormula*)(nfa->formula);

    /** pre match **/
    QString preMatch = msf->name;
    preMatch.append("_preMatch();\n");
    formula->actionData.append(preMatch);
    /** Done **/

    match->executeActions(nfa);

    /** on match **/
    QString onMatch = msf->name;
    onMatch.append("_onMatch");
    QMultiMap<QString,QPair<QString,QString> > *functionParametersMap = &(formula->functionParametersMap);
    QList<QPair<QString,QString> > params = functionParametersMap->values(onMatch);
    onMatch.append('(');

    for(int j=0; j<params.count(); j++) {
        QString msfName = params.at(j).first;
        QString field = params.at(j).second;

        QString paramValue = getParam(msfName,field);
        if(field.compare("text") == 0) {
            onMatch.append('\"' + paramValue + "\",");
        }
        else if(field.compare("position") == 0) {
            onMatch.append(paramValue + ',');
        }
        else if(field.compare("length") == 0) {
            onMatch.append(paramValue + ',');
        }
        else if(field.compare("number") == 0) {
            onMatch.append(paramValue + ',');
        }
    }
    if(params.count() == 0) {
        onMatch.append(");\n");
    }
    else {
        onMatch.chop(1);
        onMatch.append(");\n");
    }
    formula->actionData.append(onMatch);
    /** Done **/
}

QString MERFTag::getParam(QString msfName,QString param) {
    if(msf->name == msfName) {
        if(param  == "text") {
            return getText();
        }
        else if(param == "position") {
            return QString::number(getPOS());
        }
        else if(param == "number") {
            QString text = getText();
            NumNorm nn(&text);
            nn();
            int number = NULL;
            if(nn.extractedNumbers.count()!=0) {
                number = nn.extractedNumbers[0].getNumber();
                return QString::number(number);
            }
            else {
                return "";
            }
        }
        else if(param == "length") {
            return QString::number(getLength());
        }
    }
    else {
        return match->getParam(msfName,param);
    }
    return "";
}

QVariantMap MERFTag::getJSON() {
    QVariantMap merftagMap;
    merftagMap.insert("type","merftag");
    merftagMap.insert("msf",msf->name);
    merftagMap.insert("formula",formula->name);
    merftagMap.insert("match",match->getJSON());
    return merftagMap;
}

MERFTag::~MERFTag() {
    /*
    if(match == NULL) {
        return;
    }
    else {
        delete match;
    }
    match = NULL;
    */
}