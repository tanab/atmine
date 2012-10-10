#include "customsttview.h"
#include "sstream"
#include "logger.h"
#include "global.h"
#include <QGridLayout>
#include <QScrollArea>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <qjson/parser.h>

CustomSTTView::CustomSTTView(QWidget *parent) :
    QMainWindow(parent)
{
    dirty = false;

    QGridLayout *grid = new QGridLayout();
    btnSelectAll = new QPushButton(tr("Select All"), this);
    btnUnselectAll = new QPushButton(tr("Unselect All"), this);
    btnSelect = new QPushButton(tr(">"), this);
    btnUnselect = new QPushButton(tr("<"), this);
    btnAdd = new QPushButton(tr("Add\nType"), this);
    btnLoad = new QPushButton(tr("Load"), this);
    btnSave = new QPushButton(tr("Save File"), this);
    btnCancel = new QPushButton(tr("Cancel"), this);
    btnClose = new QPushButton(tr("Close"), this);
    btnRemove = new QPushButton(tr("Remove\nType"), this);

    btnRemove->setEnabled(false);
    btnSave->setEnabled(false);
    btnSelect->setEnabled(false);
    btnUnselect->setEnabled(false);

    connect(btnSelectAll,SIGNAL(clicked()),this,SLOT(btnSelectAll_clicked()));
    connect(btnUnselectAll,SIGNAL(clicked()),this,SLOT(btnUnselectAll_clicked()));
    connect(btnAdd,SIGNAL(clicked()),this,SLOT(btnAdd_clicked()));
    connect(btnRemove, SIGNAL(clicked()), this, SLOT(btnRemove_clicked()));
    connect(btnLoad,SIGNAL(clicked()),this,SLOT(btnLoad_clicked()));
    connect(btnSave, SIGNAL(clicked()), this, SLOT(btnSave_clicked()));
    connect(btnCancel,SIGNAL(clicked()), this, SLOT(btnCancel_clicked()));
    connect(btnClose, SIGNAL(clicked()), this, SLOT(btnClose_clicked()));

    grid->addWidget(btnSave,14,5);
    grid->addWidget(btnSelect,5,2);
    grid->addWidget(btnUnselect,6,2);
    grid->addWidget(btnAdd,2,2);
    grid->addWidget(btnRemove,3,2);
    grid->addWidget(btnLoad,14,6);
    grid->addWidget(btnClose,13,6);
    grid->addWidget(btnCancel,13,5);
    grid->addWidget(btnSelectAll,14,0);
    grid->addWidget(btnUnselectAll,14,1);

    lblPattern = new QLabel(tr("Pattern:"),this);
    lblFeatures = new QLabel(tr("Features:"), this);
    lblTagName = new QLabel(tr("TagName:"), this);
    lblDescription = new QLabel(tr("Description:"), this);
    lblFGColor = new QLabel(tr("Foregroud Color:"), this);
    lblBGColor = new QLabel(tr("Background Color:"), this);
    lblFont = new QLabel(tr("Font:"), this);
    lblBold = new QLabel(tr("Bold:"), this);
    lblItalic = new QLabel(tr("Italic:"), this);
    lblUnderline = new QLabel(tr("Underline:"), this);

    grid->addWidget(lblPattern,1,0);
    grid->addWidget(lblFeatures,0,0);
    grid->addWidget(lblTagName,0,3);
    grid->addWidget(lblDescription,0,5);//7,3
    grid->addWidget(lblFGColor,3,5);
    grid->addWidget(lblBGColor,4,5);
    grid->addWidget(lblFont,5,5);
    grid->addWidget(lblBold,6,5);
    grid->addWidget(lblItalic,7,5);
    grid->addWidget(lblUnderline,8,5);

    editPattern = new QLineEdit(this);
    connect(editPattern,SIGNAL(textChanged(QString)),this,SLOT(editPattern_changed(QString)));
    editDescription = new QTextEdit(this);

    grid->addWidget(editPattern,1,1);
    grid->addWidget(editDescription,1,5,2,2);

    /** random color routine **/

    QStringList colorNames = QColor::colorNames();
    int size = colorNames.size();
    double randomNumber = ((double) rand() / (RAND_MAX));
    int index = size * randomNumber;
    QColor initColor = colorNames[index];
    colorfgcolor = new ColorListEditor(this);
    colorfgcolor->setColor(initColor);

    int contrastIndex = 147 - index;
    QColor contrastColor = colorNames[contrastIndex];

    colorbgcolor = new ColorListEditor(this);
    colorbgcolor->setColor(contrastColor);

    /** end of routine **/

    grid->addWidget(colorfgcolor,3,6);
    grid->addWidget(colorbgcolor,4,6);

    cbfont = new QComboBox(this);
    for(int i=5; i<20; i++) {
        std::stringstream out;
        out << i;
        std::string str = out.str();
        cbfont->addItem(QString::fromStdString(str));
    }

    cbTagName = new QComboBox(this);
    for(int i=0; i<_atagger->tagTypeVector->count(); i++) {
        cbTagName->addItem(_atagger->tagTypeVector->at(i)->tag);
    }

    connect(cbTagName, SIGNAL(editTextChanged(QString)), this, SLOT(tagName_Edited(QString)));
    connect(cbTagName, SIGNAL(currentIndexChanged(QString)), this, SLOT(tagName_changed(QString)));

    cbTagType = new QComboBox(this);
    cbTagType->addItem("Prefix");
    cbTagType->addItem("Stem");
    cbTagType->addItem("Suffix");
    cbTagType->addItem("Prefix-POS");
    cbTagType->addItem("Stem-POS");
    cbTagType->addItem("Suffix-POS");
    cbTagType->addItem("Gloss");
    cbTagType->addItem("Category");

    grid->addWidget(cbfont,5,6);
    grid->addWidget(cbTagName,0,4);
    grid->addWidget(cbTagType,0,1);

    cbunderline = new QCheckBox(this);
    cbBold = new QCheckBox(this);
    cbItalic = new QCheckBox(this);
    cbCaseSensetive = new QCheckBox(this);
    cbCaseSensetive->setText("Case Sensetive");

    connect(cbunderline, SIGNAL(clicked(bool)), this, SLOT(underline_clicked(bool)));
    connect(cbBold, SIGNAL(clicked(bool)), this, SLOT(bold_clicked(bool)));
    connect(cbItalic, SIGNAL(clicked(bool)), this, SLOT(italic_clicked(bool)));

    grid->addWidget(cbBold,6,6);
    grid->addWidget(cbItalic,7,6);
    grid->addWidget(cbunderline,8,6);
    grid->addWidget(cbCaseSensetive,1,2);

    listPossibleTags = new QListWidget(this);
    listPossibleTags->setSelectionMode(QAbstractItemView::MultiSelection);
    listSelectedTags = new QTreeWidget(this);
    listSelectedTags->setColumnCount(2);
    QStringList columnsD;
    columnsD << "Feature" << "Value";
    QTreeWidgetItem* itemD=new QTreeWidgetItem(columnsD);
    listSelectedTags->setHeaderItem(itemD);
    listSelectedTags->setSelectionMode(QAbstractItemView::MultiSelection);

    grid->addWidget(listPossibleTags,2,0,12,2);
    grid->addWidget(listSelectedTags,1,3,13,2);

    QWidget *widget = new QWidget(this);

    widget->setLayout(grid);
    setCentralWidget(widget);
    setWindowTitle(tr("Custom Sarf Tag Types"));
    //resize(480, 320);

    /** Do queries and Fill Tables**/
    theSarf->query.exec("SELECT raw_data FROM stem_category");
    while(theSarf->query.next()) {
        if(!(theSarf->query.value(0).toString().isEmpty()))
            listStems << theSarf->query.value(0).toString();
    }

    theSarf->query.exec("SELECT raw_data FROM prefix_category");
    while(theSarf->query.next()) {
        if(!(theSarf->query.value(0).toString().isEmpty()))
            listPrefix << theSarf->query.value(0).toString();
    }

    field = "Prefix";
    listPossibleTags->clear();
    listPossibleTags->addItems(listPrefix);

    theSarf->query.exec("SELECT raw_data FROM suffix_category");
    while(theSarf->query.next()) {
        if(!(theSarf->query.value(0).toString().isEmpty()))
            listSuffix << theSarf->query.value(0).toString();
    }

    theSarf->query.exec("SELECT name FROM description");
    while(theSarf->query.next()) {
        if(!(theSarf->query.value(0).toString().isEmpty()))
            listGloss << theSarf->query.value(0).toString();
    }

    theSarf->query.exec("SELECT POS FROM stem_category");
    while(theSarf->query.next()) {
        if(!(theSarf->query.value(0).toString().isEmpty())) {
            listStemPOS << theSarf->query.value(0).toString().split('/').at(1);
            listStemPOS.removeDuplicates();
        }
    }

    theSarf->query.exec("SELECT POS FROM prefix_category");
    while(theSarf->query.next()) {
        if(!(theSarf->query.value(0).toString().isEmpty())) {
            listPrefixPOS << theSarf->query.value(0).toString().split('/').at(1);
            listPrefixPOS.removeDuplicates();
        }
    }

    theSarf->query.exec("SELECT POS FROM suffix_category");
    while(theSarf->query.next()) {
        if(!(theSarf->query.value(0).toString().isEmpty())) {
            listSuffixPOS << theSarf->query.value(0).toString().split('/').at(1);
            listSuffixPOS.removeDuplicates();
        }
    }

    theSarf->query.exec("SELECT id,name FROM category where abstract=1");
    while(theSarf->query.next()) {
        if(!(theSarf->query.value(0).toString().isEmpty())) {
            listCategoryId << theSarf->query.value(0).toString();
            listCategory << theSarf->query.value(1).toString();
            listCategory.removeDuplicates();
        }
    }

    if(_atagger->tagTypeVector->count() != 0) {
        //cbTagName->setEditable(true);
        const SarfTagType * stt = (SarfTagType*)(_atagger->tagTypeVector->at(0));

        cbTagName->setCurrentIndex(0);
        cbunderline->setChecked(stt->underline);
        cbBold->setChecked(stt->bold);
        cbItalic->setChecked(stt->italic);
        colorfgcolor->setColor(QColor(stt->fgcolor));
        colorbgcolor->setColor(QColor(stt->bgcolor));
        editDescription->setText(stt->description);
        int index = cbfont->findText(QString::number(stt->font));
        if(index != -1) {
            cbfont->setCurrentIndex(index);
        }

        listSelectedTags->clear();
        for(int i=0; i<stt->tags.count(); i++) {
            QStringList list;
            list << stt->tags.at(i).first << stt->tags.at(i).second;
            listSelectedTags->addTopLevelItem(new QTreeWidgetItem(list));
        }

        btnRemove->setEnabled(true);
        btnSave->setEnabled(true);
        btnSelect->setEnabled(true);
        btnUnselect->setEnabled(true);
    }

    connect(colorfgcolor, SIGNAL(currentIndexChanged(QString)), this, SLOT(fgcolor_changed(QString)));
    connect(colorbgcolor, SIGNAL(currentIndexChanged(QString)), this, SLOT(bgcolor_changed(QString)));
    connect(cbTagType,SIGNAL(currentIndexChanged(QString)), this, SLOT(cbTagType_changed(QString)));
    connect(cbfont, SIGNAL(currentIndexChanged(QString)), this, SLOT(font_changed(QString)));
    connect(editDescription, SIGNAL(textChanged()), this, SLOT(desc_edited()));
    connect(btnSelect,SIGNAL(clicked()),this,SLOT(btnSelect_clicked()));
    connect(btnUnselect,SIGNAL(clicked()),this,SLOT(btnUnselect_clicked()));

    //sttVector = new QVector<TagType*>(*(_atagger->tagTypeVector));
    sttVector = new QVector<TagType*>();

    for(int i=0; i<_atagger->tagTypeVector->count(); i++) {
        TagType* tt = _atagger->tagTypeVector->at(i);
        QString tag = tt->tag;
        QString desc = tt->description;
        QString fgcolor = tt->fgcolor;
        QString bgcolor = tt->bgcolor;
        int font = tt->font;
        bool underline = tt->underline;
        bool italic = tt->italic;
        bool bold = tt->bold;
        int id = tt->id;
        Source source = tt->source;

        if(_atagger->tagTypeVector->at(i)->source == sarf) {
            SarfTagType* _stt = (SarfTagType*)(_atagger->tagTypeVector->at(i));
            QVector< QPair< QString , QString > > tags = QVector< QPair<QString,QString> >(_stt->tags);
            SarfTagType* stt = new SarfTagType(tag,tags,desc,id,fgcolor,bgcolor,font,underline,bold,italic,source);
            sttVector->append(stt);

        }
        else {
            TagType* stt = new TagType(tag,desc,id,fgcolor,bgcolor,font,underline,bold,italic,source);
            sttVector->append(stt);
        }
    }
}

void CustomSTTView::cbTagType_changed(QString text) {
    field = text;
    listPossibleTags->clear();
    editPattern->clear();
    if(field == "Prefix") {
        listPossibleTags->addItems(listPrefix);
    }
    else if(field == "Prefix-POS") {
        listPossibleTags->addItems(listPrefixPOS);
    }
    else if(field == "Stem") {
        listPossibleTags->addItems(listStems);
    }
    else if(field == "Stem-POS") {
        listPossibleTags->addItems(listStemPOS);
    }
    else if(field == "Suffix") {
        listPossibleTags->addItems(listSuffixPOS);
    }
    else if(field == "Suffix-POS") {
        listPossibleTags->addItems(listSuffixPOS);
    }
    else if(field == "Gloss") {
        listPossibleTags->addItems(listGloss);
    }
    else if(field == "Category") {
        listPossibleTags->addItems(listCategory);
    }
}

void CustomSTTView::disconnect_Signals() {
    disconnect(cbunderline, SIGNAL(clicked(bool)), this, SLOT(underline_clicked(bool)));
    disconnect(cbBold, SIGNAL(clicked(bool)), this, SLOT(bold_clicked(bool)));
    disconnect(cbItalic, SIGNAL(clicked(bool)), this, SLOT(italic_clicked(bool)));
    disconnect(cbTagName, SIGNAL(editTextChanged(QString)), this, SLOT(tagName_Edited(QString)));
    disconnect(cbTagName, SIGNAL(currentIndexChanged(QString)), this, SLOT(tagName_changed(QString)));
    disconnect(editPattern,SIGNAL(textChanged(QString)),this,SLOT(editPattern_changed(QString)));
    disconnect(colorfgcolor, SIGNAL(currentIndexChanged(QString)), this, SLOT(fgcolor_changed(QString)));
    disconnect(colorbgcolor, SIGNAL(currentIndexChanged(QString)), this, SLOT(bgcolor_changed(QString)));
    disconnect(cbTagType,SIGNAL(currentIndexChanged(QString)), this, SLOT(cbTagType_changed(QString)));
    disconnect(cbfont, SIGNAL(currentIndexChanged(QString)), this, SLOT(font_changed(QString)));
    disconnect(editDescription, SIGNAL(textChanged()), this, SLOT(desc_edited()));
}

void CustomSTTView::connect_Signals() {
    connect(cbunderline, SIGNAL(clicked(bool)), this, SLOT(underline_clicked(bool)));
    connect(cbBold, SIGNAL(clicked(bool)), this, SLOT(bold_clicked(bool)));
    connect(cbItalic, SIGNAL(clicked(bool)), this, SLOT(italic_clicked(bool)));
    connect(cbTagName, SIGNAL(editTextChanged(QString)), this, SLOT(tagName_Edited(QString)));
    connect(cbTagName, SIGNAL(currentIndexChanged(QString)), this, SLOT(tagName_changed(QString)));
    connect(editPattern,SIGNAL(textChanged(QString)),this,SLOT(editPattern_changed(QString)));
    connect(colorfgcolor, SIGNAL(currentIndexChanged(QString)), this, SLOT(fgcolor_changed(QString)));
    connect(colorbgcolor, SIGNAL(currentIndexChanged(QString)), this, SLOT(bgcolor_changed(QString)));
    connect(cbTagType,SIGNAL(currentIndexChanged(QString)), this, SLOT(cbTagType_changed(QString)));
    connect(cbfont, SIGNAL(currentIndexChanged(QString)), this, SLOT(font_changed(QString)));
    connect(editDescription, SIGNAL(textChanged()), this, SLOT(desc_edited()));
}

void CustomSTTView::btnAdd_clicked() {

    QString tagName = QInputDialog::getText(this,"Tag Name","Please insert a Tag Type Name");
    if(tagName.isEmpty()) {
        QMessageBox::warning(this,"Warning","Empty TagType Name!");
        return;
    }

    for(int i=0; i<sttVector->count(); i++) {
        const SarfTagType * stt = (SarfTagType*)(sttVector->at(i));
        if(stt->tag == tagName) {
            QMessageBox::warning(this, "Warning", "This Tag Name already exists!");
            return;
        }
    }
    disconnect_Signals();

    dirty = true;

    btnRemove->setEnabled(true);
    btnSave->setEnabled(true);

    if(!btnSelect->isEnabled()) {
        btnSelect->setEnabled(true);
        btnUnselect->setEnabled(true);
        btnRemove->setEnabled(true);
    }

    listSelectedTags->clear();
    editDescription->clear();
    cbTagName->addItem(tagName);
    cbTagName->setCurrentIndex(cbTagName->findText(tagName));
    cbfont->setCurrentIndex(cbfont->findText("12"));
    cbunderline->setChecked(false);
    cbBold->setChecked(false);
    cbItalic->setChecked(false);
    //cbTagName->setEditable(true);
    //editDescription->setText(tagDescription);

    /** Random Color Routine **/

    QStringList colorNames = QColor::colorNames();
    int size = colorNames.size();
    double randomNumber = ((double) rand() / (RAND_MAX));
    int index = size * randomNumber;
    QColor initColor = colorNames[index];
    //colorfgcolor = new ColorListEditor(this);
    colorfgcolor->setColor(initColor);

    int contrastIndex = 147 - index;
    QColor contrastColor = colorNames[contrastIndex];

    //colorbgcolor = new ColorListEditor(this);
    colorbgcolor->setColor(contrastColor);

    /** routine End **/

#if 0
    colorfgcolor = new ColorListEditor(this);
    colorfgcolor->setColor(QColor("Red"));

    colorbgcolor = new ColorListEditor(this);
    colorbgcolor->setColor(QColor("Yellow"));
#endif

    QVector < QPair< QString, QString > > tags;
    int id = sttVector->count();
    QString fgcolor = colorfgcolor->color().name();
    QString bgcolor = colorbgcolor->color().name();
    int font = cbfont->currentText().toInt();
    bool underline = cbunderline->isChecked();
    bool bold = cbBold->isChecked();
    bool italic = cbItalic->isChecked();
    SarfTagType* sarftagtype = new SarfTagType(tagName,tags,QString(),id,fgcolor,bgcolor,font,underline,bold,italic,sarf);
    sttVector->append(sarftagtype);

    connect_Signals();
}

void CustomSTTView::btnSelectAll_clicked() {
    listPossibleTags->selectAll();
}

void CustomSTTView::btnSelect_clicked() {
    if(sttVector->count() == 0) {
        return;
    }

    dirty = true;

    SarfTagType * stt;
    for(int i=0; i<sttVector->count(); i++) {
        if(sttVector->at(i)->tag == cbTagName->currentText()) {
            stt = (SarfTagType*)((*(sttVector))[i]);
            break;
        }
    }

    foreach(QListWidgetItem* item, listPossibleTags->selectedItems()) {

        QStringList list;
        list << field << item->text();
        listSelectedTags->addTopLevelItem(new QTreeWidgetItem(list));
        QString value = item->text();
        if(field == "Category") {
            int index = -1;//listCategory.indexOf('^' + value + '$');
            for(int i=0; i<listCategory.size(); i++) {
                if(listCategory[i] == value) {
                    index = i;
                    break;
                }
            }
            if(index != -1) {
                value = listCategoryId[index];
            }
        }
        QPair <QString, QString> pair(field, value);
        stt->tags.append(pair);
    }
    listSelectedTags->sortItems(0,Qt::AscendingOrder);
    listPossibleTags->clearSelection();
}

void CustomSTTView::btnUnselectAll_clicked() {
    listPossibleTags->clearSelection();
}

void CustomSTTView::btnUnselect_clicked() {
    dirty = true;

    qDeleteAll(listSelectedTags->selectedItems());

    SarfTagType * stt;
    for(int i=0; i<sttVector->count(); i++) {
        if(sttVector->at(i)->tag == cbTagName->currentText()) {
            stt = (SarfTagType*)((*(sttVector))[i]);
            break;
        }
    }

    stt->tags.clear();

    for(int i=0; i<listSelectedTags->topLevelItemCount(); i++) {
        QString data1 = listSelectedTags->topLevelItem(i)->text(0);
        QString data2 = listSelectedTags->topLevelItem(i)->text(1);
        QPair <QString, QString> pair(data1, data2);
        stt->tags.append(pair);
    }
}

void CustomSTTView::editPattern_changed(QString text) {
    QStringList *list;
    if(cbTagType->currentText() == "Stem") {
        list = &listStems;
    }
    else if(cbTagType->currentText() == "Prefix") {
        list = &listPrefix;
    }
    else if(cbTagType->currentText() == "Suffix") {
        list = &listSuffix;
    }
    else if(cbTagType->currentText() == "Gloss") {
        list = &listGloss;
    }
    else if(cbTagType->currentText() == "Prefix-POS") {
        list = &listPrefixPOS;
    }
    else if(cbTagType->currentText() == "Stem-POS") {
        list = &listStemPOS;
    }
    else if(cbTagType->currentText() == "Suffix-POS") {
        list = &listSuffixPOS;
    }
    else if(cbTagType->currentText() == "Category") {
        list = &listCategory;
    }

    Qt::CaseSensitivity sensetive;

    if(cbCaseSensetive->isChecked()) {
        sensetive = Qt::CaseSensitive;
    }
    else {
        sensetive = Qt::CaseInsensitive;
    }

    QRegExp regExp(text, sensetive);

    listPossibleTags->clear();
    if(editPattern->text().isEmpty()) {
        listPossibleTags->addItems(*list);
    }
    else {
        listPossibleTags->addItems(list->filter(regExp));
    }
}

void CustomSTTView::btnLoad_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this,
             tr("Open Sarf Tag Types"), "",
             tr("Sarf Tag Types (*.stt.json);;All Files (*)"));

    if (fileName.isEmpty()) {
        return;
    }
    else {
        QFile file(fileName);
         if (!file.open(QIODevice::ReadOnly)) {
             QMessageBox::information(this, tr("Unable to open file"),file.errorString());
             return;
         }

         sttVector->clear();
         _atagger->tagtypeFile = fileName;

         QByteArray sarfTT = file.readAll();
         file.close();

         QJson::Parser parser;
         bool ok;

         QVariantMap result = parser.parse(sarfTT,&ok).toMap();
         foreach(QVariant type, result["TagSet"].toList()) {
             QVariantMap typeElements = type.toMap();

             QString tag = typeElements["Tag"].toString();
             QString desc = typeElements["Description"].toString();
             int id = typeElements["Legend"].toInt();
             QString foreground_color = typeElements["foreground_color"].toString();
             QString background_color = typeElements["background_color"].toString();
             int font = typeElements["font"].toInt();
             bool underline = typeElements["underline"].toBool();
             bool bold = typeElements["bold"].toBool();
             bool italic = typeElements["italic"].toBool();

             QVector < QPair < QString, QString> > tags;
             foreach(QVariant sarfTags, typeElements["Features"].toList()) {
                 QVariantMap st = sarfTags.toMap();
                 QPair<QString, QString> pair;
                 if(!(st.value("Prefix").isNull())) {
                     pair.first = "Prefix";
                     pair.second = st.value("Prefix").toString();
                     tags.append(pair);
                 }
                 else if(!(st.value("Stem").isNull())) {
                     pair.first = "Stem";
                     pair.second = st.value("Stem").toString();
                     tags.append(pair);
                 }
                 else if(!(st.value("Suffix").isNull())) {
                     pair.first = "Suffix";
                     pair.second = st.value("Suffix").toString();
                     tags.append(pair);
                 }
                 else if(!(st.value("Prefix-POS").isNull())) {
                     pair.first = "Prefix-POS";
                     pair.second = st.value("Prefix-POS").toString();
                     tags.append(pair);
                 }
                 else if(!(st.value("Stem-POS").isNull())) {
                     pair.first = "Stem-POS";
                     pair.second = st.value("Stem-POS").toString();
                     tags.append(pair);
                 }
                 else if(!(st.value("Suffix-POS").isNull())) {
                     pair.first = "Suffix-POS";
                     pair.second = st.value("Suffix-POS").toString();
                     tags.append(pair);
                 }
                 else if(!(st.value("Gloss").isNull())) {
                     pair.first = "Gloss";
                     pair.second = st.value("Gloss").toString();
                     tags.append(pair);
                 }
                 else if(!(st.value("Category").isNull())) {
                     pair.first = "Category";
                     pair.second = st.value("Category").toString();
                     tags.append(pair);
                 }
             }

             _atagger->insertSarfTagType(tag,tags,desc,id,foreground_color,background_color,font,underline,bold,italic,sarf);
         }

         cbTagName->clear();
         listSelectedTags->clear();

         for(int i=0; i<_atagger->tagTypeVector->count(); i++) {
             const SarfTagType * stt = (SarfTagType*)(_atagger->tagTypeVector->at(i));
             cbTagName->addItem(stt->tag);
         }

         cbTagName->setCurrentIndex(0);
         btnRemove->setEnabled(true);
         btnSave->setEnabled(true);
         dirty = false;
     }
}

void CustomSTTView::btnSave_clicked() {
    QByteArray sarftagtypeData = _atagger->dataInJsonFormat(sarfTTV);

    QString fileName;
    if(_atagger->tagtypeFile.isEmpty()) {
        fileName = QFileDialog::getSaveFileName(
                this,
                tr("Save Sarf Tag Types"), "",
                tr("Text (*.stt.json);;All Files (*)"));
        if(fileName.isEmpty()) {
            QMessageBox::warning(this, "Warning", "The Sarf Tag Types file wasn't saved");
            return;
        }
        else {
            fileName += ".stt.json";
            _atagger->tagtypeFile = fileName;
        }
    }
    else {
        fileName = _atagger->tagtypeFile;
    }

    /*
    if(fileName.isEmpty()) {
        QMessageBox::warning(this, "Warning", "The Sarf Tag Types file wasn't saved");
        return;
    }
    */

    QFile tfile(fileName);
    /*
    if(_atagger->sarftagtypeFile.isEmpty() && tfile.exists()) {
        QMessageBox::warning(this,"Warning", "File already exists, please try another name");
        return;
    }
    */
    if (!tfile.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this,"Warning","Can't open tagtypes file to Save");
        return;
    }
    _atagger->tagtypeFile = fileName;

    QTextStream outtags(&tfile);
    outtags << sarftagtypeData;
    tfile.close();
    dirty = false;
}

void CustomSTTView::fgcolor_changed(QString color) {
    if(color.isNull() || color.isEmpty()) {
        return;
    }
    if(sttVector->count() == 0) {
        return;
    }

    dirty = true;
    QString tag = cbTagName->currentText();
    for(int i=0; i<sttVector->count(); i++) {
        SarfTagType * stt = (SarfTagType*)((*(sttVector))[i]);
        if(stt->tag == tag) {
            stt->fgcolor = color;
        }
    }
}

void CustomSTTView::bgcolor_changed(QString color) {
    if(color.isNull() || color.isEmpty()) {
        return;
    }
    if(sttVector->count() == 0) {
        return;
    }

    dirty = true;
    QString tag = cbTagName->currentText();
    for(int i=0; i<sttVector->count(); i++) {
        SarfTagType * stt = (SarfTagType*)((*(sttVector))[i]);
        if(stt->tag == tag) {
            stt->bgcolor = color;
        }
    }
}

void CustomSTTView::font_changed(QString fontSize) {
    if(fontSize.isNull() || fontSize.isEmpty()) {
        return;
    }
    if(sttVector->count() == 0) {
        return;
    }

    dirty = true;
    QString tag = cbTagName->currentText();
    for(int i=0; i<sttVector->count(); i++) {
        SarfTagType * stt = (SarfTagType*)((*(sttVector))[i]);
        if(stt->tag == tag) {
            stt->font = fontSize.toInt();
        }
    }
}

void CustomSTTView::tagName_changed(QString name) {
    listSelectedTags->clear();
    if(name.isEmpty() || name.isNull()) {
        return;
    }

    disconnect_Signals();
    for(int i=0; i<sttVector->count(); i++) {
        SarfTagType * stt = (SarfTagType*)((*(sttVector))[i]);
        if(stt->tag == name) {
            editDescription->setText(stt->description);
            int index = cbfont->findText(QString::number(stt->font));
            if(index != -1) {
                cbfont->setCurrentIndex(index);
            }
            colorfgcolor->setColor(QColor(stt->fgcolor));
            colorbgcolor->setColor(QColor(stt->bgcolor));
            cbunderline->setChecked(stt->underline);
            cbBold->setChecked(stt->bold);
            cbItalic->setChecked(stt->italic);
            for(int j=0; j<stt->tags.count(); j++) {
                QStringList list;
                list << stt->tags.at(j).first << stt->tags.at(j).second;
                listSelectedTags->addTopLevelItem(new QTreeWidgetItem(list));
            }
        }
    }

    connect_Signals();
}

void CustomSTTView::tagName_Edited(QString name) {

}

void CustomSTTView::underline_clicked(bool underline) {
    if(sttVector->count() == 0) {
        return;
    }

    dirty = true;
    QString tag = cbTagName->currentText();
    for(int i=0; i<sttVector->count(); i++) {
        SarfTagType * stt = (SarfTagType*)((*(sttVector))[i]);
        if(stt->tag == tag) {
            stt->underline = underline;
        }
    }
}

void CustomSTTView::bold_clicked(bool bold) {
    if(sttVector->count() == 0) {
        return;
    }

    dirty = true;
    QString tag = cbTagName->currentText();
    for(int i=0; i<sttVector->count(); i++) {
        SarfTagType * stt = (SarfTagType*)((*(sttVector))[i]);
        if(stt->tag == tag) {
            stt->bold = bold;
        }
    }
}

void CustomSTTView::italic_clicked(bool italic) {
    if(sttVector->count() == 0) {
        return;
    }

    dirty = true;
    QString tag = cbTagName->currentText();
    for(int i=0; i<sttVector->count(); i++) {
        SarfTagType * stt = (SarfTagType*)((*(sttVector))[i]);
        if(stt->tag == tag) {
            stt->italic = italic;
        }
    }
}

void CustomSTTView::desc_edited() {
    if(sttVector->count() == 0) {
        return;
    }

    dirty = true;
    QString tag = cbTagName->currentText();
    for(int i=0; i<sttVector->count(); i++) {
        SarfTagType * stt = (SarfTagType*)((*(sttVector))[i]);
        if(stt->tag == tag) {
            stt->description = editDescription->toPlainText();
        }
    }
}

void CustomSTTView::btnRemove_clicked() {
    if(sttVector->count() == 0) {
        return;
    }
    dirty = true;
    QString tagRemoved = cbTagName->currentText();
    for(int i=0; i<sttVector->count(); i++) {
        const SarfTagType * stt = (SarfTagType*)(sttVector->at(i));
        if(stt->tag == tagRemoved) {
            sttVector->remove(i);
            break;
        }
    }
    cbTagName->removeItem(cbTagName->currentIndex());
    if(sttVector->count() == 0) {
        btnSelect->setEnabled(false);
        btnRemove->setEnabled(false);
    }
}

void CustomSTTView::btnCancel_clicked() {
    dirty = false;
    this->close();
}

void CustomSTTView::btnClose_clicked() {
    this->close();
}

void CustomSTTView::closeEvent(QCloseEvent *event) {

    if(dirty) {
        QMessageBox msgBox;
         msgBox.setText("The document has been modified.");
         msgBox.setInformativeText("Do you want to save your changes?");
         msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
         msgBox.setDefaultButton(QMessageBox::Save);
         int ret = msgBox.exec();

         switch (ret) {
         case QMessageBox::Save:
             /*
             _atagger->sarfTagTypeVector->clear();
             for(int i=0; i< sttVector->count(); i++) {
                 _atagger->sarfTagTypeVector->append(sttVector->at(i));
             }
             */
             //_atagger->tagTypeVector = new QVector<TagType*>(*(sttVector));
             _atagger->tagTypeVector->clear();
             for(int i=0; i<sttVector->count(); i++) {
                 const TagType* tt = sttVector->at(i);
                 QString tag = tt->tag;
                 QString desc = tt->description;
                 QString fgcolor = tt->fgcolor;
                 QString bgcolor = tt->bgcolor;
                 int font = tt->font;
                 bool underline = tt->underline;
                 bool italic = tt->italic;
                 bool bold = tt->bold;
                 int id = tt->id;
                 Source source = tt->source;

                 if(sttVector->at(i)->source == sarf) {
                     SarfTagType* _stt = (SarfTagType*)(sttVector->at(i));
                     QVector< QPair< QString , QString > > tags = QVector< QPair<QString,QString> >(_stt->tags);
                     SarfTagType* stt = new SarfTagType(tag,tags,desc,id,fgcolor,bgcolor,font,underline,bold,italic,source);
                     _atagger->tagTypeVector->append(stt);

                 }
                 else {
                     TagType* stt = new TagType(tag,desc,id,fgcolor,bgcolor,font,underline,bold,italic,source);
                     _atagger->tagTypeVector->append(stt);
                 }
             }
             break;
         case QMessageBox::Discard:
             break;
         default:
             break;
         }
     }
}
