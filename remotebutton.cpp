#include "remotebutton.h"

RemoteButton::RemoteButton(CommandFile *file, QWidget *parent) : QPushButton(parent) {
    this->file = file;
    quit = false;
    setText(file->getLabel());
    if(file->isAutoRepeat()) {
        setAutoRepeat(true);
        setAutoRepeatInterval(autoRepeatDelay()/10.0);
    }
    connect(this,SIGNAL(clicked(bool)),this,SLOT(send(bool)));
}

RemoteButton::~RemoteButton() {
    delete file;
}

void RemoteButton::setHost(QString host) {
    this->host = host;
}

void RemoteButton::send(bool) {
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QUrl url = QUrl();
    url.setScheme(file->getScheme());
    url.setHost(host);
    url.setPort(file->getPort().toInt());
    url.setPath(file->getPath());
    cout << "Sending request for " << file->getLabel().toStdString() << endl;

    QNetworkRequest request = QNetworkRequest(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader,file->getContentTypeHeader());
    request.setHeader(QNetworkRequest::UserAgentHeader,file->getUserAgentHeader());
    QMap<QString,QString> otherHeaders = file->getRawHeaders();
    QList<QString> headers = otherHeaders.keys();
    for(int i=0; i<headers.length(); i++) {
        QString headerName = headers.at(i);
        QString headerValue = otherHeaders[headerName];
        request.setRawHeader(headerName.toLocal8Bit(),headerValue.toLocal8Bit());
    }
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(logResponse(QNetworkReply*)));

    manager->post(request,file->getDataFileContents().toLocal8Bit());
}

void RemoteButton::logResponse(QNetworkReply * reply) {
    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(httpStatus!=200) {
        cout << "HTTP response: " << httpStatus << endl;
        cout << "Got response with QNetworkReploy::error value: " << reply->error() << endl;
    } else {
        IPSaver::getInstance()->addNewHost(host);
    }
    if(quit) {
        exit(0);
    }
}

void RemoteButton::setQuitOnResponse(bool quit) {
    this->quit = quit;
}
