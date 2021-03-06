#include "remotewidget.h"

RemoteWidget::RemoteWidget(QString pathToConfig, QString autoExecCmd, QWidget *parent) : QWidget(parent) {
    this->configPath = pathToConfig;
    this->autoExecCmd = autoExecCmd;
    layout = new QGridLayout();
    buttonLayout = new QGridLayout();
    setLayout(layout);
    initUi();
}

RemoteWidget::~RemoteWidget() {
    for(int i=0; i<buttons.length(); i++) {
        QPushButton * button = buttons.at(i);
        delete button;
    }
    delete buttonLayout;
    delete layout;
    delete hostSelector;
    delete hostLabel;
    delete splitter;
}

void RemoteWidget::initUi() {
    hostSelector = new QComboBox(this);
    layout->addWidget(hostSelector,0,1,1,2);
    hostSelector->setEditable(true);
    hostSelector->setToolTip("Select a host");
    connect(hostSelector,SIGNAL(editTextChanged(QString)),this,SLOT(hostChanged()));
    hostSelector->addItems(IPSaver::getInstance()->getHosts());
    hostSelector->setCurrentText(IPSaver::getInstance()->getMostRecentHost());

    hostLabel = new QLabel(this);
    hostLabel->setText("IP Address of TV:");
    layout->addWidget(hostLabel,0,0,1,1);

    splitter = new QSplitter(this);
    layout->addWidget(splitter,1,0,1,4);

    layout->addLayout(buttonLayout,2,0,1,4);

    QString path = QCoreApplication::applicationDirPath()
            .append(QDir::separator())
            .append(configPath);
    QDir configDir = QDir(path);
    if(!configDir.exists()) {
        cerr << "Couldn't init Remote ui as config dir "
             << path.toStdString()
             << " doesn't exist" << endl;
    }
    QDirIterator it(configDir,QDirIterator::NoIteratorFlags);
    while(it.hasNext()) {
        QString fileName = it.next();
        if(fileName.contains(".control")) {
            //RemoteButton handles the commandFile now.
            CommandFile *commandFile = new CommandFile(fileName,path);
            RemoteButton *button = new RemoteButton(commandFile,this);
            buttonLayout->addWidget(button,commandFile->getRow(),commandFile->getColumn(),1,1);
            QFileInfo fileInfo(fileName);
            if(fileInfo.baseName()==autoExecCmd) {
                button->setHost(IPSaver::getInstance()->getMostRecentHost());
                button->setQuitOnResponse(true);
                button->clicked();
                cout << "AutoExec sent" << endl;
                return;
            }
            buttons.push_back(button);
        }
    }
    hostChanged();
}

void RemoteWidget::hostChanged() {
    for(int i=0; i<buttons.length(); i++) {
        RemoteButton * button = buttons.at(i);
        button->setHost(hostSelector->currentText());
    }
}

void RemoteWidget::pingHostClicked() {

}
