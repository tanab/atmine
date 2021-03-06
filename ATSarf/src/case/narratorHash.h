#ifndef NARRATORHASH_H
#define NARRATORHASH_H

#include "hadithCommon.h"
#include <QtCore/qdatastream.h>
#include <QDataStream>
#include <QMultiHash>
#include <QVector>
#include "graph_nodes.h"


class NarratorGraph;

class NarratorHash {
    public:
        class Action {
            public:
                virtual bool isFoundAction() {
                    return false;
                }
        };
        class FoundAction: public Action {
            public:
                virtual void action(const QString &searchKey, GroupNode *node, double similarity) = 0;
                bool isFoundAction() {
                    return true;
                }
        };
        class BiographyAction: public Action {
            public:
                virtual void action(const QString &searchKey, GroupNode *node, int position, double similarity) = 0;
        };

        class HashValue {
            public:
                HashValue(GroupNode *node, int value, int total) {
                    this->node = node;
                    this->value = value;
                    this->total = total;
                    index = -1;
                }
                HashValue(GroupNode *node, int index, int value, int total) {
                    this->node = node;
                    this->value = value;
                    this->total = total;
                    this->index = index;
                }
                bool operator==(const HashValue &rhs) {
                    if (node == rhs.node) {
                        assert(value == rhs.value && total == rhs.total && index == rhs.index);
                        return true;
                    }

                    return false;
                }

                GroupNode *node;
                int value, total;
                int index;
        };
        typedef QMultiHash<QString, HashValue> HashTable;
        typedef Narrator::NarratorPrimList NarratorPrimList;
        typedef Narrator::NarratorPrimHierarchy NarratorPrimHierarchy;
        typedef Narrator::PossessiveList PossessiveList;

        inline static QString getKey(const NarratorPrimHierarchy &hierarchy, const PossessiveList &possessives, int size,
                                     bool poss, bool skipFirst) {
            QString key = "";

            for (int i = 0; i < size; i++) {
                if (!skipFirst || i != 0) {
                    const NarratorPrimList &list = hierarchy[i];
                    int levelSize = list.size();

                    for (int j = 0; j < levelSize; j++) {
                        NarratorPrim *prim = list[j];

                        if (!prim->isNamePrim()) {
                            NameConnectorPrim *connector = (NameConnectorPrim *)prim;

                            if (connector->isAB()) {
                                key.append("AB");
                            } else if (connector->isOM()) {
                                key.append("OM");
                            } else {
                                key.append(connector->getString());
                            }
                        } else {
                            QString s = prim->getString();

                            if (s.size() > 0 && alefs.contains(s[0])) {
                                s[0] = alef;
                            }

                            key.append(s);
                        }

                        if (j != levelSize - 1) {
                            key.append(" ");
                        }
                    }
                }

                if (i == size - 1) {
                    if (poss) {
                        int possSize = possessives.size();

                        if (possSize > 0) {
                            key.append(":");

                            for (int j = 0; j < possSize; j++) {
                                key.append(possessives[j]->getString());

                                if (j != possSize - 1) {
                                    key.append(" ");
                                }
                            }
                        }
                    }
                } else {
                    key.append("-");
                }
            }

            return key;
        }
        inline static bool isFirstNameAmbiguous(QString key) {
            if (key.size() == 0) {
                return false;
            }

            return key[0] == '-';
        }

    private:
        HashTable hashTable;
        NarratorGraph *graph;

        class Visitor {
            public:
                virtual void initialize(GraphNodeItem *) { }
                virtual void visit(const QString &s, GraphNodeItem *c, int value, int total) = 0;
        };
        class InsertVisitor: public Visitor {
            private:
                int index;
                NarratorHash *hash;
            public:
                InsertVisitor(NarratorHash *hash, int index = -1) {
                    this->hash = hash;
                    this->index = index;
                }
                void initialize(GraphNodeItem *node) {
                    assert(node->size() > 0);
                    GroupNode *n = dynamic_cast<GroupNode *>(node);

                    if (index < 0) {
                        n->allKeys.clear();
                    } else {
                        assert(n->allKeys.size() > 0);
                    }
                }
                void visit(const QString &s, GraphNodeItem *c, int value, int total) {
                    GroupNode *node = dynamic_cast<GroupNode *>(c);
                    hash->hashTable.insert(s, HashValue(node, index, value, total));

                    if (index < 0) {
                        node->allKeys.append(GroupNode::KeyInfo(s, value, total));
                    }
                }
        };
        class DeleteVisitor: public Visitor {
            private:
                NarratorHash *hash;
            public:
                DeleteVisitor(NarratorHash *hash) {
                    this->hash = hash;
                }
                void initialize(GraphNodeItem * /*node*/) {
                }
                void visit(const QString &s, GraphNodeItem *c, int value, int total) {
                    GroupNode *node = dynamic_cast<GroupNode *>(c);
                    hash->hashTable.remove(s, HashValue(node, value, total));
                }
        };
        class FindOneVisitor: public Visitor {
            private:
                NarratorHash *hash;
                double largestEquality;
                GraphNodeItem *node;
            public:
                FindOneVisitor(NarratorHash *hash) {
                    this->hash = hash;
                    largestEquality = 0;
                    node = NULL;
                }
                void visit(const QString &s, GraphNodeItem *node1, int /*value*/, int /*total*/) {
                    HashTable::iterator i = hash->hashTable.find(s);

                    while (i != hash->hashTable.end() && i.key() == s) {
                        HashValue v = *i;
                        Narrator &narr1 = (dynamic_cast<ChainNarratorNode &>((*node1)[0])).getNarrator();
                        Narrator &narr2 = (dynamic_cast<ChainNarratorNode &>((*v.node)[0])).getNarrator();
                        double curr_equality = equal(narr1, narr2);

                        //double curr_equality=v.value/v.total*value/total;
                        if (curr_equality > largestEquality) {
                            largestEquality = curr_equality;
                            node = v.node;
                        }

                        ++i;
                    }
                }
                double getEqualityValue() {
                    return largestEquality;
                }
                GraphNodeItem *getCorrespondingNode() {
                    return node;
                }
        };
        class FindAllVisitor: public Visitor {
            private:
                Action &visitor;
                NarratorHash *hash;
            public:
                FindAllVisitor(NarratorHash *h, Action &v): visitor(v), hash(h) {}
                void visit(const QString &s, GraphNodeItem *node, int /*value*/, int /*total*/) {
                    HashTable::iterator i = hash->hashTable.find(s);
                    Narrator &narr1 = (dynamic_cast<ChainNarratorNode &>((*node)[0])).getNarrator();

                    while (i != hash->hashTable.end() && i.key() == s) {
                        HashValue v = *i;
                        Narrator &narr2 = (dynamic_cast<ChainNarratorNode &>((*v.node)[0])).getNarrator();
                        double similarity = equal(narr1, narr2);
                        //double similarity=(double)(v.value)/v.total*value/total;

                        if (similarity > 0.0001) {
                            if (visitor.isFoundAction()) {
                                assert(v.index < 0);
                                dynamic_cast<FoundAction &>(visitor).action(s, v.node, similarity);
                            } else {
                                assert(v.index >= 0);
                                dynamic_cast<BiographyAction &>(visitor).action(s, v.node, v.index, similarity);
                            }
                        }

                        ++i;
                    }
                }
        };
        class DebuggingVisitor: public Visitor {
            public:
                virtual void visit(const QString &s, GraphNodeItem *, int value, int total) {
                    qDebug() << "\t" << s << "\t" << value << "/" << total;
                }
        };

        void generateAllPosibilities(GraphNodeItem *node,
                                     Visitor &v) {   //can be ChainNode only if we are searching, not if we are inserting
            v.initialize(node);

            if (node->isGroupNode()) { //if we are searching and isGroupNode, use its keys
                GroupNode *n = (GroupNode *)node;
                int size = n->allKeys.size();

                if (size > 0) {
                    for (int i = 0; i < size; i++) {
                        GroupNode::KeyInfo &key = n->allKeys[i];
                        v.visit(key.first, node, key.second, key.third); //TODO: save numbers along with keys
                    }

                    return;
                }
            }

            Narrator *n = &(dynamic_cast<ChainNarratorNode &>((*node)[0])).getNarrator();
            bool abihi = isRelativeNarrator(*n);

            if (abihi) {
                return;    //we dont know yet to what person is the ha2 in abihi a reference so they might not be equal.
            }

            if (n->isRasoul) {
                v.visit(alrasoul, node, 1, 1);
                return;
            }

            NarratorPrimHierarchy names;
            PossessiveList possessives;
            n->preProcessForEquality(names, possessives);
            int levelSize = names.size();
            assert(levelSize > 0);
            bool possAvailable = possessives.count() > 0,
                 levelOneEmpty = names.at(0).size() == 0;
            int total = (possAvailable ? (levelSize - 1) * 2 : levelSize - 1) + (!levelOneEmpty ?
                        (possAvailable ? levelSize * 2 : levelSize) : 0);
            int minLevel = (levelOneEmpty ? 2 : 1);
            int value;

            for (int level = levelSize; level >= minLevel; level--) {
                value = (level - minLevel + 1) * (!levelOneEmpty ? 2 : 1) + (minLevel - 2);
                v.visit(getKey(names, possessives, level, false, false), node, value, total);

                if (possAvailable) {
                    value *= 2;
                    v.visit(getKey(names, possessives, level, true, false), node, value, total);
                }

                if (level > 1 && !levelOneEmpty) {
                    value = level - 1;
                    v.visit(getKey(names, possessives, level, false, true), node, value, total);

                    if (possAvailable) {
                        value *= 2;
                        v.visit(getKey(names, possessives, level, true, true), node, value, total);
                    }
                }
            }
        }
    public:
        NarratorHash(NarratorGraph *graph) {
            this->graph = graph;
        }
        void serialize(QDataStream &streamOut);
        void deserialize(QDataStream &streamIn);
        void addNode(GroupNode *node, int index = -1) {
            InsertVisitor v(this, index);
            generateAllPosibilities(node, v);
        }
        GraphNodeItem *findCorrespondingNode(Narrator *n) {
            ChainNarratorNode *node = new ChainNarratorNode();
            node->narrator = n;
            FindOneVisitor v(this);
            generateAllPosibilities(node, v);
            delete node;
            GraphNodeItem *c = v.getCorrespondingNode();
            return c;
        }
        void performActionToAllCorrespondingNodes(GraphNodeItem *node, Action &visitor) {
            FindAllVisitor v(this, visitor);
            generateAllPosibilities(node, v);
        }
        void performActionToAllCorrespondingNodes(Narrator *n, Action &visitor) {
            ChainNarratorNode *node = new ChainNarratorNode();
            node->narrator = n;
            performActionToAllCorrespondingNodes(node, visitor);
            delete node;
        }
        void performActionToExactCorrespondingNodes(GraphNodeItem *node,
                                                    Action &visitor) {   //dont generate all keys, just use the primary key
            FindAllVisitor v(this, visitor);
            v.initialize(node);
            v.visit(node->getKey(), node, 1, 1);
        }
        void clear() {
            hashTable.clear();
        }
        HashTable::iterator begin() {
            return hashTable.begin();
        }
        HashTable::iterator end() {
            return hashTable.end();
        }
        int size() const {
            return hashTable.size();
        }
        void remove(GroupNode *node) {
            DeleteVisitor v(this);
            generateAllPosibilities(node, v);
        }
};


#endif
